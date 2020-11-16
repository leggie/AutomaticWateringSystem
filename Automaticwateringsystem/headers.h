
//Includes
#include <Wire.h>  // Required by DS130RTC
#include <LiquidCrystal.h>  // Required by LCDKeypad
#include <LCDKeypad.h>
#include <TimeLib.h>
#include <EEPROM.h>   //https://github.com/arduino/Arduino/issues/3783
//#include <TimeAlarms.h>
#include <ArduinoLog.h>
#include <DS1307RTC.h>
#include <TimeLib.h>
#include <PCF8574.h>

//Defines
#define ON LOW                     //used during Active Low Trigger 0 to 1 in Plant.h
#define OFF HIGH                   //used during Active Low Trigger 1 to 0 in Plant.h
#define BUZZER_PIN 11              // Output PWM pin for the buzzer
#define NOOFPLANTS 8			   // Number of plants supported by sketch	
#define PLANT_1_PORT 0             // output pin for the relay 1
#define PLANT_2_PORT 1             // output pin for the relay 2
#define PLANT_3_PORT 2             // output pin for the relay 3
#define PLANT_4_PORT 3             // output pin for the relay 4
#define PLANT_5_PORT 4             // output pin for the relay 5
#define PLANT_6_PORT 5             // output pin for the relay 6
#define PLANT_7_PORT 6             // output pin for the relay 7
#define PLANT_8_PORT 7             // output pin for the relay 8
#define TIME_OUT 5                 // One of the system's FSM transitions
#define WATERING_TIME_MET 6        // One of the system's FSM transitions
#define SketchVersion "5.1"        // Version of the sketch.  To display in the LCD and Logger
#define LOG_LEVEL LOG_LEVEL_NOTICE // The Log level used in this compile.  set to one of the levels defined in ArduinoLog.h, and LoggerFunctions.  0 highest.  6 lowest

// The different states of the system
enum mcstates {
  SHOW_TIME,                    // 0  Displays the time and date
  SHOW_TIME_WATERING_ON,        // 1  Displays the time and date, and teh watering ON indicator
  SHOW_PLANT_WATERING_TIME,     // 2  Displays the plants WateringTime time and goes back to time and date after 3 seconds
                                //    The left buttons toggle between the different plant watering times
  SELECT_PLANT_NUMBER,          // 3  Option for selecting the plant no.  If Provided, it moves on to Watering Time Hours
  SET_WATERING_TIME_WEEKDAYS,   // 4  Option for selecting the days of the week when the plant will be watered.
  SET_WATERING_TIME_HOUR,       // 5  Option for setting the WateringTime hours. If provided, it moves on to WateringTime minutes.
                                //    Otherwise, it times out after 5 seconds and returns to time and date
  SET_WATERING_TIME_MINUTES,    // 6  Option for setting the WateringTime minutes. If provided, it finally sets the WateringTime time and WateringTime.
                                //    Otherwise, it times out after 5 seconds and returns to time and date
  SET_WATERING_AMOUNT,          // 7  Sets the time the watering is on.
  WATER_PLANTS,                 // 8  Water Plants
  SET_YEAR,                     // 9  For setting the time.  Year
  SET_MONTH,                    // 10 For setting the time.  Month
  SET_DAY,                      // 11 For setting the time.  Day
  SET_HOUR,                     // 12 For setting the time.  Hour
  SET_MINUTE,                   // 13 For setting the time.  Minute
  SET_SECOND,                   // 14 For setting the time.  Second
};

// Creates an LCDKeypad instance
// It handles the LCD screen and buttons on the shield
LCDKeypad lcd;

/** PCF8574 instance */
PCF8574 expander;

mcstates state; // Holds the current state of the system
int8_t button;  // Holds the current button pressed

//Global Variables
bool timeOut = true;
unsigned long timeRef;
//to detect if any button has been pressed for more than 2 seconds
bool buttonLongPress = false;
bool isWateringActive;  //if set, watering will occur
uint16_t eeAddress;  //to keep track of the last used EEPROM address


//Function prototypes
//RTC Functions
void setSystemTimeFromRTC();  ////to set the time from the RTC. Definition in the RTCFunctions
void setRTCTime(tmElements_t tm);

//LCD display functions to Show time, show watering times of plants, and LCD function to select elements for set plant watering plant.  Definition in LCDFunctions
void showTime();
void showWateringTime();
uint16_t LCDUpdateElement(char* strTitle, char* strElementName, uint16_t initialValue, uint16_t minValue, uint16_t maxValue);  //get the watering time (Hours minutes) - to update the Plant  
uint16_t LCDUpdateElement(const __FlashStringHelper* strTitle, const __FlashStringHelper* strElementName, uint16_t initialValue, uint16_t minValue, uint16_t maxValue);   // overloaded function to be able to use the F() macro
void showVersionOnLCD();  //Show version of sketch
uint8_t LCDUpdateWeekDays(uint8_t initialValue);  //get the weekdays which the plant has to be watered - to updat the Plant
//uint8_t LCDUpdateWeekDays(const __FlashStringHelper* strTitle, uint8_t initialValue);   // overloaded function to be able to use the F() macro

//functions for checking if any watering time has occurred, and function to water plants.  Defined in Check and water plants
void waterPlants();  //in CheckandWaterPlants
bool checkWateringTime();

//Functions for the 4 steps to set the watering time of a plant
void setWateringTimeWeekdays(); //in SetPlantWateringTime
void selectPlantNumber(); //inSetPlantWateringTime
void setWateringTimeHour();
void setWateringTimeMinute();
void setWateringAmount();

//Statetransition.  Effects the transition from one state to another, based on triggers recieved
void Statetransition(uint8_t trigger, bool buttonLongPress = false);

//Logger Functions
void digitalClockDisplayonLogger(uint8_t LogLevel = LOG_LEVEL_NOTICE);
void plantDetailsonLogger(uint8_t initialspace);

//Functions for storing and retreiving state in EEPROM.   in EEPROMFunctions
void storePlantinEEPROM() ;
bool getPlantsFromEEPROM();
void storeWateringStatus();
void getWateringStatus();

//Functions for the 6 steps to set time
void setYear();
void setMonth();
void setDay();
void setHour();
void setMinute();
void setSecond();

//Utility Functions.
char * formatDigits(uint16_t digits, char *FormattedString);
//char * getFormattedTime(uint8_t iHour, uint8_t iMinute, uint8_t iSecond, char *FormattedTime);
char* weekDisplay(uint8_t byteToString);
char* weekDisplayShort(uint8_t byteToString);
int freeRam ();