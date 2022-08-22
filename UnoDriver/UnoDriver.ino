/* 
 * Mixing 2 channels from an RC hobby controller for arcade drive
 * https://gist.github.com/ShawnHymel/ccc28335978d5d5b2ce70a2a9f6935f4
 *
 *|Joystick    |   X   |   Y   |  Left | Right |    Robot    |
 *|            |       |       |  Y+X  |  Y-X  |     Move    |
 *------------------------------------------------------------
 *|     o      |   0   |   0   |   0   |   0   | Stop        |
 *|     ^      |   0   |  255  |  255  |  255  | Forward     |
 *|     /      |  255  |  255  |  510  |   0   | Turn Right  |
 *|     >      |  255  |   0   |  255  | -255  | Spin Right  |
 *|     \_     |  255  | -255  |   0   | -510  | Back Right  |  
 *|     v      |   0   | -255  | -255  | -255  | Backward    |
 *|    _/      | -255  | -255  | -510  |   0   | Back Left   |
 *|     <      | -255  |   0   | -255  |  255  | Spin Left   |
 *|     \      | -255  |  255  |   0   |  510  | Turn Left   |
 *
 *******************************************************************************/
#include "lcd_display.h"
#include "motor.h"
//#include "encoder.h"
#include "rc_controller.h"
//#include "my_ultrasonic.h"
#include "imu.h"
#include "quad_encoder.h"

#define SPEED_LIMIT   0.7
#define SWITCH_PIN    10

// Motors E-STOP Interrupt PIN
const byte ledPin = 13;
//const byte eStopPin = 21;

int driveMode = 0;                        // variable for reading the pushbutton status
unsigned long lastctrl;
static long lbDistance =0; 
static long rbDistance =0; 
static bool stopFlag =false;
float ctrlrate=3.0;


// Motors status
int left_motor= 0;
int right_motor= 0;

signed int speedLeft, speedRight;


// Print motors status & robot moves
void printMotorStatus(int x, int y, int left_motor, int right_motor){
  char buff[50];
  String robot_move = String("");
  
  if (left_motor > 0 && right_motor > 0){
    robot_move = String("[Forward   ]");
  } else if (left_motor > 0 && right_motor == 0){
    robot_move = String("[Turn Right]");
  } else if (left_motor > 0 && right_motor < 0){
    robot_move = String("[Spin Right]");
  } else if (left_motor == 0 && right_motor < 0){
    robot_move = String("[Back Right]");
  } else if (left_motor < 0 && right_motor < 0){
    robot_move = String("[Backward  ]");
  } else if (left_motor < 0 && right_motor == 0){
    robot_move = String("[Back Left ]");
  } else if (left_motor < 0 && right_motor > 0){
    robot_move = String("[Spin Left ]");
  } else if (left_motor == 0 && right_motor > 0){
    robot_move = String("[Turn Left ]");
  }else if (left_motor == 0 && right_motor == 0){
    robot_move = String("[Stop      ]");
  }
  sprintf(buff, "%s X: %4d Y: %4d  |  L: %4d R: %4d", robot_move.c_str(), x, y, left_motor, right_motor);
  Serial.println(buff);

}

void estop() {
  //Motors stop
  drive(0, 0);
  // Turn on LED
  digitalWrite(ledPin, HIGH);
  Serial.println("eStop activated!!!");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Controller's started!"); 
  // Setup internal LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // initialize the switch pin as an input:
  pinMode(SWITCH_PIN, INPUT);
  driveMode = digitalRead(SWITCH_PIN);
  Serial.print("Drive Mode: "); 
  Serial.println(driveMode); 
  

  

  // Setup interrupt pin for motor encoders
  //pinMode(eStopPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(eStopPin), estop, LOW);
  
  #ifdef ENABLE_LCD
    display_setup(driveMode);
  #endif
  
  q_encoder_setup();
  #ifdef ENABLE_ENCODER
    encoder_setup();
  #endif

  #ifdef BNO085
    BNO085_setup();
  #endif
  
  lastctrl=millis();
  left_motor = 50;
  right_motor = 50;
  
}

