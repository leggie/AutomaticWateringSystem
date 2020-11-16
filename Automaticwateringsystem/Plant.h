//why is a class declaration in a .h file?  http://stackoverflow.com/questions/17799906/error-aggregate-first-one-has-incomplete-type-and-cannot-be-defined
#include "headers.h"
class Plant {
  private:
    uint8_t plantNumber;
    uint8_t dayOfWeekToWater;  // Each byte denoting a weekday.  7 th significant = SUN.  Least significant SAT
    uint8_t wateringTimeHours;
    uint8_t wateringTimeMinutes;
    uint8_t wateringTimeSeconds;
    uint8_t wateringQuantity;
    uint8_t mydigitalPIN;
    bool    beingWatered;
  public:
    //Constructors
    Plant() {}; //constructor
    
    Plant(uint8_t ThePlantNumber) { //constuctor 2
      plantNumber = ThePlantNumber;
      //setting this to 0 in the constuctor, as this is not going to be set in the setclock function now
      dayOfWeekToWater = 127;  //bit with place value 0 (right most) is Sun, bit with place value 6 is Sat
      wateringTimeHours = 0;
      wateringTimeMinutes = 0;
      wateringTimeSeconds = 0;
      wateringQuantity = 0;
      beingWatered = false;
    }
    
    //Setters and Getters
    void setplantNumber(uint8_t TheplantNumber) {
      plantNumber = TheplantNumber;
    }
    
    uint8_t getplantNumber() {
      return plantNumber;
    }

    void setdayOfWeekToWater(uint8_t wateringDays) {
      dayOfWeekToWater = wateringDays;
    }

    uint8_t getdayOfWeekToWater(){
      return dayOfWeekToWater;
    }
    
    void setwateringTimeHours(uint8_t ThewateringTimeHours) {
      wateringTimeHours = ThewateringTimeHours;
    }
    
    uint8_t getwateringTimeHours() {
      return wateringTimeHours;
    }
    
    void setwateringTimeMinutes(uint8_t ThewateringTimeMinutes) {
      wateringTimeMinutes = ThewateringTimeMinutes;
    }
    
    uint8_t getwateringTimeMinutes() {
      return wateringTimeMinutes;
    }
    
    void setwateringTimeSeconds(uint8_t ThewateringTimeSeconds) {
      wateringTimeSeconds = ThewateringTimeSeconds;
    }
    
    uint8_t getwateringTimeSeconds() {
      return wateringTimeSeconds;
    }
    
    void setwateringQuantity(uint8_t ThewateringQuantity) {
      wateringQuantity = ThewateringQuantity;
    }
    
    uint8_t getwateringQuantity() {
      return wateringQuantity;
    }
    
    void setmydigitalPIN(uint8_t thisdigitalPIN) {
      mydigitalPIN = thisdigitalPIN;
    }
    
    uint8_t getmydigitalPIN() {
      return mydigitalPIN;
    }
    
    //Other Methods
    bool isItMyWateringTime() {
      bool WateringTimeMet = false;
      //check the bit corresponding to the weekday (sunday is 1), to see if it is set to 1
      if (
        ((dayOfWeekToWater >> (weekday()-1)) & 1U) &&   
        hour() == wateringTimeHours && 
        minute() == wateringTimeMinutes &&
        (second() == wateringTimeSeconds || second() == wateringTimeSeconds + 1)
      ) WateringTimeMet = true;
      return WateringTimeMet;
    }
    
    //Called by void showWateringTimeofPlant(uint8_t PlantNo) (LCDFunctions.ino)
    char* getwateringTimeFull() {
      //Using the first, "static array technique technique " from http://c-faq.com/~scs/cclass/int/sx5.html as this is used only in Plant.getwateringTimeFull
      static char FormattedTime[9];
      //reset to "" before use
      FormattedTime[0] = '\0';
      char sHours[3] = "";
      char sMinutes[3] = "";
      char sSeconds[3] = "";
      strcat(FormattedTime, formatDigits(wateringTimeHours,sHours));
      strcat(FormattedTime, ":");
      strcat(FormattedTime, formatDigits(wateringTimeMinutes,sMinutes));
      strcat(FormattedTime, ":");
      strcat(FormattedTime, formatDigits(wateringTimeSeconds,sSeconds));
      return FormattedTime;
    }
    
    void startWateringMe() {
      expander.digitalWrite(mydigitalPIN, ON);
      beingWatered = true;
    }
    
    void stopWateringMe() {
      expander.digitalWrite(mydigitalPIN, OFF);
      beingWatered = false;
    }
    
    bool amIbeingWatered() {
      return beingWatered;
    }
    
    bool validateMe(uint8_t PlantNo) {
      uint8_t SetOfValidPins[4] = {PLANT_1_PORT , PLANT_2_PORT , PLANT_3_PORT , PLANT_4_PORT};
      bool amIValid = true;
      if (plantNumber         != PlantNo)                   {Log.error(F("The plantNumber of Plant %d  is invalid. Its value is %d" CR),        PlantNo, plantNumber);         amIValid = false;} 
      if (dayOfWeekToWater    >  127)                       {Log.error(F("The dayOfWeekToWater of Plant %d  is invalid. Its value is %d" CR),   PlantNo, dayOfWeekToWater);    amIValid = false;}       
      if (wateringTimeHours   >  23)                        {Log.error(F("The wateringTimeHours of Plant %d  is invalid. Its value is %d" CR),  PlantNo, wateringTimeHours);   amIValid = false;} 
      if (wateringTimeMinutes >  59)                        {Log.error(F("The wateringTimeSeconds of Plant %d  is invalid. Its value is %d" CR),PlantNo, wateringTimeMinutes); amIValid = false;} 
      if (wateringTimeSeconds >  59)                        {Log.error(F("The wateringTimeSeconds of Plant %d  is invalid. Its value is %d" CR),PlantNo, wateringTimeSeconds); amIValid = false;} 
      if (wateringQuantity    >  99)                        {Log.error(F("The wateringQuantity of Plant %d  is invalid. Its value is %d" CR),   PlantNo, wateringQuantity);    amIValid = false;} 
      if (mydigitalPIN        != SetOfValidPins[PlantNo-1]) {Log.error(F("The mydigitalPIN of Plant %d  is invalid. Its value is %d" CR),       PlantNo, mydigitalPIN);        amIValid = false;} 
      return amIValid;
    }

    void setDefaults(uint8_t PlantNo) {
      uint8_t SetOfValidPins[8] = {PLANT_1_PORT , PLANT_2_PORT , PLANT_3_PORT , PLANT_4_PORT, PLANT_5_PORT , PLANT_6_PORT , PLANT_7_PORT , PLANT_8_PORT };
      plantNumber         = PlantNo;
      dayOfWeekToWater    = 0b01111111;
      wateringTimeHours   = 0;
      wateringTimeMinutes = 0;
      wateringTimeSeconds = 0;
      wateringQuantity    = 0;
      mydigitalPIN        = SetOfValidPins[PlantNo-1];
      beingWatered        = false;  
    }
};