#include "I0ServoArray.h"

#define SERVO_1_PIN 14
#define SERVO_2_PIN 12
#define SERVO_3_PIN 13


#define DS3235_SERVO_MIN_MICRO_SECONDS 500
#define DS3235_SERVO_MAX_MICRO_SECONDS 2500
#define DS3235_SERVO_MIN_ANGLE 0
#define DS3235_SERVO_MAX_ANGLE 270

// serial
#define BAUDRATE 115200

I0ServoArray<3> i0ServoArray;

void serialSetup() {
  Serial.begin(BAUDRATE);
}

void setup() {
  // serial
  serialSetup();

  //servo
  ESP32PWMX::allocateTimer(0);
  ESP32PWMX::allocateTimer(1);
  ESP32PWMX::allocateTimer(2);
  ESP32PWMX::allocateTimer(3);

  i0ServoArray.servos[0].attach(SERVO_1_PIN, DS3235_SERVO_MIN_MICRO_SECONDS, DS3235_SERVO_MAX_MICRO_SECONDS, DS3235_SERVO_MIN_ANGLE, DS3235_SERVO_MAX_ANGLE);
  i0ServoArray.servos[1].attach(SERVO_2_PIN, DS3235_SERVO_MIN_MICRO_SECONDS, DS3235_SERVO_MAX_MICRO_SECONDS, DS3235_SERVO_MIN_ANGLE, DS3235_SERVO_MAX_ANGLE);
  i0ServoArray.servos[2].attach(SERVO_3_PIN, DS3235_SERVO_MIN_MICRO_SECONDS, DS3235_SERVO_MAX_MICRO_SECONDS, DS3235_SERVO_MIN_ANGLE, DS3235_SERVO_MAX_ANGLE);

  i0ServoArray.servos[0].write(90);
  i0ServoArray.servos[1].write(90);
  i0ServoArray.servos[2].write(90);
  delay(1000);

  Serial.printf("Servo angles: [%.2f, %.2f, %.2f]\r\n", i0ServoArray.servos[0].readAngle(), i0ServoArray.servos[1].readAngle(), i0ServoArray.servos[2].readAngle());
  Serial.printf("Servo pwm microseconds: [%d, %d, %d]\r\n", i0ServoArray.servos[0].readMicroseconds(), i0ServoArray.servos[1].readMicroseconds(), i0ServoArray.servos[2].readMicroseconds());

  while (!Serial)
    ;
}

void loop() {
   // put your main code here, to run repeatedly:
  delay(10);
}
