/*
 TrafficLights.ino
 Author: Simon Corless
 Released: June 2019
*/
#include <Wire.h>

// General Variables
bool pedestrians = false;   //Flag to remember that a pedestrian pressed on the switch

// State Machine
enum TrafficLights {ALL_RED, UP_RED, UP_REDAMBER, UP_GREEN, UP_AMBER, DOWN_RED, DOWN_REDAMBER, DOWN_GREEN, DOWN_AMBER, WALK_ENTER, WALK_ON, WALK_FLASH, WALK_OFF, FLASH_AMBER, FLASH_AMBER_OFF};
TrafficLights autoState = ALL_RED;
TrafficLights manualState = ALL_RED;
TrafficLights lastUpState = ALL_RED;
TrafficLights lastDownState = ALL_RED;

enum PedestrianLights {PED_RED, PED_GREEN};
PedestrianLights pedState = PED_RED;

bool stateEntered = false;
unsigned long chrono;   //Light changes are triggered by time lapses

// Manual State Machine
bool manual = false;

// Traffic light sequences (junctions)
int sequence = 0; // Start sequence (o indexed)
int sequences = 2; // Total Sequences

// Relays
#include <multi_channel_relay.h>
//Multi_Channel_Relay relayPed;
Multi_Channel_Relay relayCar;

const int carRedRelay = 1;
const int carAmberRelay = 2; 
const int carGreenRelay = 3;

//const int pedRedRelay = 1;
//const int pedGreenRelay = 2;
const int pedWaitRelay = 4;

// Clocks
#include <TM1637Display.h>
// Requires 2 pins, one for data and 2 for luminance
const int pedClockPin = A2;
const int pedClockLumPin = A3;
const int carClockPin = A4;
const int carClockLumPin = A5;
TM1637Display pedClock(pedClockPin, pedClockLumPin);
TM1637Display carClock(carClockPin, carClockLumPin);

// Define the timer structure
typedef struct {
    long timer;
    unsigned long now;
} timer;

timer timers[] {
    {0, 0}, // Car Timer
    {0, 0} // Pedestrian Timer
};

#define CAR_TIMER 0 // Car Timer
#define PED_TIMER 1 // Pedestrian Timer

// LCD
#include <rgb_lcd.h>
rgb_lcd lcd;

// PWM LED Board
#include "PCA9685.h"
PCA9685 buttonLed;

//The LEDs
#define MAIN_G_PIN 6    //Main Street green LED pin
#define MAIN_Y_PIN 5    //Main Street yellow LED pin
#define MAIN_R_PIN 2    //Main Street red LED pin
#define PINE_G_PIN 9    //Pine Street green LED pin
#define PINE_Y_PIN 10    //Pine Street yellow LED pin
#define PINE_R_PIN 4    //Pine Street red LED pin
//#define TURN_PIN 16    //Turn left LED pin
#define WALK_W_PIN 12    //Pedestrian LED pin
#define WALK_R_PIN 13    //Pedestrian LED pin
#define WALK_G_PIN 14    //Pedestrian LED pin

#define FLASH_AMBER_PIN 11    //Pedestrian LED pin
#define MANUAL_PIN 16    //Pedestrian LED pin

#define BLINK_SPEED 250        //Blink every 250 milliseconds (4 times per seconds)
unsigned long blinkChrono;     //We will use the Blink Without Delay algorithm (See part 1)

//int buttonLedPin = 1;           // the PWM pin the LED is attached to
int buttonLedPins = 16;           // the PWM number of pins used (max 16)
int buttonLedBrightness = 4000;    // how bright the LED is
//int buttonLedFade = 50;    // how many points to fade the LED by

// Buttons
#include <Button.h>

#define FLASH_BUTTON 26
#define ALL_RED_BUTTON 40
#define MANUAL_BUTTON 34
#define UP_RED_BUTTON 52
#define UP_AMBER_BUTTON 50
#define UP_GREEN_BUTTON 48
#define DOWN_RED_BUTTON 46
#define DOWN_AMBER_BUTTON 44
#define DOWN_GREEN_BUTTON 42

#define PED_WAIT_BUTTON 28
#define PED_RED_BUTTON 24
#define PED_GREEN_BUTTON 32

Button flashButton = Button(FLASH_BUTTON, BUTTON_PULLUP_INTERNAL);
Button allRedButton = Button(ALL_RED_BUTTON, BUTTON_PULLUP_INTERNAL);

