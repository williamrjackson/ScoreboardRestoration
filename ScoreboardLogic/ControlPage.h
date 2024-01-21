const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en" class="js-focus-visible">

<title>Keverian Scoreboard Control</title>

  <style>
    html, body {
      margin: 0;
      padding: 0;
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
      background-color: #00000000;
      border: none;
      color: rgb(255, 130, 5);
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-weight: bold;
      font-size: 9vw; 
      height: 100%;
      width: 100%;
      cursor: pointer;
    }
    .score {
      color: rgb(180, 87, 44);
      font-family: 'Courier New', monospace;
      font-weight:bold;
      /* font-size: 11vw;  */
      text-align: center;
      vertical-align: middle;
      line-height: 100%;
    }
    .dot {
      background-color:gray;
      /* display:block; */
      height:25px;
      width:25px;
      border-radius:50%;
      border:2px solid #000;
      /* margin:auto; */
      line-height:50px;
      /* text-align:center */
    }
    
  </style>

  <body style="background-color: #434343" onload="process()">
  <table style="width: 100vw; height: 100vh;">
    <tbody>
        <tr>
          <td colspan="6"><button style="color: rgb(247, 218, 100)" type="button" class="btn" onclick="StartStopTimer()"><span id="StartStopTimer">Run Timer</span><br>
          <span id= "timeDisp" style="font-size:200%">00:00</span></button></td>
        </tr>
        <tr>
          <td><button type="button" class="btn" id="TimeDn1" onclick="TimeDn1()">-1</button></td>
          <td><button type="button" class="btn" id="TimeDn10" onclick="TimeDn10()">-10</button></td>
          <td><button type="button" class="btn" id="TimeDn60" onclick="TimeDn60()">-60</button></td>
          <td><button type="button" class="btn" id="TimeUp60" onclick="TimeUp60()">+60</button></td>
          <td><button type="button" class="btn" id="TimeUp10" onclick="TimeUp10()">+10</button></td>
          <td><button type="button" class="btn" id="TimeUp1" onclick="TimeUp1()">+1</button></td>
          <td></td>
        </tr>
        <tr>
          <td><button type="button" class="btn" id="TimerSet12" onclick="TimerSet12()">Set<br>12:00</button></td>
          <td><button type="button" class="btn" id="TimerSet10" onclick="TimerSet10()">Set<br>10:00</button></td>
          <td colspan="2"><button type="button" class="btn" id="PeriodUp" style="color: rgb(192, 255, 239); height: 90%; width:90%;" onclick="PeriodUp()">Period<br><span id="periodDisp" style="font-size: 5vw">1</span></button></td>
          <td><button type="button" class="btn" id="TimerSet2" onclick="TimerSet2()">Set<br>2:00</button></td>
          <td><button type="button" class="btn" id="TimerSet20" onclick="TimerSet20()">Set<br>20:00</button></td>
          <td></td>
        </tr>
        <tr>
          <td colspan="2"><button type="button" class="btn" id="homePosessionDisp" style="height: 90%; width:90%; color: rgb(254, 194, 103)" onclick="HomePos()">Pos</button></td>
          <td><button type="button" class="btn" id="homeBonusDisp" style="height: 90%; width:90%; color: rgb(253, 255, 192)" onclick="HomeBonus()">Bonus</button></td>
          <td><button type="button" class="btn" id="visitorBonusDisp" style="height: 90%; width:90%; color: rgb(253, 255, 192)" onclick="VisitorBonus()">Bonus</button></td>
          <td colspan="2"><button type="button" class="btn"  id="visitorPosessionDisp" style="height: 90%; width:90%; color: rgb(254, 194, 103)" onclick="VisitorPos()">Pos</button></td>
        </tr>
        <tr>
          <td><button type="button" class="btn" id="HomeScoreUp2" style="color: pink" onclick="HomeScoreUp2()">Score<br>+ 2</button></td>
          <td colspan="4"><button type="button" class="scorebtn"><span id="ScoreDisp">00 - 00</span></button></td>
          <td><button type="button" class="btn" id="VisitorScoreUp2" style="color: pink" onclick="VisitorScoreUp2()">Score<br>+ 2</button></td>
        </tr>
        <tr>              
          <td><button type="button" class="btn" id="HomeScoreUp1" style="color: pink" onclick="HomeScoreUp1()">Score<br>+ 1</button></td>
          <td><button type="button" class="btn" id="HomeScoreUp3" style="color: pink" onclick="HomeScoreUp3()">Score<br>+ 3</button></td>
          <td colspan="2" rowspan="2"><button type="button" class="btn" id="Buzzer" style="color: rgb(192, 255, 239); height: 50%; width:80%;" onclick="Buzzer()">Buzzer</button></td>
          <td><button type="button" class="btn" id="VisitorScoreUp3" style="color: pink" onclick="VisitorScoreUp3()">Score<br>+ 3</button></td>
          <td><button type="button" class="btn" id="VisitorScoreUp1" style="color: pink" onclick="VisitorScoreUp1()">Score<br>+ 1</button></td>
        </tr>
        <td><button type="button" class="btn" id="HomeScoreDown1" style="color: pink" onclick="HomeScoreDown1()">Score<br> - 1</button></td>
        <td><button type="button" class="btn" id="HomeScoreReset" style="color: pink" onclick="HomeScoreReset()">Reset<br>0</button></td>
        <td><button type="button" class="btn" id="VisitorScoreReset" style="color: pink" onclick="VisitorScoreReset()">Reset<br>0</button></td>
        <td><button type="button" class="btn" id="VisitorScoreDown1" style="color: pink" onclick="VisitorScoreDown1()">Score<br>- 1</button></td>
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

    function HomeScoreUp1() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "HomeScoreUp1", false);
      xhttp.send();
    }
    function HomeScoreUp2() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "HomeScoreUp2", false);
      xhttp.send();
    }
    function HomeScoreUp3() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "HomeScoreUp3", false);
      xhttp.send();
    }
    function HomeScoreReset() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "HomeScoreReset", false);
      xhttp.send();
    }
    function HomeScoreDown1() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "HomeScoreDown1", false);
      xhttp.send();
    }
    function PeriodUp() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "PeriodUp", false);
      xhttp.send();
    }
    function VisitorScoreDown1() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "VisitorScoreDown1", false);
      xhttp.send();
    }
    function VisitorScoreUp1() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "VisitorScoreUp1", false);
      xhttp.send();
    }
    function VisitorScoreUp2() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "VisitorScoreUp2", false);
      xhttp.send();
    }
    function VisitorScoreUp3() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "VisitorScoreUp3", false);
      xhttp.send();
    }
    function VisitorScoreReset() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "VisitorScoreReset", false);
      xhttp.send();
    }
    function HomeBonus() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "HomeBonus", false);
      xhttp.send();
    }
    function HomePos() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "HomePos", false);
      xhttp.send();
    }
    function VisitorPos() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "VisitorPos", false);
      xhttp.send();
    }
    function VisitorBonus() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "VisitorBonus", false);
      xhttp.send();
    }
    function StartStopTimer() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "StartStopTimer", false);
      xhttp.send();
    }
    function TimeDn1() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimeDn1", false);
      xhttp.send();
    }
    function TimeDn10() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimeDn10", false);
      xhttp.send();
    }
    function TimeDn60() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimeDn60", false);
      xhttp.send();
    }
    function TimeUp60() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimeUp60", false);
      xhttp.send();
    }
    function TimeUp10() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimeUp10", false);
      xhttp.send();
    }
    function TimeUp1() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimeUp1", false);
      xhttp.send();
    }
    function TimerSet12() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimerSet12", false);
      xhttp.send();
    }
    function TimerSet10() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimerSet10", false);
      xhttp.send();
    }
    function TimerSet0() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimerSet0", false);
      xhttp.send();
    }
    function TimerSet2() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimerSet2", false);
      xhttp.send();
    }
    function TimerSet20() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "TimerSet20", false);
      xhttp.send();
    }
    function Buzzer() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "Buzzer", false);
      xhttp.send();
    }

    // function to handle the response from the ESP
    function response(){
      var message;
      var xmlResponse;
      var xmldoc;
     
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
      if (message == 0){
        document.getElementById("visitorPosessionDisp").style.backgroundColor="rgb(39, 39, 39)"; 
        document.getElementById("homePosessionDisp").style.backgroundColor="rgb(93, 76, 52)";
      }
      else {
        document.getElementById("visitorPosessionDisp").style.backgroundColor="rgb(93, 76, 52)";
        document.getElementById("homePosessionDisp").style.backgroundColor="rgb(39, 39, 39)";
      }
      xmldoc = xmlResponse.getElementsByTagName("HomeBonus");
      message = xmldoc[0].firstChild.nodeValue;
      if (message == 1){
        document.getElementById("homeBonusDisp").style.backgroundColor="rgb(66, 78, 37)";
      }
      else {
        document.getElementById("homeBonusDisp").style.backgroundColor="rgb(39, 39, 39)";
      }
            xmldoc = xmlResponse.getElementsByTagName("VisitorBonus");
      message = xmldoc[0].firstChild.nodeValue;
      if (message == 1){
        document.getElementById("visitorBonusDisp").style.backgroundColor="rgb(66, 78, 37)"; 
      }
      else {
        document.getElementById("visitorBonusDisp").style.backgroundColor="rgb(39, 39, 39)"; 
      }
      xmldoc = xmlResponse.getElementsByTagName("TimerRunning");
      message = xmldoc[0].firstChild.nodeValue;
      if (message == 1){
        document.getElementById("StartStopTimer").innerHTML="Stop Timer";
      }
      else {
        document.getElementById("StartStopTimer").innerHTML="Run Timer";
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
        // you may have to play with this value, big pages need more porcessing time, and hence
        // a longer timeout
        setTimeout("process()", 200);
    }
  </script>
</html>
)=====";