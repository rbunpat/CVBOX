#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "pitches.h"
#include "MLX90614.h"

#define SOUND         7
#define ENA           8
#define IntIR1Pin     4
int tempo = 80;
const int melody[] PROGMEM = {
  NOTE_E5, 16, NOTE_DS5, 16,
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, -8, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16,
  NOTE_C5, 8,  REST, 16, NOTE_E4, 16, NOTE_E5, 16,  NOTE_DS5, 16,
  
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16, 
  NOTE_B4, -8, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16, 
  NOTE_A4 , 4, REST, 8,
};
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;
unsigned long currentTime = millis();
unsigned long oldTime = currentTime;

esl::MLX90614 mlx90614;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(IntIR1Pin,INPUT_PULLUP);
  pinMode(ENA, OUTPUT);
  digitalWrite(ENA, LOW);
  Serial.begin(115200);
  while (!Serial) ;
  Wire.begin();
  mlx90614.init();
  Serial.print( "Address = 0x" );
  Serial.println( mlx90614.readSlaveAddress(), HEX );
  lcd.begin(16, 2);
  lcd.noBacklight();
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = pgm_read_word_near(melody+thisNote + 1);
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }
    tone(SOUND, pgm_read_word_near(melody+thisNote), noteDuration * 0.9);
    delay(noteDuration);
    noTone(SOUND);
  }
  lcd.setCursor(0, 0);
  lcd.print("Scan Temperature");
  lcd.backlight();
}

void loop() {  
  currentTime = millis();
  if((currentTime - oldTime) > 100){
    //Serial.println(digitalRead(IntIR1Pin));
    if (digitalRead(IntIR1Pin) == 0){
      digitalWrite(ENA, HIGH);
      delay(100);
      digitalWrite(ENA, LOW);
      checkTouchSensor();
    }
    oldTime = currentTime;
  }
}

void checkTouchSensor(){
  getSensorDisplay();
  playSoundTouch();
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan Temperature");
}

void getSensorDisplay(){
  float tempA, tempO;
  Serial.print( "Ambient temp. (Ta): " );
  tempA = mlx90614.readAmbientTemperature(); // ambient temperature
  if ( !mlx90614.is_crc_error() ) {
    Serial.println(tempA);
  } 
  Serial.print( "Object1 temp. (To): " );
  tempO = mlx90614.readObjectTemperature(); // object1 temperature
  if ( !mlx90614.is_crc_error() ) {
    Serial.println(tempO);
  } 
  lcd.setCursor(5, 1);
  lcd.print(tempO);
  if(tempO > 37.5){
    alertSound();
  }
}

void playSoundTouch(){
  tone(SOUND, 4300, 150);
}

void alertSound(){
  for (int i=0; i<10; i++){
    tone(SOUND, 4300, 150);
    delay(500);
  }
}
