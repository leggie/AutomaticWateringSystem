/**
  Description:
   I wrote this sketch with my Son, Reuben Jacob, for his and his friends freshman project.  This is a house-plant watering system, that can water up to 4 plants.
   The components are a Arduino Uno, an LCD Keypad Shield, a DS1307 Real Time Clock, a 4 port relay board, and 4 12v DC pumps to water 4 plants.
   The design of the code is based on a a Finite state machine (FSM).  The design used the design of the alarm clock sketch 
   from https://github.com/pAIgn10/AlarmClock, Copyright (c) 2014 Nick Lamprianidis) as a starting point.
   The sketch displays the date and time, sets the date and time, and sets the watering times of 4 plants.
   There are 4 modes of operation.  The buttons on the LCD have specific functions in each mode.

   Display Date Time mode: Displays Time, and an indication showing if watering is on or of on the LCD (default mode)
        SELECT: Lets you set the watering times of the 4 plants.
        LEFT: Displays the current alarm time
        RIGHT: Turns the Watering system, on and off.  (If off. the set watering times will not activate the motors)
   SHOW Plant Watering Time mode: Displays the times and days of the week set for the different plants
        UP and DOWN: Cycles through the different watering times.   After 5 (configurable) seconds of inactivity, the clock returns to default mode
   Set Watering Time mode: Sets the weekday and watering times of the plants. Day of week, hours & minutes).  After 5 configurable) seconds of inactivity, the clock returns to default mode.
        In weekday mode, LEFT and RIGHT navigates to the weekday.  UP and DOWN selects/ deselects the day
        In Time mode, UP: Increases hours/minutes.  DOWN: Decreases hours/minutes.
        SELECT: Accepts the weekday/hours/minutes value.
        Goes back to default mode after 5 seconds of inactivity
   Watering Time mode: In this mode, waits for the watering time of each plant to be met.  When met, the plants are watered and the buzzer rings


   ToDo:
    Use the I2C EEPROM on the ZS-042 board using the below library
    DS3241_Simple Library, https://github.com/sleemanj/DS3231_Simple.  The parts of the libray to read temperature and use the 2K I2C EEPROM that is in the chip.
    Change the LCD Librry to https://github.com/DFRobot/LCD-KeyPad-Shield, as the one used seems to be obsolete.  not available from https://forum.arduino.cc/index.php?topic=38061.0

   Last compiled
    ‎April ‎26, ‎2017, ‏‎11:57:22   // V1.0       
    April ‎27, ‎2017, ‏‎01:57:00
    April ‎27, ‎2017, ‏‎16:02:00    // V1.0  Added states SHOW_TIME_WATERING_TIME, WATER_PLANTS.  Added getWateringTimeFull in the Plant class.  Rationalized transition flow
    April ‎27, ‎2017, ‏‎18:18:00    // V1.1  Fixed issues in the selection of the plant.  Calls to the Plant array were going as 1 to 4, instead of 0 to 3
    April ‎27, ‎2017, ‏‎21:33.00    // V1.2   Added EEPROM storage!.  testing failed.  not used in this vesrion
    April ‎27, ‎2017, ‏‎03:00:00    // V1.3   Fixed watering logic.  Updated EEPROM logic.  did not enable
    May   06  2017, 13:20:00    // V2.0   Fixed EEPROM store functionality
    May   06  2017, 21:28:00    // V3.0   Replaced Serial.print with Logger.
    May   08  2017, 01:42:00    // V4.0   Implemented Set time flow
    May   15  2017  00:47:00    // V4.1   Various improvements in logger output.  Set time, long press exits to time set routine after 2 seconds
    May   18  2017  11:57:00    // V4.2   Limited the coming out of the loop behaviour when a key is pressed for more than 2 seconds to only the select key  (the last while loop in the loop function)
                                // V4.2.1 Implementing the dayofweek state.  Removing the String object from code.  WIP
    Sep   16  2017  23:26:00    // V5.0   With Day of Week.  The day of week plants should be watered can be set.  Removed usage of String object to improve memory footprint
    Sep   29  2017  12:30:00    // V5.1   With Quantity in 0.1 second intervals. 
    Nov   05  2017  11:21:00    // V5.1   Finally debugged. (Some (random) log statements restart sketch. Commented them out)  
   
   Libraries used:
    Time Library:       https://www.pjrc.com/teensy/td_libs_Time.html
    DS1307RTC Library:  https://www.pjrc.com/teensy/td_libs_DS1307RTC.html
    LCDKeypad.h:        https://osepp.com/electronic-modules/shields/45-16-2-lcd-display-keypad-shield
    ArduinoLog          https://github.com/thijse/Arduino-Log

   Authors:
    Jacob Oommen (leggie [dot] oommen [at] gmail [dot] com ), Reuben Jacob (Reubeojacob [at] outlook [dot] com)


   License:
    Copyright (c) 2017 Jacob Oommen, Reuben Jacob
    This code is released under the MIT license
    http://www.opensource.org/licenses/mit-license.php

*/
#include "Plant.h"

