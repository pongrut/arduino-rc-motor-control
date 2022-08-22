#include <SimpleKalmanFilter.h>

/*
 This sample code demonstrates how to use the SimpleKalmanFilter object. 
 Use a potentiometer in Analog input A0 as a source for the reference real value.
 Some random noise will be generated over this value and used as a measured value.
 The estimated value obtained from SimpleKalmanFilter should match the real
 reference value.

 https://github.com/denyssene/SimpleKalmanFilter
 SimpleKalmanFilter(e_mea, e_est, q);
 e_mea: Measurement Uncertainty   - How much do we expect to our measurement vary
 e_est: Estimation Uncertainty  - Can be initilized with the same value as e_mea since the kalman filter will adjust its value.
 q: Process Noise - usually a small number between 0.001 and 1 - how fast your measurement moves. Recommended 0.01. Should be tunned to your needs.
 */
#define E_MEA 2
#define E_EST 2
#define Q_VAR 0.1

//SimpleKalmanFilter kf = SimpleKalmanFilter(E_MEA, E_EST, Q_VAR);


int getEstimatedIntValue(SimpleKalmanFilter *kf, float measured_value){
  float f_est_pulse = kf->updateEstimate(measured_value);
  if(f_est_pulse == 0){
    return 0;
  }
  int i_est_pulse = (f_est_pulse > 0) ? ceil(f_est_pulse) : floor(f_est_pulse); // Cast float to int by ceil or floor
  return i_est_pulse;
}
