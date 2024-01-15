import customtkinter as c
import ConnectionManager
from frames import Game, lobby, login, lobbyMenu


class App:

    def __init__(self, msg_queue, title=None, geometry=None):
        self.msg_queue = msg_queue
        self.connection_manager = ConnectionManager.ConnectionManager()
        self.app = c.CTk()

        if title:
            self.app.title(title)
        if geometry:
            self.app.geometry(geometry)

    def frame_manager(self, scene):
        if scene == 0:
            login.get_login(self)
        if scene == 1:
            lobbyMenu.server_rooms(self)
        if scene == 2:
            lobby.in_lobby(self)
        if scene == 3:
            Game.start_game(self)

    def get_window(self):
        child = self.app.winfo_children()

        for ch in child:
            if isinstance(ch, c.CTkFrame):
                ch.pack_forget()

        return self.app

    def get_connection_manager(self):
        return self.connection_manager

    def start(self):
        self.app.mainloop()