Button manualButton = Button(MANUAL_BUTTON, BUTTON_PULLUP_INTERNAL);
Button upRedButton = Button(UP_RED_BUTTON, BUTTON_PULLUP_INTERNAL);
Button upAmberButton = Button(UP_AMBER_BUTTON, BUTTON_PULLUP_INTERNAL);
Button upGreenButton = Button(UP_GREEN_BUTTON, BUTTON_PULLUP_INTERNAL);
Button downRedButton = Button(DOWN_RED_BUTTON, BUTTON_PULLUP_INTERNAL);
Button downAmberButton = Button(DOWN_AMBER_BUTTON, BUTTON_PULLUP_INTERNAL);
Button downGreenButton = Button(DOWN_GREEN_BUTTON, BUTTON_PULLUP_INTERNAL);

Button pedButton = Button(PED_WAIT_BUTTON, BUTTON_PULLUP_INTERNAL);
Button pedRedButton = Button(PED_RED_BUTTON, BUTTON_PULLUP_INTERNAL);
Button pedGreenButton = Button(PED_GREEN_BUTTON, BUTTON_PULLUP_INTERNAL);

bool manualButtonOn = false;

void manualButtonOnPress(Button& b){
  Serial.print("onPress: ");
  Serial.println(b.pin);
  //Serial.println(b.stateChanged());
  
  if (manualButtonOn == false) {
    manualButtonOn = true;
    changeButtonLed(MANUAL_PIN, buttonLedBrightness);
    manual = true;
    stateEntered = false;
  } else {
    manualButtonOn = false;
    changeButtonLed(MANUAL_PIN, 0);
    manual = false;
  }

  manualState = ALL_RED;
  autoState = ALL_RED;
}

void allRedButtonOnPress(Button& b){
  Serial.print("onPress: ");
  Serial.println(b.pin);
}

// Manual Button Press Method
void manualLightButtonOnPress(Button& b){
  Serial.print("onPress: ");
  Serial.println(b.pin);

  // If not in manual do nothing
  if (manual == false) {
    return;
  }

  switch(b.pin) {
    case UP_RED_BUTTON: {
      manualState = UP_RED;
      break;
    }
    case UP_AMBER_BUTTON: {
      manualState = UP_AMBER;
      break;
    }
    case UP_GREEN_BUTTON: {
      manualState = UP_GREEN;
      break;
    }
    case DOWN_RED_BUTTON: {
      manualState = DOWN_RED;
      break;
    }
    case DOWN_AMBER_BUTTON: {
      manualState = DOWN_AMBER;
      break;
    }
    case DOWN_GREEN_BUTTON: {
      manualState = DOWN_GREEN;
      break;
    }
    case PED_RED_BUTTON: {
      manualState = WALK_OFF;
      break;
    }
    case PED_GREEN_BUTTON: {
      manualState = WALK_ON;
      break;
    }
  }

  stateEntered = false;
  
}

// Detect a pedestrian button press
void pedButtonOnPress(Button& b){
    Serial.print("onPress: ");
    Serial.println(b.pin);
  
    if (manual == true && pedestrians == true) {
        pedestrians = false; //Set the pedestrian flag as false (The switch has been unclicked)
    
        setPedLightsWaitOff();
        
        if (pedState == PED_RED) {
            lcdPedStatus("Red");
        } else {
            lcdPedStatus("Green");
        }
        return;
    }
  
    // Check if the flag is not already true
    if (pedestrians == true) {
        return;
    }
  
    pedestrians = true; //Set the pedestrian flag as true (The switch has been clicked)
    setPedLightsWaitOn();
    lcdPedStatus("Request");
}

// Detect a flash button press
bool flashButtonOn = false;
void flashButtonOnPress(Button& b){
  Serial.print("onPress: ");
  Serial.println(b.pin);

  // Do nothing if the pedestrian request is true
  if (pedestrians == true && manual == false) {
    return;
  }

  if (flashButtonOn == false) {
    flashButtonOn = true;
    autoState = FLASH_AMBER; //Change state to flash amber
    manualState = FLASH_AMBER; //Change state to flash amber
    changeButtonLed(FLASH_AMBER_PIN, buttonLedBrightness);
    stateEntered = false;
  } else {
    flashButtonOn = false;
    autoState = FLASH_AMBER_OFF; //Change state to flash amber off
    manualState = FLASH_AMBER_OFF; //Change state to flash amber off
    changeButtonLed(FLASH_AMBER_PIN, 0);
    stateEntered = false;
  }
}

void blink(byte ID) {
  
  if (millis() - blinkChrono >= BLINK_SPEED) {         //If time has elapsed
    blinkChrono = millis();                              //Reset chrono
    Serial.println("BLINK");
    Serial.println(ID);
    digitalWrite(ID, !digitalRead(ID));                  //Change pin's value
  }
}

