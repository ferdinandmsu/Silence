"use strict"

// --------------- INCLUDES ---------------
const express = require('express');
const app = express()
const http = require('http').Server(app);
const io = require('socket.io')(http);
const fs = require("fs")
const fileUpload = require('express-fileupload');

// --------------- VARIABLES ---------------
const port = process.env.PORT || 3000;
let allClients = []

app.use(fileUpload({}));
app.use("/", express.static(__dirname + "/panel"))

// --------------- ROUTES ---------------
app.post('/upload', function (req, res) {
    let uploadFile;
    let uploadPath;

    if (!req.files || Object.keys(req.files).length === 0) {
        res.status(400).send('No files were uploaded.');
        return;
    }

    console.log('req.files >>>', req.files); // eslint-disable-line

    uploadFile = req.files.uploadFile;
    uploadPath = __dirname + '/panel/uploads/' + uploadFile.name;

    uploadFile.mv(uploadPath, function (err) {
        if (err) {
            return res.status(500).send(err);
        }

        res.send('File uploaded to ' + uploadPath);
    });
});

// --------------- SOCKIO CONNECTION ---------------
io.on('connection', (socket) => {
    let addedClient = false

    // --------------- CLIENT FUNCTIONS ---------------
    socket.on("add_client", (options) => {
        if (addedClient) return

        socket.clientData = Object.assign(options, {id: allClients.length})
        addedClient = true
        allClients.push(socket)

        socket.emit("command", {event: "upload", file: "./test.jpg"})
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