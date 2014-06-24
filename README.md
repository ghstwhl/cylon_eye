cylon_eye
=========

Arduino sketch for controlling the Cylon Eye build documented at http://www.instructables.com/id/Put-a-Cylon-in-it/


This latest version supports two new features:

 1) A nine pixel strand of WS8211/WS8212 LEDs for a preview of the activity on the 25 pixel TCL strand
 2) Auto-dimming, controlled by a CdS photocell connected to A5 with a 1k pull-down to ground
 
 While the code will run fine without the preview strand, it will likely be very erratic without the photocell.