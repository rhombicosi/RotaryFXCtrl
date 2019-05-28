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
<body onload="javascript:init()">
  <h1>RotaryFXCtrl</h1>
  <div class="rTable">
  <div class="rTableRow">
  <div class="rTableHead"><strong>Encoder</strong></div>
  <div class="rTableHead"><span>Counter</span></div>
  <div class="rTableHead"><span>Angle</span></div>
  <div class="rTableHead"><span>Period</span></div>
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

  <script>
   let webSocket;

   function init() {
     webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');

     webSocket.onmessage = function(event) {
        var data = JSON.parse(event.data);                    
        console.log(data.encoders);
        for (let i = 0; i < 2; i++) {
          document.getElementById(`counter${i+1}`).innerHTML  = data.encoders[i].counter; 
          document.getElementById(`angle${i+1}`).innerHTML  = data.encoders[i].angle; 
          document.getElementById(`period${i+1}`).innerHTML  = data.encoders[i].period; 
        }
     }    
   }   
  </script>
</body>
</html>
)====="
