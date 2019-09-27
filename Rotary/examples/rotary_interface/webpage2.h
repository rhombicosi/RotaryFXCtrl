R"=====(
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>RotaryFXCtrl</title>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/p5.js/0.7.2/p5.min.js'></script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/p5.js/0.7.2/addons/p5.dom.min.js'></script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/three.js/108/three.min.js'></script>
  <style type="text/css">
    body {
      margin: 0;
      padding: 0;
      background: black;
      color: white;
      line-height: 100%;
      font-size: 1rem;
    }
    h1 {
        font-size: 1em;
        font-family: sans-serif;
        color: gray;
    }
    .rTable {
      display: table;
      width: 100%;
    }
    .rTableRow {
      display: table-row;
    }
    .rTableHeading {
      display: table-header-group;
      background-color: #ddd;
      font-weight: bold;
    }
    .rTableBody {
      display: table-row-group;
    }
    .rTableFoot {
      display: table-footer-group;
      background-color: #ddd;
      font-weight: bold;
    }
    .rTableCell, .rTableHead {
      display: table-cell;
      padding: 3px 10px;
      border: 1px solid #999999;
    }
  </style>
</head>
<body>
  <!DOCTYPE html>
  <html>
  	<head>
  		<meta charset=utf-8>
  		<title>My first three.js app</title>
  		<style>
  			body { margin: 0; }
  			canvas { width: 100%; height: 100% }
  		</style>
  	</head>
  	<body>
  		<script src="js/three.js"></script>
  		<script>
  			// Our Javascript will go here.
  		</script>
  	</body>
  </html>
  <script>
    const webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');

    /*
     * arrays for storing encoders
    */
    let num = 4;
    let counters = [];
    let angles = [];
    let periods = [];

    /*
    * encoder web interface
    */
    let coswave = [];
    let outsideRadius = 150;
    let insideRadius = 100;

    webSocket.onmessage = function(event) {
      let data = JSON.parse(event.data);
      for (let i = 0; i < num; i++) {
        counters[i] = data.encoders[i].counter;
        angles[i] = data.encoders[i].angle;
        periods[i] = data.encoders[i].period;
//        document.getElementById(`counter${i+1}`).innerHTML  = data.encoders[i].counter;
//        document.getElementById(`angle${i+1}`).innerHTML  = data.encoders[i].angle;
//        document.getElementById(`period${i+1}`).innerHTML  = data.encoders[i].period;
      }
    }

    function setup(){
      let cnv = createCanvas(windowWidth, windowHeight, WEBGL);
      cnv.style('position', 'absolute');
      cnv.style('top', '0');
      cnv.style('left', '0');
      cnv.style('width', '100%');
      cnv.style('height', '100%');
      cnv.style('z-index', '-1');
      cnv.parent('3d');

      for (let i = 0; i < width; i++) {
        let amount = map(i, 0, width, 0, PI);
        coswave[i] = abs(cos(amount));
      }
    }
    
  </script>
</body>
</html>
)====="
