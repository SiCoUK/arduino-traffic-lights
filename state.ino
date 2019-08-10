void manualLights() {
  if (!manual) {
    return;
  }
  
  switch(manualState) {
    case ALL_RED: {
      if (stateEntered == false) {
        Serial.println("MANUAL ALL RED");
        stateEntered = true;
  
        setLightsRed();
        setPedLightsRed();
  
        lcdCarStatus("Red");
        lcdPedStatus("Red");

        // Technically this is UP_RED
        lastUpState = UP_RED;
      }
      break;
    }
    case UP_RED: {
      if (stateEntered == false) {
        Serial.println("MANUAL UP RED");
        stateEntered = true;

        setLightsRed();
        lcdCarStatus("Red");

        lastUpState = manualState;
      }
      break;
    }
    case UP_AMBER: {
      
      if (stateEntered == false) {
        Serial.println("MANUAL UP_AMBER Enter");
        stateEntered = true;
        
        lcdCarStatus("Red/Amber");

        // Decide whether to show red/amber or just amber
        if (lastUpState == UP_GREEN) {
          setLightsAmber();
        } else if (lastUpState == UP_RED) {
          setLightsAmberRed();
        }
        lastUpState = manualState;
      }
      
      break;
    }
    case UP_GREEN: {
      
      if (stateEntered == false) {
        Serial.println("MANUAL UP_GREEN Enter");
        stateEntered = true;
        
        lcdCarStatus("Green");
        setLightsGreen();

        lastUpState = manualState;
      }
      
      break;
    }
    case FLASH_AMBER: { // Turn on flash amber
      if (stateEntered == false) {
        Serial.println("FLASH AMBER");

        // Change all lights to off
        setLightsOff();
        lcdCarStatus("Amber Flash");
        
        stateEntered = true;
      }
      
      blinkAmber();

      break;
    }
    case FLASH_AMBER_OFF: { // Turn off flash amber
      Serial.println("FLASH AMBER OFF");

      // Change to all red
      manualState = ALL_RED;
      
      break;
    }
  }

  // Pedestrian State Machine
  switch(manualState) {
    
    case WALK_OFF: {
      if (stateEntered == false) {
        Serial.println("MANUAL PED RED");
        stateEntered = true;

        setPedLightsRed();
        lcdPedStatus("Red");
        pedState = PED_RED;
  
        //lastDownState = manualState;
      }
      break;
    }
    case WALK_ON: {
      if (stateEntered == false) {
        Serial.println("MANUAL PED GREEN");
        stateEntered = true;

        setPedLightsGreen();
        lcdPedStatus("Green");
        pedState = PED_GREEN;
  
        //lastDownState = manualState;
      }
      break;
    }
  }
}

