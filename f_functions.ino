/*
 * At the beginning of each sequence we have to pair the switches
 * with the corresponding bulb. Each sequence have a different pairing
 * method. Here we select the pairing according to the sequence we are
 * into. The sequence to pair is a parameter you can tell to the function,
 * but its preloaded with the defaut queue.
 */
void pairSwitches(byte seq) {
	//if the switches are already paired, we exit
	if (swPairDone == true) return;

#ifdef DEBUG
	//show sequence number
	Serial.print(F("\r\n> Starting sequence "));
	Serial.print(seq);
	Serial.print(F(": "));
#endif

	switch (seq) {
		/* SEQ.0: NORMALY WORKING SWITCHES */
	case 0:
		switchPairing0();	//pair switches
		break;

		/* SEQ.1: MIX COLORS */
	case 1:
		switchPairing1();	//pair switches
		break;

		/* SEQ.2: EXCHANGE TWO LED OR TWO SWITCH POSITION */
	case 2:
		switchPairing2();	//pair switches
		break;

		/* SEQ.3: REMOVE ONE SWITCH CAP */
	case 3:
		switchPairing3();	//pair switches
		break;

		/* SEQ.4: DUPLICATED COLOR LED */
	case 4:
		switchPairing4();	//pair switches
		break;

		/* SEQ.5: EXCHANGE COLOR LED FOR WHITE LED */
	case 5:
		switchPairing5();	//pair switches
		break;

		/* SEQ.6: DOUBLE CAP */
	case 6:
		switchPairing6();	//pair switches
		break;

		/* SEQ.7: IN CRESCENDO */
	case 7:
		switchPairing7();	//pair switches
		break;

		/* if no match, return to sequence 0 */
	default:
		queuePosition = 0;
	}

#ifdef DEBUG
	Serial.println(F("Pairing done, working like this:"));
	showAssignments();
#endif
}

/*
 * Read the status of all switches.
 * The read status of the switch is compared to whether the switch is active high or low, so it will always
 * save 1 for turned ON switch, and 0 for turned OFF switch, no matter if it is active high or low.
 */
void readSwitches(bool show) {
	byte temp = 0;

	oldSwValue = swValue; //save the old values of the switches

	//read status of all switches
	for (byte x = 0; x < 4; x++) {
		if (GET_SW_STATE(x) == true) {
			temp = temp + (1 << x);
		}
	}

	swValue = temp; //new value of the switches

	delay(DEBOUNCE_MS);

#ifdef DEBUG
	//if we dont want to show values, then exit
	if (show == false) return;

	//send the status of the switches to the serial port
	if (swValue != oldSwValue) {
		Serial.print(F("Switches: { "));

		for (byte x = 0; x < 4; x++) {
			if (GET_SW_STATE(x) == true) {
				Serial.print(F("ON  "));
			}
			else {
				Serial.print(F("OFF "));
			}
		}

		Serial.print(F("} = "));
		Serial.println(swValue);
	}
#endif
}

/*
 * Turn OFF all LEDs
 */
void turnOffLEDs(void) {
	for (byte y = 3; y < 4; y--) {
		SHOW_LED(y, false);
	}
}

/*
 * This will turn ON or OFF the lights, according to the switches values
 */
void turnOnLEDs(void) {
	turnOffLEDs();	//first turn off all LEDs

	for (byte x = 0; x < 4; x++) {
		showSwitch(x);
	}
}

/*
 * Find LEDs matching the color of the switch and
 * turn them ON or OFF according to the switch status
 */
void showSwitch(byte swNum) {
#warning "Se puede implementar que haya dos interruptores blancos?"
	//boolean parity;

	//run through all the LEDs checking if its the same color as the switch
	for (byte L = 0; L < 4; L++) {
		//if the LED is the same color as the switch
		if (ledColor[L] == swColor[swNum]) {
			//for sequence 6, once the programming is done
			/*if ((seqQueue[queuePosition] == 6) && (swPairDone == true)) {
				parity = !(getParity() ^ LED_ON);
				SHOW_LED(x, parity);
			}
			//for all sequences
			else {*/
			SHOW_LED(L, GET_SW_STATE(swNum));
			//}
		}

		//if there is a double cap on this switch, show the second bulb
		if (doubleCapPos == swNum) {
			//if the double cap color is the same as this led
			if (ledColor[L] == doubleCapColor) {
				SHOW_LED(L, GET_SW_STATE(swNum));
			}
		}
	}
}

