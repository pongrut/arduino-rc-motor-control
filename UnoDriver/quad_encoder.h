/* Encoder Library - TwoKnobs Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */
#define ENCODER_USE_INTERRUPTS
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>




#ifdef MDDS30 
  #define REVERSE_LEFT  true 
  #define REVERSE_RIGHT  false 
  
  const int lb_enc_a_pin = 2;                      // pin number for left front A channel encoder
  const int lb_enc_b_pin = 3;                      // pin number for left front B channel encoder
  const int rb_enc_a_pin = 19;                     // pin number for left front A channel encoder
  const int rb_enc_b_pin = 18;                     // pin number for left front B channel encoder
  const float wheel_d_m = 137.16;                  // 5.4" Wheel diameter (mm)to meter
  const float counts_per_rev = 1288.848;           // Countable Events Per Revolution (Output Shaft) 

#else
  // MDD10 or MDD3A Driver
  #define REVERSE_LEFT  true 
  #define REVERSE_RIGHT  true 
    
  const int lb_enc_a_pin = 4;                      // pin number for left front A channel encoder
  const int lb_enc_b_pin = 5;                      // pin number for left front B channel encoder
  const int rb_enc_a_pin = 6;                     // pin number for left front A channel encoder
  const int rb_enc_b_pin = 7;                     // pin number for left front B channel encoder
  const float wheel_d_m = 130;                     // 130 Wheel diameter (mm)to meter
  const float counts_per_rev = 1232*2;             // ((11 pairs N-S) * (56:1 gearbox) * (2 falling/rising edges) = 1232 (manual count 2464)
#endif
// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability




class QuadEncoder{
    public:
    Encoder *enc; 
    float wheelDia;                             // Wheel diameter 
    float tickPerRev;                           // Countable Events Per Revolution (Output Shaft)
    float distPerTick; 
    float wheelCir;                             // Wheel circumference 
    long lastEnc;
    long currentEnc;
    bool reverse;
    unsigned long lastTime;
    unsigned long currentTime;    
    long deltaEnc;
    float deltaT;
    int pinA, pinB;
    
    
      QuadEncoder(int pinA, int pinB, float wheelDia, float tickPerRev, bool reverse){
      this->wheelDia = wheelDia;      
      this->tickPerRev=tickPerRev;
      this->reverse=reverse;
      this->pinA = pinA;
      this->pinB = pinB;
      enc = new Encoder(pinA, pinB);

      currentEnc=0;
      deltaEnc=0;
      deltaT=0.1;//TODO

    }

    // Initializing instance
    void init(){
      lastTime=0;
      lastEnc=0;
      wheelCir=wheelDia*PI;                   // Wheel circumference
      distPerTick = wheelCir/tickPerRev;      // Distance per tick count
      Serial.print("wheelCir: ");
      Serial.print(wheelCir, 4);
      Serial.print("\tdistPerTick: ");
      Serial.println(distPerTick, 4);
 
      lcd.setCursor (0, 0); // move to the begining of the second line
      lcd.print("Dist/Tick: ");
      lcd.print(distPerTick);
      lcd.setCursor (0, 1); // move to the begining of the second line
      lcd.print("A:");
      lcd.print(pinA);
       lcd.print(", B: ");
      lcd.print(pinB);
      delay(1000);
      lcd.clear();
    }

    void CountTick(){
      currentTime=millis();
      currentEnc=enc->read();
      currentEnc=reverse?-currentEnc:currentEnc;
    }
    
    void saveTick(){
      lastTime=currentTime;
      lastEnc=currentEnc;
    }    

    
    float getSpeed(){
      deltaEnc=currentEnc-lastEnc;
      deltaT=(currentTime-lastTime)/1000.0;
      float wheelSpeed  = (deltaT != 0) ? deltaEnc*distPerTick/deltaT : 0;
      return wheelSpeed;
    }

    float getDistance(){
      return currentEnc*distPerTick;
    }
    
    
    void write(int data){
      enc->write(data);
    }
    
    float getDeltaT(){
      return deltaT;
    }

    long getCurrentEnc(){
      return currentEnc;
    }

    long getLastEnc(){
      return lastEnc;
    }

  };


QuadEncoder encLeft(lb_enc_a_pin, lb_enc_b_pin, wheel_d_m, counts_per_rev, REVERSE_LEFT);
QuadEncoder encRight(rb_enc_a_pin, rb_enc_b_pin, wheel_d_m, counts_per_rev, REVERSE_RIGHT);

void q_encoder_setup() {
  //Serial.begin(9600);
  //Serial.println("Encoder Test:");
  encLeft.init();
  encRight.init();
}

/*
void loop() {
  //long newLeft, newRight;
  //newLeft = encLeft.read();
  //newRight = encRight.read();
  //tic();
  encLeft.CountTick();
  long lastEnc= encLeft.getLastEnc();
  long currentEnc = encLeft.getCurrentEnc();
  float distance = encLeft.getDistance();

  if (currentEnc != lastEnc) {
    Serial.print("Left = ");
    Serial.print(currentEnc);
    Serial.print("\tDistance = ");
    Serial.print(distance);
    Serial.print("\tSpeed = ");
    Serial.print(encLeft.getSpeed());
    Serial.print("\t");
    Serial.println();
    
  }
  encLeft.saveTick();
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    encLeft.write(0);
    //knobRight.write(0);
  }
}*/
