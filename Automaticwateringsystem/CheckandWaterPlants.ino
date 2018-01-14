//called by SHOW_TIME_WATERING_ON:
bool checkWateringTime() {
  //set this to false inside the loop
  bool checkWT;
  checkWT = false;
  for (uint8_t i = 0; i < 4; i++) {
    if (Plants[i].isItMyWateringTime())
         Log.notice(F("YES | "));
    else Log.notice(F("NO  | "));
    checkWT = checkWT || Plants[i].isItMyWateringTime();
  }
  if (checkWT) {
       Log.notice(F("Watering Time met |" CR));
  }
  else Log.notice(F(CR));
  return checkWT;
}

//called by case WATER_PLANTS:, which is in turn called by checkWateringTime() in Case SHOW_TIME_WATERING_ON and case SHOW_TIME
//function to water the plant whose watering time has reached
void waterPlants() {
  uint8_t largestwateringQuantity = 0;
  digitalClockDisplayonLogger(LOG_LEVEL_WARNING);
  Log.warning(F("Watering the Plant(s)"));
  for (uint8_t i = 0; i < 4; i++) {
    if (Plants[i].isItMyWateringTime()) {
      Log.warning(F(", %d "),(i + 1));
      Plants[i].startWateringMe();
    }
  }
   //find the largest watering quantity (getwateringQuantity()) at this point of time.  getwateringQuantity()
  largestwateringQuantity = 0;
  for (uint8_t i = 0; i < 4; i++) {
    if (Plants[i].amIbeingWatered())
      if (largestwateringQuantity < Plants[i].getwateringQuantity())
        largestwateringQuantity = Plants[i].getwateringQuantity();
  }

  //Sketch Restarting if float parameter is used as soon as teh log is completed
  //Log.warning(F("| Watering is on for a maximum of %F  seconds. Starting the buzzer |" CR),float(largestwateringQuantity)/10);
  Log.warning(F("| Watering is on for a maximum of %d / 10 seconds. Starting the buzzer |" CR),largestwateringQuantity);
  //whether Serial.flush is used or not
  Serial.flush();
  //make a noise!
  analogWrite(BUZZER_PIN, 220);
  //loop for largestwateringQuantity seconds, incrementing the time by 1 second each time;
  /*
  //To check if the behavoir of the log statement restarting the sketch is related to teh delay() statement.  It is not
  timeRef = millis();  //take a snapshot of the time
  while ( unsigned long lmiliseconds = (unsigned long)(millis() - timeRef) < (largestwateringQuantity*100+1)) {
    //Display time every second
    if (lmiliseconds%1000 == 0){
      digitalClockDisplayonLogger();
      Log.notice(F(CR));
     }
    for (uint8_t i = 0; i < 4; i++) {
      if (Plants[i].getwateringQuantity()*100 <= lmiliseconds && Plants[i].amIbeingWatered() ) {
        Plants[i].stopWateringMe();
        digitalClockDisplayonLogger(LOG_LEVEL_WARNING);
        //Sketch restarting if the Log.notice is used with two parameters.  30/9.  Also with version 1.0.2 version of ArduinoLog
        Log.warning(F("Stopping plant %d after %F seconds |" CR),(i+1),(lmiliseconds/1000));
        //Log.warning(F("Stopping plant %d "),(i+1));
        //Log.warning(F("after %F seconds |" CR),float(lmiliseconds/1000));
       }
    }
      
  }
  */
  
  for (uint8_t iseconds = 0; iseconds < largestwateringQuantity + 1; iseconds++) {
    //Display time every second
    if (iseconds%10 ==0){
      digitalClockDisplayonLogger();
      //Log.notice(F(CR));
      //Sketch restarting if %f (float) parameter is used as soon as the log is completed
      //Log.notice(F("Elapsed time %F seconds |" CR),float(iseconds)/10);
      //Serial.flush();
      Log.notice(F("Elapsed time: %d  seconds / 10 |" CR),iseconds);
    }
    for (uint8_t i = 0; i < 4; i++) {
      if (Plants[i].getwateringQuantity() <= iseconds && Plants[i].amIbeingWatered() ) {
        Plants[i].stopWateringMe();
        digitalClockDisplayonLogger(LOG_LEVEL_WARNING);
        //Sketch restarting if the Log.notice is used with two parameters.  30/9.  Also with version 1.0.2 version of ArduinoLog
        //Log.warning(F("Stopping plant %d after %F seconds |" CR),(i+1),float(iseconds)/10);
        Log.warning(F("Stopping plant %d after %d / 10 seconds" CR),(i+1),iseconds);
        //Sketch restarting on the the next log statement, as soon as the log is completed.  whether Serial.flush is used or not
        //Serial.flush();
        //Log.warning(F("after %d /10 seconds |" CR),iseconds);
        //Serial.flush();
       }
    }
    //Serial.flush();
    delay(100);
  }
  
  //stop the buzzer
  digitalClockDisplayonLogger(LOG_LEVEL_WARNING);
  Log.warning(F("Stopping the Buzzer" CR));
  analogWrite(BUZZER_PIN, 0);
  //todo mute buzzer using an lcd button
  //Sketch restarting if the below two log.notices are uncomented :-( 
  //digitalClockDisplayonLogger();
  //Log.notice(F("Watering time met for plants | 1   | 2   | 3   | 4   |" CR));   //  set the title of the met watering times only once
  Statetransition(TIME_OUT);
}
