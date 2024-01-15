from tkinter import ttk
from typing import Optional

import customtkinter as c

import ConnectionManager

main_font = "System"

connection_manager = None
_in_count_label = None
_button_start = None
_table = None
_app = None


def leave_action(app):
    connection_manager.leave_lobby()
    app.frame_manager(1)


def start_game_action(app):
    data = connection_manager.start_new_game()
    if data[0] == "OK":
        app.frame_manager(3)


def set_buttons(buttons_frame, lobby_info, app):
    in_count = 0
    button_state = "disabled"
    if lobby_info:
        in_count = len(lobby_info) - 1

        if lobby_info[0] == "leader" and in_count > 1:
            button_state = "normal"
        else:
            button_state = "disabled"

    in_count_label = c.CTkLabel(buttons_frame, text=f"{in_count}/4", padx=10, pady=5)

    button_start = c.CTkButton(buttons_frame, text="start Game", fg_color="#00690d",
                               hover_color="#00ab15", state=button_state,
                               command=lambda: start_game_action(app))

    button_leave = c.CTkButton(buttons_frame, text="X", width=30, fg_color="#c40d00", hover_color="#fc1100",
                               command=lambda: leave_action(app))

    global _in_count_label, _button_start
    _in_count_label = in_count_label
    _button_start = button_start
    in_count_label.pack(side=c.LEFT)
    button_start.place(relx=0.5, rely=0.5, anchor=c.CENTER)
    button_leave.pack(side=c.RIGHT)


def show_players(lobby_frame, lobby_info):
    table = ttk.Treeview(master=lobby_frame, height=25, columns='players', show='headings')
    table.heading('players', text="Players in this lobby")
    table.pack(fill='both', expand=True)

    style = ttk.Style()
    style.configure("Custom.Treeview", font=(main_font, 15))
    table.configure(style="Custom.Treeview")

    global _table
    _table = table
    fill_table(table, lobby_info)


def fill_table(table, lobby_info):
    table.delete(*table.get_children())

    for name in lobby_info:
        table.insert('', index='end', iid=name, values=name)


def notify(data):
    global _app
    if data:
        if data[0] == "START":
            print("[Lobby] got notification about new game start")
            _app.frame_manager(3)

        elif data[1] == 2 or data[1] == ConnectionManager.ConnectionManager.ANY_STATUS:
            if data[0] == "UPDATE":
                update(data[2])

            elif data[0] == "DISCONNECTED":
                _app.frame_manager(0)
        else:
            print("[game] cant read notification")


def update(lobby_info):
    global connection_manager, _button_start, _in_count_label, _table

    in_count = len(lobby_info) - 1

    if lobby_info[0] == "leader" and in_count > 1:
        button_state = "normal"
    else:
        button_state = "disabled"

    lobby_info.pop(0)
    _button_start.configure(state=button_state)
    _in_count_label.configure(text=f"{in_count}/4")
    fill_table(_table, lobby_info)


def in_lobby(app):
    window = app.get_window()
    global connection_manager, _app
    _app = app
    connection_manager = app.get_connection_manager()
    connection_manager.make_observer(notify)

    lobby_info = connection_manager.get_lobby_info()

    buttons_frame = c.CTkFrame(master=window, fg_color='transparent')
    lobby_frame = c.CTkFrame(master=window)

    buttons_frame.pack(fill='x', expand=True, pady=0, padx=50)
    lobby_frame.pack(fill='both', expand=True, pady=20, padx=50)

    set_buttons(buttons_frame, lobby_info, app)
    lobby_info.pop(0)
    show_players(lobby_frame, lobby_info)
