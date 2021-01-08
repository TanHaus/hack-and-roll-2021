const express = require('express');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);
const port = process.env.PORT || 3000;

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}  

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/index.html');
});

app.post('/', (req, res) => {
  const body = req.body;
  console.log(body);
  return res.json(body);
})

io.on('connection', async (socket) => {
  console.log('A user connected');

  socket.on('disconnect', () => {
    console.log('User disconnected');
  });

  // let i = 0;
  // while (true) {
  //   io.emit('data', i++);
  //   await sleep(1000);
  // }
  
  socket.on('reading', (data) => {
    console.log('reading: ' + data);
    io.emit('data', data);
  });
});

http.listen(port, () => {
  console.log(`Server running at http://localhost:${port}/`);
});