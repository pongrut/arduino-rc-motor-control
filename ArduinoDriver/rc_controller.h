#include "kalman_filter.h"
#define Q_VAR_1 0.5
#define Q_VAR_2 0.05

SimpleKalmanFilter kf_ch1 = SimpleKalmanFilter(E_MEA, E_EST, Q_VAR_1);
SimpleKalmanFilter kf_ch2 = SimpleKalmanFilter(E_MEA, E_EST, Q_VAR_2);


#define TIMEOUT 25000             //the number of microseconds to wait for the pulse to start

#define RC_MIN_CH1          930   //FS-GT3C full forward value is around 1001
#define RC_MAX_CH1          2000  //FS-GT3C full backward value is around 1998
#define RC_MIN_CH2          930   //FS-GT3C full forward value is around 990
#define RC_MAX_CH2          2000  //FS-GT3C full backward value is around 2000
#define RC_MIN_CH3          1000  //FS-GT3C 
#define RC_MAX_CH3          1010  //FS-GT3C


//#define MDDS30 to compile this for using MDDS30 driver in motor.h
// R/C Controller pins
#ifdef MDDS30 
 #define CH_1_PIN             4                   //define PIN 4 for channel 1
 #define CH_2_PIN             5                   //define PIN 5 for channel 2
 #define CH_3_PIN             2                   //define PIN 2 for channel 3
 #define FORWARD_SIGN        -1                   //FS-GT3C forward value will be negative sign
 #define RIGHT_SIGN           1
#endif



// Define halt range value 
const int deadzone = (int)MOTOR_MAX_SPEED*0.35;   // For MDDS30 anything between -40 and 40 is stop

// Convert RC pulse value to motor PWM value
int pulseToPWM(int pulse ,int min, int max) {
  
  // Convert numbers in range to motor speed
  if ( pulse > min && pulse < max) {
    
    // 3X increse lower & upper boundary of pulse range to be able to reach max boundary then
    // map pulse input range to min & max range
    pulse = map(pulse, min, max, (int)MOTOR_MIN_SPEED*3, (int)MOTOR_MAX_SPEED*3); 
  }else{
    pulse = 0;
 }

  // Anything in deadzone the motor must stop
  if ( abs(pulse) <= deadzone ) {
    pulse = 0;
  }
  // Ensure motor speed is in valid range
  pulse = constrain(pulse, MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);
  return pulse;
}

/* 
 *  Read Channel 3 pulses from R/C receiver
 *  and convert to ON/OFF status
 */
int getCH3Status(){
  // Read channel 3 value from rc receiver
  int pulse = pulseIn(CH_3_PIN, HIGH, TIMEOUT);
  
  // if pulse in the ON range then switch on LED and return status value
  if ( pulse > RC_MIN_CH3 && pulse < RC_MAX_CH3) {
    digitalWrite(LED_PIN, HIGH);
    return 1;
  }else{
    digitalWrite(LED_PIN, LOW);
    return 0;
  }
}

/* 
 *  Read Channel 1 & 2 pulses from R/C receiver
 *  and convert to motor speed range values
 */
void getSpeed(int *x, int *y){
  // define kalman filter deadzone
  const int est_deadzone = 2;
  // variables for storing receiver values
  int pulse_x, pulse_y;
  // variable for storing estimated values
  int est_pulse;
    
  *y = pulseIn(CH_2_PIN, HIGH, TIMEOUT);
  *x = pulseIn(CH_1_PIN, HIGH, TIMEOUT);
   
  
  // Convert pulsein value to motor speed amd
  // adjust the sign direction according to r/c controller
  pulse_y = FORWARD_SIGN * pulseToPWM(*y, RC_MIN_CH2, RC_MAX_CH2);
  pulse_x = RIGHT_SIGN * pulseToPWM(*x, RC_MIN_CH1, RC_MAX_CH1);

  // Check Channel 3 Status
  // If on then proceed Kalman Filter Estimation
  if(getCH3Status()){
    // Kalman Filter Estimation
    est_pulse = getEstimatedIntValue(&kf_ch2, pulse_y);
    *y = est_pulse; 
    est_pulse = getEstimatedIntValue(&kf_ch1, pulse_x);
    *x = est_pulse;
  } else {
    *y = pulse_y;
    *x = pulse_x;
  }

  // Anything in deadzone must stop the motor
  *x = (abs(*x) <= est_deadzone) ? 0 : *x;
  *y = (abs(*y) <= est_deadzone) ? 0 : *y;

}
