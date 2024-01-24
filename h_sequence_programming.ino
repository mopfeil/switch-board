/*
 * Allow to program user defined sequences
 * Once programming mode has entered, show light effect so user know its OK
 * 1. Wait all switches to be off
 * 2. Choose memory bank 2, 3 or 4. Confirm with switch 1
 * 3. Wait all switches to be off
 * 4. Choose sequence with switch 2, 3 and 4. Confirm with switch 1
 * 5. Repeat, go to step 3
 * 6. When you want to finish, just remove battery
 */
void programmingMode(void) {
	byte bank = 0;
	byte bankStep = 1;
	byte seqNum = 0;

	if (enterProg == false) return;	//exit if not in programming mode

#ifdef DEBUG
	Serial.println(F("\r\n--- PROGRAMMING MODE ---"));
#endif

	progStartLights();		//show light sequence to indicate the programming mode started
	
#ifdef DEBUG
	Serial.println(F("\r\n> Choose memory bank < "));
#endif

	bank = chooseBank();	//choose a bank and confirm
	eraseBank(bank);		//erase all values in the bank

#ifdef DEBUG
	Serial.print(F("\r\n> Program sequences < "));
	Serial.print(F("\r\n-Programming STEP 0 (step 0 is always sequence 0)"));
#endif

	writeStep(bank, 0, 0);	//first step is always sequence 0

	do {
#ifdef DEBUG
		Serial.print(F("\r\n-Programming STEP ")); Serial.println(bankStep);
#endif
		seqNum = getSeqNumber();
		writeStep(bank, bankStep++, seqNum);
	} while (bankStep < BANK_LIMIT);

	memEndLights();		//chase light effect to indicate the memory has ended
}

/*
 * Routine to choose memory bank
 * Turn ON sw 2, 3 or 4 to choose a bank. Confirm turning ON sw 1
 */
byte chooseBank(void) {
	bool correctOption = false;	//used to check if the selected option is correct
	byte bank = 0;				//which memory bank we will use to program a sequence?
	byte swBank = 0;			//save the swValue to know which switch was activated

	/* --- 1. Wait for all the switches to be OFF --- */
#ifdef DEBUG
	Serial.print(F("1. "));	//next part of the text is printed on the follwing function
#endif
	waitAllSwOff();

	/* --- 2. Choose a memory bank --- */
#ifdef DEBUG
	Serial.print(F("2. Choose memory bank (2, 3 or 4)"));
#endif

	//we expect the user to choose a bank, and confirm with the first sw ON
	do {
		readSwitches(false);

		//if no changes in the switches, jump to the next iteration without executing following code
		if (swValue == oldSwValue) {continue;}

		turnOffLEDs();

		switch (swValue) {
		case OFF_ON_OFF_OFF:	//bank 2
			SHOW_LED(1, true);
			bank = 2;
			swBank = swValue;
			correctOption = true;
			break;

		case OFF_OFF_ON_OFF:	//bank 3
			SHOW_LED(2, true);
			bank = 3;
			swBank = swValue;
			correctOption = true;
			break;

		case OFF_OFF_OFF_ON:	//bank 4
			SHOW_LED(3, true);
			bank = 4;
			swBank = swValue;
			correctOption = true;
			break;

		default:
			correctOption = false;
			swBank = 0;
		}

#ifdef DEBUG
		if (correctOption == true) {
			Serial.print(F("\r\nBank: ")); Serial.print(bank); Serial.print(F(" -> Turn ON switch 1 to confirm"));
		}
		else {
			Serial.print(F("\r\nIncorrect Bank"));
		}
#endif
		
		//if no correct option was chosen, jump to the next iteration without executing following code
		if (correctOption == false) {continue;}

		/* --- 3. Confirm with switch 1 ON --- */
		do {
			readSwitches(false);

			if (swValue == oldSwValue) {
				continue;	//jump to the next iteration without executing bellow code
			}

			//switch 1 was turned ON?
			if (swValue == (swBank + ON_OFF_OFF_OFF)) {
#ifdef DEBUG
				Serial.println(F(" -> OK!"));
#endif

				turnOffLEDs();		//turn off all lights
				confirmLight();		//show confirmation light
				return(bank);
			}
			//if sw1 was not the toggled switch, then start again
			else {
#ifdef DEBUG
				Serial.print(F("\r\nIncorrect Bank"));
#endif
				turnOffLEDs();
				correctOption = false;
				break;	//exit DO
			}
		} while (true);
	} while (true);
}

/*
 * Routine to choose which sequence number to write in the current step
 * Sw 2, 3 and 4 used to choose sequence from 0 to 7. Confirm turning ON sw 1
 */