/*
 * Parity is used to turn ON or OFF white LEDs when
 * all the switches are without cap. Odd parity is used
 * so when all switches are ON, the white LED is also ON.
 * https://en.wikipedia.org/wiki/Parity_bit
 */
/*bool getParity(void) {
	bool parity = 1;
	byte n = swValue;

	while (n) {
		parity = !parity;
		n = n & (n - 1);
	}

	return parity;
}*/

/*
 * Call the checkCode function, and if the code matches call
 * the function that waits for all the switches to be off
 */
void checkCodeMatch(void) {
	if (checkCode() == true) {
		if (enterProg == false) {
			//wait for all switches to be turned OFF and wait the
			//defined time, then jumps to selected sequence
			allSwOffAndWait();
		}
	}
}

/*
 * Check if code match
 * First we have to put swValue inside the swValueArray[] array.
 * To do that we do it in a FIFO style. The smaller the index of swValueArray[]
 * the older the value. Index 0 is the oldest. So first we move every byte one possition "left".
 * The oldest value get lost and replaced with the one on index 1. Then the index 1 gets replaced
 * with the value on index 2, and so on...
 * Lastly we store our new swValue on the last possition of swValueArray[].
 * While moving values inside array we also compare with programmingCode/unlockCode/nextSeqCode
 */
bool checkCode(void) {
	bool codeOK = true;
	bool progOK = true;
	byte codeVal = 0;

	//if no change to the switches, exit
	if (swValue == oldSwValue) {
		return(false);
	}

	//update programming array code
	if (swToggleCounter < CODE_LENGTH) {
		swToggleCounter++;	//only counts until we reach CODE_LENGTH. So the code has to be introduced correctly the first time
	}
	else {
		if (programmingPossible == true) {
			programmingPossible = false;
#ifdef DEBUG
			Serial.println(F("Prog no longer possible\r\n"));	//--------colocar en otro sitio
#endif
		}
	}

#warning "Check if programming is possible on all five steps, not only on the last?"

	//update array of codes
	for (byte y = 0; y < CODE_LENGTH; y++) {
		if (y < (CODE_LENGTH - 1)) {
			swValueArray[y] = swValueArray[y + 1];		//moves the value one possition to the left and overwrites the previous value
		}
		else {
			swValueArray[CODE_LENGTH - 1] = swValue;	//on the last possition we store our current switch values
		}

		//check programming code
		if (programmingPossible == true) {
			if(swValueArray[y] != programmingCode[y]){
				progOK = false;
			}
		}

		//for unlocking the first sequence we use "unlock code"
		if (seqQueue[queuePosition] == 0) {
			codeVal = unlockCode[y];
		}
		//and for next sequence we use "next sequence code"
		else {
			codeVal = nextSeqCode[y];
		}

		//check if code is OK
		if (swValueArray[y] != codeVal) {
			codeOK = false;	//if any of the switch values dont match with our code, then the code entered is not correct
		}
	}

#ifdef DEBUG
	Serial.print(F("Code entered  ( "));
	for (byte y = 0; y < CODE_LENGTH; y++) {
		Serial.print(swValueArray[y]);

		//print a "space" in number if smaller that 10, to match formating
		if (swValueArray[y] < 10) {
			Serial.print(" ");
		}

		//if not the last value, print a dash
		if (y < (CODE_LENGTH - 1)) {
			Serial.print(F(" - "));
		}
		//if last value, print ending
		else {
			Serial.print(F(" ) -> "));
		}
	}

	//first check if programming code match
	if ((programmingPossible == true) && (progOK == true)) {
		Serial.println(F("ENTER PROGRAMMING MODE"));
	}
	//otherwise, check if other codes match
	else {
		if (codeOK == true) {
			Serial.println(F("MATCH !!!"));
		}
		else {
			Serial.println(F("DON'T MATCH"));
		}

		if (programmingPossible == true) {
			Serial.print(F("Progr. code   ( "));

			for (byte y = 0; y < CODE_LENGTH; y++) {
				Serial.print(programmingCode[y]);
				
				//print a "space" in number if smaller that 10, to match formating
				if (programmingCode[y] < 10) {
					Serial.print(" ");
				}

				if (y < (CODE_LENGTH - 1)) {
					Serial.print(F(" - "));
				}
				else {
					Serial.println(F(" )"));
				}
			}
		}

		if (seqQueue[queuePosition] == 0) {
			Serial.print(F("Unlock code   ( "));
		}
		else {
			Serial.print(F("Next Seq code ( "));
		}

		for (byte y = 0; y < CODE_LENGTH; y++) {
			if (seqQueue[queuePosition] == 0) {
				Serial.print(unlockCode[y]);

				//print a "space" if number is smaller that 10, to match formating
				if (unlockCode[y] < 10) {
					Serial.print(" ");
				}
			}
			else {
				Serial.print(nextSeqCode[y]);

				//print a "space" if number is smaller that 10, to match formating
				if (nextSeqCode[y] < 10) {
					Serial.print(" ");
				}
			}

			if (y < (CODE_LENGTH - 1)) {
				Serial.print(F(" - "));
			}
			else {
				Serial.println(F(" )\r\n"));
			}
		}
	}
#endif

	if ((programmingPossible == true) && (progOK == true)) {
		enterProg = true;
		codeOK = true;
	}

	return(codeOK);
}

