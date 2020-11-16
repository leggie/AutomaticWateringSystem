//put only state transactions here.  Calls to functions to be in the loop section (Automaticwateringsystem) only
//You dont include the default parameter in your function definition, the prototype is the only one you need to include the default value into.  https://stackoverflow.com/questions/13295530/this-is-a-new-compile-error-for-me
void Statetransition(uint8_t trigger, bool buttonLongPress) {
  Log.trace(F("Transtion! The State is %d, The trigger is %d and longpress is %T. Remaining memory is %d" CR), state, trigger, buttonLongPress, freeRam());
  switch (state) {
    //State 0
    case SHOW_TIME:
      switch (trigger) {
        case KEYPAD_LEFT:
          state = SHOW_PLANT_WATERING_TIME;
          Log.trace(F("State SHOW_TIME, Trigger KEYPAD_LEFT, Longpress %T. State change to SHOW_PLANT_WATERING_TIME. Remaining memory %d |" CR), buttonLongPress, freeRam());
          break;
        case KEYPAD_RIGHT:
          //Only this function is put in here.  putting it in the loop section will make it run every second!
          Log.trace(F("State SHOW_TIME, Trigger KEYPAD_RIGHT, Longpress %T. State change to SHOW_TIME_WATERING_ON. Remaining memory %d |" CR), buttonLongPress, freeRam());
          isWateringActive = true;
          storeWateringStatus();  //  Store the changed watering status in EEPROM when the status changes
          digitalClockDisplayonLogger();
          Log.notice(F("Watering time met for plants | 1   | 2   | 3   | 4   |" CR));   //  set the title of the met watering times only once
          state = SHOW_TIME_WATERING_ON;
          break;
        case KEYPAD_SELECT:
          if (!(buttonLongPress)) {
            state = SELECT_PLANT_NUMBER;
            Log.trace(F("State SHOW_TIME, Trigger KEYPAD_SELECT, Longpress %T. State change to SELECT_PLANT_NUMBER. Remaining memory %d |" CR), buttonLongPress, freeRam());
          }
          else {
            state = SET_YEAR;
            Log.trace(F("State SHOW_TIME, Trigger KEYPAD_SELECT, Longpress %T. State change to SET_YEAR. Remaining memory %d |" CR), buttonLongPress, freeRam());
          }
          break;
        case WATERING_TIME_MET:
          state = WATER_PLANTS;
          break;
      }
      break;

    //state 1
    case SHOW_TIME_WATERING_ON:
      switch (trigger) {
        case KEYPAD_LEFT:
          state = SHOW_PLANT_WATERING_TIME;
          Log.trace(F("State SHOW_TIME_WATERING_ON, Trigger KEYPAD_LEFT, Longpress %T. State change to SHOW_PLANT_WATERING_TIME. Remaining memory %d |" CR), buttonLongPress, freeRam());
          break;
        case KEYPAD_RIGHT:
          //Only this function is put in here.  putting it in the loop section will make it run every second!
          isWateringActive = false;
          storeWateringStatus();  //  Store the changed watering status in EEPROM when the status changes
          state = SHOW_TIME;
          Log.trace(F("State SHOW_TIME_WATERING_ON, Trigger KEYPAD_RIGHT, Longpress %T. State change to SHOW_TIME. Remaining memory %d |" CR), buttonLongPress, freeRam());
          break;
        case KEYPAD_SELECT:
          if (!(buttonLongPress)) {
            state = SELECT_PLANT_NUMBER;
            Log.trace(F("State SHOW_TIME_WATERING_ON, Trigger KEYPAD_SELECT, Longpress %T. State change to SELECT_PLANT_NUMBER. Remaining memory %d |" CR), buttonLongPress, freeRam());
          }
          else {
            state = SET_YEAR;
            Log.trace(F("State SHOW_TIME_WATERING_ON, Trigger KEYPAD_SELECT, Longpress %T. State change to SET_YEAR. Remaining memory %d |" CR), buttonLongPress, freeRam());
          }
          break;
        case WATERING_TIME_MET:
          state = WATER_PLANTS;
          break;
      }
      break;

    //State 2
    case SHOW_PLANT_WATERING_TIME:  // to show the plant watering times
      if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;

    //State 3
    case SELECT_PLANT_NUMBER:
      if ( trigger  == KEYPAD_SELECT ) state = SET_WATERING_TIME_WEEKDAYS;
      //if ( trigger  == KEYPAD_SELECT ) state = SET_WATERING_TIME_HOUR;
      else if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;

    //State 4
    case SET_WATERING_TIME_WEEKDAYS:
      if ( trigger  == KEYPAD_SELECT ) state = SET_WATERING_TIME_HOUR;
      else if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;

    //State 5
    case SET_WATERING_TIME_HOUR:
      if ( trigger == KEYPAD_SELECT ) state = SET_WATERING_TIME_MINUTES;
      else if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;

    //State 6
    case SET_WATERING_TIME_MINUTES:
      if ( trigger == KEYPAD_SELECT ) state = SET_WATERING_AMOUNT;
      else if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;

    //State 7
    case SET_WATERING_AMOUNT:
      if ( trigger == KEYPAD_SELECT ) {
        //Log.trace(F("In function Statetreansition line 88" CR));
        //delay(5000);
        digitalClockDisplayonLogger();
        Log.notice(F("Watering time met for plants | 1   | 2   | 3   | 4   |" CR));   //  set the title of the met watering times only once
        state = SHOW_TIME_WATERING_ON;
      }
      else if ( trigger == TIME_OUT ) {
        state = SHOW_TIME;
      }
      break;

    //State 8
    case WATER_PLANTS:
      if (isWateringActive) state = SHOW_TIME_WATERING_ON;
      else state = SHOW_TIME;
      break;

    //State 9
    case SET_YEAR:
      if ( trigger == KEYPAD_SELECT ) delay(150); //do nothing.  To take care of the two KEYPAD_SELECT triggers
      if ( trigger == KEYPAD_SELECT ) state = SET_MONTH;
      else if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;

    //State 10
    case SET_MONTH:
      if ( trigger == KEYPAD_SELECT ) state = SET_DAY;
      else if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;

    //State 11
    case SET_DAY:
      if ( trigger == KEYPAD_SELECT ) state = SET_HOUR;
      else if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;

    //State 12
    case SET_HOUR:
      if ( trigger == KEYPAD_SELECT ) state = SET_MINUTE;
      else if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;

    //State 13
    case SET_MINUTE:
      if ( trigger == KEYPAD_SELECT ) state = SET_SECOND;
      else if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;

    //State 14
    case SET_SECOND:
      //this if is redundant (non reachable).  remove
      if ( trigger == KEYPAD_SELECT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      else if ( trigger == TIME_OUT ) {
        if (isWateringActive) state = SHOW_TIME_WATERING_ON;
        else state = SHOW_TIME;
      }
      break;
  }
}