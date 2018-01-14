//get the Plants stored in EEPROM
  bool getPlantsFromEEPROM() {
  eeAddress = 0;
  for (uint8_t i = 0; i < 4; i++) {
    EEPROM.get(eeAddress, Plants[i]);
    eeAddress += sizeof(Plants[i]);  //update eeAdress to take the next variable
  }
  //get the Watering status
  EEPROM.get(eeAddress,  isWateringActive);
  //eeAddress += sizeof(isWateringActive);
 
  bool validatePlants = true;
  for (uint8_t i = 0; i < 4; i++)
    validatePlants = validatePlants && Plants[i].validateMe(i+1);
  digitalClockDisplayonLogger();
  if (validatePlants) {
    Log.notice(F("Stored Plants are valid. Plants details from the EEPROM loaded|" CR));
    //1
    digitalClockDisplayonLogger();
    plantDetailsonLogger(0);
    digitalClockDisplayonLogger();
    Log.notice(F("EEPROM last Address is: %d. Watering Status is: %T" CR), eeAddress, isWateringActive);
    return true;
  }
  else {
    Log.notice(F("Stored Plants are invalid.|" CR));
    return false;
  }
}

//store the Plants in EEPROM
void storePlantinEEPROM() {
  eeAddress = 0;
  digitalClockDisplayonLogger();
  Log.notice(F("Storing details of plant to EEPROM. Current address is %d" CR),eeAddress);
  for (uint8_t i = 0; i < 4; i++) {
    EEPROM.put(eeAddress, Plants[i]);
    eeAddress += sizeof(Plants[i]);  //update eeAdress to take the next variable
    Log.trace(F("Stored details of plant %d" CR),i+1);
  }
  EEPROM.put(eeAddress,  isWateringActive);
  //eeAddress += sizeof(isWateringActive);
  //3
  digitalClockDisplayonLogger();
  plantDetailsonLogger(0);
  digitalClockDisplayonLogger();
  Log.notice(F("Stored details of plant to EEPROM.  Last EEPROM Address is: %d. Watering Status is %T |" CR), eeAddress, isWateringActive);
}

void storeWateringStatus() {
  EEPROM.put(eeAddress,  isWateringActive);
  digitalClockDisplayonLogger();
  Log.notice(F("Stored the Watering Status.  Last EEPROM Address is: %d. Watering Status is %T |" CR), eeAddress, isWateringActive);
}

void getWateringStatus() {
  //get the Watering status
  digitalClockDisplayonLogger();
  Log.notice(F("Getting the Watering Status.  EEPROM Address is: %d" CR), eeAddress);
  EEPROM.get(eeAddress,  isWateringActive);
}
