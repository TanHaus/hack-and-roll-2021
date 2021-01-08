const express = require('express');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.get('/', (req, res) => {
  return res.send("hello world")
});

app.post('/', (req, res) => {
  const body = req.body;
  console.log(body);
  return res.json(body);
})

io.on('connection', (socket) => {
  console.log('a user connected');
  socket.on('disconnect', () => {
    console.log('user disconnected');
  });
  socket.on('reading', (data) => {
    console.log('reading: ' + data);
  });
});

http.listen(3000, () => {
  console.log('listening on *:3000');
});