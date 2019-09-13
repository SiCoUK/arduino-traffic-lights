void loop() {
  //readSwitch();          //Read the pedestrian switch
  pedButton.process();
  pedBoxButton.process();
  flashButton.process();
  //allRedButton.process();
  manualButton.process();

  upRedButton.process();
  upAmberButton.process();
  upGreenButton.process();
  //downRedButton.process();
  //downAmberButton.process();
  //downGreenButton.process();
  pedRedButton.process();
  pedGreenButton.process();
  
  manualLights();
  trafficLights();       //Run the Traffic light state machine
}