byte getSeqNumber(void) {
	bool confirm = false;
	byte seq = 0;	//sequence chosen

	/* --- 1. Wait for all the switches to be OFF --- */
#ifdef DEBUG
	Serial.print(F("1. "));	//next part of the text is printed on the follwing function
#endif

	waitAllSwOff();

	/* --- 2. Choose a sequence --- */
#ifdef DEBUG
	Serial.print(F("2. Choose sequence"));
	Serial.print(F("\r\nSeq: 0 -> Turn ON switch 1 to confirm"));
#endif

	//we expect the user to choose a sequence, and confirm with the sw 1 ON
	do {
		readSwitches(false);

		//if no changes in the switches, jump to the next iteration without executing following code
		if (swValue == oldSwValue) { continue; }

		turnOffLEDs();	//turn off all lights

		switch (swValue) {
		case OFF_OFF_OFF_OFF:	//sq 0
			seq = 0;
			break;

		case OFF_ON_OFF_OFF:	//sq 1
			SHOW_LED(1, true);
			seq = 1;
			break;

		case OFF_OFF_ON_OFF:	//sq 2
			SHOW_LED(2, true);
			seq = 2;
			break;

		case OFF_ON_ON_OFF:		//sq 3
			SHOW_LED(1, true);
			SHOW_LED(2, true);
			seq = 3;
			break;

		case OFF_OFF_OFF_ON:	//sq 4
			SHOW_LED(3, true);
			seq = 4;
			break;

		case OFF_ON_OFF_ON:		//sq 5
			SHOW_LED(1, true);
			SHOW_LED(3, true);
			seq = 5;
			break;

		case OFF_OFF_ON_ON:		//sq 6
			SHOW_LED(2, true);
			SHOW_LED(3, true);
			seq = 6;
			break;

		case OFF_ON_ON_ON:		//sq 7
			SHOW_LED(1, true);
			SHOW_LED(2, true);
			SHOW_LED(3, true);
			seq = 7;
			break;

		default:	//all other possible combinations means that the sw 1 was activated, so we confirm
			confirm = true;
		}

#ifdef DEBUG
		if (confirm == false) {
			Serial.print(F("\r\nSeq: "));
			Serial.print(seq);
			Serial.print(F(" -> Turn ON switch 1 to confirm"));
		}
#endif
		
		if (confirm == true) {
#ifdef DEBUG
			Serial.print(F(" -> OK!"));
#endif
			confirmLight();
			return(seq);
		}
	} while (true);
}

/*
 * Write sequence in the corresponding step and bank
 * Write empty value on the next possition, so when you want to finish
 * the programming, you just turno off the circuit
 */
void writeStep(byte bk, byte st, byte sq) {
	EEPROM.write(bankStart[bk-2] + st, sq);
	EEPROM.write(bankStart[bk-2] + st + 1, EMPTY_POS);	//write empty possition in the next byte

#ifdef DEBUG
	Serial.print(F("\r\nWriting (seq ")); Serial.print(sq); Serial.print(F(") in (step ")); Serial.print(st); Serial.print(F(") of (bank ")); Serial.print(bk); Serial.println(F(")"));
#endif
}

/*
 * Check all switches are OFF
 */
void waitAllSwOff(void) {
#ifdef DEBUG
	Serial.print(F("Waiting all switches to be OFF"));
#endif

	do {
		blinkLEDs();			//blink LEDs
		readSwitches(false);	//read switches
	} while (swValue != 0);

	turnOffLEDs();				//turn off all LEDs

#ifdef DEBUG
		Serial.println(F(" -> OK!"));
#endif
}

/*
 * Blink LEDs
 * Used to show user that is waiting for all switches off
 */
void blinkLEDs(void) {
	//toogle LED state
	for (byte y = 0; y < 4; y++) {
		digitalWrite(led[y], !digitalRead(led[y]));
	}

	delay(100);
}

/*
 * Show a confirmation light
 */
void confirmLight(void) {
	//blink first led
	SHOW_LED(0, true);
	delay(500);
	SHOW_LED(0, false);
	delay(500);

	turnOffLEDs();	//not needed, but just in case
}

/*
 * Show light effect to indicate programming mode started
 */
void progStartLights(void) {
	turnOffLEDs();	//turn off all lights
	delay(100);

	//turn on LEDs one by one
	for (byte y = 0; y < 4; y++) {
		SHOW_LED(y, true);
		delay(100);
	}

	//turn off LEDs one by one
	for (byte y = 0; y < 4; y++) {
		SHOW_LED(y, false);
		delay(100);
	}
}

/*
 * Show light effect to indicate programming memory has reached the end
 * Play forever to force restart
 */
