const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en" class="js-focus-visible">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0">
<meta name="mobile-web-app-capable" content="yes" />
<meta name="apple-mobile-web-app-capable" content="yes" />

<title>Keverian Scoreboard Control</title>

  <style>
    html, body {
      margin: 0;
      padding: 0;
    }
    header:-moz-full-screen{
    display:none;
    }
    header:-moz-fullscreen{
        display:none;
    }
    header:-ms-fullscreen{
        display:none;
    }
    header:-webkit-full-screen{
        display:none;
    }
    header:-webkit-fullscreen{
        display:none;
    }
    header:fullscreen{
        display:none;
    }
    td {
      text-align: center;
      vertical-align: middle;
    }
    .btn {
      background-color: rgb(39, 39, 39);
      border: 3px solid rgb(45, 104, 93);
      border-radius: 10px; /* Adjust this value as needed */
      color: white;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-weight: bold;
      font-size: 3.5vw; 
      height: 100%;
      width: 100%;
      cursor: pointer;
    }
    .scorebtn {
      background-color: #434343;
      border: none;
      color: rgb(255, 130, 5);
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-weight: bold;
      font-size: 9vw; 
      height: 100%;
      width: 100%;
      cursor: default;
    }
    .dot {
      background-color:gray;
      height:25px;
      width:25px;
      border-radius:50%;
      border:2px solid #000;
      line-height:50px;
    }
    
  </style>

  <body style="background-color: #434343" onload="process()">
  <table style="width: 100vw; height: 100vh;">
    <tbody>
        <tr>
          <td colspan="6"><button style="color: rgb(247, 218, 100)" type="button" class="btn" onclick="StartStopTimer()"><span id="StartStopTimer">Tap to Run Timer</span><br>
          <span id= "timeDisp" style="font-size:200%">00:00</span></button></td>
        </tr>
        <tr>
          <td><button type="button" class="btn" onclick="TimeChange(-1)">-1</button></td>
          <td><button type="button" class="btn" onclick="TimeChange(-10)">-10</button></td>
          <td><button type="button" class="btn" onclick="TimeChange(-60)">-60</button></td>
          <td><button type="button" class="btn" onclick="TimeChange(60)">+60</button></td>
          <td><button type="button" class="btn" onclick="TimeChange(10)">+10</button></td>
          <td><button type="button" class="btn" onclick="TimeChange(1)">+1</button></td>
          <td></td>
        </tr>
        <tr>
          <td><button type="button" class="btn" onclick="TimeSet(12)">Set<br>12:00</button></td>
          <td><button type="button" class="btn" onclick="TimeSet(10)">Set<br>10:00</button></td>
          <td colspan="2"><button type="button" class="btn" style="color: rgb(192, 255, 239); height: 90%; width:90%;" onclick="PeriodChange()">Period<br><span id="periodDisp" style="font-size: 5vw">1</span></button></td>
          <td><button type="button" class="btn" onclick="TimeSet(2)">Set<br>2:00</button></td>
          <td><button type="button" class="btn" onclick="TimeSet(20)">Set<br>20:00</button></td>
          <td></td>
        </tr>
        <tr>
          <td colspan="2"><button type="button" class="btn" id="homePossessionDisp" style="height: 90%; width:90%; color: rgb(254, 194, 103)" onclick="PossessionSet(0)">Pos</button></td>
          <td><button type="button" class="btn" id="homeBonusDisp" style="height: 90%; width:90%; color: rgb(253, 255, 192)" onclick="HomeBonus()">Bonus</button></td>
          <td><button type="button" class="btn" id="visitorBonusDisp" style="height: 90%; width:90%; color: rgb(253, 255, 192)" onclick="VisitorBonus()">Bonus</button></td>
          <td colspan="2"><button type="button" class="btn" id="visitorPossessionDisp" style="height: 90%; width:90%; color: rgb(254, 194, 103)" onclick="PossessionSet(1)">Pos</button></td>
        </tr>
        <tr>
          <td><button type="button" class="btn" style="color: pink" onclick="HomeScoreChange(2)">Score<br>+ 2</button></td>
          <td colspan="4"><button type="button" class="scorebtn"><span id="ScoreDisp">00 - 00</span></button></td>
          <td><button type="button" class="btn" style="color: pink" onclick="VisitorScoreChange(2)">Score<br>+ 2</button></td>
        </tr>
        <tr>              
          <td><button type="button" class="btn" style="color: pink" onclick="HomeScoreChange(1)">Score<br>+ 1</button></td>
          <td><button type="button" class="btn" style="color: pink" onclick="HomeScoreChange(3)">Score<br>+ 3</button></td>
          <td colspan="2" rowspan="2"><button type="button" class="btn" style="color: rgb(192, 255, 239); height: 50%; width:80%;" onclick="Buzzer()">Buzzer</button></td>
          <td><button type="button" class="btn" style="color: pink" onclick="VisitorScoreChange(3)">Score<br>+ 3</button></td>
          <td><button type="button" class="btn" style="color: pink" onclick="VisitorScoreChange(1)">Score<br>+ 1</button></td>
        </tr>
        <td><button type="button" class="btn" style="color: pink" onclick="HomeScoreChange(-1)">Score<br> - 1</button></td>
        <td><button type="button" class="btn" style="color: pink" onclick="HomeScoreSet(0)">Reset<br>0</button></td>
        <td><button type="button" class="btn" style="color: pink" onclick="VisitorScoreSet(0)">Reset<br>0</button></td>
        <td><button type="button" class="btn" style="color: pink" onclick="VisitorScoreChange(-1)">Score<br>- 1</button></td>
      </tr>
    </tbody>
  </table>
  <script type = "text/javascript">
  
    // global variable visible to all java functions
    var xmlHttp=createXmlHttpObject();

    // function to create XML object
    function createXmlHttpObject(){
      if(window.XMLHttpRequest){
        xmlHttp=new XMLHttpRequest();
      }
      else{
        xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
      }
      return xmlHttp;
    }

    function HomeScoreChange(amt) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "HomeScore?change="+amt, false);
      xhttp.send();
    }
    function HomeScoreSet(val) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "HomeScore?set="+val, false);
      xhttp.send();
    }
    function VisitorScoreChange(amt) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "VisitorScore?change="+amt, false);
      xhttp.send();
    }
    function VisitorScoreSet(val) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "VisitorScore?set="+val, false);
      xhttp.send();
    }
    function TimeChange(amt) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "Time?change="+amt, false);
      xhttp.send();
    }
    function TimeSet(val) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "Time?set="+val, false);
      xhttp.send();
    }
    function PossessionSet(val) { // 0=Home; 1=Visitors
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "Possession?set="+val, false);
      xhttp.send();
    }
    function PeriodChange() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "PeriodChange", false);
      xhttp.send();
    }
    function HomeBonus() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "HomeBonus", false);
      xhttp.send();
    }
    function VisitorBonus() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "VisitorBonus", false);
      xhttp.send();
    }
    function StartStopTimer() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "StartStopTimer", false);
      xhttp.send();
    }
    
    function Buzzer() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "Buzzer", false);
      xhttp.send();
    }

    // function to handle the response from the ESP
    function response(){
      var message;
      var xmlResponse;
      var xmldoc;
      buttonOffColor = "rgb(39, 39, 39)";
      posOnColor = "rgb(93, 76, 52)";
      bonusOnColor = "rgb(66, 78, 37)";

      // get the xml stream
      xmlResponse=xmlHttp.responseXML;
  
      xmldoc = xmlResponse.getElementsByTagName("Time");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("timeDisp").innerHTML=message;      

      xmldoc = xmlResponse.getElementsByTagName("Score");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("ScoreDisp").innerHTML=message;      

      xmldoc = xmlResponse.getElementsByTagName("Period");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("periodDisp").innerHTML=message;

      xmldoc = xmlResponse.getElementsByTagName("Pos");
      message = xmldoc[0].firstChild.nodeValue;
      if (message == 0) {
        document.getElementById("homePossessionDisp").style.backgroundColor=posOnColor;
        document.getElementById("visitorPossessionDisp").style.backgroundColor=buttonOffColor; 
      }
      else {
        document.getElementById("homePossessionDisp").style.backgroundColor=buttonOffColor;
        document.getElementById("visitorPossessionDisp").style.backgroundColor=posOnColor;
      }

      xmldoc = xmlResponse.getElementsByTagName("HomeBonus");
      message = xmldoc[0].firstChild.nodeValue;
      if (message == 1) {
        document.getElementById("homeBonusDisp").style.backgroundColor=bonusOnColor;
      }
      else {
        document.getElementById("homeBonusDisp").style.backgroundColor=buttonOffColor;
      }
            xmldoc = xmlResponse.getElementsByTagName("VisitorBonus");
      message = xmldoc[0].firstChild.nodeValue;
      if (message == 1){
        document.getElementById("visitorBonusDisp").style.backgroundColor=bonusOnColor; 
      }
      else {
        document.getElementById("visitorBonusDisp").style.backgroundColor=buttonOffColor; 
      }
      xmldoc = xmlResponse.getElementsByTagName("TimerRunning");
      message = xmldoc[0].firstChild.nodeValue;
      if (message == 1){
        document.getElementById("StartStopTimer").innerHTML="Tap to Pause Timer";
      }
      else {
        document.getElementById("StartStopTimer").innerHTML="Tap to Run Timer";
      }
    }
  
    // general processing code for the web page to ask for an XML steam
    // this is actually why you need to keep sending data to the page to 
    // force this call with stuff like this
    // server.on("/xml", SendXML);
    // otherwise the page will not request XML from the ESP, and updates will not happen
    function process(){
      if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
        xmlHttp.open("PUT","xml",true);
        xmlHttp.onreadystatechange=response;
        xmlHttp.send(null);
      }       
        // you may have to play with this value, big pages need more processing time, and hence
        // a longer timeout
        setTimeout("process()", 200);
    }
  </script>
</html>
)=====";