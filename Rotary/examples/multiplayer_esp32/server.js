const PORT = process.env.PORT || 5000
const FRAME_RATE = 1000 / 60

// dependencies
const express = require('express');
const http = require('http');
const path = require('path');
const socketIO = require('socket.io'); 

const app = express();
const server = http.Server(app);
const io = socketIO(server);

let game = require('./server/game');

app.set('port', PORT);
app.use('/client', express.static(__dirname + '/client')); // routing
// app.use('/client', express.static(path.join(__dirname, '/client')))

// routing
app.get('/', function(request, response){
    response.sendFile(path.join(__dirname, 'index.html'));   
}); 

require('dns').lookup(require('os').hostname(), function (err, add, fam) {
    console.log('addr: ' + add);
});

let g = new game();

io.on('connection', socket => {
    
    socket.on('new esp32', user => {
        g.add_player(user, socket);                
        io.sockets.emit('new esp', socket.id);
        console.log(`esp with id ${socket.id} is connected`);
    });

    socket.on('new browser', () => {
        g.add_browser(socket)
        g.send_init_state(socket.id)
    })

    socket.on('esp encoders', data => {
        g.update_player_on_input(socket.id, data)      
    });
    
    socket.on('disconnect', () => {
        g.remove_player(socket.id);
        io.sockets.emit('esp disconnect', socket.id);
        console.log(`esp with id ${socket.id} is disconnected`);
    });
   
});

setInterval(() => {
    g.send_state();
}, FRAME_RATE);

// starts the server.
server.listen(PORT, () => {
   // eslint-disable-next-line no-console
   console.log(`Starting server on port ${PORT}`)
})

