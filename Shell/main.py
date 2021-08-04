import socketio
from time import sleep
import ast

# standard Python
sio = socketio.Client()
responded = True


@sio.on('response')
def on_message(val):
    print("Response: \n", val)
    global responded
    responded = True


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
    while not responded:
        sleep(0.5)

    try:
        data = input("data >> ")
        sio.emit("command", ast.literal_eval(data))
        responded = False
    except:
        responded = False
        continue