// create an array of plants.  holds all the values and functions for a plant
Plant Plants[4];

void setup() {
  //setting the values of the digital pins to the 'off' position initially
  pinMode(BUZZER_PIN, OUTPUT);        digitalWrite (BUZZER_PIN, ON);         // Pin for Buzzer pin
  pinMode(WATER_PLANT_1_PIN, OUTPUT); digitalWrite (WATER_PLANT_1_PIN, OFF); // Pin for watering Plant 1
  pinMode(WATER_PLANT_2_PIN, OUTPUT); digitalWrite (WATER_PLANT_2_PIN, OFF); // Pin for watering Plant 2
  pinMode(WATER_PLANT_3_PIN, OUTPUT); digitalWrite (WATER_PLANT_3_PIN, OFF); // Pin for watering Plant 3
  pinMode(WATER_PLANT_4_PIN, OUTPUT); digitalWrite (WATER_PLANT_4_PIN, OFF); // Pin for watering Plant 4

  // Set up serial port and wait until connected
  Serial.begin(115200);
  while (!Serial && !Serial.available()) {}

  //Setup the logger
  // Pass log level, whether to show log level, and print interface.
  // Available levels are:
  // LOG_LEVEL_SILENT  0 ,LOG_LEVEL_FATAL   1,LOG_LEVEL_ERROR   2,LOG_LEVEL_WARNING 3,LOG_LEVEL_NOTICE  4,LOG_LEVEL_TRACE   5,LOG_LEVEL_VERBOSE 6
  // Note: if you want to fully remove all logging code, uncomment #define DISABLE_LOGGING in Logging.h
  //       this will significantly reduce your project size
  //do not show log level - last parameter
  //LOG_LEVEL defined in headers.h
  Log.begin(LOG_LEVEL, &Serial, false);

  // Initializes the LCD and RTC instances
  lcd.begin(16, 2);
  Wire.begin();

  Log.warning(F("BEGIN PROGRAM.  Log Level Notice.  Sketch compiled at %s %s --------------------------------------------------------------------------------------------------------------------------------" CR),__DATE__,__TIME__);
  
  Log.warning(F("Using %s version of the AutomaticWateringSystem sketch |" CR), SketchVersion);
  showVersionOnLCD();
  delay(1000);   //show for 1 seconds!
  
  setSystemTimeFromRTC();  //sets system time from RTC.  function in RTCFunctions

  //gets the plants from EEPROM if stored and valid.  else use default values for plant members
  if (!(getPlantsFromEEPROM())) {
    digitalClockDisplayonLogger();
    Log.notice(F("Loading values for Plants details.|" CR)); 
    //Update Plant number and Pin numbers to default values
    for (uint8_t i = 0; i < 4; i++)
      Plants[i].setDefaults(i + 1);
    //Set is watering active to false
    isWateringActive = false;
    //2
    digitalClockDisplayonLogger();
    plantDetailsonLogger(0);
    //Store the values in EEPROM
    storePlantinEEPROM();
  }

  // Initial state of the FSM. isWateringActive from EEPROM
  if (isWateringActive) {
    state = SHOW_TIME_WATERING_ON;
    digitalClockDisplayonLogger();
    Log.notice(F("Watering time met for plants | 1   | 2   | 3   | 4   |" CR));   //  set the title of the met watering times only once
  }
  else
    state = SHOW_TIME;

// Has the main control of the FSM (1Hz refresh rate)
}
void loop() {
  timeRef = millis();  //take a snapshot of the time

  // Uses the current state to decide what to process
  // put all calls to functions only here.  state transitions to be in function Statetransition only
  switch (state)  {
    //State 1
    case SHOW_TIME:
      showTime(); //show time and indication if watering is on.  use variable isWateringActive.  in LCDFunctions.ino
      break;
    //State 2
    case SHOW_TIME_WATERING_ON:
      showTime(); //show time and indication if wateringActive is true,  indicate that watering is active. in LCDFunctions.ino
      //water the plants if the watering time is met
      if (checkWateringTime()) Statetransition(WATERING_TIME_MET);
      break;
    //State 3
    case SHOW_PLANT_WATERING_TIME:
      showWateringTime(); //show first watering time.  cycle for up and down.  transitions to TIME_OUT when timeout.  in LCDFunctions.ino
      break;
    //State 4
    case SELECT_PLANT_NUMBER:
      selectPlantNumber();  // In tab SetPlantWateringTime
      if (state != SET_WATERING_TIME_WEEKDAYS) break;
    //State 5
    case SET_WATERING_TIME_WEEKDAYS:
      setWateringTimeWeekdays();
      if (state != SET_WATERING_TIME_HOUR) break;
    //State 6
    case SET_WATERING_TIME_HOUR:
      setWateringTimeHour();
      if ( state != SET_WATERING_TIME_MINUTES ) break;
    //State 7
    case SET_WATERING_TIME_MINUTES:
      setWateringTimeMinute();
      if ( state != SET_WATERING_AMOUNT ) break;
    //State 8
    case SET_WATERING_AMOUNT:
      setWateringAmount();
      Log.trace(F("after setWateringAmount()" CR));
      break;
    //State 9
    case WATER_PLANTS:
      waterPlants();
      break;
    //State 10
    case SET_YEAR:
      setYear();
      if ( state != SET_MONTH ) break;
    //State 11
    case SET_MONTH:
      setMonth();
      if ( state != SET_DAY ) break;
    //State 12
    case SET_DAY:
      setDay();
      if ( state != SET_HOUR ) break;
    //State 13
    case SET_HOUR:
      setHour();
      if ( state != SET_MINUTE ) break;
    //State 14
    case SET_MINUTE:
      setMinute();
      if ( state != SET_SECOND ) break;
    //State 15
    case SET_SECOND:
      setSecond();
      break;
  }

  // Waits 1 sec for events (button presses)
  // If a button is pressed, it blocks until the button is released and then it performs the applicable state transition
  // if the select button is pressed for more than two seconds, an aditional logical parameter is passed, longpress

  while ( (unsigned long)(millis() - timeRef) < 1000 ) {  //wait for transition for a second
    if ( (button = lcd.button()) != KEYPAD_NONE ) {  //    //if a key is pressed
      buttonLongPress = false;
      timeRef = millis();
      //Hold while any key is pressed is pressed.  Break when the select key is pressed for more than 2 seconds 
      while (
        (lcd.button() != KEYPAD_NONE)                         //While a key is pressed
        &&                                                    //AND                 
        (
          ((unsigned long)(millis() - timeRef)< 2001)           //Less than than 2 seconds elapses
          ||                                                    //OR
          (lcd.button() != KEYPAD_SELECT)                       //The key is not the select key
         )
      );
      if ( (unsigned long)(millis() - timeRef) > 2000 )
        buttonLongPress = true;
      Statetransition(button, buttonLongPress);
      break;
    }
  }
}
