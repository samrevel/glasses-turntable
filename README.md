# glasses-turntable
This repository has the code, schematic, board. etc for the electronics of the glasses turntable kiosk display in Revel Digital warehouse 13.


The glasses turntable electronics board (board) uses a ATmega8 microcontroller to controll a H bridge that controlls a brushed DC motor. The DC motor has a gear boc to slow the motor down and to increase torqu. The motor turns the whole display. The display is ran from a 12V dc source and is fused. All the logic on the board is running 5V.

On the board there are connector for LEDs (12V LED string), power (2.1X5.5mm barrel jack center positive 12V), homing switch, extranal button (not used), mbrushed DC motor, serial, programmer conncetor, and fan (12V power out, not used). 

The display will rotate when the board recives a serial command. There serial commands are sent by a display located need the glasses turntable. When the board recives a command it will return the same serial command back.

Serial settings:
- Buad: 9600
- Bits: 8
- Stop Bits: 1
- Parity: None
- Flow Control: None
- Data format: ASCII
- Ending charachers: None (will not hurt if send it)

Serial commands (in ASCII):
- "1" rotate to side 1
- "2" rotate to side 2
- "3" rotate to side 3
- "4" rotate for about a second
- "5" rotate for about a second backwares
- "6" rotate for about a haft second
- "7" rotate for about a haft backwares