void memEndLights(void) {
	turnOffLEDs();									//turn off all LEDs
	delay(100);

#ifdef DEBUG
	Serial.print(F("\r\n-Programming reach the end"));
	Serial.print(F("\r\nAll values saved. Please restart"));
#endif

	do {
		//move light from left to right
		for (byte y = 0; y < 4; y++) {
			turnOffLEDs();					//turn off all LEDs
			SHOW_LED(y, true);
			delay(100);
		}

		//move light from right to left
		for (byte y = 0; y < 4; y++) {
			turnOffLEDs();					//turn off all LEDs
			SHOW_LED(3-y, true);
			delay(100);
		}
	} while (true);
}

/*
 * Load the corresponding sequence into the queue
 */
void loadSequenceQueue(void) {
	byte swStartValue = 0;
	byte bankLoaded = 0;

	//read status of all switches
	for (byte x = 0; x < 4; x++) {
		if (GET_SW_STATE(x) == true) {
			swStartValue = swStartValue + (1 << x);
		}
	}

#ifdef DEBUG
	Serial.print(F("Bank "));
#endif

	switch (swStartValue) {
	/* no sw -> bank 0 */
	case OFF_OFF_OFF_OFF:
		bankLoaded = loadPreprogrammed(0);
		break;

	/* sw1 -> bank 1 */
	case ON_OFF_OFF_OFF:
		bankLoaded = loadPreprogrammed(1);
		break;

	/* sw2 -> bank 2 */
	case OFF_ON_OFF_OFF:
		bankLoaded = loadFromBank(2);
		break;

	/* sw3 -> bank 3 */
	case OFF_OFF_ON_OFF:
		bankLoaded = loadFromBank(3);
		break;

	/* sw4 -> bank 4 */
	case OFF_OFF_OFF_ON:
		bankLoaded = loadFromBank(4);
		break;

	/* default -> bank 0 */
	default:
		bankLoaded = loadPreprogrammed(0);
	}

#ifdef DEBUG
	Serial.print(bankLoaded);
	Serial.print(F(" loaded: ( "));

	for (byte x = 0; x < BANK_LIMIT; x++) {
		if (seqQueue[x] != EMPTY_POS) {
			if (x > 0) {
				Serial.print(F(" - "));
			}

			Serial.print(seqQueue[x]);
		}
		else {
			break;	//exit FOR
		}
	}

	Serial.println(F(" )"));
#endif
}

/*
 * Load one of the pre programmed sequences
 * b is 0 or 1
 */
byte loadPreprogrammed(byte b) {

	//load pre programmed bank 1
	if (b == 1) {
		for (byte x = 0; x < sizeof(seqQueue1); x++) {
			seqQueue[x] = seqQueue1[x];
		}

		return(1);
	}

#ifdef DEBUG
	//if bank selected is not 0 or 1 show message
	//we should never reach this point, but just in case
	if (b != 0) {
		Serial.print(b);
		Serial.print(" selected but out of range!\r\nBank ");
	}
#endif

	//in any other case load pre programmed bank 0
	for (byte x = 0; x < sizeof(seqQueue0); x++) {
		seqQueue[x] = seqQueue0[x];
	}

	return(0);
	
}

/*
 * Load one of the bank sequences
 * b is 2, 3 or 4
 */
byte loadFromBank(byte b) {
	byte readSeq = EMPTY_POS;
	byte start = bankStart[b - 2];
	byte end = start + BANK_LIMIT;
	byte queuePos = 0;

	if ((b == 2) || (b == 3) || (b == 4)) {
		for (byte x = start; x < end; x++, queuePos++) {
			readSeq = EEPROM.read(x);	//read possition

			//if possition is not empty, store in queue
			if (readSeq != EMPTY_POS) {
				seqQueue[queuePos] = readSeq;

				//we reached the end of the memory without encountering any EMPTY_POS
				if (x == end - 1) {
					return(b);
				}
			}
			//if its empty -> end
			else {
				//but if its empty and its first possition (nothing in bank), then load bank 0
				if ((queuePos == 0) || (queuePos == 1)) {
#ifdef DEBUG
					Serial.print(b);
					Serial.print(" selected but empty!\r\nBank ");
#endif
					loadPreprogrammed(0);
					return (0);
				}
				else {
					return (b);
				}
			}
		}
	}

	//we should never reach this point, but just in case
#ifdef DEBUG
	//show message
	Serial.print(b);
	Serial.print(" selected but out of range!\r\nBank ");
#endif

	//load default
	loadPreprogrammed(0);
	return (0);
}

/*
 * Erase all the values from the selected bank
 * "b" is the bank (2, 3 or 4)
 */
void eraseBank(byte b) {
	byte start = bankStart[b-2];
	byte end = start + BANK_LIMIT;

	for (byte x = start; x < end; x++) {
		EEPROM.write(x, EMPTY_POS);
	}

#ifdef DEBUG
	Serial.print("Bank "); Serial.print(b); Serial.println(" erased");
#endif
}