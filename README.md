# MBED USB-FAN

<br/> 

## Smart Fan, v2.00 

<img src="https://cloud.githubusercontent.com/assets/13679090/16360731/95350fc8-3ba3-11e6-88b5-1446c8ebb7c9.JPG" width="480px"> 

This smart-fan is primarily controlled by the remote-controller, and the A/C buttons increase/decrease the % of time (out of each 10 minute cycle) that the fan is switched on, while the B button turns the fan on/off completely. The power button indicates the number of entries logged in the SD-card, while the other buttons are not currently used. 

The remote-controller patterns are matched against the temperature & humidity measured by the RHT03 sensor, and the smart-fan will then average out the % of time the fan is switched on based on the remote-controller patterns saved in each temperature & humidity interval. A possible addition for v3 of this smart-fan could be a light sensor as well. 

<br/> 

## Smart Fan, v1.00  

The fan-timer controller - used to programatically turn on & off a USB fan at specified intervals. 
Requires 2 separate power sources as 1 USB power bank alone cannot supply enough electricity for both the fan and the LCD. 

<img src="https://cloud.githubusercontent.com/assets/13679090/9113139/6f3798b8-3c85-11e5-9a35-5118701616c9.jpg" width="480px">

<br/> 

### Libraries Used: 

[1] FATFileSystem - http://developer.mbed.org/users/mkilivan/code/FATFileSystem/ 

[2] SDFileSystem - http://developer.mbed.org/users/rs27/code/SDFileSystem/ 

[3] IRremote - http://mbed.org/users/leejong87/code/IRremote/ 

[4] RHT03 - http://developer.mbed.org/users/tristanjph/code/RHT03/ 

<br/> 

### References: 

[1] Spakrfun - https://learn.sparkfun.com/tutorials/mbed-starter-kit-experiment-guide 

[2] MBED Developers Site - https://developer.mbed.org/handbook/API-Documentation 