void planedDrive(int *left_m, int *right_m){
  const int d1 = 1800;
  const float turn180Dist = (2*PI*180.11)*0.71;       // 180 degree turn distance 4.5",5.48",r=7.09", circu=1131.66/2
 
  
  if(abs(lbDistance)>= d1+(int)(261.62+228.6+261.62+80)+d1){      
    *left_m =  0;
    *right_m = 0;
    stopFlag = true;
    lcd.setCursor (0, 0); // move to the begining of the second line
    lcd.print("!");   
  }else if(abs(lbDistance)>= d1+(int)(turn180Dist)){      // (261.62+228.6+261.62+20)
    *left_m =  50;
    *right_m = 50;
    lcd.setCursor (0, 0); // move to the begining of the second line
    lcd.print("^");       
  }else if(abs(lbDistance) >= d1){      // 1800
    *left_m = -50;
    *right_m = 50;
    lcd.setCursor (0, 0); // move to the begining of the second line
    lcd.print(">"); 
  }else if(!stopFlag){
    lcd.setCursor (0, 0); // move to the begining of the second line
    lcd.print("^");     
  }


}

void rcDrive(int *x, int *y, int *left_m, int *right_m){
  
  // Read R/C Controller
  getSpeed(x, y);  
  
  // Mix for arcade drive
  *left_m = *y + *x;
  *right_m = *y - *x;
}
void loop()
{
  //static long currentEnc;
  //static float wheelSpeed;
  int x, y;


  //distance = getUltrasonicDist(ULTRASONIC_FRONT);
  //if((distance<=15 && distance>0 && (left_motor+right_motor)>0)){
  //  left_motor = -10;
  //  right_motor = -10;
  //}
  

  long deltaT= millis()-lastctrl;
 
  if(driveMode && deltaT%200==0 && !stopFlag){   
      encLeft.CountTick();
      encRight.CountTick();
      
      lbDistance = encLeft.getDistance();
      rbDistance = encRight.getDistance();  
      //currentEnc = encLeft.getCurrentEnc();
      //wheelSpeed = encLeft.getSpeed();

      encLeft.saveTick();
      encRight.saveTick();
       

      lcd.setCursor (1, 0); // move to the begining of the second line
      lcd.print("LB DIST: ");
      lcd.print(lbDistance/10);
      lcd.setCursor (14, 0); // move to the begining of the second line
      lcd.print("CM");
      
  
      lcd.setCursor (1, 1); // move to the begining of the second line
      lcd.print("RB DIST: ");
      lcd.print(rbDistance/10);  
      lcd.setCursor (14, 1); // move to the begining of the second line
      lcd.print("CM");

      
    }


    // Drive motor
    drive((int)left_motor*SPEED_LIMIT, (int)right_motor*SPEED_LIMIT);
    /*Serial.print("Left Pluse = ");
    Serial.print(encLeft.getCurrentEnc());
    Serial.print("\tRight Pluse = ");
    Serial.print(encRight.getCurrentEnc());
    Serial.println();
    */
    if(driveMode){
      planedDrive(&left_motor, &right_motor);
    }else{
      rcDrive(&x, &y, &left_motor, &right_motor); 
    }


  //printMotorStatus(x, y, left_motor, right_motor);
  /*display_show(0,0,"LEFT :");
  display_show_int(6, 0, 3, x);
  display_show(0,2,"RIGHT:");
  display_show_int(6, 2, 3, y);
*/
  //printUltrasonicDist();
  //getHeadingDir(&rvc_data);delay(200);
 //plot_encoders(LEFT_BACK_ENC);

 //print_changed_encoders(LEFT_BACK_ENC);
 //print_changed_encoders(RIGHT_BACK_ENC);


 /*unsigned long currentMillis = millis();
 if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;  
 }*/
 
}
