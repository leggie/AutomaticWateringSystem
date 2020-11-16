// called by state SHOW_TIME and state SHOW_TIME_WATERING_ON:
// Displays the current date and time, and also an WateringTime indication
// e.g. SAT 04 JAN 2014, 22:59:10  WateringTime
//https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit - on how to do bit manipulation
//http://c-faq.com/~scs/cclass/int/sx5.html - How to return arrays from functions
void showVersionOnLCD() {
  lcd.clear();
  lcd.print(F("WateringSystem"));
  lcd.setCursor(0, 1);
  lcd.print(F("Version "));
  lcd.print(SketchVersion);
}
void showTime() {
  //Log.trace(F("In function showTime | "));
  lcd.clear();
  lcd.setCursor(0, 0);
  dateDisplayonLCD();
  lcd.setCursor(0, 1);
  timeDisplayonLCD();
  digitalClockDisplayonLogger();
  if (isWateringActive) {
    lcd.print(F(" WaterON"));
    Log.notice(F("Watering is Active           | "));
  }
  else {
    //lcd.print(F(""));
    Log.notice(F("Watering is Not Active |" CR));
  }
}

//called by state SHOW_PLANT_WATERING_TIME
//Displays the current watering time of the plants.  using the UP and DOWN buttons cycles between the plants.
//Transitions back to show date and time after 2 sec if there is no button press
void showWateringTime() {
  uint8_t iPlantNo = 1;
  showWateringTimeofPlant(1);
  timeRef = millis();
  while ( (unsigned long)(millis() - timeRef) < TIME_OUT * 1000 )  {
    uint8_t button = lcd.button();

    if ( button == KEYPAD_UP )    {
      iPlantNo = (iPlantNo < NOOFPLANTS ? iPlantNo + 1 : 1);
      showWateringTimeofPlant(iPlantNo);  //below in this (LCDFunctions)
      timeRef = millis();
    }
    else if ( button == KEYPAD_DOWN )    {
      iPlantNo = (iPlantNo > 1 ? iPlantNo - 1 : NOOFPLANTS);
      showWateringTimeofPlant(iPlantNo);  //below in this (LCDFunctions)
      timeRef = millis();
    }
    else if ( button == KEYPAD_SELECT )    {
      while ( lcd.button() != KEYPAD_NONE ) ;
      break;
    }
    delay(150); // to run this loop only once in 150 ms.  ef button is held down, will increase/decrease once in 150 ms
  }
  Statetransition(TIME_OUT);
}

// Called only by showWateringTime() above.
// e.g. WaterTime Plant 1 Time HOUR: 08 MIN: 20
void showWateringTimeofPlant(uint8_t PlantNo) {
  lcd.clear();
  lcd.print(F("Plant "));
  lcd.print(PlantNo);  //  refer https://arduino.stackexchange.com/questions/25125/why-do-i-get-invalid-operands-of-types-const-char-error
  lcd.print(": ");
  lcd.print(weekDisplayShort(Plants[PlantNo - 1].getdayOfWeekToWater()));
  lcd.setCursor(0, 1);
  lcd.print(Plants[PlantNo - 1].getwateringTimeFull());
  lcd.print(" Qnt: ");
  lcd.print(Plants[PlantNo - 1].getwateringQuantity());
}

// Called only by showTime() above.
void timeDisplayonLCD() {
  // LCD display of the time
  char sHours[3] = "";
  char sMinutes[3] = "";
  char sSeconds[3] = "";
  lcd.print(formatDigits(hour(), sHours));
  lcd.print(":");
  lcd.print(formatDigits(minute(), sMinutes));
  lcd.print(":");
  lcd.print(formatDigits(second(), sSeconds));
}

// Called only by showTime() above.
void dateDisplayonLCD() {
  char sDay[3] = "";
  lcd.print(dayShortStr(weekday()));
  lcd.print(" ");
  lcd.print(formatDigits(day(), sDay));
  lcd.print(" ");
  lcd.print(monthShortStr(month()));
  lcd.print(" ");
  lcd.print(year());
}

