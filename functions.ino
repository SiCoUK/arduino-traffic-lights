/**
 * Turn the car red channel on or off
 */
void setRelayRed(boolean channelState) {
    changeCarRelay(channelState, carRedRelay);
}

/**
 * Turn the car amber channel on or off
 */
void setRelayAmber(boolean channelState) {
    changeCarRelay(channelState, carAmberRelay);
}

/**
 * Turn the car green channel on or off
 */
void setRelayGreen(boolean channelState) {
    changeCarRelay(channelState, carGreenRelay);
}

/**
 * Turn the pedestrian wait channel on or off
 */
void setRelayPedWait(boolean channelState) {
    changeCarRelay(channelState, pedWaitRelay);
}

/**
 * Turn the car relay channel on or off
 */
void changeCarRelay(boolean channelState, int channel)
{
    if (channelState == true) {
        relayCar.turn_on_channel(channel);
    } else {
        relayCar.turn_off_channel(channel);
    }
}

/**
 * Change a buttons PWM pulse
 */
void changeButtonLed(int pin, int brightness) {
  //buttonLed.setPwm(pin, 0, brightness);
  analogWrite(pin, brightness);
}

/**
 * Check if the pedestrian button has been pressed
 */
void checkPedestrian() {
  //If the pedestrian switch has been pressed during this cycle
  if (pedestrians) {
    autoState = WALK_ENTER; //Change state to walk light green
    chrono = millis();
    stateEntered = false;
  }
  //return pedestrians;
}

// Blink the walk green light
bool blinkWalkState;
void blinkWalkGreen() {

  int blinkBrightness;

  if (millis() - blinkChrono >= BLINK_SPEED) {         //If time has elapsed
    blinkChrono = millis();                              //Reset chrono
    //Serial.println("BLINK WALK GREEN");

    if (blinkWalkState == true) {
      blinkBrightness = 0;
      blinkWalkState = false;
      //relayPed.turn_off_channel(pedGreenRelay);
      //Serial.println("BLINK WALK GREEN OFF");
    } else {
      blinkBrightness = buttonLedBrightness;
      blinkWalkState = true;
      //relayPed.turn_on_channel(pedGreenRelay);
      //Serial.println("BLINK WALK GREEN ON");
    }
    
    //digitalWrite(pin, !digitalRead(pin));                  //Change pin's value
    changeButtonLed(WALK_G_PIN, blinkBrightness);
  }
}

// Blink the amber light
bool blinkFlashState;
void blinkAmber() {

  int blinkBrightness;

  if (millis() - blinkChrono >= BLINK_SPEED) {         //If time has elapsed
    blinkChrono = millis();                              //Reset chrono
    //Serial.println("BLINK AMBER FLASH");

    if (blinkFlashState == true) {
      blinkBrightness = 0;
      blinkFlashState = false;
      setRelayAmber(false);
      //Serial.println("BLINK AMBER FLASH OFF");
    } else {
      blinkBrightness = buttonLedBrightness;
      blinkFlashState = true;
      setRelayAmber(true);
      //Serial.println("BLINK AMBER FLASH ON");
    }
    
    //digitalWrite(pin, !digitalRead(pin));                  //Change pin's value
    changeButtonLed(FLASH_AMBER_PIN, blinkBrightness);
    changeButtonLed(MAIN_Y_PIN, blinkBrightness);
    //changeButtonLed(PINE_Y_PIN, blinkBrightness);
  }
}

/**
 * int timer The duration of the timer in seconds
 */
void setCarCountdown(int timer)
{
  setCountdown(CAR_TIMER, timer);
}

/**
 * int timer The duration of the timer in seconds
 */
void setPedCountdown(int timer)
{
  setCountdown(PED_TIMER, timer);
}

/**
 * Set the time of a countdown timer
 */
void setCountdown(int timerId, int timer)
{
  timers[timerId].timer = timer;
}

/**
 * Check if the countdown timer has ended
 */
bool countdownEnded(int timerId) {
  if (timers[timerId].timer == -1) {
    return true;
  }
  return false;
}

/**
 * int timer The duration of the timer in seconds
 */
void runCarCountdown()
{
  runCountdown(CAR_TIMER, carClock);
}

/**
 * int timer The duration of the timer in seconds
 */
void runPedCountdown()
{
  runCountdown(PED_TIMER, carClock);
}

/**
 * Run the countdown timer and display
 */
void runCountdown(int timerId, TM1637Display clockDisplay) {

  if ((millis() - timers[timerId].now >= 1000) && timers[timerId].timer == 0) {
    Serial.print("Timer: ");
    Serial.println(timers[timerId].timer);
    timers[timerId].timer = timers[timerId].timer-1; // Reduce the countdown
    return;
  }

  if ((millis() - timers[timerId].now >= 1000) && timers[timerId].timer >= 0) { //If time has elapsed
    timers[timerId].now = millis(); //Reset chrono
    Serial.print("Timer: ");
    Serial.println(timers[timerId].timer);
    clockDisplay.showNumberDec(timers[timerId].timer); // Display the countdown
    timers[timerId].timer = timers[timerId].timer-1; // Reduce the countdown
  }
}

// LCD Car Status
void lcdCarStatus(String message)
{
  clearLine(lcd, 0);
  lcd.print("Car: " + message);
  Serial.print("Car: ");
  Serial.println(message);
}

// LCD Ped Status
void lcdPedStatus(String message)
{
  clearLine(lcd, 1);
  lcd.print("Ped: " + message);
  Serial.print("Ped: ");
  Serial.println(message);
}

// LCD Clear Display
void clearLine(rgb_lcd lcd, int line)
{
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor(0, line);
}
