# Switch Board
Created by Martin Andersen 2015-2020  
IDEAA Lab -  https://www.ideaalab.com

This software is open source, under the:  
Creative Commons Attribution-ShareAlike 4.0 International license  
http://creativecommons.org/licenses/by-sa/4.0/  
that means that you can use and adapt our work, as long as you give credit to the authors, and your derivative work stays under the same license conditions as ours.

We believe that keeping this project open can help improve the project itself and magic community as a whole. Shared and open work make us grow faster and better!

We spent a lot of time and work to create this open source project.  
If you like our products, please consider to support us by buying our products so we can continue creating more awesome open source projects!  
https://sb.ideaalab.com

## INTRODUCTION
We will not explain too much, as if you know what you are doing, much explanations are not needed. And if you don't know what you are doing, longer explanations wont make you understand.

We made this code for our Switch Board project, but tried to also make it available to other creators that would like to use this code, using their own Arduino.

We tried to comment as much as possible, make the code readable and follow a simple structure. Even though, the code may be quite complex and will require some study to understand its working.

Feel free to use it at your own risk. If you find a bug, or would like to implement a new future, pull request are welcome :)

## DEFINITIONS
**The code was made using some vocabulary, but later in order for better understanding this vocabulary changed to a more user friendly approach. We didn't had time to update the vocabulary, so it can be difficult to understand the code without some "translation".**

Here you can find some equivalence and definitions:
- Bank: this is how we call the EEPROM memory space used to store routines 2, 3 and 4
- Next sequence code: jump code
- Sequence: effect
- Sequence Queue: routine

## INSTUCTIONS TO USE THE CODE
1. Choose the board you are using in the file "a_pinout.ino". If you are using a custom board, then change the pin numbers accordingly.
2. In the file b_config.ino you can change:
	1. The sequences for your routine
	2. The unlock code
	3. The jump code
	4. The number of seconds to wait between sequences
	5. The programming order of the LEDs
3. The rest of the code can be left as it is, or if you know what you are doing you can change the code as you like. If you improve the code, it would be nice if you can share it with us, and we will implement in the project.

## CONTENT OF THIS REPO
- PCB: in this folder you can find the schematic and board arrangement of Switch Board
- Programming: here you will find instructions on how to program the board with your own firmware
- OpenSourceSwitchBoard.ini: initial file, nothing special here
- a_pinout.ino: microcontroller, pins, and other settings related to the hardware
- b_config.ino: settings related to the sequences and codes
- c_global.ino: internal configurations, defines, variables, arrays and function prototypes
- d_setup.ino: the Arduino setup() function
- e_loop.ino: the Arduino loop() function
- f_functions.ino: general functions used in the code
- g_switch_programming.ino: how to activate the switches in each sequence to tell the program where the LEDs are placed
- h_sequence_programming.ino: all related to the programming of custom sequence queues
 
## HOW TO PROGRAM
If using an Arduino board, just upload and you are ready.

If you are using the Switch Board product, its **NOT RECOMMENDED** to try to reprogram it as the process is not easy.
Still, if you want to try, be warned that you may end up with a "bricked" board. Switch Board has the RESET pin disabled in the microcontroller fuses.

So you can only use a high voltage programmer or something like this:
https://github.com/ArminJo/ATtiny-HighVoltageProgrammer_FuseEraser

If using the previous FuseEraser, then you can program with ISP programmer or ArduinoAsISP.
After programming, you have to disable RESET pin again in fuses like this:  
`-Uefuse:w:0b11111111:m -Ulfuse:w:0b01100010:m -Uhfuse:w:0b01010111:m`

On the Programming folder you can find more detailed instructions on how to do the process.

## THANKS AND CREDITS
All commercial and open source switch box out there contributed in some way. So thanks for your creations! They inspired us to make our own version :)

Also thanks to the beta testers who tested the product and gave us feedback on how to improve.

[![Hits](https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgitlab.com%2Fideaalab%2Fswitch-board&count_bg=%23D71616&title_bg=%23555555&icon=&icon_color=%23E7E7E7&title=visits+&edge_flat=false)](https://hits.seeyoufarm.com)
