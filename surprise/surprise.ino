#include<Servo.h>
#include <Chirp.h>

Servo topLeft;
Servo topRight;
Servo bot;

const int TOPLEFTPIN = 7;
const int TOPRIGHTPIN = 8;
const int BOTPIN = 9;
const int SOUNDSENSORPIN = A0;
const int SPEAKERPIN = 3;
const int CHIRPERTRIGGERPIN = A1;
const int CHIRPERECHOPIN = A2;
const int SWITCHERPIN = 2;
const int SOUNDTHRESHOLD = 40;
Chirp chirp;

void setup() {
  topLeft.attach(TOPLEFTPIN);
  topRight.attach(TOPRIGHTPIN);
  bot.attach(BOTPIN);
  pinMode(CHIRPERTRIGGERPIN, OUTPUT);
  pinMode(CHIRPERECHOPIN, INPUT);
  pinMode(SOUNDSENSORPIN, INPUT); //analog
  pinMode(SPEAKERPIN, OUTPUT); //digital
  pinMode(SWITCHERPIN,INPUT); //digital
  Serial.begin(9600);
  bot.write(170);
  topLeft.write(45);
  topRight.write(100);
}

bool within5m = false;
bool soundSensed = false;
bool within10cm = false;

int get_distance(){
  Serial.print("calling&returing: ");
  unsigned long duration;
  int distance;
  digitalWrite(CHIRPERTRIGGERPIN, HIGH);
  delay(1);
  digitalWrite(CHIRPERTRIGGERPIN, LOW);
  duration = pulseIn(CHIRPERECHOPIN, HIGH, 20000);
  distance = duration / 57;  // Divide by round-trip microseconds per cm to get cm
  return distance;
}


void loop() {
  if(digitalRead(SWITCHERPIN)==0){
    int distance = get_distance();
    int soundLevel = analogRead(SOUNDSENSORPIN);
    Serial.print(soundLevel>=SOUNDTHRESHOLD);
    Serial.print("loop: within5m:");
    Serial.print(within5m);
    Serial.print(" ,soundSensed:");
    Serial.print(soundSensed);
    Serial.print(" ,within10cm:");
    Serial.print(soundSensed);
    Serial.print(" distance:");
    Serial.print(distance);
    Serial.print(" SoundLevel:");
    Serial.println(soundLevel);
    //within5m: bird sings
    if(within5m && soundSensed && within10cm){
      /*two resetting states:
        1. when chirper no longer within 5m
        2. end of cycle(all triggers are triggered)*/
      Serial.print("Resetting due to: ");
      if(distance> 40){
        Serial.println("distance greater than 5m; ");
      }
      else{
        Serial.println("end of cycle; ");
      }
      within5m = false;
      soundSensed = false;
      within10cm = false;
      bot.write(180);
      topLeft.write(45);
      topRight.write(100);
    }
    else if(distance<=40 && distance>15 && !within5m && !within10cm && !soundSensed){
      /*Bird starts singing when chirper detecte within 5m*/
      /*NOTE: WE REQUIRE THE DISTANCE TO BE BETWEEN 10CM AND 5M*/
      //TODO: singing
      Serial.println("Within 5m");
      chirp.chirp("parrotbilllllahcm4");
      delay(500);  
      within5m = true;
    }
    else if(within5m && !soundSensed && soundLevel>=SOUNDTHRESHOLD && !within10cm){
      /*Afte bird starts singing, when sound is sensed, lower doors open*/
      Serial.println("Sound triggered");
      bot.write(100);
      soundSensed = true;
    }
    //within10cm: top doors open and bird yells
    else if(within5m && soundSensed && distance<=15){
       /*after sound triggered, when chirper detects within10cm: top doors open and bird yells*/
      //TODO: yelling
      Serial.println("Within 10cm");
      topLeft.write(160);
      topRight.write(45);
      bot.write(180);
      //chirp.chirp("parrotbilllllahcm4");
      chirp.chirp("birdhouseplzworkty");
      within10cm = true;
      delay(3000);
    }
  }
}
