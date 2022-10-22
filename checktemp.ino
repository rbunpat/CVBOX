#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "MLX90614.h"
#include "SPI.h"

int trigpin = 6;
int echopin = 5;

int relaypin = 4;

int scanstatus = 0;

unsigned long currentTime = millis();
unsigned long oldTime = currentTime;

esl::MLX90614 mlx90614;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
    Wire.begin();
    mlx90614.init();
    pinMode(relaypin, OUTPUT);
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
        if (objDist > 0 and objDist < 10) //set detection distance here (cm)
        {
            idkwhattocallthisfunction();
        }
        oldTime = currentTime;
    }
}

void idkwhattocallthisfunction()
{
    float tempOBJ;
    tempOBJ = mlx90614.readObjectTemperature();
    if (tempOBJ > 29 && tempOBJ < 37.5) //set normal temp range here (c)
    {
        lcd.setCursor(5, 1);
        lcd.print(tempOBJ);
        dispenseALC(250); //(dispense time value = > 0 in ms)
        delay(500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Scan Temperature");
    }
    else if (tempOBJ > 37.5) //set first scan high temp threshold here (c)
    {
        float scndscan;
        scndscan = mlx90614.readObjectTemperature();
        if (scndscan > 29 && scndscan < 37.5) //set second scan temp range here (c)
        {
            lcd.setCursor(5, 1);
            lcd.print(scndscan);
            dispenseALC(250); //(dispense time value = > 0 in ms)
            delay(500);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Scan Temperature");
        }
        else
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("High Temperature");
            lcd.setCursor(5, 1);
            lcd.print(scndscan);
            dispenseALC(250); //(dispense time value = > 0 in ms)
            delay(500);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Scan Temperature");
        }
    }
}

void dispenseALC(int dsplen)
{
  digitalWrite(relaypin, LOW);
  delay(dsplen);
  digitalWrite(relaypin, HIGH);  
}

long microsecondsToCentimeters(long microseconds)
{
    return microseconds / 29 / 2;
}
