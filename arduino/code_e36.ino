#include <DCF77.h>
#include <Time.h>
#include <Tone.h>

#define DCF_PIN 2
#define DCF_INTERRUPT 0

#define SPEED_PIN 4
#define FUEL_PIN  5
#define WATER_PIN 6
#define RPM_PIN   7

#define TIME_LED_OK 11
#define TIME_LED_ERR 12

#define DEBUG 1

DCF77 DCF = DCF77(DCF_PIN,DCF_INTERRUPT);

Tone rpm;
Tone kmh;

void setup() {
  DCF.Start();
  Serial.begin(9600);
  
  pinMode(TIME_LED_OK,OUTPUT);
  pinMode(TIME_LED_ERR,OUTPUT);
  pinMode(FUEL_PIN,OUTPUT);
  pinMode(WATER_PIN,OUTPUT);
  
  rpm.begin(RPM_PIN);
  kmh.begin(SPEED_PIN);
  
  if(DEBUG){
    Serial.println("Waiting for DCF77 time... ");
    Serial.println("This will take 2 minutes... ");
  }

  /*No DCF77 Update yet, turn on Error-LED*/
  digitalWrite(TIME_LED_ERR,HIGH);
}

void loop() {
    time_t DCFtime = DCF.getTime();

    if (DCFtime!=0)
    {
      Serial.println("Time is updated");
      setTime(DCFtime);
      digitalWrite(TIME_LED_ERR,LOW);
      digitalWrite(TIME_LED_OK,HIGH);
    }
  
  if(DEBUG)
    digitalClockDisplay();
  
  updateGauges();
  delay(1000);
}

void digitalClockDisplay(){
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());

  if(isPM() == true){
    Serial.print(" ");
    Serial.print("PM");
    Serial.println();
  }

  else {
    Serial.print(" ");
    Serial.print("AM");
    Serial.println();
  }
}

void printDigits(int digits){
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void updateGauges() {
  /*---WATER GAUGE---*/
  /*Water Temperature, Red = PM, Blue = AM*/
  if(isPM() == true){
    analogWrite(WATER_PIN,15);
  }
  else {
    analogWrite(WATER_PIN,95);
  }
  /*---END WATER GAUGE---*/

  /*---FUEL GAUGE---*/
  if(hour()<=6)
    analogWrite(FUEL_PIN,175);
  else if(hour()<=12)
    analogWrite(FUEL_PIN,115);
  else if(hour()<=18)
    analogWrite(FUEL_PIN,70);
  else if(hour()<=23)
    analogWrite(FUEL_PIN,15);
  /*---END FUEL GAUGE---*/
  
  /*---RPM GAUGE---*/
  if(minute() <= 8)
    rpm.play(16);
  else{
  long value1 = map(minute(),0,59,17,178);    
  
  /*Some adjustment*/
  if(minute() >= 49)
      value1 += 9;
  else if(minute() >= 56)
      value1 += 12;
  
  rpm.play(value1);
  }
  /*---END RPM GAUGE---*/

  /*---SPEED GAUGE---*/
  if(hourFormat12()==1)
    kmh.play(0);
  else{  
  long value2 = map(hourFormat12(),2,12,30,150);
  kmh.play(value2);
  }
  /*---END SPEED GAUGE---*/
  
}

