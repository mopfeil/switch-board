/*
 * SEQUENCE 0 -> NORMAL LEDS WITH SWITCHES
 *	Each switch works with its adjacent LED
 */
void switchPairing0(void) {
#ifdef DEBUG
	Serial.println(F("NORMALY WORKING SWITCHES <\r\n"));
#endif

	//assign each switch to each LED
	for (byte x = 0; x < 4; x++) {
		swColor[x] = x + 1;
		ledColor[x] = x + 1;
	}

	//clear double cap assignments
	doubleCapColor = EMPTY_COLOR;
	doubleCapPos = EMPTY_POS;

	swPairDone = true;  //switches already programmed
}

/*
 * SEQUENCE 1 -> MIX COLORS
 *	Turn on the switches according to the LED possitions
 *	a. Mix caps, LEDs or both and place in random possition
 *	b. First switch on will be paired with LED in possition PAIR_FIRST
 *	c. Second switch on will be paired with LED in possition PAIR_SECOND
 *	d. Third switch on will be paired with LED in possition PAIR_THIRD
 *	e. Fourth switch on will be paired with LED in possition PAIR_FOURTH
 *	f. At any time you can turn on or off any switch previously paired. This will not affect following pairing steps
 */
void switchPairing1(void) {
#ifdef DEBUG
	Serial.println(F("MIX COLORS <"));
	Serial.print(F("Pair color of the switches according to the color of the LEDs in this LED order: "));
	Serial.print(PAIR_FIRST);
	Serial.print(F(" - "));
	Serial.print(PAIR_SECOND);
	Serial.print(F(" - "));
	Serial.print(PAIR_THIRD);
	Serial.print(F(" - "));
	Serial.println(PAIR_FOURTH);
	Serial.println("");
#endif

	byte swStep = 0;
	
	//restore LED colors so pairing is done correctly
	for (byte L = 0; L < 4; L++) {
		ledColor[L] = L + 1;
	}

	clearSwColors();	//remove switch assignments

	//clear double cap assignments
	doubleCapColor = EMPTY_COLOR;
	doubleCapPos = EMPTY_POS;

	do {
		//this loop runs through every switch to check its status, one by one
		for (byte S = 0; S < 4; S++) {
			//this switch is ON, and no LED has been assigned to it?
			if ((GET_SW_STATE(S) == true) && (swColor[S] == EMPTY_COLOR)) {
				switch (swStep) {
					//the the first switch, we pair it with PAIR_FIRST
				case 0:
					swColor[S] = PAIR_FIRST;
					swStep++;
					break;

					//the the second switch, we pair it with PAIR_SECOND
				case 1:
					swColor[S] = PAIR_SECOND;
					swStep++;
					break;

					//the the third switch, we pair it with PAIR_THIRD
				case 2:
					swColor[S] = PAIR_THIRD;
					swStep++;
					break;

					//the the fourth switch, we pair it with PAIR_FOURTH
				case 3:
					swColor[S] = PAIR_FOURTH;
					swStep++;
					break;

				}
#ifdef DEBUG
				//show the switch assigned to the LED
				Serial.print(F("Switch "));
				Serial.print(S + 1);
				Serial.print(F(" -> LED "));
				Serial.println(swColor[S]);

				if (swStep == 4) {
					Serial.println();
				}
#endif
			}
			else {
				//switches with already asigned LED, can be turned ON and OFF even if programming didnt finish
				if (swColor[S] != EMPTY_COLOR) {
					showSwitch(S);
				}
			}
		}
	} while (swStep != 4);

	swPairDone = true;  //switches already programmed
}

/*
 * SEQUENCE 2 -> EXCHANGE 2 LED POSSITIONS
 *	First two switches ON will exchange LED assignment possition after SHORT_WAIT
 *	a. Choose any two switches and turn them on
 *	b. You can turn on or off any other swich after
 *	c. Leave the first two switches both on or both off
 *	d. Exchange possition of both LEDs from the first two switches
 *	e. After SHORT_WAIT the LEDs will exchange switch assignment
 *	f. All other switches still work normaly
 */
