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
    })

    socket.on("response", (data) => {
        socket.broadcast.emit("response", data)
    })

    socket.on("error", (options) => {
        console.log("Error: ", options)
        socket.broadcast.emit("error", options)
    })

    socket.on("info", (options) => {
        console.log("Info: ", options)
        socket.broadcast.emit("info", options)
    })

    socket.on("frame", (imageBuffer) => {
        socket.broadcast.emit("frame", imageBuffer.toString("base64"));
    })

    // --------------- PANEL FUNCTIONS ---------------
    socket.on("command", (options, callback) => {
        allClients.forEach((s) => {
            if (s.clientData["id"] === options["id"]) {
                s.emit("command", options, (data) => {
                    if (callback)
                        callback(data)
                })
            }
        })
    })

    socket.on("get_data", (ack) => {
        if (ack) {
            let dataList = []
            allClients.forEach((val) => {
                dataList.push(val.clientData)
            })
            ack(dataList)
        }
    })

    socket.emit("command", {event: "greet"})
});


// --------------- START SERVER ---------------
http.listen(port, () => {
    console.log(`Socket.IO server running at http://localhost:${port}/`);
});