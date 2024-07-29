#include "ESP32PWMX.h"

#ifdef ARDUINO_ESP32C3_DEV
#define MINIMUM_TIMER_WIDTH 10
#define MAXIMUM_TIMER_WIDTH 14
#define DEFAULT_TIMER_WIDTH 10
#else
#define MINIMUM_TIMER_WIDTH 10
#define MAXIMUM_TIMER_WIDTH 20
#define DEFAULT_TIMER_WIDTH 16
#endif
#define DEFAULT_TIMER_WIDTH_TICKS 1024

#define DEFAULT_PULSE_WIDTH 1500  // default pulse width when servo is attached
#define DEFAULT_PULSE_WIDTH_TICKS 4825
//#define REFRESH_CPS            50
#define REFRESH_USEC 5000

#define MAX_SERVOS 16  // no. of PWM channels in ESP32

#define PWM_FREQUENCY 200

#define DEFAULT_SERVO_MIN_MICRO_SECONDS 500
#define DEFAULT_SERVO_MAX_MICRO_SECONDS 2500
#define DEFAULT_SERVO_MIN_ANGLE 0
#define DEFAULT_SERVO_MAX_ANGLE 270

enum Direction {
  CW,
  CCW,
};

/*
 * This group/channel/timmer mapping is for information only;
 * the details are handled by lower-level code
 *
 * LEDC Chan to Group/Channel/Timer Mapping
 ** ledc: 0  => Group: 0, Channel: 0, Timer: 0
 ** ledc: 1  => Group: 0, Channel: 1, Timer: 0
 ** ledc: 2  => Group: 0, Channel: 2, Timer: 1
 ** ledc: 3  => Group: 0, Channel: 3, Timer: 1
 ** ledc: 4  => Group: 0, Channel: 4, Timer: 2
 ** ledc: 5  => Group: 0, Channel: 5, Timer: 2
 ** ledc: 6  => Group: 0, Channel: 6, Timer: 3
 ** ledc: 7  => Group: 0, Channel: 7, Timer: 3
 ** ledc: 8  => Group: 1, Channel: 0, Timer: 0
 ** ledc: 9  => Group: 1, Channel: 1, Timer: 0
 ** ledc: 10 => Group: 1, Channel: 2, Timer: 1
 ** ledc: 11 => Group: 1, Channel: 3, Timer: 1
 ** ledc: 12 => Group: 1, Channel: 4, Timer: 2
 ** ledc: 13 => Group: 1, Channel: 5, Timer: 2
 ** ledc: 14 => Group: 1, Channel: 6, Timer: 3
 ** ledc: 15 => Group: 1, Channel: 7, Timer: 3
 */

#ifndef I0Servo_h
#define I0Servo_h
class I0Servo {
public:
  I0Servo();
  I0Servo(float minAngle, float maxAngle, float minMicroSeconds, float maxMicroSeconds, Direction direction);
  int speed = 1;
  long angle2MicroSeconds(float x);
  static long angle2MicroSeconds(float x, float inMin, float inMax, float outMin, float outMax, Direction direction);
  static float microSeconds2Angle(int x, float inMin, float inMax, float outMin, float outMax, Direction direction);
  String getStatus();
  void setToMicroSeconds(int targetMicroseconds);
  void setToAngle(float angle, uint16_t servoSpeed);
  int attach(int pin);                                                                                             // attach the given pin to the next free channel, returns channel number or 0 if failure
  int attach(int pin, int min, int max);                                                                           // as above but also sets min and max values for writes.
  int attach(int pin, int minMicroSeconds, int maxMicroSeconds, int minAngle, int maxAngle, Direction direction);  // as above but also sets min and max values for writes.
  void detach();
  void write(int value);              // if value is < MIN_PULSE_WIDTH its treated as an angle, otherwise as pulse width in microseconds
  void writeMicroseconds(int value);  // Write pulse width in microseconds
  void writeTicks(int value);         // Write ticks, the smallest increment the servo can handle
  void release();
  float readAngle();       // returns current pulse width as an angle between 0 and 180 degrees
  int readMicroseconds();  // returns current pulse width in microseconds for this servo
  int readTicks();         // returns current ticks, the smallest increment the servo can handle
  bool attached();         // return true if this servo is attached, otherwise false

  // ESP32 only functions
  void setTimerWidth(int value);  // set the PWM timer width (ESP32 ONLY)
  int readTimerWidth();           // get the PWM timer width (ESP32 ONLY)
  void setPeriodHertz(int hertz) {
    REFRESH_CPS = hertz;
    setTimerWidth(this->_timer_width);
  }
private:
  int microSecondsToTicks(int microSeconds);
  int ticksToMicroSeconds(int ticks);
  float _minAngle = (float)DEFAULT_SERVO_MIN_ANGLE;
  float _maxAngle = (float)DEFAULT_SERVO_MAX_ANGLE;
  float _minMicroSeconds = (float)DEFAULT_SERVO_MIN_MICRO_SECONDS;
  float _maxMicroSeconds = (float)DEFAULT_SERVO_MAX_MICRO_SECONDS;
  Direction _direction = CW;
  int _pinNumber = 0;                                  // GPIO pin assigned to this channel
  int _timer_width = DEFAULT_TIMER_WIDTH;              // ESP32 allows variable width PWM timers
  int _ticks = DEFAULT_PULSE_WIDTH_TICKS;              // current pulse width on this channel
  int _timer_width_ticks = DEFAULT_TIMER_WIDTH_TICKS;  // no. of ticks at rollover; varies with width
  ESP32PWMX* getPwm();                                 // get the PWM object
  ESP32PWMX pwm;
  int REFRESH_CPS = PWM_FREQUENCY;
};
#endif