void switchPairing2(void) {
	bool exitLoop = false;			//flag to exit the loop
	byte firstSw = EMPTY_POS;		//first switch turned on
	byte secondSw = EMPTY_POS;		//second switch turned on
	byte ledColor1 = EMPTY_COLOR;	//color stored
	byte ledPos1 = EMPTY_POS;		//we use to exchange the LEDs assignment
	byte ledColor2 = EMPTY_COLOR;	//color stored
	byte ledPos2 = EMPTY_POS;		//we use to exchange the LEDs assignment
	uint32_t endTime = 0;			//used to count the timeout

#ifdef DEBUG
	Serial.println(F("EXCHANGE TWO LED POSITIONS <"));
	Serial.println(F("First two switches turned on will exchange their assigned LED possitions\r\n"));
#endif

	/* --- Check the first two switches turned ON --- */
	do {
		for (byte x = 0; x < 4; x++) {
			//is this switch ON?
			if (GET_SW_STATE(x) == true) {
				//is this the first switch we turned ON?
				if (firstSw == EMPTY_POS) {
					firstSw = x;
#ifdef DEBUG
					Serial.print(F("First switch: "));
					Serial.println(firstSw + 1);
#endif
				}
				//is this the second switch ON and its not the same than first one?
				else if ((secondSw == EMPTY_POS) && (firstSw != x)) {
					secondSw = x;
#ifdef DEBUG
					Serial.print(F("Second switch: "));
					Serial.println(secondSw + 1);
					Serial.println("");
#endif
				}
			}

			showSwitch(x);  //turn ON or OFF the corresponding LED
		}
	} while (secondSw == EMPTY_POS);

	//find which color LED is assigned to the first switch
	for (byte x = 0; x < 4; x++) {
		if (ledColor[x] == swColor[firstSw]) {
			ledColor1 = ledColor[x];	//store the color of the first match
			ledPos1 = x;				//store the position of the first match
			break;
		}
	}

	//find which color LED is assigned to the second switch
	for (byte x = 0; x < 4; x++) {
		if (ledColor[x] == swColor[secondSw]) {
			ledColor2 = ledColor[x];
			ledPos2 = x;
			break;
		}
	}

	/* --- Both switches must remain on or off for SHORT_WAIT seconds --- */
#ifdef DEBUG
	Serial.println(F("Now both switches should remain on or off"));
	Serial.print(F("Exchange LED "));
	Serial.print(ledPos1 + 1);
	Serial.print(F(" <-> LED "));
	Serial.print(ledPos2 + 1);
	Serial.println(F(" during this time"));
#endif

	//check switches for timeout
	do {
		readSwitches(false);	//read new value of switches
		turnOnLEDs();			//turn on LEDs

		//to start the timeout both switches from previous step has to be both ON or both OFF
		if ((swValue == oldSwValue) && (
			((GET_SW_STATE(firstSw) == true) && (GET_SW_STATE(secondSw) == true)) ||
			((GET_SW_STATE(firstSw) == false) && (GET_SW_STATE(secondSw) == false))
			)) {

			if (endTime == 0) {
#ifdef DEBUG
				Serial.print(F("Waiting seconds: "));
				Serial.println(SHORT_WAIT);
#endif
				endTime = millis() + (SHORT_WAIT * 1000);
			}

			//if the timeout finished we can exit the loop
			if (millis() > endTime) {
				exitLoop = true;
			}
		}
		//some switch changed state, or 1º and 2º are not in the same state
		else {
			endTime = 0;	//restart timeout
		}
	} while (exitLoop == false);

#ifdef DEBUG
	Serial.println("");
#endif
	//exchange color positions
	ledColor[ledPos1] = ledColor2;
	ledColor[ledPos2] = ledColor1;

	swPairDone = true;  //switches already programmed
}

/*
 * SEQUENCE 3 -> REMOVE ONE CAP, MAKE THE SWITCH WHITE_COLOR
 *	First switch ON will become WHITE_COLOR after SHORT_WAIT
 *	a. Choose any color switch cap
 *	b. Check that the switch turns corresponding color LED
 *	c. Remove the switch cap (and wait 3 seconds)
 *		c.1. If cap is removed while LED is OFF, then swich will no longer turn on the LED
 *		c.2. If cap is removed while LED is ON, then LED will slowly dim OFF and no longer will turn ON with the switch
 *	d. All other switches still work normaly
 */
void switchPairing3(void) {
	boolean exitLoop = false;	//flag to exit the loop
	byte firstSw = EMPTY_COLOR;	//first switch turned on
	uint32_t endTime = 0;		//used to count the timeout

#ifdef DEBUG
	Serial.println(F("REMOVE ONE SWITCH CAP <"));
	Serial.println(F("First switch on will become WHITE_COLOR\r\n"));
#endif
#warning "que pasa cuando quitamos un capuchon doble y que pasa si hay 2 interruptores blancos?"
	/* --- Check first switch turned ON --- */
	do {
		for (byte x = 0; x < 4; x++) {
			//is this switch ON?
			if (GET_SW_STATE(x) == true) {
				firstSw = x;
				showSwitch(x);
				break;
			}
		}
	} while (firstSw == EMPTY_COLOR);

#ifdef DEBUG
	Serial.print(F("Switch "));
	Serial.print(firstSw + 1);
	Serial.println(F(" selected"));
#endif

	/* --- Count SHORT_WAIT without touching the switches before continue --- */
	do {
		readSwitches(false);	//read new value of switches
		turnOnLEDs();	//turn on LEDs according to the current sequence

		//to start the timeout we have to stop touching the switches for SHORT_WAIT
		if (swValue == oldSwValue) {
			if (endTime == 0) {
#ifdef DEBUG
				Serial.print(F("Waiting seconds: "));
				Serial.println(SHORT_WAIT);
#endif
				endTime = millis() + (SHORT_WAIT * 1000);
			}

			//if the timeout finished we can exit the loop
			if (millis() > endTime) {
				exitLoop = true;
			}
		}
		//some switch changed state
		else {
			endTime = 0; //restart timeout
		}
	} while (exitLoop == false);

#ifdef DEBUG
	Serial.println("");
#endif

	/* --- If the LED is ON while removing assignment, slowly dim the LED --- */
	/* --- Color is fade out from one or more LEDs with that color --- */
	/* --- Color is fade out from a second color of the switch --- */
	/* --- If there is a white LED, dim on at the same time --- */
	if (GET_SW_STATE(firstSw) == true) {
		for (uint16_t x = FADE_OUT_TIME; x > 0; x--) {
			//check which LEDs were the same color as the switch, and turn ON
			for (byte y = 0; y < 4; y++) {
				//fade out LED from firstSw
				if (swColor[firstSw] == ledColor[y]) {
					SHOW_LED(y, true);
				}

				//there is a double cap on our switch?
				if (doubleCapPos == firstSw) {
					//fade out second color of the switch
					if (ledColor[y] == doubleCapColor) {
						SHOW_LED(y, true);
					}
				}

				//fade in white LEDs
				if (ledColor[y] == WHITE_COLOR) {
					SHOW_LED(y, false);
				}
			}
			delayMicroseconds(x);

			//check which LEDs were the same color as the switch, and turn OFF
			for (byte y = 0; y < 4; y++) {
				//fade out LED from firstSw
				if (swColor[firstSw] == ledColor[y]) {
					SHOW_LED(y, false);
				}

				//there is a double cap on our switch?
				if (doubleCapPos == firstSw) {
					//fade out second color of the switch
					if (ledColor[y] == doubleCapColor) {
						SHOW_LED(y, false);
					}
				}

				//fade in white LEDs
				if (ledColor[y] == WHITE_COLOR) {
					SHOW_LED(y, true);
				}
			}
			delayMicroseconds(FADE_OUT_TIME - x);
		}
	}

	//clear switch color
	swColor[firstSw] = WHITE_COLOR;

	//there is a double cap?
	if (doubleCapPos == firstSw) {
		//clear double cap assignments
		doubleCapColor = EMPTY_COLOR;
		doubleCapPos = EMPTY_POS;
	}

#ifdef DEBUG
	Serial.print(F("SWITCH "));
	Serial.print(firstSw + 1);
	Serial.println(F(" is now WHITE\r\n"));
#endif

	swPairDone = true;  //switches already programmed
}

