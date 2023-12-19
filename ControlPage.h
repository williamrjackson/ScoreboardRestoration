/*


  OK, ya ready for some fun? HTML + CSS styling + javascript all in and undebuggable environment

  one trick I've learned to how to debug HTML and CSS code.

  get all your HTML code (from html to /html) and past it into this test site
  muck with the HTML and CSS code until it's what you want
  https://www.w3schools.com/html/tryit.asp?filename=tryhtml_intro

  No clue how to debug javascrip other that write, compile, upload, refresh, guess, repeat

  I'm using class designators to set styles and id's for data updating
  for example:
  the CSS class .tabledata defines with the cell will look like
  <td><div class="tabledata" id = "switch"></div></td>

  the XML code will update the data where id = "switch"
  java script then uses getElementById
  document.getElementById("switch").innerHTML="Switch is OFF";


  .. now you can have the class define the look AND the class update the content, but you will then need
  a class for every data field that must be updated, here's what that will look like
  <td><div class="switch"></div></td>

  the XML code will update the data where class = "switch"
  java script then uses getElementsByClassName
  document.getElementsByClassName("switch")[0].style.color=text_color;


  the main general sections of a web page are the following and used here

  <html>
    <style>
    // dump CSS style stuff in here
    </style>
    <body>
      <header>
      // put header code for cute banners here
      </header>
      <main>
      // the buld of your web page contents
      </main>
      <footer>
      // put cute footer (c) 2021 xyz inc type thing
      </footer>
    </body>
    <script>
    // you java code between these tags
    </script>
  </html>


*/

// note R"KEYWORD( html page code )KEYWORD"; 
// again I hate strings, so char is it and this method let's us write naturally

const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en" class="js-focus-visible">

