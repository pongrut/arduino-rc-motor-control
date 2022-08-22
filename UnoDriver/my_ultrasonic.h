                                                         /*
    UltrasonicDisplayOnTerm.ino
    Example sketch for ultrasonic ranger

    Copyright (c) 2012 seeed technology inc.
    Website    : www.seeed.cc
    Author     : LG, FrankieChu
    Create Time: Jan 17,2013
    Change Log :

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/


/***************************************************************************/
//	Function: Measure the distance to obstacles in front and print the distance
//			  value to the serial terminal.The measured distance is from
//			  the range 0 to 400cm(157 inches).
//	Hardware: Grove - Ultrasonic Ranger
//	Arduino IDE: Arduino-1.0
//  https://wiki.seeedstudio.com/Grove-Ultrasonic_Ranger/
/*****************************************************************************/

#include "Ultrasonic.h"

#ifdef MDDS30 
  #define ULTRASONIC_FRONT    2  // PIN 2 is for front sensor
#else
  // MDD10 or MDD3A Driver
  #define ULTRASONIC_FRONT    7  // PIN 7 is for front sensor
#endif


Ultrasonic ultra_front(ULTRASONIC_FRONT);

long getUltrasonicDist(int sensor_id){
  switch(sensor_id){
    case ULTRASONIC_FRONT:
      return ultra_front.MeasureInCentimeters();
  }
    
}

// Print motors status & robot moves
void printUltrasonicDist(){
    long RangeInCentimeters = getUltrasonicDist(ULTRASONIC_FRONT); 
    Serial.print("[Distance  ] Front : ");
    Serial.print(RangeInCentimeters); //0~400cm
    Serial.println(" cm");
  
}
