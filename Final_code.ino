/*
  "1D Pong"

  Its a 1D Pong Game on an 1m, 60 Neopixel LED Strip

  FlyingAngel - 18.4.2020
*/

#include <Adafruit_NeoPixel.h>
#define PIN 3
#define NUMPIXELS 20
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// *********************************
// Definition personal Variables
// *********************************

// Neopixel
int playerPos;

int maxBright       =     155; // max brightness

byte playerBtnPin[] =   {4, 2}; // Pins for buttons
const int ledPin = 3;

int gameSpeedMin    =       50; // min game-speed
int gameSpeedMax    =        5; // max game-speed
int gameSpeedStep   =        1; // fasten up when change direction
int ballSpeedMax    =        1; // max ball-speed
int ballBoost0      =       25; // superboost on last position
int ballBoost1      =       15; // boost on forelast position
byte playerColor[]  =  {0, 255}; // red & green
int winRounds       =       10; // x winning rounds for winning game
int endZoneSize     =        6; // size endzone
int endZoneColor    =      160; // color endzone
int delay_time = 20;

// *********************************
// Definition System-Variablen
// *********************************

boolean activeGame = false;                 // true when game is active
unsigned long previousMoveMillis;           // time of last ball move
unsigned long previousButtonMillis;         // time of last button-press

int playerButtonPressed[2];                 // ball-position where button was pressed; „-1“ button not pressed

int previousButtonPos = -1;                 // position of last button-press
byte previousButtonColor;                   // color of field for last Button-press
int playerScore[2];                         // actual Score
byte playerStart;                           // who starts game
int gameSpeed;                              // actual game-speed
int ballDir = 1;                            // direction, ball is moving (+/- 1)
int ballPos;                                // actual ball-position
int ballSpeed;                              // actual ball-speed (higher = slower)

byte leds[NUMPIXELS];                        // Define the array of LEDs
byte previousButtonBright = maxBright / 2;  // bright of marked last position when button pressen
byte scoreDimBright       = maxBright / 4;  // bright of dimmed score


// *********************************
// Setup
// *********************************
void setup()
{

  //  Serial.begin(9600);

  randomSeed(analogRead(0));          // better Random

  

  // PIN defination
  pinMode(playerBtnPin[0], INPUT_PULLUP);   // PINs for buttons
  pinMode(playerBtnPin[1], INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);
 

  playerStart = random(2);  // random starting player
}



// *********************************
// Loop
// *********************************
void loop()
{
  menu();
}

// Menu (set brightness and start game)
void menu()
{
  playerPos = (0, NUMPIXELS);
  //digitalWrite(ledPin, HIGH);
  //digitalWrite(playerBtnPin[1], HIGH);

  GeneratePlayField(maxBright);         // show play-field
  pixels.show();

  // set brightness (button player 0)
  if (player1buttonfunc() == true)
  {
    maxBright += 50;
    if (maxBright > 255)
    {
      maxBright = 55;
    }
    GeneratePlayField(maxBright);
    pixels.show();

    while (player1buttonfunc() == true) {}   // wait for button release
  }

  // start game (button player 1)
  if (player2buttonfunc() == true)
  {
    digitalWrite(playerBtnPin[0], LOW);     // deactivate both LEDs
    digitalWrite(playerBtnPin[1], LOW);
    activeGame = true;

    while (player2buttonfunc() == true) {}   // wait for button release

    game();
  }
}

bool player1buttonfunc(){
  int frstswitchState = digitalRead(playerBtnPin[0]);
  int secswitchState = digitalRead(playerBtnPin[1]);
  int player1pos = 0;
  int player2pos = NUMPIXELS;
  while (frstswitchState == HIGH && player1pos<NUMPIXELS && secswitchState != HIGH){
    player1pos = 0;
    pixels.setPixelColor(player1pos, 255, 0, 0);
    pixels.show();
    delay(delay_time);
    pixels.setPixelColor(player1pos, 0, 0, 0);
    pixels.show();
    secswitchState = digitalRead(playerBtnPin[1]);
  }
  return (frstswitchState == HIGH);
}

