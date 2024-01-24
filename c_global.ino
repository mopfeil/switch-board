/*
 * Includes
 */

#include <EEPROM.h>
#ifdef DEBUG
#include <stdio.h>	//needed for debug
#endif

/*
 * Variables and some extra defines that dont need to be changed by user
 */

#define DEBOUNCE_MS			20							//delay to debounce switches
 //#define MS_DELAY			10							//delay per cycle, in mS
 //#define CYCLE_COUNT			(SHORT_WAIT * 1000 / MS_DELAY)	//number of cycles to complete the time of SHORT_WAIT

#define FADE_OUT_TIME		1000						//time it takes to dim off a LED
#define CROSS_FADE_TIME		1000						//time it takes to cross fade two LEDs
#define FADE_IN_TIME		2000						//time it takes to dim on a LED

//real color in each position really dont matter, instead we give them a letter
#define COLOR_A				0x01
#define COLOR_B				0x02
#define COLOR_C				0x03
#define COLOR_D				0x04
#define WHITE_COLOR			0x00						//indicates a LED or a SWITCH is white
#define EMPTY_COLOR			0xFF						//indicates a LED or a SWITCH is unassigned

#define BANK_LIMIT			16							//bank steps limit

//bank start position in the EEPROM
#define BANK_START_2		0
#define BANK_START_3		(BANK_START_2 + BANK_LIMIT)
#define BANK_START_4		(BANK_START_3 + BANK_LIMIT)

#define FIRST_TIME_FLAG		(BANK_START_4 + BANK_LIMIT)	//value stored in this position let us know if its the first time we run the circuit

#define EMPTY_POS			0xFF						//to know if the value is empty
#define INITIALIZED_POS		0x55						//to know if the EEPROM has been initialized

//reset causes
#define WTD_RESET 0b00001000							//watchdog reset
#define BOR_RESET 0b00000100							//brownout reset
#define EXT_RESET 0b00000010							//external reset
#define POR_RESET 0b00000001							//normal power up

//position in memory of each bank
byte bankStart[3] = { BANK_START_2, BANK_START_3, BANK_START_4 };

//LED array
const byte led[4] = { LED1, LED2, LED3, LED4 };

//Switch array
const byte sw[4] = { SW1, SW2, SW3, SW4 };

#if USE_RF == true
//RF array
const byte rf[4] = { RF1, RF2, RF3, RF4 };
#endif

//to each switch (swColor index from 0 to 3) a value is assigned from 1 to 5
//to each LED (ledColor index from 0 to 3) a value is assigned from 1 to 5
//0xFF means unassigned, 1 to 4 are regular color values (really dont matter which color is which number), and 5 means white color or without color
//example:
//swColor() = {3, 5, 4, 1};
//ledColor() = {1, 1, 5, 2};
//Sw 1 dont turn on any LED
//SW 2 turn on LED 3
//Sw 3 dont turn on any LED
//Sw 4 turn on LED 1 and 2
byte swColor[4];
byte ledColor[4];

bool swPairDone = false;			//we use this to know if we already paired the switches with the LEDs

byte swValueArray[CODE_LENGTH] = { 0, 0, 0, 0, 0 };	//array used to check the codes

byte seqQueue[BANK_LIMIT];			//the queue of sequences are stored here
byte queuePosition = 0;				//in which step of the sequence queue are we. NOT the sequence number!

byte swValue = 0;					//status of all the switches converted into a decimal value
byte oldSwValue = 0;				//last value of the switches

//effect 6 special variables
byte doubleCapPos = EMPTY_POS;
byte doubleCapColor = EMPTY_COLOR;

//programming mode variables
bool programmingPossible = true;	//its possible to enter in programming mode if a code is entered without error the first time after boot up
bool enterProg = false;				//do we need to enter programming mode?
byte swToggleCounter = 0;

byte reset_cause;					//store the reset cause

/* MACROS */
#define SHOW_LED(n, s)	digitalWrite(led[n], (!(s ^ LED_ON)));	//LED n will change state to s = true/false
#define GET_SW_STATE(x)	(!(digitalRead(sw[x]) ^ SWITCH_ON))		//read switch and return 0=OFF / 1=ON no matter if the switch is active high or active low

//#define LED_OUTPUT(x)	(!(GET_SW_STATE(x) ^ LED_ON))			//will return the ON/OFF output to the LED (independently if its active high or low), according to the switch status

/* FUNCTION PROTOTYPES */
//f_functions.ino
void pairSwitches(byte seq = seqQueue[queuePosition]);
void readSwitches(bool show = true);
void turnOffLEDs(void);
void turnOnLEDs(void);
void showSwitch(byte swNum);
//bool getParity(void);
void checkCodeMatch(void);
bool checkCode(void);
void allSwOffAndWait(void);
void checkFirstTime(void);
void emptyQueue(void);
void clearSwColors(void);
void clearLEDColors(void);
void checkResetCause(void);
#ifdef DEBUG
void showAssignments(void);
void printColor(byte c);
void print_eeprom_banks(void);
#endif

//g_switch_pairing.ino
void switchPairing0(void);
void switchPairing1(void);
void switchPairing2(void);
void switchPairing3(void);
void switchPairing4(void);
void switchPairing5(void);
void switchPairing6(void);
void switchPairing7(void);
void sq7_phase1(void);
void sq7_phase2(void);
void sq7_phase3(void);
void sq7_phase4(void);
void sq7_phase5(void);

//h_sequence_programming.ino
void programmingMode(void);
byte chooseBank(void);
byte getSeqNumber(void);
void writeStep(byte bk, byte st, byte sq);
void waitAllSwOff(void);
void blinkLEDs(void);
void confirmLight(void);
void progStartLights(void);
void memEndLights(void);
void loadSequenceQueue(void);
byte loadPreprogrammed(byte b);
byte loadFromBank(byte b);
void eraseBank(byte b);