<title>Keverian Scoreboard Control</title>

  <style>
    .btn {
      background-color: #444444;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 4px 2px;
      cursor: pointer;
    }
    .cell {
      width: 10%; 
      background-color: rgb(47, 66, 65); 
      border: none rgb(0, 0, 0);
      align-items: center;
      align-content: center;
    }
    .dot {
      background-color:gray;
      display:block;
      height:25px;
      width:25px;
      border-radius:50%;
      border:2px solid #000;
      margin:auto;
      line-height:50px;
      text-align:center
    }
    
  </style>

  <body style="background-color: #efefef" onload="process()">
    <table style="width: 100%; border-collapse: collapse; border: none rgb(0, 0, 0);">
    <tbody>
      <tr>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td style="width: 38.4331%;" class="cell" colspan="2">
          <div style="text-align: center;"><span style="font-family: Tahoma, Geneva, sans-serif; color: rgb(239, 239, 239);">Timer</span></div>
        </td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
      </tr>
      <tr>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td style="width: 19.9557%;" class="cell" colspan="2" rowspan="2">
          <div style="text-align: center;"><span id= "timeDisp" style='font-family: "Lucida Sans Unicode", "Lucida Grande", sans-serif; font-size: 60px; color: rgb(247, 218, 100);'>02:00</span></div>
        </td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
      </tr>
      <tr>
        <td class="cell"><br></td>
        <td style="width: 19.9557%;" class="cell" colspan="2">
          <div style="text-align: center;"><span style="font-family: Tahoma, Geneva, sans-serif; color: rgb(239, 239, 239);">Home Score</span></div>
        </td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td style="width: 19.9557%;" class="cell" colspan="2">
          <div style="text-align: center;"><span style="font-family: Tahoma, Geneva, sans-serif; color: rgb(239, 239, 239);">Visitor Score</span></div>
        </td>
        <td class="cell"><br></td>
      </tr>
      <tr>
        <td class="cell">
          <div style="text-align: center;"><span style="font-family: Tahoma, Geneva, sans-serif; color: rgb(239, 239, 239);">Bonus</span></div>
        </td>
        <td style="width: 19.9557%;" class="cell" colspan="2" rowspan="3">
          <div style="text-align: center;"><span id="homeScoreDisp" style='font-size: 72px; font-family: "Lucida Sans Unicode", "Lucida Grande", sans-serif; color: rgb(235, 107, 86);'>109</span></div>
        </td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td style="width: 19.9557%;" class="cell" colspan="2" rowspan="3">
          <div style="text-align: center;"><span id="visitorScoreDisp" style="font-family: 'Lucida Sans Unicode', 'Lucida Grande', sans-serif;"><span style="font-size: 72px; color: rgb(235, 107, 86);">90</span></span></div>
        </td>
        <td class="cell">
          <div style="text-align: center;"><span style="font-family: Tahoma, Geneva, sans-serif; color: rgb(239, 239, 239);">Bonus</span></div>
        </td>
      </tr>
      <tr>
        <td class="cell">
          <div id="homeBonusDisp" class="dot" style="background-color:Yellow"><br>
        </td>
        <td class="cell">
          <div style="text-align: center;"><span style="font-family: Tahoma, Geneva, sans-serif; color: rgb(239, 239, 239);">Posession</span></div>
        </td>
        <td style="width: 19.9557%;" class="cell" colspan="2">
          <div style="text-align: center;"><span style="font-family: Tahoma, Geneva, sans-serif; color: rgb(239, 239, 239);">Period</span></div>
        </td>
        <td class="cell">
          <div style="text-align: center;"><span style="font-family: Tahoma, Geneva, sans-serif; color: rgb(239, 239, 239);">Posession</span></div>
        </td>
        <td class="cell">
          <div id="visitorBonusDisp" class="dot"><br>
        </td>
      </tr>
      <tr>
        <td class="cell"><br></td>
        <td class="cell">
          <div id="homePosessionDisp" class="dot"><br>
        </td>
        <td style="width: 19.9557%;" class="cell" colspan="2" rowspan="2">
          <div style="text-align: center;"><span id="periodDisp" style='font-size: 48px; font-family: "Lucida Sans Unicode", "Lucida Grande", sans-serif; color: rgb(97, 189, 109);'>4</span></div>
        </td>
        <td class="cell">
          <div id="visitorPosessionDisp" class="dot" style="background-color:Orange"><br>
        </td>
        <td class="cell"><br></td>
      </tr>
      <tr>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
        <td class="cell"><br></td>
      </tr>
    </tbody>
  </table>
  <table style="width: 100%;">
    <tbody>
      <tr>
        <td><button type="button" class="btn" id="HomeScoreDown1" onclick="HomeScoreDown1()">Score - 1</button></td>
        <td><button type="button" class="btn" id="HomeScoreReset" onclick="VisitorScoreReset()">Reset 0</button></td>
        <td><button type="button" class="btn" id="HomeUpDown1" onclick="HomeScoreUp1()">Score + 1</button></td>
        <td><br></td>
        <td><button type="button" class="btn" id="PeriodUp" onclick="PeriodUp()">Period +</button></td>
        <td><br></td>
        <td><button type="button" class="btn" id="VisitorScoreDown1" onclick="VisitorScoreDown1()">Score - 1</button></td>
        <td><button type="button" class="btn" id="VisitorScoreReset" onclick="VisitorScoreReset()">Reset 0</button></td>
        <td><button type="button" class="btn" id="VisitorScoreUp1" onclick="VisitorScoreUp1()">Score + 1</button></td>
      </tr>
      <tr>
        <td><button type="button" class="btn" id="HomeBonus" onclick="HomeBonus()">Bonus</button></td>
        <td><button type="button" class="btn" id="HomePos" onclick="HomePos()">Pos</button></td>
        <td><br></td>
        <td><br></td>
        <td><br></td>
        <td><br></td>
        <td><br></td>
        <td><button type="button" class="btn" id="VisitorPos" onclick="VisitorPos()">Pos</button></td>
        <td><button type="button" class="btn" id="VisitorBonus" onclick="VisitorBonus()">Bonus</button></td>
      </tr>
      <tr>
        <td><br></td>
        <td><br></td>
        <td><br></td>
        <td><br></td>
        <td><button type="button" class="btn" id="StartStopTimer" onclick="StartStopTimer()">Run Timer</button></td>
        <td><br></td>
        <td><br></td>
        <td><br></td>
        <td><br></td>
      </tr>
      <tr>
        <td><br></td>
        <td><button type="button" class="btn" id="TimeDn1" onclick="TimeDn1()">-1</button></td>
        <td><button type="button" class="btn" id="TimeDn10" onclick="TimeDn10()">-10</button></td>
        <td><button type="button" class="btn" id="TimeDn60" onclick="TimeDn60()">-60</button></td>
        <td><br></td>
        <td><button type="button" class="btn" id="TimeUp60" onclick="TimeUp60()">+60</button></td>
        <td><button type="button" class="btn" id="TimeUp10" onclick="TimeUp10()">+10</button></td>
        <td><button type="button" class="btn" id="TimeUp1" onclick="TimeUp1()">+1</button></td>
        <td><br></td>
      </tr>
      <tr>
        <td><br></td>
        <td><br></td>
        <td><button type="button" class="btn" id="TimerSet12" onclick="TimerSet12()">12:00</button></td>
        <td><button type="button" class="btn" id="TimerSet10" onclick="TimerSet10()">10:00</button></td>
        <td><button type="button" class="btn" id="TimerSet0" onclick="TimerSet0()">00:00</button></td>
        <td><button type="button" class="btn" id="TimerSet2" onclick="TimerSet2()">02:00</button></td>
        <td><button type="button" class="btn" id="TimerSet20" onclick="TimerSet20()">20:00</button></td>
        <td><br></td>
        <td><br></td>
      </tr>
      <tr>
        <td><br></td>
        <td><br></td>
        <td><br></td>
        <td><br></td>
        <td><button type="button" class="btn" id="Buzzer" onclick="Buzzer()">Buzzer</button></td>
        <td><br></td>
        <td><br></td>
        <td><br></td>
        <td><br></td>
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

    // function to handle button press from HTML code above
    // and send a processing string back to server
    // this processing string is use in the .on method
    function ButtonPress0() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      // if you want to handle an immediate reply (like status from the ESP
      // handling of the button press use this code
      // since this button status from the ESP is in the main XML code
      // we don't need this
      // remember that if you want immediate processing feedbac you must send it
      // in the ESP handling function and here
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          message = this.responseText;
          // update some HTML data
        }
      }
      */
       
      xhttp.open("PUT", "BUTTON_0", false);
      xhttp.send();
    }
    function HomeScoreUp1() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "HomeScoreUp1", false);
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
    function VisitorScoreReset() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "VisitorScoreReset", false);
      xhttp.send();
    }
    function VisitorScoreUp1() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      xhttp.open("PUT", "VisitorScoreUp1", false);
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



    // function to handle button press from HTML code above
    // and send a processing string back to server
    // this processing string is use in the .on method
    function ButtonPress1() {
      var xhttp = new XMLHttpRequest(); 
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("button1").innerHTML = this.responseText;
        }
      }
      */
      xhttp.open("PUT", "BUTTON_1", false);
      xhttp.send(); 
    }
    
    function UpdateSlider(value) {
      var xhttp = new XMLHttpRequest();
      // this time i want immediate feedback to the fan speed
      // yea yea yea i realize i'm computing fan speed but the point
      // is how to give immediate feedback
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          // update the web based on reply from  ESP
          document.getElementById("fanrpm").innerHTML=this.responseText;
        }
      }
      // this syntax is really weird the ? is a delimiter
      // first arg UPDATE_SLIDER is use in the .on method
      // server.on("/UPDATE_SLIDER", UpdateSlider);
      // then the second arg VALUE is use in the processing function
      // String t_state = server.arg("VALUE");
      // then + the controls value property
      xhttp.open("PUT", "UPDATE_SLIDER?VALUE="+value, true);
      xhttp.send();
    }

    // function to handle the response from the ESP
    function response(){
      var message;
      var barwidth;
      var currentsensor;
      var xmlResponse;
      var xmldoc;
      var dt = new Date();
      var color = "#e8e8e8";
     
      // get the xml stream
      xmlResponse=xmlHttp.responseXML;
  
      // get host date and time
      document.getElementById("time").innerHTML = dt.toLocaleTimeString();
      document.getElementById("date").innerHTML = dt.toLocaleDateString();
  
      // A0
      xmldoc = xmlResponse.getElementsByTagName("B0"); //bits for A0
      message = xmldoc[0].firstChild.nodeValue;
      
      if (message > 2048){
      color = "#aa0000";
      }
      else {
        color = "#0000aa";
      }
      
      barwidth = message / 40.95;
      document.getElementById("b0").innerHTML=message;
      document.getElementById("b0").style.width=(barwidth+"%");
      // if you want to use global color set above in <style> section
      // other wise uncomment and let the value dictate the color
      //document.getElementById("b0").style.backgroundColor=color;
      //document.getElementById("b0").style.borderRadius="5px";
      
      xmldoc = xmlResponse.getElementsByTagName("V0"); //volts for A0
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("v0").innerHTML=message;
      document.getElementById("v0").style.width=(barwidth+"%");
      // you can set color dynamically, maybe blue below a value, red above
      document.getElementById("v0").style.backgroundColor=color;
      //document.getElementById("v0").style.borderRadius="5px";
  
      // A1
      xmldoc = xmlResponse.getElementsByTagName("B1");
      message = xmldoc[0].firstChild.nodeValue;
      if (message > 2048){
      color = "#aa0000";
      }
      else {
        color = "#0000aa";
      }
      document.getElementById("b1").innerHTML=message;
      width = message / 40.95;
      document.getElementById("b1").style.width=(width+"%");
      document.getElementById("b1").style.backgroundColor=color;
      //document.getElementById("b1").style.borderRadius="5px";
      
      xmldoc = xmlResponse.getElementsByTagName("V1");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("v1").innerHTML=message;
      document.getElementById("v1").style.width=(width+"%");
      document.getElementById("v1").style.backgroundColor=color;
      //document.getElementById("v1").style.borderRadius="5px";
  
      xmldoc = xmlResponse.getElementsByTagName("LED");
      message = xmldoc[0].firstChild.nodeValue;
  
      if (message == 0){
        document.getElementById("btn0").innerHTML="Turn ON";
      }
      else{
        document.getElementById("btn0").innerHTML="Turn OFF";
      }
         
      xmldoc = xmlResponse.getElementsByTagName("SWITCH");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("switch").style.backgroundColor="rgb(200,200,200)";
      // update the text in the table
      if (message == 0){
        document.getElementById("switch").innerHTML="Switch is OFF";
        document.getElementById("btn1").innerHTML="Turn ON";
        document.getElementById("switch").style.color="#0000AA"; 
      }
      else {
        document.getElementById("switch").innerHTML="Switch is ON";
        document.getElementById("btn1").innerHTML="Turn OFF";
        document.getElementById("switch").style.color="#00AA00";
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
        setTimeout("process()",100);
    }
  
  
  </script>

</html>



)=====";
