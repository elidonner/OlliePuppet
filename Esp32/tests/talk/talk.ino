/*
 * RC Car with ps3 controller
 * https://github.com/jvpernis/esp32-ps3/blob/master/examples/Ps3Demo/Ps3Demo.ino
 *
 * @author Ethan Donlon
 */

#define FROM_ESP 15
#define LED 10

void setup()
{
  pinMode(FROM_ESP, INPUT);
  pinMode(LED, OUTPUT);
}
void loop()
{
  if(digitalRead(FROM_ESP) == HIGH)
  {
    digitalWrite(LED, HIGH);
  }
  else
  {
    digitalWrite(LED, LOW);
  }
}