
#define LED_PIN       13               // Define internal LED pin number
#define SPEED_LIMIT   0.5             // Reduce down motor speed for testing period

#include "motor.h"                    // include motor controller program extermally
#include "rc_controller.h"            // include rc controller program extermally



// Motors status
int left_motor= 0;                    // to store left motor speed
int right_motor= 0;                   // to store right motor speed



// Funtion to print motors status & robot directions
void printMotorStatus(int x, int y, int left_motor, int right_motor){
  // buffer for storing final formatted text
  char buff[50];
  // Initiate String instant with empty text
  String robot_move = String("");

  // Check robot direction from direction of left_motor & right_motor
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
  // Create final text message with predefined template
  sprintf(buff, "%s X: %4d Y: %4d  |  L: %4d R: %4d", robot_move.c_str(), x, y, left_motor, right_motor);
  // Print message
  Serial.println(buff);

}

// Initiate function
void setup()
{
  // Set serial communication baud rate
  Serial.begin(115200);
  Serial.println("Controller's started!"); 
  
  // Setup internal LED as a output pin
  pinMode(LED_PIN, OUTPUT);
  // Turn off LED
  digitalWrite(LED_PIN, LOW);
  
}


// Get left & right motor speed from RC controller
// passing the address of x, y, left_m, right_m for storing values back
void rcDrive(int *x, int *y, int *left_m, int *right_m){
  
  // Read motor speed from R/C controller "rc_controller.h"
  getSpeed(x, y);  
  
  /* In case both channels have been pressed simultaneously
   * Mix forward/backward & left/right channel to compensate for each other
   * e.g. 
   * forward      xy(  0, 100): left 100, right 100 // Fully forward
   * forward left xy(-50, 100): left -50, right 150 // Slightly turned left
   */
  *left_m = *y + *x;
  *right_m = *y - *x;
}


// Main loop
void loop()
{
  // variables for storing motor speed
  int x, y;

  // Drive left & right motor command by passing speed value
  // also reduce speed with % of SPEED_LIMIT
  drive((int)left_motor*SPEED_LIMIT, (int)right_motor*SPEED_LIMIT);

  // Get motors speed
  rcDrive(&x, &y, &left_motor, &right_motor); 
  // Print motors speed and robot direction
  printMotorStatus(x, y, left_motor, right_motor);
 
}
