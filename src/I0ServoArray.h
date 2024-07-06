#include "I0Servo.h"
#include <math.h>

#define SERVO_SPEED 150
#define SERVOS_COUNT 3

#ifndef I0ServoArray_h
#define I0ServoArray_h

struct ServoSync {
  int diffMicroSeconds;
  float stepMicroSeconds;
  float microSeconds;
};

class I0ServoArray {
public:
  I0Servo servos[SERVOS_COUNT];
  void moveServo3Series(float servo1Angle, float servo2Angle, float servo3Angle);
  void moveServo3Sync(float servo1Angle, float servo2Angle, float servo3Angle);
private:
  int getMaxMicroSecondsDiff(int diff1MicroSeconds, int diff2MicroSeconds, int diff3MicroSeconds);
};
#endif