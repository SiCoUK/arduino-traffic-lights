/*
 * Turn all traffic related lights to off
 */
void setLightsOff() {
  changeButtonLed(MAIN_R_PIN, 0);
  changeButtonLed(MAIN_Y_PIN, 0);
  changeButtonLed(MAIN_G_PIN, 0);
  changeButtonLed(PINE_R_PIN, 0);
  changeButtonLed(PINE_Y_PIN, 0);
  changeButtonLed(PINE_G_PIN, 0);

  setRelayRed(false);
  setRelayAmber(false);
  setRelayGreen(false);
}

/*
 * Turn all traffic related lights to red
 */
void setLightsRed() {
  changeButtonLed(MAIN_R_PIN, buttonLedBrightness);
  changeButtonLed(MAIN_Y_PIN, 0);
  changeButtonLed(MAIN_G_PIN, 0);
  changeButtonLed(PINE_R_PIN, buttonLedBrightness);
  changeButtonLed(PINE_Y_PIN, 0);
  changeButtonLed(PINE_G_PIN, 0);
  setRelayRed(true);
  setRelayAmber(false);
  setRelayGreen(false);
}

/*
 * Turn all traffic related lights to amber
 */
void setLightsAmber() {
  changeButtonLed(MAIN_R_PIN, 0);
  changeButtonLed(MAIN_Y_PIN, buttonLedBrightness);
  changeButtonLed(MAIN_G_PIN, 0);
  setRelayRed(false);
  setRelayAmber(true);
  setRelayGreen(false);
}

/*
 * Turn all traffic related lights to amber/red
 */
void setLightsAmberRed() {
  changeButtonLed(MAIN_R_PIN, buttonLedBrightness);
  changeButtonLed(MAIN_Y_PIN, buttonLedBrightness);
  changeButtonLed(MAIN_G_PIN, 0);
  setRelayRed(true);
  setRelayAmber(true);
  setRelayGreen(false);
}

/*
 * Turn all traffic related lights to green
 */
void setLightsGreen() {
  changeButtonLed(MAIN_R_PIN, 0);
  changeButtonLed(MAIN_Y_PIN, 0);
  changeButtonLed(MAIN_G_PIN, buttonLedBrightness);
  setRelayRed(false);
  setRelayAmber(false);
  setRelayGreen(true);
}

/**
 * Set the pedestrian wait lights
 */
void setPedLightsWaitOn() {
  changeButtonLed(WALK_W_PIN, buttonLedBrightness);
  setRelayPedWait(true);
}


/**
 * Set the pedestrian wait lights off
 */
void setPedLightsWaitOff() {
  changeButtonLed(WALK_W_PIN, 0);
  setRelayPedWait(false);
}

/**
 * Set pedestrian lights to red
 */
void setPedLightsRed() {
  changeButtonLed(WALK_G_PIN, 0);
  changeButtonLed(WALK_R_PIN, buttonLedBrightness);
  //relayPed.turn_on_channel(pedRedRelay);
  //relayPed.turn_off_channel(pedGreenRelay);
}

/**
 * Set pedestrian lights to green
 */
void setPedLightsGreen() {
  changeButtonLed(WALK_R_PIN, 0);
  changeButtonLed(WALK_G_PIN, buttonLedBrightness);
  //relayPed.turn_on_channel(pedGreenRelay);
  //relayPed.turn_off_channel(pedRedRelay);
}
