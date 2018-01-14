//Allows the user to set the system time, and correct the RTC's drift.  Allows to select the year, month, hours, minutes, and seconds watering quantity, etc
//global variables
uint16_t tmpYear = 0;
//tmElements_t tm;  //to hold time elements - Defined in RTCFunctions

//Called by state SET_YEAR, function.  The code for setting the year
//The 1st of a 6 part process for setting/correcting the system time
//Receives the year If not provided within 5 sec, Times out and returns to a previous state
void setYear() {
  Log.trace(F("In function setYear " CR));
  //ignore the first trigger == KEYPAD_SELECT.  As the calling functions while loop comes out after 2 seconds, and then the key is depressed, a SELECT trigger is always selected bu itself. 
  tmpYear = LCDUpdateElement(F("Set Year"), F("Year"), year(), 2017, 2100);
  tmpYear = LCDUpdateElement(F("Set Year"), F("Year"), year(), 2017, 2100);
  tm.Year = CalendarYrToTm(tmpYear);
  digitalClockDisplayonLogger(LOG_LEVEL_NOTICE);
  Log.notice(F("Setting Year to %d.  Offset from 1970 is %d |" CR),tmpYear,tm.Year);
  if ( !timeOut ) Statetransition(KEYPAD_SELECT);
  else Statetransition(TIME_OUT);
}

//called by state SET_MONTH.  for setting the Month
// The 2nd of a 6 part process for setting the system time
// Receives the month If not provided within 5 sec, times out and returns to a previous  state
void setMonth() {
  tm.Month =  LCDUpdateElement(F("Set Month"), F("Month"), month(), 1, 12);
  digitalClockDisplayonLogger(LOG_LEVEL_NOTICE);
  Log.notice(F("Setting Month to %d |" CR),tm.Month);
  if ( !timeOut ) Statetransition(KEYPAD_SELECT);
  else Statetransition(TIME_OUT);
}

// called by state SET_DAY.  For setting the Day
// The 3rd of a 6 part process for setting the system time
// Receives the day If not provided within 5 sec, times out and returns to a previous  state
void setDay() {
  tm.Day = LCDUpdateElement(F("Set Day"), F("Day"), day(), 1, 31);
  digitalClockDisplayonLogger(LOG_LEVEL_NOTICE);
  Log.notice(F("Setting Day to %d |" CR),tm.Day);
  if ( !timeOut ) Statetransition(KEYPAD_SELECT);
  else Statetransition(TIME_OUT);
}

// called by state SET_HOUR.  For setting the Hour
// The 4th of a 6 part process for setting the system time
// Receives the hour. If not provided within 5 sec, times out and returns to a previous  state
void setHour() {
  tm.Hour = LCDUpdateElement(F("Set Hour"), F("Hour"), hour(), 0, 23);
  digitalClockDisplayonLogger(LOG_LEVEL_NOTICE);
  Log.notice(F("Setting Hour to %d |" CR),tm.Hour);
  if ( !timeOut ) Statetransition(KEYPAD_SELECT);
  else Statetransition(TIME_OUT);
}

// called by state SET_MINUTE.  For setting the Minute
// The 5th of a 6 part process for setting the system time
// Receives the minute. If not provided within 5 sec, times out and returns to a previous  state
void setMinute() {
  tm.Minute = LCDUpdateElement(F("Set Minute"), F("Minute"), minute(), 0, 59);
  digitalClockDisplayonLogger(LOG_LEVEL_NOTICE);
  Log.notice(F("Setting Minute to %d |" CR),tm.Minute);
  if ( !timeOut ) Statetransition(KEYPAD_SELECT);
  else Statetransition(TIME_OUT);
}

// called by state SET_SECOND.  For setting the Second 
// The 6th of a 6 part process for setting the system time
// Receives the second. If not provided within 5 sec, times out and returns to a previous  state
// If second is provider, sets/corrects the time in the RTC
void setSecond() {
  tm.Second = LCDUpdateElement(F("Set Second"), F("Second"), second(), 0, 59);
  digitalClockDisplayonLogger(LOG_LEVEL_NOTICE);
  Log.notice(F("Setting Second to %d |" CR),tm.Second);
  if ( !timeOut )     {
    //update the RTC Time
    setRTCTime(tm);  //Function in RTC Functions
    Statetransition(KEYPAD_SELECT);
  }
  else Statetransition(TIME_OUT);
}
