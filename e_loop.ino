/*
 * Main program
 */
void loop() {
	programmingMode();	//enter programming mode if code is correct
	pairSwitches();		//pair switches and LEDs if its a new sequence
	readSwitches();		//read new value of switches
	turnOnLEDs();		//turn on LEDs according to the current sequence
	checkCodeMatch();	//check if code matches the unlock code or the next sequence code
}
