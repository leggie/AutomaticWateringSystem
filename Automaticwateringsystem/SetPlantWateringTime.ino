//Selects the plant and then sets the plants member variables.  Watering time, Hours, minutes, watering quantity, etc


//global variables
uint8_t tmpHours = 0;
uint8_t tmpMinutes;
uint8_t tmpPlantNo = 1;
uint8_t tmpWateringQuantity = 3;
uint8_t tmpwateringDays = 0b01111111;

//called by state SELECT_PLANT_NUMBER
//the code for selecting the plant
//The 1st of a 5 part process for setting the watering time of a plant
void selectPlantNumber() {
  Log.trace(F("In function selectPlantNumber(). Memory remaining %d |" CR),freeRam());
  //uint16_t LCDUpdateElement(char* strTitle, char* strElementName, uint16_t initialValue, uint16_t minValue, uint16_t maxValue)
  tmpPlantNo =  LCDUpdateElement(F("Select Plant"), F("Plant"), tmpPlantNo, 1, 4);
  if ( !timeOut ) Statetransition(KEYPAD_SELECT);
  else Statetransition(TIME_OUT);
}

//called by state SET_WATERING_TIME_WEEKDAYS
//The 2nd of a 5 part process for setting the watering time of a plant
//the code for selecting the weekdays when the plant will be watered
void setWateringTimeWeekdays(){
  Log.trace(F("In function setWateringTimeWeekdays(). Memory remaining %d |" CR),freeRam());
  //uint8_t LCDUpdateWeekDays(char* strTitle, uint8_t initialValue)
  //tmpwateringDays = LCDUpdateWeekDays(F("Select Weekdays"), Plants[tmpPlantNo - 1].getdayOfWeekToWater());
  tmpwateringDays = LCDUpdateWeekDays(Plants[tmpPlantNo - 1].getdayOfWeekToWater());
  if ( !timeOut ) Statetransition(KEYPAD_SELECT);
  else Statetransition(TIME_OUT);  
}

//called by state SET_WATERING_TIME_HOUR
// The 3rd of a 5 part process for setting the WateringTime time
// Receives the WateringTime time hour. If not provided within 5 sec,
// times out and returns to a previous (time and date) state
void setWateringTimeHour() {
  Log.trace(F("In function setWateringTimeHour(). Memory remaining %d |" CR),freeRam());
  tmpHours =  LCDUpdateElement(F("Set WateringTime"), F("Hours"), Plants[tmpPlantNo - 1].getwateringTimeHours(), 0, 23);
  if ( !timeOut ) Statetransition(KEYPAD_SELECT);
  else Statetransition(TIME_OUT);
}

// called by state SET_WATERING_TIME_MINUTES
// The 4th of a 5 part process for setting the WateringTime time
// Receives the WateringTime time minutes. If not provided within 5 sec,
// times out and returns to a previous (time and date) state
void setWateringTimeMinute() {
  Log.trace(F("In function setWateringTimeMinute(). Memory remaining %d |" CR),freeRam());
  tmpMinutes = LCDUpdateElement(F("Set WateringTime"), F("Minutes"), Plants[tmpPlantNo - 1].getwateringTimeMinutes(), 0, 59);
  if ( !timeOut ) Statetransition(KEYPAD_SELECT);
  else Statetransition(TIME_OUT);
}

//Called by state SET_WATERING_AMOUNT
// The 5th of a 5 part process for setting the WateringTime time
// Receives the Watering amount in seconds.  If not provided within 5 sec,
// times out and returns to a previous (time and date) state
// If watering amount is provided, sets the WateringTime time and turns the WateringTime on
void setWateringAmount() {
  Log.trace(F("In function setWateringAmount(). Memory remaining %d |" CR),freeRam());
  tmpWateringQuantity = LCDUpdateElement(F("Set Water Amount"), F("Amount"), Plants[tmpPlantNo - 1].getwateringQuantity(), 0, 99);
  if ( !timeOut )     {
    //update the plants member variables
    Plants[tmpPlantNo - 1].setdayOfWeekToWater(tmpwateringDays);
    Plants[tmpPlantNo - 1].setwateringTimeHours(tmpHours);
    Plants[tmpPlantNo - 1].setwateringTimeMinutes(tmpMinutes);
    Plants[tmpPlantNo - 1].setwateringTimeSeconds(0);
    Plants[tmpPlantNo - 1].setwateringQuantity(tmpWateringQuantity);

    Plants[tmpPlantNo - 1].stopWateringMe();  // to set the Plants beingWatered attribute of the plant to to zero initially;
    isWateringActive = true ;  //global variable.  If you set a time, assume that you need to also set WaterActive true
    
    //Store all the Plant details in EEPROM, including this plant.  Store the changes watering status
    storePlantinEEPROM();
    storeWateringStatus();  //  Store the changed watering status in EEPROM when the status changes
    Log.trace(F("After storing plant details in EEPROM. Memory remaining %d |" CR),freeRam());
    Statetransition(KEYPAD_SELECT);
  }
  else Statetransition(TIME_OUT);
}