bool player2buttonfunc(){
  int frstswitchState = digitalRead(playerBtnPin[0]);
  int secswitchState = digitalRead(playerBtnPin[1]);
  int player1pos = 0;
  int player2pos = NUMPIXELS;
  while (secswitchState == HIGH && player2pos>0 && frstswitchState != HIGH){
    player2pos = 19;
    pixels.setPixelColor(player2pos, 255, 0, 0);
    pixels.show();
    delay(delay_time);
    pixels.setPixelColor(player2pos, 0, 0, 0);
    pixels.show();
    frstswitchState = digitalRead(playerBtnPin[0]);
  }
  return (secswitchState == HIGH);
}

/* function to debounce button (true == pressed, false == not pressed)
boolean buttonBounce(byte button, byte bounceTime) // bounce the button
{
  boolean result = false;

  if (digitalRead(playerBtnPin[button]) == LOW)
  {
    delay (bounceTime);
    if (digitalRead(playerBtnPin[button]) == LOW)
    {
      result = true;
    }
  }
  return result;
}
*/

void game()
{
  while (activeGame)
  {

    gameSpeed = gameSpeedMin;           // set starting game speed
    ballSpeed = gameSpeed;              // set starting ball speed
    memset(playerButtonPressed, -1, sizeof(playerButtonPressed)); // clear keypress

    GeneratePlayField(scoreDimBright);  // show gamefield with dimmed score
    pixels.show();

    InitializePlayers();  // set the player-settings -> wait for keypress to start game

    GameLoop();           // main loop: move ball -> ball left gamefield? -> check keypress -> check if keypress in endzone -> change direction
    
    CheckScore();         // check who made score and show it

    CheckWinner();        // check if we have a winner
  }
}


void InitializePlayers()
{

  if (playerStart == 0)   // initialize for player 0
  {
    ballDir = 1;          // set ball direction
    ballPos = 0;          // set startposition of ball

    digitalWrite(playerBtnPin[0], HIGH);              // activate LED
    while (digitalRead(playerBtnPin[0]) == HIGH) {} // wait for button
    digitalWrite(playerBtnPin[0], LOW);               // deactivate LED
  }
  else                        // initialize for player 1
  {
    ballDir = -1;             // set ball direction
    ballPos = NUMPIXELS - 1;   // set startposition of ball

    digitalWrite(playerBtnPin[1], HIGH);              // activate LED
    while (digitalRead(playerBtnPin[1]) == HIGH) {} // wait for button
    digitalWrite(playerBtnPin[1], LOW);               // deactivate LED
  }
}


void GameLoop()
{
  while (true)    // loop, exit with break when one player made a score
  {

    if (millis() - previousMoveMillis > ballSpeed)  // time to move ball
    {
      previousMoveMillis = millis();

      GeneratePlayField(scoreDimBright);

      ballPos += ballDir;
      if (ballPos < 0 || ballPos >= NUMPIXELS) // ball left endzone?
      {
        break;                // leave loop -> one player made a score
      }
      pixels.setPixelColor(ballPos, 0, 0, 0);    // generate ball (white)
      pixels.show();
    }

    CheckButtons();     // check keypress

    // fix positions of keypress for testing
    // if (ballPos == 3) playerButtonPressed[0] = 3;
    // if (ballPos == 59) playerButtonPressed[1] = 59;

    CheckButtonPressedPosition();

  } // end of while-loop
}


// *** check if buttons pressed
void CheckButtons()
{
  for (int i = 0; i < 2; i++)
  {
    // player pressed button?
    if (playerButtonPressed[i] == -1 && digitalRead(playerBtnPin[i]) == LOW && (ballDir + 1) / 2 == i)
      // (ballDir + 1) / 2 == i  -->  TRUE, when:
      // ballDir == -1  AND  i = 0  -->  player 0 is active player
      // ballDir == +1  AND  i = 1  -->  player 1 is active player
      // only the button-press of the active player is stored
    {
      playerButtonPressed[i] = ballPos;   //store position of pressed button
      previousButtonPos = ballPos;
      previousButtonColor = playerColor[i];
      previousButtonMillis = millis(); // store time when button was pressed
    }
  }
}


// *** check, if button was pressed when ball was in endzone and if so, change direction of ball
void CheckButtonPressedPosition()
{
  if (ballDir == -1 && playerButtonPressed[0] <= endZoneSize - 1 && playerButtonPressed[0] != -1)
  {
    ChangeDirection();
  }

  if (ballDir == +1 && playerButtonPressed[1] >= NUMPIXELS - endZoneSize)
  {
    ChangeDirection();
  }
}


