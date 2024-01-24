/*
 * Setup
 */
void setup() {
	//check restart cause
	reset_cause = MCUSR;	//read MCU status register
	MCUSR = 0;				//clear reset cause

#ifdef SWITCH_BOARD
	//only for Switch Board, setup RESET pin as output, and LOW value
	//this is to avoid unwanted reset due to static electricity
	//if configured as input, an ESD on the reset pin could reset the uC, even when reset is disabled on fuses
	pinMode(PIN_PB3, OUTPUT);
	digitalWrite(PIN_PB3, LOW);
#endif

	//assign each switch to each LED
	for (byte x = 0; x < 4; x++) {
		swColor[x] = x + 1;
		ledColor[x] = x + 1;

//we use this to disable the output of the LED to avoid flicker when power up
//once we setup the switches, then we can config the LEDs as outputs
#if LED_ON == LOW
		//if LEDs are active LOW, set as input and activate the pin pull up to draw no current to the LED
		pinMode(led[x], INPUT_PULLUP);
#endif
	}

	//initialize switch pins
#if USE_INTERNAL_PULLUPS == true
	pinMode(sw[0], INPUT_PULLUP);
	pinMode(sw[1], INPUT_PULLUP);
	pinMode(sw[2], INPUT_PULLUP);
	pinMode(sw[3], INPUT_PULLUP);
#else
	pinMode(sw[0], INPUT);
	pinMode(sw[1], INPUT);
	pinMode(sw[2], INPUT);
	pinMode(sw[3], INPUT);
#endif

	//now we can set the LEDs as outputs and turn them ON according to the switch
	for (byte x = 0; x < 4; x++) {
		pinMode(led[x], OUTPUT);
		showSwitch(x);
	}

#warning "Comment following line for production firmware"
	//checkResetCause();		//make LEDs to show the reset cause

#if USE_RF == true
	//inicialize RF pins
	pinMode(rf[0], INPUT);
	pinMode(rf[1], INPUT);
	pinMode(rf[2], INPUT);
	pinMode(rf[3], INPUT);
#endif

	emptyQueue();			//fill the queue with EMPTY_POS

#ifdef DEBUG
	//initialize serial port
	//if computer is connected you can see
	//status information and instructions of each sequence
	Serial.begin(19200);

	delay(500);

	Serial.println(F("------ OPEN SOURCE SWITCH BOARD ------"));
	Serial.println(F(">>>   Created by Martin Andersen   <<<"));
	Serial.println(F(">>>>>     Made at IDEAA Lab      <<<<<"));
	Serial.println(F(">>>    http://www.ideaalab.com     <<<"));
	Serial.println(F("--------------------------------------"));
	Serial.println();
#endif

	checkFirstTime();		//check if we are running the program for the first time

#ifdef DEBUG
	print_eeprom_banks();
#endif

	loadSequenceQueue();
}