// Setup
void setup() {
  Serial.begin(9600);
  Serial.println("Booting...");

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  /*for (byte i =2 ; i <= 20 ; i++) {       //For all 8 LEDs
    pinMode(i, OUTPUT);                    //Mode is OUTPUT
    digitalWrite(i, LOW);                  //All lights are closed
  }*/

  // PWM BOARD I2C
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();
  buttonLed.init(0x7f);
  // Set freq to 100Hz, range from 24Hz~1526hz
  buttonLed.setFrequency(100);
  // All on
  for (int i=1; i<=buttonLedPins; i++) {
     buttonLed.setPwm(i, 0, buttonLedBrightness);
  }
  delay(500);
  
  bootLcd();

  // Timers
  pinMode(pedClockPin, OUTPUT);
  pinMode(pedClockLumPin, OUTPUT);
  pinMode(carClockPin, OUTPUT);
  pinMode(carClockLumPin, OUTPUT);
  bootTimer();

  // All off
  for (int i=1; i<=buttonLedPins; i++) {
     buttonLed.setPwm(i, 0, 0);
  }
  // END PWM BOARD

  // Boot Relay
  // Set I2C address and start ped relay0.
  //relayPed.begin(0x11);
  // Set I2C address and start car relay
  relayCar.begin(0x21);
  bootRelay();

  //setLightsRed();
  
  //pinMode(switchPin, INPUT_PULLUP);      //Init the switch in PULLUP mode
  //digitalWrite(MAIN_R_PIN, HIGH);          //Main Street's red light is on
  //digitalWrite(PINE_R_PIN, HIGH);          //Main Street's red light is on
  //relayCar.turn_on_channel(carRedRelay);

  // Buttons
  pedButton.pressHandler(pedButtonOnPress);
  flashButton.pressHandler(flashButtonOnPress);
  allRedButton.pressHandler(allRedButtonOnPress);
  manualButton.pressHandler(manualButtonOnPress);
  
  upRedButton.pressHandler(manualLightButtonOnPress);
  upAmberButton.pressHandler(manualLightButtonOnPress);
  upGreenButton.pressHandler(manualLightButtonOnPress);
  downRedButton.pressHandler(manualLightButtonOnPress);
  downAmberButton.pressHandler(manualLightButtonOnPress);
  downGreenButton.pressHandler(manualLightButtonOnPress);
  
  pedRedButton.pressHandler(manualLightButtonOnPress);
  pedGreenButton.pressHandler(manualLightButtonOnPress);

  Serial.println("...Boot Done");
}

// Boot the LCD display
void bootLcd() {
  Serial.println("Function: Boot LCD");
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print(" Traffic Master");
  lcd.setCursor(0, 1);
  lcd.print("----- 2000 -----");
}

// Boot Timers
void bootTimer()
{
  Serial.println("Function: Boot Timer");

  carClock.setBrightness(0x0f);
  pedClock.setBrightness(0x0f);

  //pedClock.init();
  //pedClock.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  //carClock.init();
  //carClock.set(BRIGHT_TYPICAL);

  /*for (int n = 0; n <= 1234; n++)
  {
     pedClock.showNumberDec(n);
     carClock.showNumberDec(n);
  }*/

  pedClock.showNumberDec(1234);
  carClock.showNumberDec(1234);
  delay(500);
  pedClock.clear();
  carClock.clear();
}

// Boot the relays and reset
void bootRelay() {
  Serial.println("Function: Boot Relays");
  /*Serial.println("Relay 1 Test");
  relayPed.turn_on_channel(1);
  delay(500);
  relayPed.turn_off_channel(1);
  relayPed.turn_on_channel(2);
  delay(500);
  relayPed.turn_off_channel(2);
  relayPed.turn_on_channel(3);  
  delay(500);
  relayPed.turn_off_channel(3);
  relayPed.turn_on_channel(4);  
  delay(500);
  relayPed.turn_off_channel(4);
  
  Serial.println("-- End Relay 1 Test --");*/
  
  Serial.println("Relay Initialisation");
  //relayCar.turn_on_channel(1);
  setRelayRed(true);
  delay(500);
  //relayCar.turn_off_channel(1);
  //relayCar.turn_on_channel(2);
  setRelayRed(false);
  setRelayAmber(true);
  delay(500);
  //relayCar.turn_off_channel(2);
  //relayCar.turn_on_channel(3);
  setRelayAmber(false);
  setRelayGreen(true);
  delay(500);
  //relayCar.turn_off_channel(3);
  //relayCar.turn_on_channel(4);
  setRelayGreen(false);
  setRelayPedWait(true);
  delay(500);
  //relayCar.turn_off_channel(4);
  setRelayPedWait(false);
  Serial.println("-- End Relay Initialisation --");
}
