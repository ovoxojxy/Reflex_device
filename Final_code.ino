#include <Adafruit_NeoPixel.h>
#define PIN 3
#define NUMPIXELS 20
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delay_time = 20; // 200 msec = 1/5th of a second
const int ledPin = 3;
const int frstbuttonPin = 4;
const int secbuttonPin = 2;
int frstswitchState = 0;
int secswitchState = 0;
//int player1pos = 0;
//int player2pos = NUMPIXELS;

void player1buttonfunc(){
  frstswitchState = digitalRead(frstbuttonPin);
  secswitchState = digitalRead(secbuttonPin);
  int player1pos = 0;
  int player2pos = NUMPIXELS;
  while (frstswitchState == HIGH && player1pos<NUMPIXELS && secswitchState != HIGH){
    player1pos++;
    pixels.setPixelColor(player1pos, 255, 0, 0);
    pixels.show();
    delay(delay_time);
    pixels.setPixelColor(player1pos, 0, 0, 0);
    pixels.show();
    secswitchState = digitalRead(secbuttonPin);
  }
}

void player2buttonfunc(){
  frstswitchState = digitalRead(frstbuttonPin);
  secswitchState = digitalRead(secbuttonPin);
  int player1pos = 0;
  int player2pos = NUMPIXELS;
  while (secswitchState == HIGH && player2pos>0 && frstswitchState != HIGH){
    player2pos--;
    pixels.setPixelColor(player2pos, 255, 0, 0);
    pixels.show();
    delay(delay_time);
    pixels.setPixelColor(player2pos, 0, 0, 0);
    pixels.show();
    frstswitchState = digitalRead(frstbuttonPin);
  }
}

void setup() {
  pixels.begin();
  pinMode(frstbuttonPin, INPUT);
  pinMode(secbuttonPin,INPUT);
  pinMode(ledPin, OUTPUT);
  
}

void loop() {
	player1buttonfunc();
  	player2buttonfunc();
}
