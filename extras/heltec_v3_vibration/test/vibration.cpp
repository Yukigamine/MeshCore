#define assert(x) do { if (!(x)) __builtin_trap(); } while (0)
#define DM_VIBRATION_PIN 47
#include "DirectMessageVibration.h"
uint32_t fake_now = 0;
int pin_value = -1;
int main() {
  DirectMessageVibration motor;
  motor.begin(); assert(pin_value == LOW);
  motor.trigger(); assert(pin_value == HIGH); // Works even at millis()==0.
  fake_now=199; motor.loop(); assert(pin_value == HIGH);
  fake_now=200; motor.loop(); assert(pin_value == LOW);
  fake_now=300; motor.trigger(); assert(pin_value == LOW); // Coalesce bursts.
  fake_now=400; motor.loop(); assert(pin_value == HIGH);
  fake_now=600; motor.loop(); assert(pin_value == LOW);
  fake_now=1000; motor.trigger(1); assert(pin_value == HIGH);
  fake_now=1199; motor.loop(); assert(pin_value == HIGH);
  fake_now=1200; motor.loop(); assert(pin_value == LOW);
  fake_now=2000; motor.trigger(1);
  fake_now=2100; motor.trigger(2); // DM upgrades active channel pattern.
  fake_now=2200; motor.loop(); assert(pin_value == LOW);
  fake_now=2400; motor.loop(); assert(pin_value == HIGH);
  motor.trigger(1); // Channel does not shorten active DM.
  fake_now=2599; motor.loop(); assert(pin_value == HIGH);
  fake_now=2600; motor.loop(); assert(pin_value == LOW);
  fake_now=UINT32_MAX-100; motor.trigger(); assert(pin_value == HIGH);
  fake_now=99; motor.loop(); assert(pin_value == LOW); // Unsigned rollover.
  fake_now=299; motor.loop(); assert(pin_value == HIGH);
  fake_now=499; motor.loop(); assert(pin_value == LOW);
  motor.trigger(); motor.stop(); assert(pin_value == LOW);
  fake_now+=400; motor.loop(); assert(pin_value == LOW);
}
