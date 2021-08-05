# import socketio
# from time import sleep
# import ast
#
# ONLINE_CLIENTS = []
# OFFLINE_CLIENTS = []
#
# sio = socketio.Client()
# responded = False
# current_client = None
#
#
# @sio.on('response')
# def on_message(val):
#     print("Response: \n", val)
#     global responded
#     responded = True
#
#
# @sio.on('response')
# def on_message(val):
#     print("Response: \n", val)
#     global responded
#     responded = True
#
#
# @sio.on('error')
# def on_message(val):
#     print("Error: \n", val)
#
#
# @sio.on('info')
# def on_message(val):
#     print("Info: \n", val)
#
#
# def set_client():
#     global current_client
#     result = input("Set shell [-1] >> ")
#     current_client = int(result) - 1
#
#
# def on_clients(clients):
#     global ONLINE_CLIENTS, responded
#     ONLINE_CLIENTS = clients
#     print("Current Clients: ")
#     for idx, client in enumerate(ONLINE_CLIENTS):
#         print(f"    [{idx + 1}]", client["username"])
#     print()
#     set_client()
#     responded = True
#
#
# def get_shell_input():
#     global current_client
#     if current_client is not None:
#         data = input(ONLINE_CLIENTS[current_client]["username"] + " >> ")
#         return data
#     else:
#         data = input(" >> ")
#         return data
#
#
# sio.connect('http://localhost:3000')
# sio.emit("get_clients", callback=on_clients)
#
# while True:
#     while not responded:
#         sleep(0.5)
#
#     try:
#         data = get_shell_input()
#         sio.emit("command", ast.literal_eval(data))
#         responded = False
#     except:
#         responded = False
#         continue

from AdminShell import AdminShell

shell = AdminShell("http://localhost:3000")
shell.run()
