//function prototypes, used only in this module.  defined below.
bool getDate(const char *str);
bool getTime(const char *str);

tmElements_t tm;  //to hold time elements

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void setSystemTimeFromRTC() {
  //from the example TimeRTCSet in Time libray
  setSyncProvider(RTC.get);   // Function from time library.  sets the function to get the time from the RTC.  where is the reference to RTC set???
  //1
  if (timeStatus() == timeSet) {
    digitalClockDisplayonLogger();  //function defined in LoggerFunctions
    Log.notice(F("RTC has set the system time |" CR));
  }
  //2
  else {
    Log.error(F("Unable to sync with the RTC |" CR));
    //2.1. if RTC is present but stopped
    if (RTC.chipPresent()) {
      Log.warning(F("The RTC has stopped.  Updating RTC from the compile time of sketch |" CR));

      //2.1.1.  if we can parse the date and time from compile time
      if (getDate(__DATE__) && getTime(__TIME__)) {
        // 2.1.1.1 and can configure the RTC with this info
        if (RTC.write(tm)) {
          digitalClockDisplayonLogger();
          Log.warning(F("Set time on RTC from compile date and time |" CR));
        }
      }
    }
    //2.2 if RTC is not present, or not working
    else {
      Log.error(F("RTC read error!  Please check the circuitry |" CR));
      Log.warning(F("Setting time from sketch compiled time.  Date = %s and time = %s | "), __DATE__, __TIME__);

      //2.2.1  //set time from compiled time
      if (getDate(__DATE__) && getTime(__TIME__)) {
        setTime(makeTime(tm));
        Log.warning(F("Have Set the time from compiled date and time |" CR));
      }
      //2.2.2
      //error in parsing compiled time
      else {
        Log.warning(F("Error in parsing time from compile time String of sketch.  _TIME_ is: %s and Date is: %s. Setting the time to 23:30 14th May 2107 |" CR), __DATE__, __TIME__);
        //void    setTime(int hr,int min,int sec,int day, int month, int yr);
        setTime(23, 30, 0, 14, 5, 2017);
      }
    }
  }
}

void setRTCTime(tmElements_t tm) {
  //if RTC is present
  if (RTC.chipPresent()) {
    digitalClockDisplayonLogger();
    Log.warning(F("Setting the time on the RTC |" CR));
    if (RTC.write(tm)) {
      digitalClockDisplayonLogger();
      Log.warning(F("Set time on RTC to %d/%d/%d %d:%d;%d. |" CR),tm.Day,tm.Month,tmYearToCalendar(tm.Year),tm.Hour,tm.Minute,tm.Second);
    setTime(makeTime(tm));
      return true;
    }
    //if RTC not present, or time could not be set
  } else {
    digitalClockDisplayonLogger();
    Log.error(F("RTC write ERROR, or RTC not present!  Please check the circuitry |" CR));
    digitalClockDisplayonLogger();
    Log.warning(F("Setting the system time to %d/%d/%d %d:%d:%d.  (will revert to sketch compile time after power off) |" CR),tm.Day,tm.Month,tmYearToCalendar(tm.Year),tm.Hour,tm.Minute,tm.Second);
    setTime(makeTime(tm));
  }
}

bool getTime(const char *str) {
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str) {
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}