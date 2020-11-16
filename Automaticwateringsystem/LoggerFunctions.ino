void digitalClockDisplayonLogger(uint8_t LogLevel) {
  char sHours[3] = "";   formatDigits(hour(),sHours);
  char sMinutes[3] = ""; formatDigits(minute(),sMinutes);
  char sSeconds[3] = ""; formatDigits(second(),sSeconds);
  char sDay[3] = "";     formatDigits(day(),sDay);
  char sWeekDay[4] = ""; strncpy(sWeekDay, dayShortStr(weekday()),3);
  char sMonth[4] = "";   strncpy(sMonth, monthShortStr(month()),3);

  switch (LogLevel){
    case LOG_LEVEL_FATAL:
      Log.trace(F("Remaining memory is %d | "),freeRam());
      Log.fatal("%s:%s:%s %s %s %s %d | ", sHours, sMinutes, sSeconds, sWeekDay, sDay, sMonth, year()  );
      break;
    case LOG_LEVEL_ERROR:
      Log.trace(F("Remaining memory is %d | "),freeRam());
      Log.error("%s:%s:%s %s %s %s %d | ", sHours, sMinutes, sSeconds, sWeekDay, sDay, sMonth, year()  );
      break;
    case LOG_LEVEL_WARNING:
      Log.trace(F("Remaining memory is %d | "),freeRam());
      Log.warning("%s:%s:%s %s %s %s %d | ", sHours, sMinutes, sSeconds, sWeekDay, sDay, sMonth, year()  );
      break;
    case LOG_LEVEL_NOTICE:
      Log.trace(F("Remaining memory is %d | "),freeRam());
      Log.notice("%s:%s:%s %s %s %s %d | ", sHours, sMinutes, sSeconds, sWeekDay, sDay, sMonth, year()  );
      break;
    case LOG_LEVEL_TRACE:
      Log.trace(F("Remaining memory is %d | "),freeRam());
      Log.trace("%s:%s:%s %s %s %s %d | ", sHours, sMinutes, sSeconds, sWeekDay, sDay, sMonth, year()  );
      break;
    case LOG_LEVEL_VERBOSE:
      Log.verbose(F("Remaining memory is %d | "),freeRam());
      Log.verbose("%s:%s:%s %s %s %s %d | ", sHours, sMinutes, sSeconds, sWeekDay, sDay, sMonth, year()  );
      break;
  }

/*
    if (LOG_LEVEL <= LogLevel) {
    Log.trace(F("Remaining memory is %d | "),freeRam());
    Log.notice("%s:%s:%s %s %s %s %d | ", sHours, sMinutes, sSeconds, sWeekDay, sDay, sMonth, year()  );
  }
*/
}

void plantDetailsonLogger(uint8_t initialspace) {
  //char sDayofWeek[15] = "";
  char sHours[3] = "";
  char sMinutes[3] = "";
  char sSeconds[3] = "";
  char sQuantity[3] = "";
  char sPin[3] = "";
  char spaces[initialspace+1] = "";
  for (uint8_t i = 0 ; i < (initialspace) ; i++) {
    strcat(spaces, " ");
  }
  //print header
  Log.notice(F("No|    Weekday     | H  | M  | S  | Q  | P  |" CR));
  for (uint8_t i = 0; i < NOOFPLANTS; i++) {
    sHours[0] = '\0';
    sMinutes[0] = '\0';
    sSeconds[0] = '\0';
    sQuantity[0] = '\0';
    sPin[0] = '\0';
    //sDayofWeek[0] = '\0';
    
    //log time
    digitalClockDisplayonLogger();
    //log plant details
    Log.notice("%s%d | %s | %s | %s | %s | %s | %s |" CR,
               spaces,
               i + 1,
               weekDisplay(Plants[i].getdayOfWeekToWater()),
               formatDigits(Plants[i].getwateringTimeHours(),sHours),
               formatDigits(Plants[i].getwateringTimeMinutes(),sMinutes),
               formatDigits(Plants[i].getwateringTimeSeconds(),sSeconds),
               formatDigits(Plants[i].getwateringQuantity(),sQuantity),
               formatDigits(Plants[i].getmydigitalPIN(),sPin)
               );
  }
}

/*
 * 
 * **** Wildcards
 
* %s replace with an string (char*)
* %c  replace with an character
* %d  replace with an integer value
* %l  replace with an long value
* %x  replace and convert integer value into hex
* %X  like %x but combine with 0x123AB
* %b  replace and convert integer value into binary
* %B  like %x but combine with 0b10100011
* %t  replace and convert boolean value into "t" or "f"
* %T  like %t but convert into "true" or "false"

**** Loglevels

* 0 - LOG_LEVEL_SILENT     no output 
* 1 - LOG_LEVEL_FATAL      fatal errors 
* 2 - LOG_LEVEL_ERROR      all errors  
* 3 - LOG_LEVEL_WARNING    errors, and warnings 
* 4 - LOG_LEVEL_NOTICE     errors, warnings and notices 
* 5 - LOG_LEVEL_TRACE      errors, warnings, notices, traces
* 6 - LOG_LEVEL_VERBOSE    all 
*/