/*
 * SEQUENCE 4 -> REPEATED COLOR LED
 *	Remove one color LED, and add other (repeated) color LED. First switch will not work, second switch will turn 2 LEDs
 *	a. Choose and remove any color LED bulb
 *	b. Add a repeated color LED in that place.
 *		In case there is more than one LED of this color, the first match from the left is the one that should be replaced
 *	c. First switch turned on will not turn on any LED
 *	d. Second switch turned on will turn on 2 LEDs
 *	e. All other switches still work normaly
 */
void switchPairing4(void) {
	bool exitLoop = false;			//flag to exit the loop
	byte firstSw = EMPTY_POS;		//first switch turned on
	byte ledPos = EMPTY_POS;		//position of the duplicated led

#ifdef DEBUG
	Serial.println(F("DUPLICATED COLOR LED <"));
	Serial.println(F("Remove one LED and replace with a repeated color LED"));
	Serial.println(F("Once LED is replaced, turn on the switch that used to work with the removed LED\r\n"));
#endif

#warning "Que ocurre si el interruptor desparejado tiene 2 leds asignados?"

	/* --- Check the first switch turned ON --- */
	do {
		for (byte x = 0; x < 4; x++) {
			//is this switch ON?
			if (GET_SW_STATE(x) == true) {
				exitLoop = true;
				firstSw = x;

				//find the first LED turned ON by this switch
				for (byte y = 0; y < 4; y++) {
					if (swColor[firstSw] == ledColor[y]) {
						ledPos = y;
						break;	//exit second for
					}
				}

#ifdef DEBUG
				Serial.print(F("Remove assignment for LED "));
				Serial.println(ledPos + 1);
#endif

				break;	//exit first for
			}
		}
	} while (exitLoop == false);

	exitLoop = false;

	/* --- Check the second switch turned ON, and assign previous saved LED --- */
#ifdef DEBUG
	Serial.println(F("Next switch on will turn on both LEDs"));
#endif

	do {
		for (byte x = 0; x < 4; x++) {
			//is this switch ON?
			if ((GET_SW_STATE(x) == true) && (x != firstSw)) {
				exitLoop = true;
				ledColor[ledPos] = swColor[x]; //asign new color to previous led possition

#ifdef DEBUG
				Serial.print(F("Previous LED reasigned to SWITCH "));
				Serial.println(x + 1);
				Serial.println("");
#endif
				break;	//exit for
			}
		}
	} while (exitLoop == false);

	swPairDone = true;  //switches already programmed
}

/*
 * SEQUENCE 5 -> EXCHANGE COLOR LED FOR A WHITE LED
 *	Exchange color LED for a white one
 *	a. Choose and remove any color LED (in case there is more than one LED of the same color, you must remove the left most one
 *	b. Add a white LED in its place
 *	c. Turn on the switch corresponding to the removed color LED to indicate which color was removed
 *		If LED removed has no switch of the  same color, turn on white switch (from sequence 3)
 *	d. All other switches still work normaly
 */
