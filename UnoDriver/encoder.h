/**
 * Adventures in Science: Fred Bot
 * SparkFun Electronics
 * Author: M. Hord (October 8, 2013)
 * Modified by: B. Huang (October 31, 2014)
 * Modified by: Shawn Hymel (July 21, 2017)
 *
 * Use two magnetic encoders on Fred's motor shafts (48:1
 * gearbox, 60mm wheels) to make him move in a straight line for
 * 1m.
 *
 * License: Beerware (https://en.wikipedia.org/wiki/Beerware)
 * 
 *  Hardware Connection:
 *   2 ------------ ENC_L
 *   3 ------------ ENC_R
 * 
 * Use two magnetic encoders to make a robot drive in a straight line for 1 meter
 * https://gist.github.com/ShawnHymel/1de08ffaca990b65fade81cb8d01a44a
 * 
 * Sample-Code-For-Using-Encoders
 * https://github.com/WillDonaldson/Sample-Code-For-Using-Encoders/blob/master/Hall_Effect_Quadrature_Encoder.ino
 * 
 * 313 RPM HD Premium Planetary Gear Motor w/Encoder
 * https://www.servocity.com/content/downloads/313_rpm_hd_precision_planetary_gearmotor_-_specifications_2.pdf
 * https://www.servocity.com/content/downloads/ne12_-_use_parameter.pdf
 * 
 * https://tutorial.cytron.io/2016/04/04/arduino-2a-motor-shield-encoder-motor/
 */
 
/*
 * https://www.pjrc.com/teensy/td_libs_Encoder.html
 * https://github.com/PaulStoffregen/Encoder/blob/master/Encoder.h
 *                           _______         _______       
 *              Pin1 ______|       |_______|       |______ Pin1
 *negative <---         _______         _______         __      --> positive
 *              Pin2 __|       |_______|       |_______|   Pin2

 *                    new     new     old     old
 *                    pin2    pin1    pin2    pin1    Result
 *                    ----    ----    ----    ----    ------
 *                      0       0       0       0       no movement
 *                      0       0       0       1       +1
 *                      0       0       1       0       -1
 *                      0       0       1       1       +2  (assume pin1 edges only)
 *                      0       1       0       0       -1
 *                      0       1       0       1       no movement
 *                      0       1       1       0       -2  (assume pin1 edges only)
 *                      0       1       1       1       +1
 *                      1       0       0       0       +1
 *                      1       0       0       1       -2  (assume pin1 edges only)
 *                      1       0       1       0       no movement
 *                      1       0       1       1       -1
 *                      1       1       0       0       +2  (assume pin1 edges only)
 *                      1       1       0       1       -1
 *                      1       1       1       0       +1
 *                      1       1       1       1       no movement
*/
// uncomment #define ENABLE_ENCODER to compile this for using Encoders
//#define ENABLE_ENCODER

