#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MLX90614.h>
#include <SPI.h>

int trigpin = 13;
int echopin = 12;

int motorIN1 = 2;
int motorIN2 = 3;
int motorPWM = 6;

unsigned long currentTime = millis();
unsigned long oldTime = currentTime;

esl::MLX90614 mlx90614;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
    Serial.begin(115200);
    while (!Serial);
    Wire.begin();
    mlx90614.init();
    Serial.print("Address = 0x");
    Serial.println(mlx90614.readSlaveAddress(), HEX);
    pinMode(motorIN1, OUTPUT);
    pinMode(motorIN2, OUTPUT);
    pinMode(motorPWM, OUTPUT);
    lcd.begin(16, 2);
    lcd.clear();
    lcd.noBacklight();
    lcd.setCursor(0, 0);
    lcd.print("Scan Temperature");
    lcd.backlight();
}

void loop()
{
    currentTime = millis();
    if ((currentTime - oldTime) > 100)
    {
        long duration, objDist;
        pinMode(trigpin, OUTPUT);
        digitalWrite(trigpin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigpin, HIGH);
        delayMicroseconds(5);
        digitalWrite(trigpin, LOW);
        pinMode(echopin, INPUT);
        duration = pulseIn(echopin, HIGH);
        objDist = microsecondsToCentimeters(duration);
        if (objDist > 10 and objDist < 15) //set detection distance here (cm)
        {
            idkwhattocallthisfunction();
        }
        oldTime = currentTime;
    }
}

void idkwhattocallthisfunction()
{
    float tempAMB, tempOBJ;
    tempAMB = mlx90614.readAmbientTemperature();
    tempOBJ = mlx90614.readObjectTemperature();
    if (tempOBJ > 29 && tempOBJ < 37.5) //set normal temp range here (c)
    {
        lcd.setCursor(5, 1);
        lcd.print(tempOBJ);
        dispenseALC(200, 500); //(motor speed value = 0-255, dispense time value = > 0 in ms)
        delay(500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Scan Temperature");
    }
    else if (tempOBJ > 37.5) //set first scan high temp threshold here (c)
    {
        float scndscan
        scndscan = mlx90614.readObjectTemperature();
        if (scndscan > 29 && scndscan < 37.5) //set second scan temp range here (c)
        {
            lcd.setCursor(5, 1);
            lcd.print(scndscan);
            dispenseALC(200, 500); //(motor speed value = 0-255, dispense time value = > 0 in ms)
            delay(500);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Scan Temperature");
        }
        else
        {
            lcd.setCursor(0, 1);
            lcd.print("High Temperature");
            dispenseALC(200, 500); //(motor speed value = 0-255, dispense time value = > 0 in ms)
            delay(500);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Scan Temperature");
        }
    }
}

void dispenseALC(int mtrspeed, int dsplen)
{
    digitalWrite(motorIN1, HIGH);
    digitalWrite(motorIN2, LOW); //turn the motor on and set direction to forward
    analogWrite(motorPWM, mtrspeed);
    delay(dsplen);
    digitalWrite(motorIN1, LOW);
    digitalWrite(motorIN2, LOW); //turn dat motor off
}

long microsecondsToCentimeters(long microseconds)
{
    return microseconds / 29 / 2;
}
