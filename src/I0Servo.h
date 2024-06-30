#include <ESP32ServoX.h>

#define SERVO_MIN_USECONDS 500
#define SERVO_MAX_USECONDS 2500
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 270

#ifndef I0Servo_h
#define I0Servo_h
class I0Servo : public ServoX {
public:
  using ServoX::ServoX;
  int uSeconds;
  int diffuSeconds;
  int diffuSecondsIndex;
  static long angle2uSeconds(float x, float in_min, float in_max, float out_min, float out_max);
  String getStatus();
  void setServoUSeconds(int targetUseconds);
  void moveServoSyncX(float angle, uint16_t servoSpeed);
private:
};
#endif