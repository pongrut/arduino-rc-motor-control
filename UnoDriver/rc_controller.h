 /* Hardware Connection:
 *
 *   Arduino Uno    FS-GT3C Receiver
 *   GND ---------- GND
 *   VCC----------- VCC
 *   2 ------------ CH1
 *   3 ------------ CH2
 */   
#include "kalman_filter.h"

#define TIMEOUT 25000 //the number of microseconds to wait for the pulse to start

#define RC_MIN_CH1          930  //FS-GT3C full forward value is around 1001
#define RC_MAX_CH1          2000  //FS-GT3C full backward value is around 1998
#define RC_MIN_CH2          930   //FS-GT3C full forward value is around 990
#define RC_MAX_CH2          2000  //FS-GT3C full backward value is around 2000


//#define MDDS30 to compile this for using MDDS30 driver in motor.h
// R/C Controller pins
#ifdef MDDS30 
 #define CH_1_PIN             4
 #define CH_2_PIN             5
 #define FORWARD_SIGN         -1     //FS-GT3C forward value will be negative sign
 #define RIGHT_SIGN           -1
#else
  // MDD10 or MDD3A Driver
  #define CH_1_PIN              2
  #define CH_2_PIN              3
  #define FORWARD_SIGN         -1     //FS-GT3C forward value will be negative sign
  #define RIGHT_SIGN            1  
#endif

SimpleKalmanFilter kf_ch1 = SimpleKalmanFilter(E_MEA, E_EST, 0.5);
SimpleKalmanFilter kf_ch2 = SimpleKalmanFilter(E_MEA, E_EST, Q_VAR);

// Parameters
const int deadzone = (int)MOTOR_MAX_SPEED*0.35;  // For MDDS30 anything between -40 and 40 is stop

// Convert RC pulse value to motor PWM value
int pulseToPWM(int pulse ,int min, int max) {
  
  // If we're receiving numbers, convert them to motor PWM
 /* if (pulse > RC_MIN_CH2 && pulse < RC_CENTER_CH2_LB) {
    pulse = map(pulse, min, RC_CENTER_CH2_LB, min, 0);
  } else if(pulse > RC_CENTER_CH2_UB && pulse < RC_MAX_CH2){
    pulse = map(pulse, RC_CENTER_CH2_LB, max, 0, max);
  } else {
    pulse = 0;
  }*/

  
  if ( pulse > min && pulse < max) {
    //pulse = map(pulse, RC_MIN_CH2, RC_MAX_CH2, -500, 500);
    pulse = map(pulse, min, max, (int)MOTOR_MIN_SPEED*3, (int)MOTOR_MAX_SPEED*3); // Increse 20% of pulse range to be able to reach max speed
    //pulse = constrain(pulse, MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);
  } else {
    pulse = 0;
 }

  // Anything in deadzone should stop the motor
  if ( abs(pulse) <= deadzone ) {
    pulse = 0;
  }
  pulse = constrain(pulse, MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);
  return pulse;
}

/* 
*  Read Channel 1 & 2 pulses from R/C receiver
*  and convert to motor speed range values
*/
void getSpeed(int *x, int *y){
  const int est_deadzone = 2;
  int pulse_x, pulse_y, est_pulse;
    
  *y = pulseIn(CH_2_PIN, HIGH, TIMEOUT);
  *x = pulseIn(CH_1_PIN, HIGH, TIMEOUT);
  
  /*
    Serial.print("CH_1: "); 
    Serial.println(*x);
    Serial.print("CH_2: "); 
    Serial.println(*y);
 */ 
  /*display_clear();
  display_show(0,0,"LEFT :");
  display_show_int(6, 0, *x);
  display_show(0,2,"RIGHT:");
  display_show_int(6, 2, *y);
  */
  //delay(100);
  
  // Convert to PWM value (MOTOR_MIN_SPEED to MOTOR_MAX_SPEED)
  //Adjust the sign according to r/c controller
  pulse_y = FORWARD_SIGN * pulseToPWM(*y, RC_MIN_CH2, RC_MAX_CH2);
  pulse_x = RIGHT_SIGN * pulseToPWM(*x, RC_MIN_CH1, RC_MAX_CH1);

  // Kalman Filter Estimation
  est_pulse = getEstimatedIntValue(&kf_ch2, pulse_y);
  *y = est_pulse;


  est_pulse = getEstimatedIntValue(&kf_ch1, pulse_x);
  *x = est_pulse;

  // Anything in deadzone should stop the motor
  *x = (abs(*x) <= est_deadzone) ? 0 : *x;
  *y = (abs(*y) <= est_deadzone) ? 0 : *y;

}