//Uses the LCD up and down buttons to update and return a uint16_t value, a time eelement.
//Used in selectPlantNumber(), setWateringTimeHour(), setWateringTimeMinute(), setWateringAmount() in SetPlantWateringTime.ino
uint16_t LCDUpdateElement(char strTitle[], char strElementName[], uint16_t initialValue, uint16_t minValue, uint16_t maxValue) {
  //Log.trace(F("In function LCDUpdateElement(const char* %s, const char* %s, uint16_t %d, uint16_t %d, uint16_t %d )" CR), strTitle, strElementName, initialValue, minValue, maxValue);
  uint16_t tmpOutput;
  uint8_t ilenofstr;
  timeOut = true;  //  resets timeOut to true for next variable

  lcd.clear();
  lcd.print(strTitle);
  tmpOutput = initialValue;
  lcd.setCursor(0, 1);
  lcd.print(strElementName);
  lcd.print(F(" "));
  lcd.print(initialValue);
  ilenofstr = strlen(strElementName) + 1 ;
  timeRef = millis();   //global variable
  while ( (unsigned long)(millis() - timeRef) < (10 * 1000) )  {
    if ( (button = lcd.button()) != KEYPAD_NONE ) {  //    //if a key is pressed
      //button = lcd.button();
      switch (button) {
        case KEYPAD_UP:
          tmpOutput = tmpOutput < maxValue ? tmpOutput + 1 : minValue;
          lcd.setCursor(ilenofstr, 1);
          //Delete all that was printed in this from this point on
          //for (uint8_t i = 0 ; i < (16 - ilenofstr + 1 ) ; i++) {
          lcd.print(F("  "));
          //}
          lcd.setCursor(ilenofstr, 1);
          lcd.print(tmpOutput);
          timeRef = millis();
          break;
        case KEYPAD_DOWN:
          tmpOutput = tmpOutput > minValue ? tmpOutput - 1 : maxValue;
          lcd.setCursor(ilenofstr, 1);
          lcd.print(F("  "));
          lcd.setCursor(ilenofstr, 1);
          lcd.print(tmpOutput);
          timeRef = millis();
          break;
        case KEYPAD_SELECT:
          while ( lcd.button() != KEYPAD_NONE ) ;
          timeOut = false;
          return tmpOutput;
          break;
      }
    }
    delay(150); // to run this loop only once in 150 ms.  if button is held down, will increase/decrease once in 150 ms
  }
  return tmpOutput;
}

//  overloaded function to be able to use the F() macro.  Calls the function with char*
uint16_t LCDUpdateElement(const __FlashStringHelper * strTitle, const __FlashStringHelper * strElementName, uint16_t initialValue, uint16_t minValue, uint16_t maxValue) {
  //Log.trace(F("In function LCDUpdateElement(const __FlashStringHelper*, const __FlashStringHelper*, uint16_t, uint16_t, uint16_t )" CR));
  //https://stackoverflow.com/questions/22708300/understanding-cryptic-c-statement
  char convstrTitle[16];
  char convstrElementName[16];
  //from https://www.arduino.cc/en/Reference/PROGMEM, ... and modifed!
  //strcpy_P(convstrTitle, (char*)pgm_read_word(&(strTitle)));
  //strcpy_P(convstrElementName, (char*)pgm_read_word(&(strElementName)));
  strcpy_P(convstrTitle, (const char*)strTitle);
  strcpy_P(convstrElementName, (const char*)strElementName);
  //Log.trace(F("After conversion Title is %s and Element Name is %s" CR), convstrTitle, convstrElementName);
  return LCDUpdateElement(convstrTitle, convstrElementName, initialValue, minValue, maxValue);
}

