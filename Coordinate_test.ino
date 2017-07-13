// AHR AIR HOCKEY ROBOT PROJECT

// Coordinate TEST!
// If you change some parameter on Configuration.h 
// remember to copy it to the final project AHRobot.

// Author: Jose Julio (@jjdrones)
// Hardware: Arduino MEGA + Ramps 1.4
// Date: 21/01/2014
// Updated: 
// Version: 1.01
// Project page : Spanish: English:
// GIT repository: 
// License: Open Software GPL License

// ROBOT and USER configuration parameters
#include "Configuration.h"
#include "Definitions.h"   // Variable definitions


void print_values()
{
  //Serial.print(com_pos_x);
  //Serial.print(",");
  //Serial.print(com_pos_y);
  //Serial.print(",");
  //Serial.print(position_x);
  //Serial.print(",");
  //Serial.println(position_y);
  
}

void setup() 
{ 
  // STEPPER MOTOR PINS (SAME AS RAMPS 1.4)
  // X MOTOR
  //     X-STEP: A0    (PF0)
  //     X-DIR:  A1    (PF1)
  //     X-ENABLE: D38 (PD7)
  // Y MOTOR (Y-LEFT)
  //     Y-STEP: A6    (PF6)
  //     Y-DIR:  A7    (PF7)
  //     Y-ENABLE: A2  (PF2)
  // Z MOTOR (Y-RIGHT)
  //     Z-STEP: D46   (PL3)
  //     Z-DIR:  D48   (PL1)
  //     Z-ENABLE: A8  (PK0)

  // STEPPER PINS 
  // X_AXIS
  pinMode(38,OUTPUT);  // ENABLE MOTOR
  pinMode(A0,OUTPUT);  // STEP MOTOR
  pinMode(A1,OUTPUT);  // DIR MOTOR
  // Y_AXIS (Y-LEFT)
  pinMode(A2,OUTPUT);  // ENABLE MOTOR
  pinMode(A6,OUTPUT);  // STEP MOTOR
  pinMode(A7,OUTPUT);  // DIR MOTOR
  // Z_AXIS (Y-RIGHT)
  pinMode(A8,OUTPUT);  // ENABLE MOTOR
  pinMode(46,OUTPUT);  // STEP MOTOR
  pinMode(48,OUTPUT);  // DIR MOTOR 

  pinMode(A3,OUTPUT);  // DEBUG PIN FOR OSCILLOSCOPE TIME MEASURES

  pinMode(19,INPUT);  // RX1 Serial Port 1
  pinMode(18,OUTPUT); // TX1

  //FANS and LEDS
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(13,OUTPUT);

  // Disable Motors
  digitalWrite(38,HIGH);
  digitalWrite(A2,HIGH);
  digitalWrite(A8,HIGH);

  Serial.begin(115200);
//  Serial.println("AHR Robot Motor test v1.01");
//  Serial.println("Initializing robot...");
//  Serial.print("Free Memory: ");
//  Serial.print(freeRam());
//  Serial.println();
  delay(2500);

  //LED blink
  for (uint8_t k=0;k<4;k++)
  {
    digitalWrite(13,HIGH);
    delay(300);
    digitalWrite(13,LOW);
    delay(300);
  } 

  // We use TIMER 1 for stepper motor X AXIS and Timer 3 for Y AXIS
  // STEPPER MOTORS INITIALIZATION
  // TIMER1 CTC MODE
  TCCR1B &= ~(1<<WGM13);
  TCCR1B |=  (1<<WGM12);
  TCCR1A &= ~(1<<WGM11); 
  TCCR1A &= ~(1<<WGM10);

  // output mode = 00 (disconnected)
  TCCR1A &= ~(3<<COM1A0); 
  TCCR1A &= ~(3<<COM1B0); 

  // Set the timer pre-scaler
  // Generally we use a divider of 8, resulting in a 2MHz timer on 16MHz CPU
  TCCR1B = (TCCR1B & ~(0x07<<CS10)) | (2<<CS10);

  OCR1A = ZERO_SPEED;   // Motor stopped
  dir_x = 0;
  TCNT1 = 0;

  // We use TIMER 3 for stepper motor Y AXIS 
  // STEPPER MOTORS INITIALIZATION
  // TIMER3 CTC MODE
  TCCR3B &= ~(1<<WGM13);
  TCCR3B |=  (1<<WGM12);
  TCCR3A &= ~(1<<WGM11); 
  TCCR3A &= ~(1<<WGM10);

  // output mode = 00 (disconnected)
  TCCR3A &= ~(3<<COM1A0); 
  TCCR3A &= ~(3<<COM1B0); 

  // Set the timer pre-scaler
  // Generally we use a divider of 8, resulting in a 2MHz timer on 16MHz CPU
  TCCR3B = (TCCR3B & ~(0x07<<CS10)) | (2<<CS10);

  OCR3A = ZERO_SPEED;   // Motor stopped
  dir_y = 0;
  TCNT3 = 0;

  //Initializing init position
  position_x = ROBOT_INITIAL_POSITION_X*X_AXIS_STEPS_PER_UNIT;
  position_y = ROBOT_MIN_Y*Y_AXIS_STEPS_PER_UNIT;

  delay(1000);

//  Serial.println("Initializing Stepper motors...");
  delay(1000);
  TIMSK1 |= (1<<OCIE1A);  // Enable Timer1 interrupt
  TIMSK3 |= (1<<OCIE1A);  // Enable Timer1 interrupt

  // Enable steppers
  digitalWrite(38,LOW);   // X-axis
  digitalWrite(A2,LOW);   // Y-axis left
  digitalWrite(A8,LOW);   // Z-axis (Y-axis right)

//  Serial.print("Max_acceleration_x: ");
//  Serial.println(max_acceleration_x);
//  Serial.print("Max_acceleration_y: ");
//  Serial.println(max_acceleration_y);
//  Serial.println("Moving to initial position...");
//  Serial.println("Ready to start test!!");
  delay(1000);

  // Initializing Robot command variables
  com_pos_x = ROBOT_INITIAL_POSITION_X;
  com_pos_y = ROBOT_MIN_Y;
  com_speed_x = 1000;
  com_speed_y = 1000;

  setSpeedS(com_speed_x,com_speed_y);
  print_values();
  setPosition(com_pos_x,com_pos_y);

  timer_old = micros();
  timer_packet_old = timer_old;
  micros_old = timer_old;

  // Enable Air Hockey FANS
  //digitalWrite(9,HIGH);
  //digitalWrite(10,HIGH);

}

int stopped_counter=0;

// Main loop
void loop() 
{ 
  int dt;
  uint8_t logOutput=0;

  debug_counter++;
  timer_value = micros();
  if ((timer_value-timer_old)>=1000)  // 1Khz loop
  {
    char data[7];
    char x[4];
    char y[4];
    while (Serial.available()) {
      while (Serial.available() >= 7)
      {
        for(int i = 0; i < 7; i++)
          data[i] = Serial.read();
      }
      for(int i = 0; i < 3; i++)
        x[i] = data[i];
      x[3] = 0;
      for(int i = 4; i < 7; i++)
        y[i - 4] = data[i];
      y[3] = 0;
      com_pos_x = atoi(x);
      com_pos_y = atoi(y);
      print_values();
      setPosition(com_pos_x, com_pos_y);
      print_values();
    }
  positionControl();

  }
}





