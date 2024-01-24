# ESP32 & FASTLED Wireless Scoreboard
1980s Basketball Scoreboard Restoration
    
Scoreboard appears as an access point. Panels are lit using WS2812b addressable LEDs. 

![Physical Scoreboard](Scoreboard.jpeg)
Control page hosted at http://scoreboard.local

Commands:
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


![Mobile Web Controller](Controller.png)

