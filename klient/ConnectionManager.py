import socket
import sys
import threading
import time
from datetime import datetime
import queue

import select


def decode_msg(data):
    data = data.decode('utf-8').rstrip().split('|')

    status = -404
    state = "ERR"

    if len(data) < 2:
        print("[Con.M] wrong data format to decode ", data)
        return state, status, []

    state = data[0]
    status = int(data[1])
    data.pop(0)
    data.pop(0)

    return state, status, data


def decode_object(data):
    objects = []

    for obj in data:
        obj = obj.split(';')
        objects.append(obj)

    return objects


def get_local_ip():
    try:
        # Create a temporary socket and connect to an external server
        # to get the local IP address assigned to the machine
        temp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        temp_socket.connect(("8.8.8.8", 80))
        local_ip = temp_socket.getsockname()[0]
        temp_socket.close()
        return local_ip
    except socket.error:
        return "127.0.0.1"  # Default to localhost if unable to determine the local IP


class ConnectionManager:
    indirect, direct, to_send = queue.Queue(), queue.Queue(), queue.Queue()

    ANY_STATUS = 420
    LISTEN_TIMEOUT = 0.1
    RECV_TIMEOUT = 3

    def __init__(self):
        self.PING_CYCLE = 0.3
        self.max_message_gap = sys.maxsize
        self.reconnecting = False
        self.last_message = None
        self.client_socket = None
        self.last_login = None
        self.callback = None
        self.lock = threading.Lock()

    def disconnect(self):
        if self.client_socket is not None:
            self.client_socket.close()
            self.client_socket = None
        self.last_message = None

        while not self.to_send.empty():
            self.direct.get()

    def start_listening(self):
        while True:
            if self.client_socket is not None:
                try:
                    read_sockets = [self.client_socket]
                    write_sockets = []
                    error_sockets = [self.client_socket]

                    readable, writable, exceptional = select.select(read_sockets, write_sockets, error_sockets,
                                                                    self.LISTEN_TIMEOUT)

                    for sock in readable:
                        if sock == self.client_socket:

                            stream = self.client_socket.recv(1024)

                            if len(stream) < 1024 and self.handle_data_stream(stream):
                                self.last_message = datetime.now()
                            else:
                                print("not accepting data")
                                self.disconnect()

                    if self.client_socket is not None:
                        self.check_connection()
                        self.handle_indirect_queue()
                        self.send_pending_messages()
                except OSError:
                    print("socket is invalid")
                except Exception as e:
                    print(e)
                    time.sleep(0.5)
            else:
                self.handle_indirect_queue()
                time.sleep(0.5)

    def handle_data_stream(self, stream):
        data_stream = stream.split(b'\n')
        valid = False

        for data in data_stream:
            try:
                action = data.decode('utf-8').split('|')[0]
            except UnicodeDecodeError:
                print("[Con.M] data are not in utf-8")
                return False

            if action == "OK" or action == "ERR":
                print("[Con.M] putting data into direct", data)
                self.direct.put(data)
                valid = True
            elif action == "START" or action == "UPDATE" or action == "WINNER" or action == "GAMER" or action == "PING":
                print("[Con.M] putting data into indirect", data)
                self.indirect.put(data)
                valid = True
            else:
                pass
                # print("[Con.M] throwing away data", data)

        return valid

    def check_connection(self):
        if self.last_message is None or self.client_socket is None:
            return

        time_interval = datetime.now() - self.last_message
        duration_in_seconds = time_interval.total_seconds()

        if duration_in_seconds > self.max_message_gap and self.reconnecting is False:
            self.reconnecting = True
            release_event = threading.Event()
            reconnection_thread = threading.Thread(target=self.reconnect, args=(release_event,))
            reconnection_thread.daemon = True
            reconnection_thread.start()
            release_event.wait()

    def reconnect(self, release_event):
        server_address = self.client_socket.getpeername()
        print("[Con.M - recon] Reconnecting to " + str(server_address[0]) + " as " + self.last_login)

        self.client_socket.close()

        if self.connect_server(server_address[0], server_address[1], self.last_login, release_event) < 0:
            print("[Con.M - recon] Cant connect to server - leaving ")
            msg = "DISCONNECTED|" + str(ConnectionManager.ANY_STATUS)
            self.indirect.put(msg.encode())
            self.last_message = None

        self.reconnecting = False

    def make_observer(self, observer):
        if callable(observer):
            self.callback = observer

    def server_msg_handler(self, message):
        if not message:
            return

        if message.rstrip() == b"PING":
            self.to_send.put("PONG")

        elif callable(self.callback):
            self.handle_request(message)
        else:
            print("[Con.M] No handler set for server msg ", message)

    def handle_request(self, message):

        decoded_data = decode_msg(message)
        print("[Con.M] handling server message ", decoded_data)
        my_thread = threading.Thread(target=self.callback, args=(decoded_data,))

        my_thread.start()

    def safe_recv(self):
        try:
            return self.direct.get(timeout=self.RECV_TIMEOUT)

        except queue.Empty:
            print("[Con.M] WARNING NO DATA TO READ, returning empty bytes")
            empty_binary_data = bytes()
            return empty_binary_data

    def send_pending_messages(self):
        if self.client_socket is None:
            while not self.to_send.empty():
                print("[Con.M] deleting message ", self.to_send.get())

        while not self.to_send.empty():
            msg = self.to_send.get()
            if not msg == "PONG":
                print("[Con.M] sanding message to server", msg)
            msg = msg + "\n"
            try:
                self.client_socket.send(msg.encode())
            except Exception:
                print(f"cant send message to server")

    def connect_server(self, ip, port, login, release=None):
        self.disconnect()

        if ip == "localhost":
            ip = get_local_ip()
        server_address = (ip, port)
        try:
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect(server_address)
            self.client_socket.settimeout(ConnectionManager.RECV_TIMEOUT)

            msg = "LOGIN|" + login
            self.to_send.put(msg)

        except OSError as e:
            print("[Con.M] Can't connect to " + ip)
            self.disconnect()
            return -3

        finally:
            if release is not None:
                release.set()

        msg = decode_msg(self.safe_recv())
        if msg[0] == "OK":
            self.last_login = login
            self.last_message = datetime.now()
            self.PING_CYCLE = int(msg[2][0])
            self.max_message_gap = self.PING_CYCLE * int(msg[2][1]) + 2
            print("setting done with ", msg)

        return msg[1]

    def get_lobbies(self):
        if self.client_socket is None or self.client_socket.fileno() == -1:
            return []

        msg = "LOBBY"
        self.to_send.put(msg)
        msg_back = decode_msg(self.safe_recv())

        if msg_back[0] == "OK":
            return decode_object(msg_back[2])
        else:
            print("didnt received lobbies")
            return []

    def join_lobby(self, lobby_name):
        if self.client_socket is None or self.client_socket.fileno() == -1:
            return False

        msg = "JOIN|" + lobby_name
        self.to_send.put(msg)

        msg_back = decode_msg(self.safe_recv())
        if msg_back[0] == "OK":
            return True

        return False

    def leave_lobby(self):
        msg = "LEAVE"
        self.to_send.put(msg)
        decode_msg(self.safe_recv())

    def get_lobby_info(self):
        msg = "LINFO"

        self.to_send.put(msg)
        data = decode_msg(self.safe_recv())

        if data[0] == "ERR":
            return []

        lobby_info = data[2]
        return lobby_info

    def start_new_game(self):
        msg = "START"

        self.to_send.put(msg)
        data = decode_msg(self.safe_recv())

        return data

    def game_state(self):
        msg = "GAIN"

        self.to_send.put(msg)
        data = decode_msg(self.safe_recv())

        return decode_object(data[2])

    def make_move(self):
        msg = "MOVE"

        self.to_send.put(msg)
        data = decode_msg(self.safe_recv())

        return data

    def handle_indirect_queue(self):
        while not self.indirect.empty():
            msg = self.indirect.get()
            self.server_msg_handler(msg)