#ifdef ENABLE_ENCODER
  // uncomment #define PROWLER if it's Prowler Rover robot
  #define DEBUG               
  #define PROWLER
  
  #define LEFT_FRONT_ENC      1       // encoder ID for left front encoder
  #define LEFT_BACK_ENC       2       // encoder ID for left back encoder
  #define RIGHT_FRONT_ENC     3       // encoder ID for right front encoder
  #define RIGHT_BACK_ENC      4       // encoder ID for right front encoder
  
  // Pins
  //const int enc_l_pin = 2;          // Motor A
  //const int enc_r_pin = 3;          // Motor B
  
  // Globals
  //volatile unsigned long enc_l = 0;
  //volatile unsigned long enc_r = 0;
  
  
  int rf_pulsesChanged = 0;           // store right front pules changed flag
  volatile long lb_pulses;            // store left back pulse count
  int lb_pulsesChanged = 0;           // store left back pules changed flag
  volatile long rb_pulses;            // store right back pulse count
  int rb_pulsesChanged = 0;           // store right back pules changed flag
  
  
  
  #ifdef PROWLER
    const int lb_enc_a_pin = 2;                         // pin number for left front A channel encoder
    const int lb_enc_b_pin = 3;                         // pin number for left front B channel encoder
    const int lb_enc_reversed = 0;                      // no swap A & B channel
    
    
    const int rb_enc_a_pin = 19;                         // pin number for right front A channel encoder
    const int rb_enc_b_pin = 18;                         // pin number for right front B channel encoder
    const int rb_enc_reversed = 0;                      // swap A & B channel
    
    const int wheel_d = 137.16;                           // 5.4" Wheel diameter (mm)
    const int counts_per_rev = (int)1288.848;           // Countable Events Per Revolution (Output Shaft)
  #else
    const int rf_enc_reversed = 1;                      // swap A & B channel
    const int rf_enc_a_pin = 3;                         // pin number for right front A channel encoder
    const int rf_enc_b_pin = 2;                         // pin number for right front B channel encoder
    const int wheel_d = 130;                            // 13cm Wheel diameter (mm)
    const int counts_per_rev = (int)1232*2;               // ((11 pairs N-S) * (56:1 gearbox) * (2 falling/rising edges) = 1232 (manual count 2464)
  #endif
  
  // Parameters
  const int drive_distance = 100;                       // cm
  const float wheel_c = PI * wheel_d;                   // Wheel circumference (mm)
  
  
  const float dist_per_count = wheel_c/counts_per_rev;  // Distance per count (mm)
  
  
  // Calculate accumulated distance(in cm.) from the begining
  float get_distance(int encoder_id){
      switch (encoder_id)
      {
        case LEFT_BACK_ENC:
        { 
          return dist_per_count*lb_pulses/100;
        }
        
        case RIGHT_BACK_ENC:
        { 
          return dist_per_count*rb_pulses/100;
        }
        
    }
  }
  int get_encoder_pulse(int encoder_id){
    switch (encoder_id)    
    {
        case LEFT_BACK_ENC:
        { 
          return rb_pulses;
        }
        case RIGHT_BACK_ENC:
        { 
          return rb_pulses;
        }
        
    }
  }
  
  // To visualize on serial plotter
  void plot_encoders(int encoder_id){
    int enc_a, enc_b;
    String str_a = "CH_A:";
    String str_b = "CH_B:";
  
    switch (encoder_id)    
    {
        case LEFT_BACK_ENC:
        { 
          enc_a = digitalRead(lb_enc_a_pin);
          enc_b = digitalRead(lb_enc_b_pin);
          if(lb_enc_reversed){
            str_a = String("CH_B:");
            str_b = String("CH_A:");
          }
          break;
        }
        case RIGHT_BACK_ENC:
        { 
          enc_a = digitalRead(rb_enc_a_pin);
          enc_b = digitalRead(rb_enc_b_pin);
          if(rb_enc_reversed){
            str_a = String("CH_B:");
            str_b = String("CH_A:");
          }
          break;
        }
    }   
    Serial.print(str_a);
    Serial.print(enc_a);      
    Serial.print('\t');
    Serial.print(str_b);
    Serial.print(enc_b+2);    // +2 shifts output B up so both A and B are visible 
    Serial.print('\t');
    Serial.println(); 
    
  }
  
  void print_changed_encoders(int encoder_id){
    long pulses=0, pulsesChanged=0;
    float dist=0.0;
    String str_encoder = "N/A : ";
    
    switch (encoder_id)    
    {
  
        case LEFT_BACK_ENC:
        { 
          pulses = lb_pulses;
          pulsesChanged = lb_pulsesChanged;
          dist= dist_per_count*lb_pulses/100;
          str_encoder = String("LB: ");
          lb_pulsesChanged = 0;
          if (pulsesChanged != 0) {
            #ifdef ENABLE_LCD
              lcd.setCursor (0, 0); // move to the begining of the second line
              lcd.print(str_encoder);
              lcd.print(dist);
              lcd.print(" cm."); 
            #endif
            break;
          }
        }
        
        case RIGHT_BACK_ENC:
        { 
          pulses = rb_pulses;
          pulsesChanged = rb_pulsesChanged;
          dist= dist_per_count*rb_pulses/100;
          str_encoder = String("RB: ");
          rb_pulsesChanged = 0;
          if (pulsesChanged != 0) {        
            #ifdef ENABLE_LCD
              lcd.setCursor (0, 1); // move to the begining of the second line
              lcd.print(str_encoder);
              lcd.print(dist);
              lcd.print(" cm."); 
            #endif        
            break;
          }
        }
        
    }   
    if (pulsesChanged != 0) {
      #ifdef DEBUG
        Serial.print(str_encoder);
        Serial.print(pulses);
        Serial.print("\t");
        Serial.print(dist);
        Serial.print(" cm.");
        Serial.println(); 
      #endif
      /*
      #ifdef ENABLE_LCD
        lcd.setCursor (0, 0); // move to the begining of the second line
        lcd.print(str_encoder);
        lcd.print(pulses);
        lcd.print(" tic.   ");
        lcd.setCursor (0, 1); // move to the begining of the second line
        lcd.print(dist);
        lcd.print(" cm."); 
      #endif
  */
      pulsesChanged = 0;    
    }
  }
  
  void LB_A_CHANGE(){
    if( digitalRead(lb_enc_b_pin) == 0 ) {
      if ( digitalRead(lb_enc_a_pin) == 0 ) {
        // A fell, B is low
        lb_pulses--; // moving reverse
      } else {
        // A rose, B is low
        lb_pulses++; // moving forward
      }
    }else {
      if ( digitalRead(lb_enc_a_pin) == 0 ) {
        // B fell, A is high
        lb_pulses++; // moving reverse
      } else {
        // B rose, A is high
        lb_pulses--; // moving forward
      }
    }
    lb_pulsesChanged = 1;
  }
  
  void LB_B_CHANGE(){
    if ( digitalRead(lb_enc_a_pin) == 0 ) {
      if ( digitalRead(lb_enc_b_pin) == 0 ) {
        // B fell, A is low
        lb_pulses++; // moving forward
      } else {
        // B rose, A is low
        lb_pulses--; // moving reverse
      }
   } else {
      if ( digitalRead(lb_enc_b_pin) == 0 ) {
        // B fell, A is high
        lb_pulses--; // moving reverse
      } else {
        // B rose, A is high
        lb_pulses++; // moving forward
      }
    }
    lb_pulsesChanged = 1;
  }
  
  void RB_A_CHANGE(){
    if( digitalRead(rb_enc_b_pin) == 0 ) {
      if ( digitalRead(rb_enc_a_pin) == 0 ) {
        // A fell, B is low
        rb_pulses--; // moving reverse
      } else {
        // A rose, B is low
        rb_pulses++; // moving forward
      }
    }else {
      if ( digitalRead(rb_enc_a_pin) == 0 ) {
        // B fell, A is high
        rb_pulses++; // moving reverse
      } else {
        // B rose, A is high
        rb_pulses--; // moving forward
      }
    }
    rb_pulsesChanged = 1;
  }
  
  void RB_B_CHANGE(){
    if ( digitalRead(rb_enc_a_pin) == 0 ) {
      if ( digitalRead(rb_enc_b_pin) == 0 ) {
        // B fell, A is low
        rb_pulses++; // moving forward
      } else {
        // B rose, A is low
        rb_pulses--; // moving reverse
      }
   } else {
      if ( digitalRead(rb_enc_b_pin) == 0 ) {
        // B fell, A is high
        rb_pulses--; // moving reverse
      } else {
        // B rose, A is high
        rb_pulses++; // moving forward
      }
    }
    rb_pulsesChanged = 1;
  }
  
  /*
  void RF_A_CHANGE(){
    if( digitalRead(rf_enc_b_pin) == 0 ) {
      if ( digitalRead(rf_enc_a_pin) == 0 ) {
        // A fell, B is low
        rf_pulses--; // moving reverse
      } else {
        // A rose, B is low
        rf_pulses++; // moving forward
      }
    }else {
      if ( digitalRead(rf_enc_a_pin) == 0 ) {
        // B fell, A is high
        rf_pulses++; // moving reverse
      } else {
        // B rose, A is high
        rf_pulses--; // moving forward
      }
    }
    rf_pulsesChanged = 1;
  }
  
  void RF_B_CHANGE(){
    if ( digitalRead(rf_enc_a_pin) == 0 ) {
      if ( digitalRead(rf_enc_b_pin) == 0 ) {
        // B fell, A is low
        rf_pulses++; // moving forward
      } else {
        // B rose, A is low
        rf_pulses--; // moving reverse
      }
   } else {
      if ( digitalRead(rf_enc_b_pin) == 0 ) {
        // B fell, A is high
        rf_pulses--; // moving reverse
      } else {
        // B rose, A is high
        rf_pulses++; // moving forward
      }
    }
    rf_pulsesChanged = 1;
  }
  */
  
  
  void encoder_setup(){
    
    // Setup interrupt pin for motor encoders
    pinMode(lb_enc_a_pin, INPUT_PULLUP);
    pinMode(lb_enc_b_pin, INPUT_PULLUP);
    
    pinMode(rb_enc_a_pin, INPUT_PULLUP);
    pinMode(rb_enc_b_pin, INPUT_PULLUP);
  
    
    // Set up interrupts
    attachInterrupt(digitalPinToInterrupt(lb_enc_a_pin), LB_A_CHANGE, CHANGE);
    attachInterrupt(digitalPinToInterrupt(lb_enc_b_pin), LB_B_CHANGE, CHANGE);
    
    attachInterrupt(digitalPinToInterrupt(rb_enc_a_pin), RB_A_CHANGE, CHANGE);
    attachInterrupt(digitalPinToInterrupt(rb_enc_b_pin), RB_B_CHANGE, CHANGE);
  
  }
#endif
/*
 * need to call less interrupt by using RISING
 * 
 */