void trafficLights() {
  if (manual) {
    return;
  }
  
  switch(autoState) {
    case ALL_RED: {
      Serial.println("ALL_RED");

      // Change all lights to off and turn on reds
      setLightsRed();

      lcdCarStatus("Red");
      lcdPedStatus("Red");
      
      autoState = UP_RED;
      break;
    }
    case UP_RED: { // Up head red
      if (stateEntered == false) {
        Serial.println("UP_RED Enter");

        if (sequence == 1) {
          sequence = 2;
        } else {
          sequence = 1;
        }
        
        lcdCarStatus("Red");
        setCountdown(CAR_TIMER, 5);

        //If the pedestrian switch has been pressed during this cycle
        checkPedestrian();
        
        stateEntered = true;
      }

      runCountdown(CAR_TIMER, carClock);

      //Serial.println("UP_RED Body");
      //if (millis() - chrono >=  5000) {
      if (countdownEnded(CAR_TIMER)) {
        Serial.print("UP_RED Exit");
        carClock.clear();
        chrono = millis();
        autoState = UP_REDAMBER;
        stateEntered = false;
      }
      break;
    }
    case UP_REDAMBER: { // Up headht red / amber

      if (stateEntered == false) {
        Serial.println("UP_REDAMBER Enter");
        stateEntered = true;
        lcdCarStatus("Red/Amber");
        setCountdown(CAR_TIMER, 2);

        switch (sequence) {
          case 1: {
            //changeButtonLed(MAIN_R_PIN, buttonLedBrightness);
            changeButtonLed(MAIN_Y_PIN, buttonLedBrightness);
            ////relayCar.turn_on_channel(carRedRelay);
            relayCar.turn_on_channel(carAmberRelay);
            break;
          }
          case 2: {
            //changeButtonLed(PINE_R_PIN, buttonLedBrightness);
            changeButtonLed(PINE_Y_PIN, buttonLedBrightness);
            break;
          }
        }
      }
      
      runCountdown(CAR_TIMER, carClock);
      
      //if (millis() - chrono >=  2000) {
      if (countdownEnded(CAR_TIMER)) {
        Serial.print("UP_REDAMBER Exit");
        chrono = millis();
        autoState = UP_GREEN;
        carClock.clear();
        stateEntered = false;
      } 
      break;
    }
    case UP_GREEN: { // Up head green

      if (stateEntered == false) {
        Serial.println("UP_GREEN Enter");
        stateEntered = true;
        
        lcdCarStatus("Green");
        setCountdown(CAR_TIMER, 5);

        switch (sequence) {
          case 1: {
            changeButtonLed(MAIN_R_PIN, 0);
            changeButtonLed(MAIN_Y_PIN, 0);
            changeButtonLed(MAIN_G_PIN, buttonLedBrightness);
            relayCar.turn_off_channel(carRedRelay);
            relayCar.turn_off_channel(carAmberRelay);
            relayCar.turn_on_channel(carGreenRelay);
            break;
          }
          case 2: {
            changeButtonLed(PINE_R_PIN, 0);
            changeButtonLed(PINE_Y_PIN, 0);
            changeButtonLed(PINE_G_PIN, buttonLedBrightness);
            break;
          }
        }
      }

      runCountdown(CAR_TIMER, carClock);

      //if (millis() - chrono >=  2000) {
      if (countdownEnded(CAR_TIMER)) {
        Serial.print("UP_GREEN Exit");
        chrono = millis();
        autoState = UP_AMBER;
        carClock.clear();
        stateEntered = false;
      } 
      break;
    }
    case UP_AMBER: { // Up head amber

      if (stateEntered == false) {
        Serial.println("UP_AMBER Enter");
        stateEntered = true;
        
        lcdCarStatus("Amber");
        setCountdown(CAR_TIMER, 3);

        switch (sequence) {
          case 1: {
            changeButtonLed(MAIN_G_PIN, 0);
            changeButtonLed(MAIN_Y_PIN, buttonLedBrightness);
            
            relayCar.turn_off_channel(carRedRelay);
            relayCar.turn_off_channel(carGreenRelay);
            relayCar.turn_on_channel(carAmberRelay);
            break;
          }
          case 2: {
            changeButtonLed(PINE_G_PIN, 0);
            changeButtonLed(PINE_Y_PIN, buttonLedBrightness);
            break;
          }
        }

        //digitalWrite(MAIN_G_PIN, LOW);
        //digitalWrite(MAIN_Y_PIN, HIGH);
        //blink(MAIN_Y_PIN);
      }

      runCountdown(CAR_TIMER, carClock);
      
      //if (millis() - chrono >=  2000) {
      if (countdownEnded(CAR_TIMER)) {
        Serial.print("UP_AMBER Exit");
        chrono = millis();
        autoState = ALL_RED;
        carClock.clear();
        stateEntered = false;
      } 
      break;
    }
    case WALK_ENTER: { // Walk

      // Make sure we have red traffic lights
      setLightsRed();
      lcdPedStatus("Wait");
      
      // Adds a delay to the walk transition so it doesn't happen instantly
      if (millis() - chrono >=  2000) {
        Serial.println("WALK_ENTER Exit");
        chrono = millis();
        autoState = WALK_ON;
        stateEntered = false;
      } 
      break;
    }
    case WALK_ON: { // Walk
      if (stateEntered == false) {
        Serial.println("WALK_ON Enter");
        changeButtonLed(WALK_G_PIN, buttonLedBrightness); // Turn off wait pin
        changeButtonLed(WALK_R_PIN, 0); // Turn off red pin
        changeButtonLed(WALK_W_PIN, 0); // Turn off wait pin
        //relayPed.turn_off_channel(pedWaitRelay);
        //relayPed.turn_off_channel(pedRedRelay);
        //relayPed.turn_on_channel(pedGreenRelay);
        
        lcdPedStatus("Green");
        
        chrono = millis();
        stateEntered = true;
        setCountdown(PED_TIMER, 5);
      }
      
      runCountdown(PED_TIMER, pedClock);
      
      if (millis() - chrono >=  2500) {
        Serial.println("WALK_ON Exit");
        chrono = millis();
        autoState = WALK_FLASH;
        stateEntered = false;
      } 
      break;
    }
    case WALK_FLASH: { // Walk Flash

      blinkWalkGreen();

      runCountdown(PED_TIMER, pedClock);
      
      //if (millis() - chrono >=  2500) {
      if (countdownEnded(PED_TIMER)) {
        Serial.println("WALK_FLASH Exit");
        chrono = millis();
        autoState = WALK_OFF;
        
        pedClock.clear();
      } 
      break;
    }
    case WALK_OFF: { // Don't Walk
      Serial.println("WALK OFF");
      
      pedestrians = false; //Bring the flag low and allow another request
      
      changeButtonLed(WALK_R_PIN, buttonLedBrightness); // Turn on red pin
      changeButtonLed(WALK_G_PIN, 0); // Turn off green pin

      lcdPedStatus("Red");
      
      //relayPed.turn_on_channel(pedRedRelay);
      //relayPed.turn_off_channel(pedGreenRelay);
      
      if (flashButtonOn) {
        autoState = FLASH_AMBER;
      } else {
        autoState = ALL_RED;
      }
      break;
    }
    case FLASH_AMBER: { // Turn on flash amber
      if (stateEntered == false) {
        Serial.println("FLASH AMBER");

        // Change all lights to off
        setLightsOff();
        lcdCarStatus("Amber Flash");
        
        stateEntered = true;
      }
      
      blinkAmber();

      //If the pedestrian switch has been pressed during this cycle
      checkPedestrian();
      
      break;
    }
    case FLASH_AMBER_OFF: { // Turn off flash amber
      Serial.println("FLASH AMBER OFF");

      // Change to all red
      autoState = ALL_RED;
      
      break;
    }
  }
}
