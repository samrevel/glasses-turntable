# glasses-turntable
This repository has the code, schematic, board. etc for the electronics of the glasses turntable kiosk display in Revel Digital warehouse 13.


The glasses turntable electronics board (board) uses a ATmega8 microcontroller to controll a H bridge that controlls a brushed DC motor. The DC motor has a gear boc to slow the motor down and to increase torqu. The motor turns the whole display. 

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