void switchPairing5(void) {
	bool exitLoop = false;		//flag to exit the loop
	byte whiteSw = EMPTY_POS;	//check if there is any white switch
	byte ledPos = EMPTY_POS;

#ifdef DEBUG
	Serial.println(F("EXCHANGE COLOR LED FOR WHITE LED <"));
	Serial.println(F("Remove a color LED and replace with a white LED"));
#endif

	/* --- Check if any switch can turn ON white LEDs --- */
	for (byte S = 0; S < 4; S++) {
		if (swColor[S] == WHITE_COLOR) {
			whiteSw = S;
		}
	}

	/* --- Check the first switch turned ON to indicate possition of white LED --- */
#ifdef DEBUG
	Serial.println(F("Turn on switch of the same color as the removed LED\r\n"));
#endif

#warning "Que pasa si se enciende un interruptor sin led emparejado?"

	do {
		//this FOR is checking the status of the switches
		for (byte x = 0; x < 4; x++) {
			//is this switch ON?
			if (GET_SW_STATE(x) == true) {
				exitLoop = true;

				//switch is white?
				//this can happen if replaced LED is the one that has been "unpaired"
				//when removing one switch cap on seq3
				if (swColor[x] == WHITE_COLOR) {
					//we need to find the first bulb with no matching switch
					//and assume this is the white one
					for (byte L = 0; L < 4; L++) {
						for (byte S = 0; S < 4; S++) {
							//if LED color match with switch color, then go to next LED
							if (ledColor[L] == swColor[S]) {
								break;	//exit -S- FOR
							}
							//if we reached the end and LED does not match
							//with any swich, this is the LED we are looking for!
							else {
								if (S == 3) {
									ledPos = L;
									ledColor[L] = WHITE_COLOR;
									break;	//exit -S- FOR - Not needed?
								}
							}
						}

						//if ledPos is not empty, means we found the replaced LED
						if (ledPos != EMPTY_POS) {
							break;	//exit -L- FOR
						}
					}
				}
				//any color switch
				else {
					//find the first LED that can be turned on with this switch
					for (byte L = 0; L < 4; L++) {
						if (swColor[x] == ledColor[L]) {
							ledPos = L;
							ledColor[L] = WHITE_COLOR;
							break;	//exit -L- FOR
						}
					}
				}
#ifdef DEBUG
				Serial.print(F("Change to white -> LED "));
				Serial.println(ledPos + 1);
#endif
				break;	//exit -x- FOR
			}
		}
	} while (exitLoop == false);

//	exitLoop = false;
//
//	/* --- If there is no white switch, next switch turned ON will become white --- */
//	if (whiteSw == EMPTY_POS) {
//#ifdef DEBUG
//		Serial.println(F("There is no white switch"));
//		Serial.println(F("Turn off all switches, and first switch turned on will become white\r\n"));
//#endif
//		//wait for all the switches to be turned OFF
//		while (swValue != OFF_OFF_OFF_OFF) {
//			readSwitches(false);	//read new value of switches
//			turnOnLEDs();			//turn on LEDs according to the current sequence
//		}
//
//		//next switch ON will "become" white
//		do {
//			for (byte S = 0; S < 4; S++) {
//				//is this switch ON?
//				if (GET_SW_STATE(S) == true) {
//					swColor[S] = WHITE_COLOR;
//					exitLoop = true;
//
//#ifdef DEBUG
//					Serial.print(F("SWITCH "));
//					Serial.print(S + 1);
//					Serial.print(F(" now turn ON white LEDs"));
//					Serial.println(F("\r\n"));
//#endif
//					break;	//exit FOR
//				}
//			}
//		} while (exitLoop == false);
//	}
//#ifdef DEBUG
//	/* --- There is one white switch, nothing to do --- */
//	else {
//		Serial.print(F("SWITCH "));
//		Serial.print(whiteSw + 1);
//		Serial.print(F(" can turn on white LED "));
//		Serial.print(ledPos + 1);
//		Serial.println(F("\r\n"));
//	}
//#endif

	swPairDone = true;  //switches already programmed
}

/*
 * EFFECT 6 - > REMOVE ALL SWITCH CAPS --- OLD ROUTINE
 *	Remove all the color switch caps, one by one, and the matching color bulb will fade out.
 *	At the end you have 4 uncapped switches. This will not work with any color bulb, but if there
 *	is any white bulb, it will be activated by every switch.
 *	a. Turn on all switches
 *	b. Starting from right to left, remove the first cap
 *	c. If there is any bulb matching this cap it will fade out after SHORT_WAIT
 *	d. Repeat step b and c, continue removing caps from right to left. If at any point there is no cap, jump to the next
 *	e. Finally there will be no caps. If there is any white bulb, this can be activated with every switch
 *	f. After entering the jump code and turning off all switches, you have LONG_WAIT time to remove the white bulbs
 *		(if there is any, otherwise they will turn off after the time, and that will be a strange behaviour)
 */
//void switchPairing6(void) {
//	bool exitLoop = false;
//	byte swLastStatus;
//	uint32_t endTime = 0;
//
//#ifdef DEBUG
//	Serial.println(F("REMOVE ALL SWITCH CAPS <\r\n"));
//	Serial.println(F("Waiting for all switches to be ON\r\n"));
//#endif
//
//	/* --- Wait for all the switches to be turned ON --- */
//	while (swValue != ON_ON_ON_ON) {
//		readSwitches(false);	//read new value of switches
//		turnOnLEDs();			//turn on LEDs according to the current sequence
//	}
//
//	//do the same for all the switches, starting from the right to the left (0<1<2<3)
//	for (byte x = 3; x < 4; x--) {
//		exitLoop = false;
//		swLastStatus = swValue;
//
//		//if switch is white, there is no cap to remove
//		//continue to next switch
//		if (swColor[x] == WHITE_COLOR) {
//			continue;	//go to next switch
//		}
//
//		//count 3 seconds without touching the switches before continue
//		do {
//			readSwitches(false);	//read new value of switches
//			turnOnLEDs();			//turn on LEDs according to the current sequence
//
//			//to start the timeout we need all switches like previous status
//			if (swValue == swLastStatus) {
//				if (endTime == 0) {
//#ifdef DEBUG
//					Serial.print(F("Waiting seconds: "));
//					Serial.println(SHORT_WAIT);
//					Serial.print(F("Remove cap from switch "));
//					Serial.print(x + 1);
//					Serial.println(F(" during this time"));
//#endif
//					endTime = millis() + (SHORT_WAIT * 1000);
//				}
//
//				//if the timeout finished we can exit the loop
//				if (millis() > endTime) {
//					exitLoop = true;
//				}
//			}
//			//some switch changed state, or 1º and 2º are not ON
//			else {
//				endTime = 0; //restart timeout
//			}
//		} while (exitLoop == false);
//
//#ifdef DEBUG
//		Serial.print(F("\r\nDimming OFF LEDs from switch "));
//		Serial.println(x + 1);
//#endif
//
//		//dim OFF LED
//		for (uint16_t y = FADE_OUT_TIME; y > 0; y--) {
//			for (byte z = 0; z < 4; z++) {
//				//check which LEDs were the same color as the switch, and turn ON
//				if (swColor[x] == ledColor[z]) {
//					SHOW_LED(z, true);
//				}
//			}
//			delayMicroseconds(y);
//
//			//check which LEDs were the same color as the switch, and turn OFF
//			for (byte z = 0; z < 4; z++) {
//				if (swColor[x] == ledColor[z]) {
//					SHOW_LED(z, false);
//				}
//			}
//			delayMicroseconds(FADE_OUT_TIME - y);
//		}
//
//		swColor[x] = WHITE_COLOR;
//
//#ifdef DEBUG
//		Serial.print(F("Switch "));
//		Serial.print(x);
//		Serial.println(F(" is now WHITE"));
//#endif
//
//	}
//
//#ifdef DEBUG
//	Serial.println("");
//#endif
//
//	delay(LONG_WAIT);
//
//	swPairDone = true;  //switches already programmed
//}

