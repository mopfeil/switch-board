 /*
  * BUILD YOUR ROUTINE
  *
  * Choose the effects and the order you like, you can make your routine up to 16 effects long,
  * you can even repeat effects.
  *
  * Refer to file g_switch_pairing.ino to know all the possible effects available.
  *
  * This are queues of effects programmed by default, you can change them if you want, but remember
  * that you have 3 empty banks to create and store your routines. Maybe its better to leave this
  * as they are, and use the programming mode to set your own routines.
  */
#warning "Cambiar secuencias por efectos"
#warning "Cambiar effectsQueue por rutinas"
#warning "cambiar Bank por rutinas"

const byte seqQueue0[] = { 0,	//working normally
							1,	//mix LEDs, color caps or everything
							1	//mix LEDs, color caps or everything
};

const byte seqQueue1[] = { 0,	//working normally
							2,	//exchange 2 color LEDs
							1,	//mix LEDs, color caps or everything
							5,	//repeated color LED
							6	//double cap
};

/*
 * UNLOCK AND NEXT SEQUENCE CODES
 *
 * Switchboard start in a normal working mode, where every Sw turns on its adjacent LED. No magic.
 * This way, spectator can turn on the circuit and nothing strange will happen.
 * To get out from this normal mode you need to enter a code.
 * Also we enter a code to go to different sequence of the routine
 *
 * HOW TO DEFINE THE CODES
 *
 * Each switch has a value. If a switch is OFF, that switch value is 0. But if its ON then the value is:
 * Switch 1 = 1 (binary 0001)
 * Switch 2 = 2 (binary 0010)
 * Switch 3 = 4 (binary 0100)
 * Switch 4 = 8 (binary 1000)
 *
 * Status of every switch (combination of ON and OFF states) can be known with a
 * single value, that is the addition of the value of the four switches. This value
 * is stored in the program in swValue variable.
 * To get this value you should add the values shown before.
 * For example:
 * Sw1 and Sw4 ON (1 + 0 + 0 + 8), swValue = 9
 * Sw2, Sw3 and Sw4 ON (0 + 2 + 4 + 8), swValue = 14
 *
 * When all the switches are OFF, swValue = 0
 * When all the switches are ON, swValue = 15
 *
 * So we can create a "password", as a sequence of ON and OFF swtiches. To do this,
 * on every switch status change, swValue will change accordingly. If the changes in
 * swValue change accordingly to our password, then we can unlock or change sequence.
 *
 * Our unlock code is this:
 * 1. All Sw OFF, swValue = 0
 * 2. ON Sw1 (+1), swValue = 1
 * 3. ON Sw4 (+8), swValue = 9
 * 4. ON Sw3 (+4), swValue = 13
 * 5. ON Sw2 (+2), swValue = 15
 *
 * Be careful to only change (add or substract) one switch at a time. Each step in the
 * sequence is when the status of any switch changes. For ex, in previous example, step 2
 * has swValue = 1. Next step swValue = 9, becase Sw4 turned ON. This is OK. But you can not
 * go from swValue = 1 to swValue = 8. Because you need two steps to do this. First turn OFF
 * Sw1 and then turn ON Sw4.
 *
 * Starting at a random state of the switches, you have 16 possible scenarios.
 * Each step multiply by 4 the possibilities. To know the possible combinations:
 * Possible combinations = 16 * (4 ^ (CODE_LENGTH - 1))
						 = 4 ^ (CODE_LENGTH + 1)
 * CODE_LENGTH	POSSIBILITIES
 * 1			16
 * 2			64
 * 3			256
 * 4			1024
 * 5			4096
 * 6			16384
 * ...
 * 
 * Moreover, after you entered the correct code, you first have to turn OFF all switches,
 * and then wait some seconds after you input the correct code to start the next sequence,
 * otherwise it will not unlock or change sequence. Chances all this to happen are very little!
 */

 //Switches defines to better help you with the codes
#define OFF_OFF_OFF_OFF	0
#define ON_OFF_OFF_OFF	1
#define OFF_ON_OFF_OFF	2
#define ON_ON_OFF_OFF	3

#define OFF_OFF_ON_OFF	4
#define ON_OFF_ON_OFF	5
#define OFF_ON_ON_OFF	6
#define ON_ON_ON_OFF	7

#define OFF_OFF_OFF_ON	8
#define ON_OFF_OFF_ON	9
#define OFF_ON_OFF_ON	10
#define ON_ON_OFF_ON	11

#define OFF_OFF_ON_ON	12
#define ON_OFF_ON_ON	13
#define OFF_ON_ON_ON	14
#define ON_ON_ON_ON		15

//how many steps for unlock and next sequence?
#define CODE_LENGTH		5

//password to come out from the "normal" working sequence
const byte unlockCode[CODE_LENGTH] = {	OFF_OFF_OFF_OFF,	//OFF-OFF-OFF-OFF
										ON_OFF_OFF_OFF,		//turn on 1
										ON_OFF_OFF_ON,		//turn on 4
										ON_ON_OFF_ON,		//turn on 2
										ON_ON_ON_ON			//turn on 3
};

//password to jump to the next sequence
const byte nextSeqCode[CODE_LENGTH] = {	ON_ON_ON_ON,		//ON-ON-ON-ON
										ON_ON_ON_OFF,		//turn off 4
										OFF_ON_ON_OFF,		//turn off 1
										OFF_ON_OFF_OFF,		//turn off 3
										OFF_OFF_OFF_OFF		//turn off 2
};

//password to enter in the programming mode
const byte programmingCode[CODE_LENGTH] = {	OFF_ON_OFF_ON,	//OFF-ON-OFF-ON
											ON_ON_OFF_ON,	//turn on 1
											ON_OFF_OFF_ON,	//turn off 2
											ON_OFF_ON_ON,	//turn on 3
											ON_OFF_ON_OFF	//turn off 4
};

/*
 * Once you enter your sequence code, you have to wait some seconds before this sequence becomes active
 */
#define SHORT_WAIT	3	//time, in seconds

#define LONG_WAIT	5	//time it takes to happen things like replace cap or bulb, in seconds

/*
 * LED PROGRAMMING
 *
 * When mixing colors, you have to "tell" the program the position of each LED.
 * To do this we assign a LED possition to a switch possition.
 * We could do this straightforward: first turned ON switch will be assigned to LED 1,
 * second turned ON switch will be assigned to LED 2, and so on...
 * After this, the program "knows" which switch has to turn ON wich LED.
 * This is how originally was made. But we can use a custom order to make it more deceptive.
 * For example, we can make the first LED to program to be the second LED, so the first switch
 * turned ON will be assigned to the second LED. So change the values as you wish.
 */
#define PAIR_FIRST	1	//first LED to program is LED number 1
#define PAIR_SECOND	4	//second LED to program is LED number  4
#define PAIR_THIRD	2	//third LED to program is LED number 2
#define PAIR_FOURTH	3	//fourth LED to program is LED number 3
