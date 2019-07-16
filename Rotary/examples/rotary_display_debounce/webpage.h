R"=====(
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>RotaryFXCtrl</title>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/p5.js/0.7.2/p5.min.js'></script>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/p5.js/0.7.2/addons/p5.dom.min.js'></script> 
  <script src='https://cdnjs.cloudflare.com/ajax/libs/smoothie/1.32.0/smoothie.min.js'></script> 
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
  <h1>Rotary FX Ctrl</h1>
  <div class="rTable">
    <div class="rTableRow">
      <div class="rTableHead"><strong>Encoder</strong></div>
      <div class="rTableHead"><strong>Counter</strong></div>
      <div class="rTableHead"><strong>Angle</strong></div>
      <div class="rTableHead"><strong>Period</strong></div>
    </div>
    <div class="rTableRow">
      <div class="rTableCell">1</div>
      <div class="rTableCell"><a id="counter1"></a></div>
      <div class="rTableCell"><a id="angle1"></a></div>
      <div class="rTableCell"><a id="period1"></a></div>
    </div>
    <div class="rTableRow">
      <div class="rTableCell">2</div>
      <div class="rTableCell"><a id="counter2"></a></div>
      <div class="rTableCell"><a id="angle2"></a></div>
      <div class="rTableCell"><a id="period2"></a></div>
    </div>
  </div>
  
  <div id="3d"></div>
  <script>
    let webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');

    /*
     * arrays for storing encoders
    */
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
      for (let i = 0; i < 2; i++) {
        counters[i] = data.encoders[i].counter;
        angles[i] = data.encoders[i].angle;
        periods[i] = data.encoders[i].period;
        document.getElementById(`counter${i+1}`).innerHTML  = data.encoders[i].counter; 
        document.getElementById(`angle${i+1}`).innerHTML  = data.encoders[i].angle; 
        document.getElementById(`period${i+1}`).innerHTML  = data.encoders[i].period; 
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
    
    function draw(){    
      background(0);
      stroke(0,0,255,195);
      fill(0,0,255,100);

      rotateY(angles[0] * 0.01);

      for (let j = 0; j < 5; j++) {        
        push();
        for (let i = 0; i < 80; i++) {
          translate(
            sin(frameCount * 0.001 + j) * 100,
            sin(frameCount * 0.001 + j) * 100,
            i * 0.1
          );
          rotateZ(angles[1] * 0.002);
          push();
          sphere(8, 6, 4);
          pop();
        }
        pop();
      }
    }
  </script>
</body>
</html>
)====="
