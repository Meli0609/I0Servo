#include <I0Servo.h>
#include <math.h>

#define SERVOS_COUNT 3
#ifndef SERVO_SPEED
#define SERVO_SPEED 150
#endif

#ifndef I0ServoArray_h
#define I0ServoArray_h
struct ServoSync {
  int diffuSeconds;
  double stepuSeconds;
  double uSeconds;
};

class I0ServoArray {
public:
  I0Servo servos[SERVOS_COUNT];
  void moveServo3Series(double servo1Angle, double servo2Angle, double servo3Angle);
  void moveServo3Sync(double servo1Angle, double servo2Angle, double servo3Angle);
  int getMaxuSecondsDiff(int diffuSeconds1, int diffuSeconds2, int diffuSeconds3);
private:
};
#endif