/*
 * EFFECT 6 -> DOUBLE COLOR SWITCH CAP
 *	Remove one switch cap and place it over another switch. First switch will not work, second switch will turn 2 LEDs
 *	a. Choose and remove any color cap
 *	b. Place the cap over another switch (making it double color)
 *	c. First switch turned on will not turn on any LED
 *	d. Second switch turned on will turn on 2 LEDs
 *	e. All other switches still work normaly
 */
void switchPairing6(void) {
	bool exitLoop = false;				//flag to exit the loop
	byte firstSw = EMPTY_POS;			//first switch turned on

#ifdef DEBUG
	Serial.println(F("DUPLICATED COLOR SWITCH CAP <"));
	Serial.println(F("Remove one switch cap and place it over other switch"));
	Serial.println(F("Once cap is repositioned, turn on the switch that used to work with that cap\r\n"));
#endif

		/* --- Check the first switch turned ON --- */
		do {
			for (byte S = 0; S < 4; S++) {
				//is this switch ON?
				if (GET_SW_STATE(S) == true) {
					exitLoop = true;
					firstSw = S;
					doubleCapColor = swColor[S];	//assigne new color to the double cap
					swColor[S] = WHITE_COLOR;		//assign white color to this switch

#ifdef DEBUG
					Serial.print(F("Switch "));
					Serial.print(S + 1);
					Serial.println(F(" is now white"));
#endif

					break;	//exit first for
				}
			}
		} while (exitLoop == false);

		exitLoop = false;

		/* --- Check the second switch turned ON, and assign previous saved color --- */
#ifdef DEBUG
		Serial.println(F("Next switch on will be assigned previous cap color"));
#endif

		do {
			for (byte S = 0; S < 4; S++) {
				//show status of first switch
				if (S == firstSw) {
					showSwitch(S);
				}
				//wait for second switch to turn on
				else {
					//is this switch ON?
					if ((GET_SW_STATE(S) == true)) {
						exitLoop = true;
						doubleCapPos = S;	//assign new possition to the double cap

						//if the switch was white, then no doubleCap. Just assign color
						if (swColor[S] == WHITE_COLOR) {
							swColor[S] = doubleCapColor;
							doubleCapColor = EMPTY_COLOR;
							doubleCapPos = EMPTY_POS;
						}

#ifdef DEBUG
						Serial.print(F("Previous color reasigned to SWITCH "));
						Serial.println(S + 1);
						Serial.println("");
#endif
						break;	//exit for
					}
				}
			}
		} while (exitLoop == false);

		swPairDone = true;  //switches already programmed
}

/*
 * SEQUENCE 7 -> IN CRESCENDO (divided in phases for code simplicity)
 *
 * Phase 1 -> ONE LED AND ONE CAP
 *	a. Start with no bulbs or caps. Ask for a color and place bulb and cap in different places
 *	b. Turn on all switches leaving the switch with cap for the last
 *	c. Turn off all white switches starting with the switch in the same possition of the LED
 *		to indicate where is it
 *	d. Switches workign normally
 *	e. CAREFUL: all switches on (and wait 3 seconds) will trigger phase 2
 *
 * Phase 2 -> EXCHANGE CAP, LEDS FADE TRANSITION
 *	a. Add any other color bulb 1 possition right to the COLOR_A LED
 *	b. All switches ON will start:
 *		i. LONG_WAIT timeout. If any switch change before timeout, time will stop and return to step "b"
 *			During timeout, we should replace COLOR_A cap for new COLOR_B cap
 *		ii. Fade out COLOR_A LED and fade in COLOR_B LED
 *		iii. SHORT_WAIT delay
 *			During delay we should place COLOR_A cap 2 possitions right to the COLOR_B cap
 *		iv. COLOR_A LED will fade in
 *	c. Switches workign normally
 *	d. CAREFUL: two white switches off (and wait 3 seconds) will trigger phase 3
 *
 * Phase 3 -> TWO WHITE BULBS AND A COLOR CAP
 *	a. Waiting for two empty switches to turn off and SHORT_WAIT to start next
 *	b. Add two white bulb
 *	c. Add new color cap and show dont turn on any bulb
 *	d. Switches workign normally
 *	e. CAREFUL: all switches off (and wait 3 seconds) will trigger phase 4
 *
 * Phase 4 -> REPLACE WHITE BULB WITH COLOR_C
 *	a. Waiting all switch to turn off and wait SHORT_WAIT to start next
 *	b. Replace left most white bulb with COLOR_C bulb
 *	c. Switches workign normally
 *	d. CAREFUL: all switches off (and wait 3 seconds) will trigger phase 5
 *
 * Phase 5 -> BACK TO NORMAL
 *	a. Waiting all switches off (and wait 3 seconds)
 *	b. Put everything back to normal:
 *		i. Replace white LED for COLOR_D LED
 *		ii. Place all caps on the same possition as their matching LED
 *	c. Switches workign normally
 *	d. Finish sequence, finally!
 */
