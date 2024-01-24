//Uncomment the board you are using and comment the others
#define SWITCH_BOARD
//#define PROMICRO
//#define CUSTOM

/* HARDWARE CONNECTIONS
 *
 *			SWITCH_BOARD	PROMICRO	CUSTOM
 * LED1		4				5			-
 * LED2		5				4			-
 * LED3		6				3			-
 * LED4		10				2			-
 *
 * SW1		3				10			-
 * SW2		2				11			-
 * SW3		1				12			-
 * SW4		0				13			-

 * RF1		X				6			-
 * RF2		X				7			-
 * RF3		X				8			-
 * RF4		X				9			-
 */

/* Pin definition for IDEAA Lab Switch Board circuit, using ATtiny84A uC */
#ifdef SWITCH_BOARD
// Need to install ATtinyCore (currently working with v1.5.2): https://github.com/SpenceKonde/ATTinyCore

// COMPILE SETTINGS:
// Board:				ATtiny24/44/84(a) (no bootloader)
// Chip:				ATtiny84(a)
// Clock:				1Mhz (internal)
// Pin mapping:			Clockwise (like damellis core)
// LTO:					Enabled
// tinyNeopixel port:	Port A
// millis()/micros:		Enabled
// Save EEPROM:			EEPROM Retained
// B.O.D. Level:		B.O.D. Disabled

// FUSES:
// Extended:			0b11111111 (0xFF)
// High:				0b01010111 (0x57)
// Low:					0b01100010 (0x62)

	//Config
	#define LED_ON					LOW
	#define SWITCH_ON				HIGH
	#define USE_INTERNAL_PULLUPS	true
	#define USE_RF					false

	//LED pins
	#define LED1					PIN_PA4
	#define LED2					PIN_PA5
	#define LED3					PIN_PA6
	#define LED4					PIN_PB0		//PA7 for PCB v0.3

	//Switch pins
	#define SW1						PIN_PA3		//PB3 for PCB v1.1a
	#define SW2						PIN_PA2
	#define SW3						PIN_PA1
	#define SW4						PIN_PA0
#endif

/* Pin definition for IDEAA Lab board, using Arduino Pro Micro */
// Product link: https://ideaalab.com/shop/diy-switch-board-detail (not ready yet)
#ifdef PROMICRO
	#define DEBUG

	//Config
	#define LED_ON					HIGH
	#define SWITCH_ON				LOW
	#define USE_INTERNAL_PULLUPS	false
	#define USE_RF					true

	//LED pins
	#define LED1					5
	#define LED2					4
	#define LED3					3
	#define LED4					2

	//Switch pins
	#define SW1						10
	#define SW2						16
	#define SW3						14
	#define SW4						15

	//RF pins
	#define RF1						6
	#define RF2						7
	#define RF3						8
	#define RF4						9
#endif

/* Define your own pin arrangement */
#ifdef CUSTOM
	//#define DEBUG //uncomment this line if you want to monitor the program over serial port
					//strings used on printf are included on F() macro to save SRAM space
					//http://playground.arduino.cc/learning/memory
					//https://www.arduino.cc/reference/en/language/variables/utilities/progmem/#_the_code_f_code_macro

	//Config
	#define LED_ON					LOW		//the LEDs are active HIGH or active LOW? You should check the hardware connections for this

	#define SWITCH_ON				HIGH	//do you want the switches to activate the leds when input reads HIGH or LOW?
											//it dont have to match with your hardware. You can use the opposite to invert the output (for ex. turn on LED when switch is off)

	#define USE_INTERNAL_PULLUPS	true	//the switch will need pull-up or pull-down resistors.
											//If you want you can use the internal pull-up resistors and avoid use externals

	#define USE_RF					true	//set to true or false whether your board have 4CH RF receiver module

	//LED pins
	#define LED1					1
	#define LED2					2
	#define LED3					3
	#define LED4					4

	//Switch pins
	#define SW1						5
	#define SW2						6
	#define SW3						7
	#define SW4						8

	//RF pins
	//if you are using RF, uncomment this lines
	#define RF1						9
	#define RF2						10
	#define RF3						11
	#define RF4						12
#endif

#ifndef SWITCH_BOARD
#ifndef PROMICRO
#ifndef CUSTOM
#error "No board selected. Choose a board in file a_pinout.ino"
#endif
#endif
#endif
