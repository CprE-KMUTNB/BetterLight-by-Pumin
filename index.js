const express = require('express')
const app = express()
const path = require('path')
const WebSocket = require('ws')
const port = 3000

app.get('/', (req, res) => {
  res.sendFile('index.html', { root: path.join(__dirname, '/public') });
})

app.use(express.static(path.join(__dirname, 'public')));

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
})

// Received data from outside
const sockserver = new WebSocket.Server({ port: 443 })
sockserver.on('connection', ws => {
  console.log('New client connected to port 443!')
  ws.on('close', () => console.log('Client has disconnected from port 443!'))
  ws.on('message', data => {
    sockserver.clients.forEach(client => {
      // console.log(`Received: ${data}`)
      client.send(`${data}`)
    })
  })
  ws.onerror = function () {
    console.log('websocket error')
  }
})