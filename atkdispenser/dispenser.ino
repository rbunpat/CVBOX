#include <Servo.h>
Servo atkServo;

int defaultPosition = 0;
const int allowPin = 5;

void setup()
{
    pinMode(allowPin, INPUT)
    atkServo.attach(9);
}

void loop()
{
    int allowStatus;
    allowStatus = digitalRead(allowPin);
    if (allowStatus == 1)
    {
        atkServo.write(90);
        delay(500);
        atkServo.write(0);
    }
}