void ChangeDirection()
{
  ballDir *= -1;
  gameSpeed -= gameSpeedStep;
  ballSpeed = gameSpeed;
  if (ballPos == 0 || ballPos == NUMPIXELS - 1)  // triggered on first or last segment
  {
    ballSpeed -= ballBoost0;      // Super-Boost
  }

  if (ballPos == 1 || ballPos == NUMPIXELS - 2)  // triggered on second or forelast segment
  {
    ballSpeed -= ballBoost1;      // Boost
  }

  ballSpeed = max(ballSpeed, ballSpeedMax);                     // limit the maximum ballspeed
  memset(playerButtonPressed, -1, sizeof(playerButtonPressed)); // clear keypress
}


void CheckScore()
{
  previousButtonPos = -1;     // clear last ball-position at button-press

  // check who made score
  if (ballPos < 0)            // player1 made the score
  {
    playerScore[1] += 1;      // new score for player1

    GeneratePlayField(maxBright);   // show new score full bright
    BlinkNewScore(NUMPIXELS / 2 - 1 + playerScore[1], playerColor[1]); // blink last score

    playerStart = 1;          // define next player to start (player, who made the point)
  }
  else                        // player0 made the score
  {
    playerScore[0] += 1;      // new score for player0

    GeneratePlayField(maxBright);   // show new score full bright
    BlinkNewScore(NUMPIXELS / 2 - playerScore[0], playerColor[0]); // blink last score

    playerStart = 0;          // define next player to start (player, who made the point)
  }

  GeneratePlayField(maxBright);     // show new score full bright
  pixels.show();

  delay(10);
}


void CheckWinner()
{
  // check if we have a winner
  if (playerScore[0] >= winRounds || playerScore[1] >= winRounds)
  { // we have a winner!

    activeGame = false;

    pixels.clear();
    Rainbow(playerScore[0] > playerScore[1]); // TRUE if player 0 won; FALSE when player 1 won

    memset(playerScore, 0, sizeof(playerScore));  // reset all scores

    playerStart = abs(playerStart - 1);   // next game starts looser

  }
}


void GeneratePlayField(byte bright)
{
  pixels.clear();        // clear all
  GenerateEndZone();      // generate endzone
  GenerateScore(bright);  // generate actual score
  GenerateLastHit();      // generate mark of position of last button-press
}


void GenerateEndZone()
{
  for (int i = 0; i < endZoneSize; i++)
  {
    pixels.setPixelColor(i, 255, 0, 0);
    pixels.setPixelColor((NUMPIXELS - 1 - i), 255, 0, 0);
  }
}


void GenerateScore(int bright)
{
  int i;

  // Player 0
  for (i = 0; i < playerScore[0]; i++)
  {
    pixels.setPixelColor((NUMPIXELS / 2 - 1 - i), 0, 255, 0);
  }

  // Player 1
  for (i = 0; i < playerScore[1]; i++)
  {
    pixels.setPixelColor((NUMPIXELS / 2 + i), 50, 255, 50);
  }
}


void GenerateLastHit()
{
  if (previousButtonPos != -1 && previousButtonMillis + 500 > millis())
  {
    pixels.setPixelColor((previousButtonPos), previousButtonColor, 255, previousButtonBright);
  }
}


void BlinkNewScore(int pos, byte color)
{
  for (int i = 1; i <= 4; i++)
  {
    pixels.setPixelColor(pos, color, 255, (i * 2) * maxBright);
    pixels.show();
    delay(300);
  }
}


void Rainbow(boolean won)
{
  for (int k = 0; k < 3; k++)   // 3 rounds rainbow
  {
    for (int j = 0; j <= 255; j++)
    {
      for (int i = 0; i < NUMPIXELS / 2; i++)
      {
        if (won == true)    // player 0 won --> Rainbow left
        {
          pixels.setPixelColor(i, ((i*256 / NUMPIXELS) + j) % 256, 255, maxBright);
        }
        else                // player 1 won --> Rainbow right
        {
          pixels.setPixelColor((NUMPIXELS - i - 1), ((i * 256 / NUMPIXELS) + j) % 256, 255, maxBright);
        }
      }
      pixels.show();
      delay(7);
    }
  }
}