/*
 * Check if all switches are turned OFF, then wait
 * some time to continue to next sequence
 */
void allSwOffAndWait(void) {
	bool exitLoop = false;	//flag to exit the loop
	uint32_t endTime = 0;

#ifdef DEBUG
	Serial.println(F("Waiting for all the switches to be turned OFF"));
	if (swValue != 0) {
		Serial.println();
	}
#endif

	do {
		readSwitches();		//read new value of switches
		turnOnLEDs();		//turn on LEDs according to the current sequence

		if (swValue == OFF_OFF_OFF_OFF) {	//all switches are OFF
			if (endTime == 0) {
#ifdef DEBUG
				Serial.print(F("\r\nWaiting seconds: "));
				Serial.println(SHORT_WAIT);
#endif
				endTime = millis() + (SHORT_WAIT * 1000);
			}

			//if the timeout finished we can exit the loop
			if (millis() > endTime) {
				exitLoop = true;
			}
		}
		else {				//all switches are not turned OFF
			endTime = 0;	//restart timeout
		}
	} while (exitLoop == false);

	//if we are here, THE TIMEOUT FINISHED

	queuePosition++;		//go to next sequence

	//next value is empty or we reached the end of the queue?
	if ((seqQueue[queuePosition] == EMPTY_POS) || (queuePosition == BANK_LIMIT)){
		//back to first possition
		queuePosition = 0;

#ifdef DEBUG
		Serial.println(F("\r\nQueue reached the end. Starting again from queue position 0"));	
#endif
	}
	
	swPairDone = false;		//so we can enter next switch programming
}

/*
 * Check if thee EEPROM has been initialized
 * If not, then clear EEPROM banks, and flag it as initialized
 */
void checkFirstTime(void) {
	if (EEPROM.read(FIRST_TIME_FLAG) != INITIALIZED_POS) {
		//erase all banks
		eraseBank(2);
		eraseBank(3);
		eraseBank(4);

		//flag EEPROM as initialized
		EEPROM.write(FIRST_TIME_FLAG, INITIALIZED_POS);

#ifdef DEBUG
		Serial.println(F("First program run -> EEPROM banks erased, memory initialized\r\n"));
#endif
	}
}

/*
 * fill the array with empty possitions to avoid garbage values that may confuse our queue
 */
void emptyQueue(void) {
	for (byte x = 0; x < BANK_LIMIT; x++) {
		seqQueue[x] = EMPTY_POS;
	}
}

/*
 * Clear array of colors assigned to the switches
 */
void clearSwColors(void) {
	for (byte S = 0; S < 4; S++) {
		swColor[S] = EMPTY_COLOR;
	}
}

/*
 * Clear array of colors assigned to the LEDs
 */
void clearLEDColors(void) {
	for (byte L = 0; L < 4; L++) {
		ledColor[L] = EMPTY_COLOR;
	}
}


/*
 * Check the reset cause (not used in production)
 */
