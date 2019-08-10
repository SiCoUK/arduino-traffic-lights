# Arduino Powered Traffic Light Controller
Utilises an Arduino Mega with Grove Shield and Grove 4 Channel Relays to power a set of 12v traffic lights with pedestrian push button.

The original project was to power a real traffic light in the garden from 'cool' (read lots of LEDs and buttons) control panel for the kids to play with. The concept was to have a real traffic light and an imaginery traffic light (represented by the buttons lighting up) simulating a crossing (up and down traffic).

I am currently in the process of simplifying the project as although all the light up arcade buttons I have used are 'cool' the kids simply don't understand the imaginery second set of lights, they want to hit buttons and see actual changes not just the buttons light up!

## Hardware

This project currently uses:

* Old Siemens UK Traffic Light
  * The non LED traffic lights are all 240v, each head has a 12v transformer in it. I simply bypassed these and fit 12v LED bulbs.
* Old UK Pedestrian Crossing Button & Wait Display
  * Again as standard the non LED versions are 240v, I have fitted a 12v LED bulb.
* Arduino Mega
  * Swapped after I realised the Uno didn't have enough PWM pins for light up arcade buttons or inputs for the buttons
* Grove Shield
  * In the early days of my research the relays used in the Grove relay module seemed the best so I also bought in to the system for easy prototyping.
* Grove 4 Channel Relays
* Grove LCD
* Grove TM1637 Displays 'Clocks'
* Numerous Adafruit Mini Illuminated Arcade Buttons
  * Latter sets bought from Arcade World UK as exactly the same but much cheaper!
* Grove PWM Board (Servo Driver)
  * Originally bought for PWM of arcade button LEDs until I realised a Mega was a better purchase! Also I coulcn't seem to find a library for PWM effects using a board as opposed to the native pins.