//Uses the LCD left, right, up and down buttons to update and return a uint16_t value, that represents the valid days of week
//Used in setWateringTimeWeekdays() in SetPlantWateringTime.ino
//uint8_t LCDUpdateWeekDays(char strTitle[], uint8_t initialValue) {
uint8_t LCDUpdateWeekDays(uint8_t initialValue) {
  //1   Sketch restarting if the below log.trace is uncometed :-( 
  //Log.trace(F("In function LCDUpdateWeekDays  Memory remaining %d |" CR),freeRam());
  uint8_t tmpOutput;  //if an array had to be returned, this should be static.  See first method in http://c-faq.com/~scs/cclass/int/sx5.html
  uint8_t weekdayposition = 1;  //1 to 7.  1 is Sun
  timeOut = true;  //  resets timeOut to true for next variable
  tmpOutput = initialValue;
  lcd.clear();
  //lcd.setCursor(0, 0);
  lcd.print(F("Select Weekdays"));
  lcd.setCursor(0, 1);
  lcd.print(weekDisplay(tmpOutput));  //callweekDiplay (defined in utility functions) which will convert the code to the week display on the LCD
  lcd.setCursor(((weekdayposition - 1) * 2), 1);  // cursor position on 0, 2, 4 etc.  2(n-1)+1
  //lcd.blink();
  lcd.cursor();
  //2   Sketch restarting if the below log.trace is uncometed :-( 
  //Log.trace(F("Cursor On. Memory remaining %d |" CR),freeRam());
  timeRef = millis();   //global variable
  //3  Sketch restarting if the below log.trace is uncometed :-( 
  //Log.trace(F("After Resetting TimeRef.  TimeRef is %l. Memory remaining %d |" CR),timeRef,freeRam());
  while ( (millis() - timeRef) < (10 * 1000) )  {  //wait for a 10 seconds for a button to be pushed
    //4 Sketch restarting if the below log.trace is uncometed :-( 
    //Log.trace(F("before calling lcd.button.  Memory remaining %d |" CR),freeRam());
    if ( (button = lcd.button()) != KEYPAD_NONE ) {  //    //if a key is pressed
      //5  Sketch restarting if the below log.trace is uncometed :-( 
      //Log.trace(F("Last Button pressed is %d. Memory remaining %d |" CR), button,freeRam());
      switch (button) {
        case KEYPAD_RIGHT:
          weekdayposition = weekdayposition < 7 ? weekdayposition + 1 : 1;
          //Log.trace(F("Button Right.  weekdayposition is now : %d" CR), weekdayposition);
          lcd.setCursor(0, 1);
          lcd.print(weekDisplay(tmpOutput));  //callweekDisplay (defined in UtilityFunctions) which will convert the code to the week display on the LCD
          lcd.setCursor(((weekdayposition - 1) * 2), 1);  // cursor position on 0, 2, 4 etc.  2(n-1+1
          timeRef = millis();
          break;
        case KEYPAD_LEFT:
          weekdayposition = weekdayposition > 1 ? weekdayposition - 1 : 7;
          //Log.trace(F("Button Left.  weekdayposition is now : %d" CR), weekdayposition);
          lcd.setCursor(0, 1);
          lcd.print(weekDisplay(tmpOutput));  //callweekDisplay (defined in UtilityFunctions) which will convert the code to the week display on the LCD
          lcd.setCursor(((weekdayposition - 1) * 2), 1);
          timeRef = millis();
          break;
        case KEYPAD_UP:
          //https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit - on how to do bit manipulation
          tmpOutput ^= 1U << (weekdayposition - 1); //left shifts the first bit set to 1 unsigned to the correct position and uses exclusive or to toggle.  bit with place value 0 (right most) is Sun, bit with place value 6 is Sat
          //Log.trace(F("Button UP.  Output Byte is now : %d" CR), tmpOutput);
          lcd.setCursor(0, 1);
          lcd.print(weekDisplay(tmpOutput));  //callweekDisplay (defined in UtilityFunctions) which will convert the code to the week display on the LCD
          lcd.setCursor(((weekdayposition - 1) * 2), 1);  // cursor position on 0, 2, 4 etc.  2(n-1+1
          timeRef = millis();
          break;
        case KEYPAD_DOWN:
          tmpOutput ^= 1U << (weekdayposition - 1); //left shifts the first bit set to 1 to thw correct position and uses exclusive or to toggle.  bit with place value 0 (left most) is Sun, bit with place value 6 is Sat
          //Log.trace(F("Button DOWN.  Output Byte is now : %d" CR), tmpOutput);
          lcd.setCursor(0, 1);
          lcd.print(weekDisplay(tmpOutput));  //callweekDisplay (defined in UtilityFunctions) which will convert the code to the week display on the LCD
          lcd.setCursor(((weekdayposition - 1) * 2), 1);  // cursor position on 0, 2, 4 etc.  2(n-1+1
          timeRef = millis();
          break;
        case KEYPAD_SELECT:
          while ( lcd.button() != KEYPAD_NONE ) ;
          //Log.trace(F("Button SELECT.  Output Byte is now : %d" CR), tmpOutput);
          timeOut = false;
          //lcd.noBlink();
          lcd.noCursor();
          //Log.trace(F("Exit from function LCDUpdateWeekDays(...)  Memory remaining %d |" CR),freeRam());
          return tmpOutput;
          break;
      }
    }
    delay(150); // to run this loop only once in 150 ms.  if button is held down, will increase/decrease once in 150 ms
  }
  lcd.noBlink();
  return tmpOutput;
}
/*
//  overloaded function to be able to use the F() macro.  Calls the function with const char*
uint8_t LCDUpdateWeekDays(const __FlashStringHelper * strTitle, uint8_t initialValue) {
  char convstrTitle[16];
  strcpy_P(convstrTitle, (const char*)strTitle);
  //Log.trace(F("In function LCDUpdateWeekDays(const __FlashStringHelper*, uint8_t). String converted to %s" CR),convstrTitle);
  return LCDUpdateWeekDays(convstrTitle, initialValue);
}
*/
/*  The Different Triggers
  In LCD.h
  #define KEYPAD_NONE -1
  #define KEYPAD_RIGHT 0
  #define KEYPAD_UP 1
  #define KEYPAD_DOWN 2
  #define KEYPAD_LEFT 3
  #define KEYPAD_SELECT 4

  in Automaticwateringsystem
  #define TIME_OUT 5                      // One of the system's FSM transitions
  #define WATERING_TIME_MET 6             // One of the system's FSM transitions
*/