void switchPairing7(void) {

#ifdef DEBUG
	Serial.print(F("CRAZY SEQUENCE <\r\n"));
#endif

	//clear double cap assignments
	doubleCapColor = EMPTY_COLOR;
	doubleCapPos = EMPTY_POS;

	sq7_phase1();
	sq7_phase2();
	sq7_phase3();
	sq7_phase4();
	sq7_phase5();

#ifdef DEBUG
	Serial.println(F("Seq. 7 - END\r\n"));
#endif

	swPairDone = true;  //switches already programmed
}

/*
 * Switch pairing 7, phase 1 -> ONE LED AND ONE CAP
 *	a. Start with no bulbs or caps. Ask for a color and place bulb and cap in different places
 *	b. Turn on all switches leaving the switch with cap for the last
 *	c. Turn off all white switches starting with the switch in the same possition of the LED
 *		to indicate where is it
 *	d. Switches workign normally
 *	e. CAREFUL: all switches on (and wait 3 seconds) will trigger phase 2
 */
void sq7_phase1(void) {
	bool exitLoop = false;
	byte swStep = 0;

#ifdef DEBUG
	Serial.println(F("+ Seq. 7 - Phase 1\r\n"));
	Serial.println(F("Place one color LED and same color cap on any possition"));
	Serial.println(F("Then turn on every switch, and last the one with the cap\r\n"));
#endif

#warning "Mostrar LEDs? asi si hay algun LED encendido de la fase 6 se apaga"

	clearSwColors();	//remove color assignments to the switches

	//all bulbs assigned to COLOR_A
	for (byte S = 0; S < 4; S++) {
		ledColor[S] = COLOR_A;
	}

	/* --- Assign WHITE_COLOR to first three switches, 4th switch will be COLOR_A --- */
	do {
		//this loop runs through every switch to check its status, one by one
		for (byte S = 0; S < 4; S++) {
			//this switch is ON, and no LED has been assigned to it?
			if ((GET_SW_STATE(S) == true) && (swColor[S] == EMPTY_COLOR)) {
				//first three switches are assigned white
				swColor[S] = WHITE_COLOR;
				swStep++;

#ifdef DEBUG
				//show the switch assigned to the LED
				Serial.print(F("Switch "));
				Serial.print(S + 1);
				Serial.println(F(" is now WHITE"));
#endif

				//on third switch on, we assign color to the last unassigned switch
				if (swStep == 3) {
					//there should be only one unassigned switch left
					for (byte x = 0; x < 4; x++) {
						if (swColor[x] == EMPTY_COLOR) {
							swColor[x] = COLOR_A;
							//chosenPos = x;
							exitLoop = true;

#ifdef DEBUG
							Serial.print(F("Switch "));
							Serial.print(x + 1);
							Serial.println(F(" will now turn ON all LEDs\r\n"));
#endif
							break;	//exit small FOR
						}
					}

					break;	//exit bigger FOR
				}
			}
		}
	} while (exitLoop == false);

	exitLoop = false;	//reset variable

	/* --- Wait 4th switch to be turned on (all other switches must be also on) --- */
	do {
		readSwitches(false);	//read new value of switches
		turnOnLEDs();			//turn on LEDs

		if (swValue == ON_ON_ON_ON) {
			exitLoop = true;
		}
	} while (exitLoop == false);

	/* --- 1st switch turned off will indicate where LED is located --- */
#ifdef DEBUG
	showAssignments();
	Serial.println(F("First switch turned off will indicate LED possition"));
#endif

	exitLoop = false;	//reset variable

	do {
		for (byte S = 0; S < 4; S++) {
			if (GET_SW_STATE(S) == false) {
				for (byte x = 0; x < 4; x++) {
					//remove color to all other LED possitions
					if (x != S) {
						ledColor[x] = EMPTY_COLOR;
					}
				}

#ifdef DEBUG
				Serial.print(F("LED "));
				Serial.print(S + 1);
				Serial.println(F(" is now A\r\n"));
				showAssignments();
#endif

				turnOnLEDs();		//show lights with switches
				exitLoop = true;
				break;				//exit FOR
			}
		}
	} while (exitLoop == false);

#ifdef DEBUG
	Serial.println(F("Working normally"));
	showAssignments();
#endif

}

/*
 * Switch pairing 7, phase 2 -> EXCHANGE CAP, LEDS FADE TRANSITION
 *	a. Add any other color bulb 1 possition right to the COLOR_A LED
 *	b. All switches ON will start:
 *		i. LONG_WAIT timeout. If any switch change before timeout, time will stop and return to step "b"
 *			During timeout, we should replace COLOR_A cap for new COLOR_B cap
 *		ii. Fade out COLOR_A LED and fade in COLOR_B LED
 *		iii. SHORT_WAIT delay
 *			During delay we should place COLOR_A cap 2 possitions right to the COLOR_B cap
 *		iv. COLOR_A LED will fade in
 *	c. Switches workign normally
 *	d. CAREFUL: two white switches off (and wait 3 seconds) will trigger phase 3
 */
