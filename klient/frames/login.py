import customtkinter as c
import re

main_font = "System"
clientConnection = None


def button_wrapper(entry_ip, entry_port, entry_name, label_info, app):
    result = button_connect(entry_ip, entry_port, entry_name)
    if result[0] == 1:
        label_info.configure(text=result[1], text_color='green')
        app.frame_manager(1)
    if result[0] == 2:
        label_info.configure(text=result[1], text_color='green')
        app.frame_manager(3)
    else:
        label_info.configure(text=result[1], text_color='red')


def button_connect(entry_ip, entry_port, entry_name):
    # ip test ##############
    ip = entry_ip.get().replace(" ", "")
    if not re.match(r'^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$', ip) and not ip == "localhost":
        result = "wrong ip address format"
        return [0, result]

    # port test ###########
    try:
        port = int(entry_port.get().replace(" ", ""))
    except ValueError:
        return [0, "port must be a decimal number"]

    if port < 0 or port > 65535:
        result = "port must be in range (0 - 65535)"
        return [0, result]

    # name test ########
    login = entry_name.get()

    if len(login.replace(" ", "")) == 0 or len(login) > 20:
        result = "invalid login"
        return [0, result]

    return create_connection(ip, port, login)


def create_connection(ip, port, login):
    status = clientConnection.connect_server(ip, port, login)
    if status == 0:
        return [1, "Connected - OK"]

    if status == 1:
        return [1, "reconnected - OK"]

    if status == 2:
        return [2, "reconnected into game - OK"]

    if status == -1:
        return [0, "LOGIN error - name taken"]
    if status == -2:
        return [0, "LOGIN error - name has wrong format"]
    if status == -3:
        return [0, "LOGIN error - cant connect to server"]

    else:
        return [0, "cant connect - unknown error"]


def get_login(app):
    window = app.get_window()
    global clientConnection
    clientConnection = app.get_connection_manager()

    frame = c.CTkFrame(master=window)
    frame.grid()
    frame.pack(fill='both', expand=True, pady=20, padx=60)

    label_head = c.CTkLabel(text="Připojení k serveru", master=frame, font=(main_font, 26))
    entry_ip = c.CTkEntry(master=frame, placeholder_text="ip", font=(main_font, 15))
    label_ip = c.CTkLabel(master=frame, text="ip adresa", font=(main_font, 15))
    entry_port = c.CTkEntry(master=frame, placeholder_text="port", font=(main_font, 15))
    label_port = c.CTkLabel(master=frame, text="Port", font=(main_font, 15))
    entry_name = c.CTkEntry(master=frame, placeholder_text="zadejte Login", font=(main_font, 15))
    label_name = c.CTkLabel(master=frame, text="login", font=(main_font, 15))
    label_info = c.CTkLabel(master=frame, font=(main_font, 15), text="")

    label_head.pack(pady=20)
    entry_ip.pack(pady=5)
    label_ip.pack(pady=0, ipady=0)
    entry_port.pack(pady=5)
    label_port.pack(pady=0)
    entry_name.pack(pady=5)
    label_name.pack(pady=0, ipady=0)
    label_info.pack(side='bottom', pady=5)

    entry_ip.insert(0, "localhost")
    entry_port.insert(0, "10000")
    entry_name.insert(0, "viktor")

    connect_b = c.CTkButton(master=frame, text="Připojit",
                            command=lambda: button_wrapper(entry_ip, entry_port, entry_name, label_info, app),
                            fg_color=('lightgray', 'black'), font=(main_font, 15), bg_color='red', hover_color='red',
                            corner_radius=0, text_color=("black", "white"))

    connect_b.pack(padx=100, pady=10)
