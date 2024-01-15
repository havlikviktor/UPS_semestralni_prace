import os.path
import threading
import time
from time import sleep
from tkinter import *

import PIL.Image
import customtkinter as c
import ConnectionManager

_app = None
labels = None
info_label = None
lock = threading.Lock()

main_font = "System"
img_source = os.path.join("..", "img")
img_format = ".png"


def find_img(img_name):
    img_path = os.path.join(os.path.join(os.path.dirname(__file__), img_source), f"{img_name}" + img_format)
    return c.CTkImage(light_image=PIL.Image.open(img_path), size=(90, 144))


def map_cards(board_state):
    for i in range(len(board_state)):
        if board_state[i][1] == "0":
            board_state[i][1] = "back"

        if board_state[i][1] == "-1":
            board_state[i][1] = "out"

        if board_state[i][1] == "-2":
            board_state[i][1] = "recon"

        if board_state[i][1] == "-3":
            board_state[i][1] = "dead"

    return board_state


def flip():
    global _app
    data = _app.get_connection_manager().make_move()
    if data:
        if data[0] == "OK":
            update_me(data[1])


def start_game(app):
    window = app.get_window()

    global info_label, _app
    _app = app
    connection_manager = app.get_connection_manager()
    connection_manager.make_observer(notify)

    board_frame = c.CTkFrame(master=window)
    board_frame.pack(fill='both', expand=True, pady=20, padx=50)
    board_frame.pack_propagate(False)

    global labels
    game_info = connection_manager.game_state()
    print("new game window created with: ", game_info)
    labels = get_labels(len(game_info), board_frame)
    info_label = c.CTkLabel(master=board_frame, font=(main_font, 24), text="")
    set_cards(map_cards(game_info), labels)

    flip_button = c.CTkButton(master=board_frame, text="Otočit",
                              command=lambda: flip(),
                              fg_color=('lightgray', 'black'), font=(main_font, 15), bg_color='red', hover_color='red',
                              corner_radius=0, text_color=("black", "white"))

    flip_button.place(relx=1.0, rely=1.0, anchor='se', bordermode='outside', x=-100, y=-50)
    info_label.place(relx=0.5, rely=0.5, anchor="center")


def get_labels(players_count, master_f):
    labels_list = []

    img_label = c.CTkLabel(master=master_f, image=find_img("back"), text="")
    img_label.place(relx=0.5, rely=0.75, anchor=CENTER)

    name_label = c.CTkLabel(master=master_f, text="NAME_PLAYER", font=(main_font, 15))
    name_label.place(relx=0.5, rely=0.75, anchor=CENTER, y=-90)

    labels_list.append(name_label)
    labels_list.append(img_label)

    step = 1 / players_count
    act = 0

    for _ in range(players_count - 1):
        act = act + step
        img_label = c.CTkLabel(master=master_f, image=find_img("back"), text="")
        name_label = c.CTkLabel(master=master_f, text="NAME_PLAYER", font=(main_font, 15))
        img_label.place(relx=act, rely=0.25, anchor=CENTER)
        name_label.place(relx=act, rely=0.25, anchor=CENTER, y=-90)
        labels_list.append(name_label)
        labels_list.append(img_label)

    return labels_list


def update_me(card):
    global labels
    img = find_img(card)
    labels[1].configure(image=img)


def set_cards(board_state, board_labels):
    # print(f"setting cards({len(board_labels)}) with ", board_state)
    if len(board_state) * 2 != len(board_labels):
        print(f"wrong state/labels {len(board_state) * 2}/{len(board_labels)}")

    i = 0
    for j in range(len(board_state)):
        img_path = os.path.join(os.path.join(os.path.dirname(__file__), img_source),
                                f"{board_state[j][1]}" + img_format)
        img = c.CTkImage(light_image=PIL.Image.open(img_path), size=(90, 144))

        board_labels[i].configure(text=board_state[j][0])
        i = i + 1
        board_labels[i].configure(image=img)
        i = i + 1


def show_winner(winner_name, info):
    global info_label
    info_label.configure(text=winner_name + " " + info)
    sleep(2)
    info_label.configure(text="")


def notify(data):
    with lock:
        global labels, _app
        while labels is None and not data[0] == "DISCONNECTED":
            time.sleep(0.1)

        if (data and data[1] == 3) or data[1] == ConnectionManager.ConnectionManager.ANY_STATUS:
            if data[0] == "UPDATE":
                set_cards(map_cards(ConnectionManager.decode_object(data[2])), labels)

            elif data[0] == "WINNER":
                winner = data[2][0]
                data[2].pop(0)
                show_winner(winner, "wins the ROUND")
                set_cards(map_cards(ConnectionManager.decode_object(data[2])), labels)

            elif data[0] == "GAMER":
                winner = data[2][0]
                data[2].pop(0)
                show_winner(winner, "is winner of the GAME")
                set_cards(map_cards(ConnectionManager.decode_object(data[2])), labels)
                sleep(1)
                _app.frame_manager(2)

            elif data[0] == "DISCONNECTED":
                _app.frame_manager(0)

