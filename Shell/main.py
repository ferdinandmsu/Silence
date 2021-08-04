import socketio
from time import sleep
import ast

# standard Python
sio = socketio.Client()


@sio.on('response')
def on_message(val):
    print("Response: \n", val)


@sio.event
def connect():
    print("I'm connected!")


@sio.event
def connect_error():
    print("The connection failed!")


@sio.event
def disconnect():
    print("I'm disconnected!")


sio.connect('http://localhost:3000')
while True:
    data = input("data >> ")
    sio.emit("command", ast.literal_eval(data))
    sleep(5)
