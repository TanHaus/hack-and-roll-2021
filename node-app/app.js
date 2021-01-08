const express = require('express');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);
const port = process.env.PORT || 3000;

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.use("/", express.static('public'));

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/index.html');
});

app.post('/', (req, res) => {
  const body = req.body;
  console.log(body);
  io.emit('data', body);
  return res.json(body);
})

io.on('connection', async (socket) => {
  console.log('A user connected');

  socket.on('disconnect', () => {
    console.log('User disconnected');
  });

  socket.on('client', () => {
    socket.join('client');
  })

  socket.on('sensor', (payload) => {
    console.log(payload)
    io.to('client').emit('data', payload)
  })
});

http.listen(port, () => {
  console.log(`Server running at http://localhost:${port}/`);
});