from tkinter import ttk

import customtkinter as c
import ConnectionManager

main_font = "System"
connection_manager = None
_table = None
_app = None


def fill_table(table, table_data):
    if len(table.get_children()) > 0 and not table_data:
        print("[Menu] will not fill lobby with", table_data)
        return

    table.delete(*table.get_children())

    print("filling lobbies with: ", table_data)

    for name, leader, players in table_data:
        table.insert('', index='end', iid=name, values=(name, leader, players))


def on_double_click(event, table, app):
    item_id = table.focus()
    if len(item_id) == 0:
        return

    if connection_manager.join_lobby(item_id):
        print("lobby " + item_id + " joined")
        app.frame_manager(2)

    else:
        print("cant join lobby " + item_id)


def notify(data):
    global _table, _app
    if (data and data[1] == 1) or data[1] == ConnectionManager.ConnectionManager.ANY_STATUS:
        if data[0] == "UPDATE":
            fill_table(_table, ConnectionManager.decode_object(data[2]))
        if data[0] == "DISCONNECTED":
            _app.frame_manager(0)


def server_rooms(app):
    global connection_manager, _table, _app
    window = app.get_window()
    _app = app
    connection_manager = app.get_connection_manager()
    connection_manager.make_observer(notify)

    frame = c.CTkFrame(master=window)
    frame.pack(fill='both', expand=True, pady=20, padx=50)

    table = ttk.Treeview(master=frame, height=25, columns=('name', 'leader', 'players'), show='headings')
    table.heading('name', text="Room name")
    table.heading('leader', text="Leader")
    table.heading('players', text="Players")
    table.pack(fill='both', expand=True)

    table.bind("<Double-1>", lambda event: on_double_click(event, table, app))

    style = ttk.Style()
    style.configure("Custom.Treeview", font=(main_font, 15))
    table.configure(style="Custom.Treeview")

    _table = table
    table_data = connection_manager.get_lobbies()
    fill_table(table, table_data)
