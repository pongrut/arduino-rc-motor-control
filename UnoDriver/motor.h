/* 
 * This example shows how to control MDDS30 in PWM mode with Arduino.
 * Set MDDS30 input mode to 0b10110100
 * 
 * Motor Controls: 
 * Send   0: Motor stops
 * Send  50: Motor rotates CW with half speed
 * Send  100: Motor rotates CW with full speed
 * Send  -50: Motor rotates CCW with half speed
 * Send -100: Motor rotates CCW with full speed
 * 
 * Note: This example also compatible with MDDS10 and MDDS60
 *
 * Hardware Connection:
 *   Arduino Uno    MDDS30
 *   GND ---------- GND
 *   8 ------------ IN1
 *   6 ------------ AN1
 *   5 ------------ AN2
 *   7 ------------ IN2
 *   
 *   
 * Related Products:
 * - SmartDriveDuo-30: http://www.cytron.com.my/n P-MDDS60
 * - CT UNO: http://www.cytron.com.my/p-ct-uno
 * - DC Brush Motors: http://www.cytron.com.my/c-84-dc-motor
 * - LiPo Battery: http://www.cytron.com.my/c-87-power/c-97-lipo-rechargeable-battery-and-charger
 * - Male to Male Jumper: https://www.cytron.com.my/p-wr-jw-mm65
 *
 * URL: http://www.cytron.com.my
 * 
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
//#define MDD10
// uncomment #define MDDS30 to compile this for using MDDS30 driver
#define MDDS30

#include <Cytron_SmartDriveDuo.h>
#include "CytronMotorDriver.h"


#ifdef MDDS30 
  #define MOTOR_MIN_SPEED    -100
  #define MOTOR_MAX_SPEED     100
  #define IN1 7 // Arduino pin 7 is connected to MDDS30 pin IN1. Dir Left
  #define AN1 9 // Arduino pin 9 is connected to MDDS30 pin AN1. Speed Left
  #define AN2 8 // Arduino pin 8 is connected to MDDS30 pin AN2. Speed Right
  #define IN2 6 // Arduino pin 6 is connected to MDDS30 pin IN2. Dir Right
  Cytron_SmartDriveDuo smartDriveDuo30(PWM_INDEPENDENT, IN1, IN2, AN1, AN2);
#else
  // MDD10 or MDD3A Driver
  #define MOTOR_MIN_SPEED    -255
  #define MOTOR_MAX_SPEED     255
  #define PWM1 9 // Arduino pin 9 is connected to MDD10 pin PWM 1. Dir Left
  #define DIR1 8 // Arduino pin 8 is connected to MDD10 pin DIR 1. Speed Left
  #define PWM2 11 // Arduino pin 11 is connected to MDD10 pin PWM 2. Speed Right
  #define DIR2 13 // Arduino pin 13 is connected to MDD10 pin DIR 2. Dir Right
  // Configure the motor driver.
  CytronMD motor1(PWM_DIR, PWM1, DIR1); 
  CytronMD motor2(PWM_DIR, PWM2, DIR2);  
#endif








// Positive for forward, negative for reverse
void drive(int speed_a, int speed_b) {

  // Limit speed between MOTOR_MIN_SPEED and MOTOR_MAX_SPEED
  speed_a = constrain(speed_a, MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);
  speed_b = constrain(speed_b, MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);

  // Set speed to motors

  #ifdef MDDS30
    smartDriveDuo30.control(speed_a, speed_b);
  #else
    motor1.setSpeed(speed_a);   // Motor 1 runs at speed_a speed.
    motor2.setSpeed(speed_b);   // Motor 2 runs at speed_b speed.
  #endif
}
