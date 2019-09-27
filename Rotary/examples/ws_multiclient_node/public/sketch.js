//var sock = new WebSocket("ws://localhost:5001");
var sock = new WebSocket("ws://10.0.0.4:3000");  //replace this address with the one the node.js server prints out. keep port 3000
//var display = document.getElementById("display")

// encoders #
const num = 4;
// let counters = [];
// let angles = [];
// let periods = [];

// sock.onopen = () => {
//     //alert("Socket connected succesfully!!!")
//     setTimeout(function(){sock.send('connection succeeded');},1000);
//     // display.innerHTML+="connection succeeded <br />";
// };

sock.onmessage = (event) => {
    let data = JSON.parse(event.data);
    
    for (let i = 0; i < num; i++) {          
        // counters[i] = data.encoders[i].counter;
        // angles[i] = data.encoders[i].angle;
        // periods[i] = data.encoders[i].period;
        console.log(data.encoders);
        document.getElementById(`counter${i+1}`).innerHTML = data.encoders[i].counter; 
        document.getElementById(`angle${i+1}`).innerHTML = data.encoders[i].angle; 
        document.getElementById(`period${i+1}`).innerHTML = data.encoders[i].period; 
    }
}