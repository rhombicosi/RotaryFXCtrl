const socket = io();

socket.on('message', function(data){
    console.log(data);
});

socket.emit('new browser');

let canvas = document.getElementById('canvas');
canvas.width = 800;
canvas.height = 600;
let context = canvas.getContext('2d');

socket.on('new esp', id => {
    console.log(`new esp connected ${id}`);    
    add_row(id);
})

socket.on('esp disconnect', id => {
    console.log(`esp disconnected ${id}`);
    delete_row(id);    
})

socket.on('state update', plrs => {
    let p = JSON.parse(plrs);
    
    // for test purposes
    p.forEach((o) => console.log('game state ' +  JSON.stringify(o)));    

    context.clearRect(0, 0, 800, 600);
    
    p.forEach(o => {
        context.fillStyle = o.color;
        context.beginPath();
        context.arc(o.position.x, o.position.y, 10, 0, 2 * Math.PI);
        context.fill();
    }) 

    p.forEach(o => {
        document.getElementById(`counter_${o.socketid}`).innerHTML = o.state[0].counter; 
        document.getElementById(`angle_${o.socketid}`).innerHTML = o.state[0].angle; 
        document.getElementById(`period_${o.socketid}`).innerHTML = o.state[0].period; 
    });
})

socket.on('state init', plrs => {
    let p = JSON.parse(plrs);
    
    p.forEach((o) => {
        // for test purposes
        // console.log('state init ' +  JSON.stringify(o.socketid));
        add_row(o.socketid)
    });
})

function add_row(socketid) {
    let table = document.getElementsByClassName("rTable");

    let newdiv = document.createElement("div");
    newdiv.className += "rTableRow";
    newdiv.setAttribute("id",`${socketid}`);

    let cell0 = document.createElement("div");
    cell0.className += "rTableCell";
    cell0.innerHTML = `${socketid}`;
    let a0 = document.createElement("a");
    a0.setAttribute("id", `name_${socketid}`);
    cell0.appendChild(a0);
     
    let cell1 = document.createElement("div");
    cell1.className += "rTableCell";
    let a1 = document.createElement("a");
    a1.setAttribute("id", `counter_${socketid}`);
    cell1.appendChild(a1);
    
    let cell2 = document.createElement("div");
    cell2.className += "rTableCell";
    let a2 = document.createElement("a");
    a2.setAttribute("id", `angle_${socketid}`);
    cell2.appendChild(a2);

    let cell3 = document.createElement("div");
    cell3.className += "rTableCell";
    let a3 = document.createElement("a");
    a3.setAttribute("id", `period_${socketid}`);
    cell3.appendChild(a3);

    newdiv.appendChild(cell0);
    newdiv.appendChild(cell1);
    newdiv.appendChild(cell2);
    newdiv.appendChild(cell3);

    table[0].appendChild(newdiv);
}

function delete_row(socketID) {
    const row = document.getElementById(`${socketID}`);
    if(row != null) {
        while (row.firstChild) {
            row.removeChild(row.firstChild);
        }
    }
}

function display_rows(p) {
    p.forEach(o => {
        document.getElementById(`counter_${o.socketid}`).innerHTML = o.state[0].counter; 
        document.getElementById(`angle_${o.socketid}`).innerHTML = o.state[0].angle; 
        document.getElementById(`period_${o.socketid}`).innerHTML = o.state[0].period; 
    });
}