void checkResetCause(void){
	//if no reset, return
	if(reset_cause == 0) return;

	//if normal power up, return
	if (reset_cause == POR_RESET) return;

	//turn off all LEDs
	SHOW_LED(0, false);
	SHOW_LED(1, false);
	SHOW_LED(2, false);
	SHOW_LED(3, false);

	if(reset_cause != 0){
		//Watchdog reset
		if(reset_cause & WTD_RESET){
			SHOW_LED(0, true);
		}

		//Brown out reset
		if(reset_cause & BOR_RESET){
			SHOW_LED(1, true);
		}

		//External reset
		if(reset_cause & EXT_RESET){
			SHOW_LED(2, true);
		}

		//Normal start up
		if (reset_cause & POR_RESET){
			SHOW_LED(3, true);
		}

		delay(5000);

		//turn off all LEDs
		SHOW_LED(0, false);
		SHOW_LED(1, false);
		SHOW_LED(2, false);
		SHOW_LED(3, false);
	}
	else{
		//NO RESET CAUSE SET - yet here we are??? This should NOT HAPPEN
		//turn on LEDs one by one, and then off one by one
		progStartLights();
	}

	if(GPIOR0 != 0){
		//GPIOR register is 0 on reset. At the end of this, we set it to 0xFF
		//if it isn't 0 when this function is called, presumably it didn't get cleared, no reset happened, and probably nothing else got reinitialized either

		//move light from left to right
		for (byte y = 0; y < 4; y++) {
			turnOffLEDs();          //turn off all LEDs
			SHOW_LED(y, true);
			delay(100);
		}

		//move light from right to left
		for (byte y = 0; y < 4; y++) {
			turnOffLEDs();          //turn off all LEDs
			SHOW_LED(3-y, true);
			delay(100);
		}
	}

	GPIOR0 = 0xFF;
}

#ifdef DEBUG
 /*
  * Prints the color of each LED and switch
  */
void showAssignments(void) {
	Serial.print(F("LED colors: ( "));
	for (byte L = 0; L < 4; L++) {
		printColor(ledColor[L]);
		if (doubleCapPos == L) {
			Serial.print(F("  "));
		}
		Serial.print(F(" "));
	}
	Serial.println(F(")"));

	Serial.print(F("SW colors:  ( "));
	for (byte S = 0; S < 4; S++) {
		printColor(swColor[S]);
		if (doubleCapPos == S) {
			Serial.print(F("/"));
			printColor(doubleCapColor);
		}
		Serial.print(F(" "));
	}
	Serial.println(F(")\r\n"));

}

/*
 * Prints a letter for each color to help visualize the order
 * of colors of LEDs and switches. The actual letters of the color
 * is not represented, but letters A/B/C/D are used for colors, and
 * W for white. A-D letters are used because it really dont matter
 * which color is each, but if it match the LED with the switch
 */
void printColor(byte c) {
	switch (c) {
	case COLOR_A:
		Serial.print(F("A"));
		break;

	case COLOR_B:
		Serial.print(F("B"));
		break;

	case COLOR_C:
		Serial.print(F("C"));
		break;

	case COLOR_D:
		Serial.print(F("D"));
		break;

	case WHITE_COLOR:
		Serial.print(F("W"));
		break;

	case EMPTY_COLOR:
		Serial.print(F("~"));
		break;

	default:
		Serial.print(F("?"));
	}
}

/*
 * Read the internal EEPROM, not needed, but useful for debugging
 */
void print_eeprom_banks(void) {
	byte start, end;
	char val[5];					//string for sprintf

	for (byte b = 0; b < 3; b++) {
		start = bankStart[b];		//for some reason this sometimes doesnt work ��??
		end = start + BANK_LIMIT;	//end of bank
		
		//print bank and positions
		Serial.print(F("BANK ")); Serial.print(b + 2);
		Serial.print(F(" (Start: ")); Serial.print(start);
		Serial.print(F(" End: ")); Serial.print(end-1); Serial.print(F(") ---"));

		//print values
		for (byte x = start; x < end; x++) {
			//change line every 8 values
			if (x % 8 == 0) {
				//print line number in HEX
				sprintf(val, "\r\n%02X: ", x);
				Serial.print(val);
			}

			//print value in HEX
			sprintf(val, "%02X ", EEPROM.read(x));
			Serial.print(val);
		}

		Serial.print(F("\r\n\r\n"));
	}
}
#endif