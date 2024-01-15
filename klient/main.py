import queue
import threading

import customtkinter as c

from App import App

main_font = "System"


def main():
    c.set_appearance_mode("dark")

    data_queue = queue.Queue()
    app = App(data_queue, "ups Client", f"800x500+450+80")
    communication_thread = threading.Thread(target=app.connection_manager.start_listening)
    communication_thread.daemon = True
    communication_thread.start()

    app.frame_manager(0)
    app.start()


if __name__ == "__main__":
    main()
