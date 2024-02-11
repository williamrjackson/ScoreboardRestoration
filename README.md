# ESP32 & FASTLED Wireless Scoreboard
### 1980s Basketball Scoreboard Functional Restoration
    
Scoreboard appears as a WiFi Access Point. Display panels are lit using WS2812b addressable LEDs. 

![Physical Scoreboard](Scoreboard.jpeg)    
### Updated with ESP32 & LEDs    

![Updated Wireless Test](Install.gif)


## Mobile Control Panel:
Control page hosted at http://scoreboard.local
    
![Mobile Web Controller](Controller.png)

## Commands:
- /HomeScore?set={value}
- /HomeScore?change={amount}
- /VisitorScore?set={value}
- /VisitorScore?change={amount}
- /Time?set={valueInMinutes}
- /Time?change={amountInSeconds}
- /StartStopTimer
- /PeriodChange
- /HomeBonus
- /VisitorBonus
- /HomePos
- /VisitorPos
- /Buzzer