void sq7_phase2(void) {
	bool exitLoop = false;
	byte firstLED = EMPTY_POS;
	byte secondLED = EMPTY_POS;
	byte firstSw = EMPTY_POS;
	byte secondSw = EMPTY_POS;
	uint32_t endTime = 0;

#ifdef DEBUG
	Serial.println(F("+ Seq. 7 - Phase 2\r\n"));
#endif

	//find out which switch and LED are the ones that have COLOR_A assigned
	for (byte x = 0; x < 4; x++) {
		if (swColor[x] == COLOR_A) {
			firstSw = x;
		}

		if (ledColor[x] == COLOR_A) {
			firstLED = x;
		}
	}

	//increment 1 position firstLED
	secondLED = firstLED + 1;

	//if we overflow, we start from 0 again
	if (secondLED > 3) {
		secondLED = secondLED - 4;
	}

	//increase 2 positions firstSw
	secondSw = firstSw + 2;

	//if we overflow, we start from 0 again
	if (secondSw > 3) {
		secondSw = secondSw - 4;
	}

	/* --- Place new color LED one possition right of COLOR_A LED, ALL SWITCHES ON TO START TIMEOUT --- */
#ifdef DEBUG
	Serial.print(F("Place new color LED on possition "));
	Serial.println(secondLED + 1);
	Serial.println(F("All switches ON to start timeout\r\n"));
#endif

	do {
		readSwitches(false);	//read new value of switches
		turnOnLEDs();			//turn on LEDs

		if (swValue == ON_ON_ON_ON) {
			if (endTime == 0) {
#ifdef DEBUG

				Serial.print(F("Waiting seconds: "));
				Serial.println(LONG_WAIT);
				Serial.print(F("Remove cap from switch "));
				Serial.print(firstSw + 1);
				Serial.println(F(" during this time and replace with new color cap!"));
#endif
				endTime = millis() + (LONG_WAIT * 1000);
			}

			//if the timeout finished we can exit the loop
			if (millis() > endTime) {
				exitLoop = true;
			}
		}
		//some switch changed state, or 1º and 2º are not in the same state
		else {
#ifdef DEBUG
			if (endTime != 0) {
				Serial.println(F("All switches ON to start timeout"));
			}
#endif
			endTime = 0;	//restart timeout
		}
	} while (exitLoop == false);

	//exitLoop = false;	//restart variable

	/* --- During the time out, replace COLOR_A with new COLOR_B cap --- */

	swColor[firstSw] = COLOR_B;	//now first switch is COLOR_B

	/* --- Fade out COLOR_A LED and fade in COLOR_B LED --- */
#ifdef DEBUG
	Serial.print(F("\r\nFading out LED "));
	Serial.println(firstLED + 1);
	Serial.print(F("Fading in LED "));
	Serial.println(secondLED + 1);
#endif

	//fade in secondLED
	//fade out firstLED
	for (uint16_t x = 0; x < CROSS_FADE_TIME; x++) {
		SHOW_LED(secondLED, true);
		SHOW_LED(firstLED, false);
		delayMicroseconds(x);

		SHOW_LED(secondLED, false);
		SHOW_LED(firstLED, true);
		delayMicroseconds(CROSS_FADE_TIME - x);
	}

	SHOW_LED(secondLED, true);	//keep LED on
	SHOW_LED(firstLED, false);	//keep LED off

	ledColor[secondLED] = COLOR_B;	//assign color to the LED

#ifdef DEBUG
	Serial.print(F("Switch "));
	Serial.print(firstSw + 1);
	Serial.println(" is now B\r\n");

	Serial.print(F("Place removed cap on switch possition "));
	Serial.println(secondSw + 1);
	Serial.println("");
#endif

	/* --- Place COLOR_A cap 2 possitions right to the COLOR_B cap. COLOR_A LED fade in again --- */

	delay(LONG_WAIT * 1000);

#ifdef DEBUG
	Serial.print(F("Fading in LED "));
	Serial.println(firstLED + 1);
	Serial.print(F("Switch "));
	Serial.print(secondSw + 1);
	Serial.println(" is now A\r\n");
#endif

	//fade in COLOR_A LED
	for (uint16_t x = 0; x < FADE_IN_TIME; x++) {
		SHOW_LED(firstLED, true);
		delayMicroseconds(x);

		SHOW_LED(firstLED, false);
		delayMicroseconds(FADE_IN_TIME - x);
	}

	SHOW_LED(firstLED, true);	//keep LED on

	swColor[secondSw] = COLOR_A;	//assign new possition to switch

#ifdef DEBUG
	Serial.println(F("Working normally"));
	showAssignments();
#endif
}

/*
 * Switch pairing 7, phase 3 -> TWO WHITE BULBS AND A COLOR CAP
 *	a. Waiting for two empty switches to turn off and SHORT_WAIT to start next
 *	b. Add two white bulb
 *	c. Add new color cap and show dont turn on any bulb
 *	d. Switches workign normally
 *	e. CAREFUL: all switches off (and wait 3 seconds) will trigger phase 4
 */
