#include <Servo.h>
Servo atkServo;

int defaultServoPos = 0;
int buttonState = 0;
int allowStatus = 0;

const int allowPin = 5;
const int buttonPin = 4;

void setup()
{
    pinMode(buttonPin, INPUT);
    pinMode(allowPin, INPUT);
    atkServo.attach(9);
}

void loop()
{
    allowStatus = digitalRead(allowPin);
    buttonState = digitalRead(buttonPin);
    while (allowStatus == HIGH)
    {
        if (buttonState == HIGH)
        {
            atkServo.write(90);
            delay(500);
            atkServo.write(0);
        }
    }
}
