char * formatDigits(uint16_t digits, char *FormattedString) {
  //https://stackoverflow.com/questions/25798977/returning-string-from-c-function
  //https://stackoverflow.com/questions/1496313/returning-c-string-from-a-function
  //using the second, "caller-passes-an-array technique " from http://c-faq.com/~scs/cclass/int/sx5.html as this is used in a  \log.xxx statement as parameters.  
  char strbuffer[3] = "";
  if (digits > 99) {
    Log.fatal(F("Fatal Error.  Digit, %d > 99 sent to formatDigits!" CR),digits);
    delay(10000);
  }
  
  if (digits < 10) strcat(FormattedString, "0");
  itoa(digits, strbuffer, 10);
  strcat(FormattedString, strbuffer);
  return FormattedString;
}

/*
char * getFormattedTime(uint8_t iHour, uint8_t iMinute, uint8_t iSecond, char *FormattedTime){
  using the first, "static array technique technique " from http://c-faq.com/~scs/cclass/int/sx5.html as this is used only in Plant.getwateringTimeFull
  char sHours[3] = "";
  char sMinutes[3] = "";
  char sSeconds[3] = "";
  strcat(FormattedTime, formatDigits(iHour,sHours));
  strcat(FormattedTime, ":");
  strcat(FormattedTime, formatDigits(iMinute,sMinutes));
  strcat(FormattedTime, ":");
  strcat(FormattedTime, formatDigits(iSecond,sSeconds));
  return FormattedTime;
}
*/
//used in uint8_t LCDUpdateWeekDays(char strTitle[], uint8_t initialValue) in LCDFunctions.ino
char* weekDisplay(uint8_t byteToString) {
  //using the static array technique from http://c-faq.com/~scs/cclass/int/sx5.html to return a string.  Multiple return values need not be returned.  values returned are printed immediately
  if (byteToString > 0b01111111) {
    Log.fatal(F("Fatal Error.  Digit, %d > 0b01111111 sent to weekDisplay!" CR),byteToString);
    delay(10000);
  }
  //Log.trace(F("In function weekDisplay(uint8_t byteToString). The input value is %d..."),byteToString);

  char displayweekdays[7][3] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}; // why const?  https://forum.arduino.cc/index.php?topic=372563.0
  static char actDisp[15];
  //reset to "" before use
  //actDisp = "";  doesnt work.  see https://arduino.stackexchange.com/questions/13429/deprecated-conversion-from-string-constant-to-char
  actDisp[0] = '\0';
  //strcpy(actDisp,"");
  for (int i = 0; i < 7; i++) {
    if ((byteToString >> i) & 1U) strcat(actDisp, displayweekdays[i]); //https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit - on how to do bit manipulation
    else strcat(actDisp, "  ");
  }
  if (strlen(actDisp)> 14) {
    Log.fatal(F("Fatal Error.  Length of string returned by weekDisplay more than 14 characters!" CR));
    delay(10000);
  }
  //Log.trace(F(" and the return value is %s |" CR),actDisp);
  return actDisp;
}

//used in void showWateringTimeofPlant(uint8_t PlantNo) in LCDFunctions.ino
char* weekDisplayShort(uint8_t byteToString) {
  //using the static array technique from http://c-faq.com/~scs/cclass/int/sx5.html to return a string.  Multiple return values need not be returned.  values returned are printed immediately
  if (byteToString > 0b01111111) {
    Log.fatal(F("Fatal Error.  Digit, %d > 0b01111111 sent to weekDisplayShort!" CR),byteToString);
    delay(10000);
  }
  //Log.trace(F("In function weekDisplayShort(uint8_t byteToString).  The input value is %d"),byteToString);
  char displayweekdays[7][2] = {"S", "M", "T", "W", "T", "F", "S"}; // why const?  https://forum.arduino.cc/index.php?topic=372563.0
  static char actDispShort[8];
  //reset to "" before use
  //actDispShort = "";  doesnt work.  see https://arduino.stackexchange.com/questions/13429/deprecated-conversion-from-string-constant-to-char
  actDispShort[0] = '\0';
  //strcpy(actDispShort,"");
  for (int i = 0; i < 7; i++) {
    if ((byteToString >> i) & 1U) strcat(actDispShort, displayweekdays[i]); //https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit - on how to do bit manipulation
    else strcat(actDispShort, " ");
  }
  if (strlen(actDispShort) > 7) {
    Log.fatal(F("Fatal Error.  Lenngth of string returned by weekDisplayShort more than 7 characters!" CR));
    delay(10000);
  }
  //Log.trace(F(" and the return value is %s" CR),actDispShort);
  return actDispShort;
}

//https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
//returns available free memory
int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}