void sq7_phase3(void) {
	bool exitLoop = false;
	byte whiteSw1 = EMPTY_POS;
	byte whiteSw2 = EMPTY_POS;
	uint32_t endTime = 0;

#ifdef DEBUG
	Serial.println(F("+ Seq. 7 - Phase 3\r\n"));
#endif

	//find out possiiton of white switches
	for (byte S = 0; S < 4; S++) {
		if (swColor[S] == WHITE_COLOR) {
			if (whiteSw1 == EMPTY_COLOR) {
				whiteSw1 = S;
			}
			else {
				whiteSw2 = S;
				break;	//no need to continue
			}
		}
	}

	/* --- Wait for white switches to turn off --- */
#ifdef DEBUG
	Serial.println(F("Turn off two white switches\r\n"));
#endif

	do {
		readSwitches(false);	//read new value of switches
		turnOnLEDs();			//turn on LEDs according to the current sequence

		if ((GET_SW_STATE(whiteSw1) == false) && (GET_SW_STATE(whiteSw2) == false)) {	//white switches are OFF
			if (endTime == 0) {
#ifdef DEBUG
				Serial.print(F("Waiting seconds: "));
				Serial.println(SHORT_WAIT);
				Serial.println("");
#endif
				endTime = millis() + (SHORT_WAIT * 1000);
			}

			//if the timeout finished we can exit the loop
			if (millis() > endTime) {
				exitLoop = true;
			}
		}
		else {				//white switches are not turned OFF
			endTime = 0;	//restart timeout
		}
	} while (exitLoop == false);

#warning "BUG: cuando se apagan los interruptores blancos y pasa el tiempo, luego los otros interruptores no se pueden apagar"

	/* --- Add two white LEDs --- */
#ifdef DEBUG
	Serial.println(F("Add two white LEDs"));
#endif

	//find EMPTY_COLOR LEDs and turn them WHITE_COLOR
	for (byte S = 0; S < 4; S++) {
		if (ledColor[S] == EMPTY_COLOR) {
			ledColor[S] = WHITE_COLOR;
		}
	}

	/* --- Add color cap to any empty switch --- */
#ifdef DEBUG
	Serial.println(F("Add color cap to any empty switch and then turn it on\r\n"));
#endif

	exitLoop = false;	// reset variable

	//first white switch on will become COLOR_C
	do {
		for (byte S = 0; S < 4; S++) {
			if ((S == whiteSw1) || (S == whiteSw2) ){
				if (GET_SW_STATE(S) == true) {
					swColor[S] = COLOR_C;
					exitLoop = true;
				}
			}
			else {
				showSwitch(S);
			}
		}
	} while (exitLoop == false);



	/* --- Switches working normally --- */
#ifdef DEBUG
	Serial.println(F("Working normally"));
	showAssignments();
#endif
}

/*
 * Switch pairing 7, phase 4 -> REPLACE WHITE BULB WITH COLOR_C
 *	a. Waiting all switch to turn off and wait SHORT_WAIT to start next
 *	b. Replace left most white bulb with COLOR_C bulb
 *	c. Switches workign normally
 *	d. CAREFUL: all switches off (and wait 3 seconds) will trigger phase 5
 */
void sq7_phase4(void) {
	bool exitLoop = false;
	uint32_t endTime = 0;

#ifdef DEBUG
	Serial.println(F("+ Seq. 7 - Phase 4\r\n"));
#endif

	/* --- Wait all switches to turn off --- */
#ifdef DEBUG
	Serial.println(F("Turn off all switches to continue\r\n"));
#endif

	do {
		readSwitches(false);	//read new value of switches
		turnOnLEDs();			//turn on LEDs according to the current sequence

		if (swValue == OFF_OFF_OFF_OFF) {	//all switches off
			if (endTime == 0) {
#ifdef DEBUG
				Serial.print(F("Waiting seconds: "));
				Serial.println(SHORT_WAIT);
				Serial.println("");
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

	/* --- Replace left most white LED for COLOR_C LED --- */
#ifdef DEBUG
	Serial.println(F("Replace left most white LED for COLOR_C LED"));
#endif

	//find first WHITE_COLOR LED and turn into COLOR_C
	for (byte S = 0; S < 4; S++) {
		if (ledColor[S] == WHITE_COLOR) {
			ledColor[S] = COLOR_C;

#ifdef DEBUG
			Serial.print(F("LED "));
			Serial.print(S + 1);
			Serial.println(F(" is now C"));
#endif

			break;	//exit FOR
		}
	}

	/* --- Switches working normally --- */
#ifdef DEBUG
	Serial.println(F("\r\nWorking normally"));
	showAssignments();
#endif

	/* --- Wait for some switch to turn on, so it does not jump directly to phase 5 --- */
	do {
		readSwitches(false);	//read new value of switches
		turnOnLEDs();			//turn on LEDs according to the current sequence
	} while (swValue == OFF_OFF_OFF_OFF);
}

/*
 * Switch pairing 7, phase 5 -> BACK TO NORMAL
 *	a. Waiting all switches off (and wait 3 seconds)
 *	b. Put everything back to normal:
 *		i. Replace white LED for COLOR_D LED
 *		ii. Place all caps on the same possition as their matching LED
 *	c. Switches workign normally
 *	d. Finish sequence, finally!
 */
void sq7_phase5(void) {
	bool exitLoop = false;
	uint32_t endTime = 0;

#ifdef DEBUG
	Serial.println(F("+ Seq. 7 - Phase 5\r\n"));
#endif

	/* --- Wait all switches to turn off --- */
#ifdef DEBUG
	Serial.println(F("Turn off all switches to continue\r\n"));
#endif

	do {
		readSwitches(false);	//read new value of switches
		turnOnLEDs();			//turn on LEDs according to the current sequence

		if (swValue == OFF_OFF_OFF_OFF) {	//all switches off
			if (endTime == 0) {
#ifdef DEBUG
				Serial.print(F("Waiting seconds: "));
				Serial.println(SHORT_WAIT);
				Serial.println("");
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

	/* --- Restore LED and switches assignment to the "normal" state --- */
		//assign each switch to each LED
	for (byte x = 0; x < 4; x++) {
		swColor[x] = x + 1;
		ledColor[x] = x + 1;
	}
}