//ArduDino side scrollingreaction time game inspired by chrome dino.
//version 1.2. added more gems on higher difficulties and bosses. Made game infinite ant not crash at max dif of 151
//LCD setup

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F, 16, 2);

//defining pins

#define button 2
#define buzzer 11

//global variables
int hiScore = 0;
int8_t zamieniacz, przyciagacz, przyspieszacz;
int8_t gameBoard[16][2]; // array holds every obstacle on the map
int bossTimer; //sets delay between boss spawn and it's attack
  int8_t charX = 0; //x value of character position
//custom characters

//protagonist

byte protag1[8] = {
  0b00100,
  0b00111,
  0b00111,
  0b00100,
  0b11100,
  0b11100,
  0b10100,
  0b00100
};

byte protag2[8] = {
  0b00100,
  0b00111,
  0b00111,
  0b00100,
  0b11100,
  0b11100,
  0b10100,
  0b10000
};

//spikes

byte spike[8] = {
  0b00000,
  0b11100,
  0b00100,
  0b11100,
  0b00100,
  0b11100,
  0b00100,
  0b11111
};

//gem
byte gem[8] = {
  0b00100,
  0b01010,
  0b10101,
  0b10101,
  0b10101,
  0b10101,
  0b01010,
  0b00100
};

//functions

int game(int startinglvl);
int8_t character();
void spawn(int8_t opponent);
 void moveLeft () {

   

   

    for (int8_t j = 0; j < 2; j++) {
      for (int8_t i = 0; i < 14; i += 1) {

        gameBoard[i][j] = gameBoard[i + 1][j];
      }
    }};
 


 void renderCharacter( int sprite ,  bool negation, int level, int bossModifier)  {
  lcd.setCursor(charX, abs(character() - negation));
    lcd.write(sprite);
    if (level < 100)
      delay((300 - level * 2) / bossModifier); //wait between frames, gets faster with the game

 };





void setup() {



  //setting up pins

  pinMode(button, INPUT);
  pinMode(buzzer, OUTPUT);

  //initiating   LCD

  lcd.init(); // initiating LCD screen
  lcd.backlight(); // enabling LCD light
  lcd.createChar(0, protag1);
  lcd.createChar(1, protag2);
  lcd.createChar(2, spike);
  lcd.createChar(3, gem);

  //intro sequence (game name moving  left to right)
  tone(11, 300, 1000);
  lcd.setCursor(0, 0);
  lcd.print("ArduDino");
  for (int8_t i = 0; i < 17; i++) {
    lcd.scrollDisplayRight();
    delay(100);
  }
  lcd.clear();
  tone(11, 400, 1000);
  lcd.setCursor(0, 1);
  lcd.print("ArduDino");
  delay(200);
  for (int8_t i = 0; i < 17; i++) {
    lcd.scrollDisplayRight();

    delay(100);
  }
  tone(11, 500, 50);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ArduDino");
  //music
  tone(11, 700, 150);
  delay(200);
  tone(11, 300, 400);
  delay(500);
  tone(11, 1500, 75);
}

void loop() {

  int score;

  //starting screen


if(hiScore!=2137){


  lcd.setCursor(9, 0);
  lcd.print("Hi:");
  lcd.print(hiScore);
  lcd.setCursor(14, 3);
  lcd.print("  ");
}
  //calling for  game after press of button

  if (digitalRead(button) == HIGH) {
    tone(11, 700, 150);
    delay(200);
    tone(11, 300, 400);
    score = game(1); //initiating game

    //showing current score

    lcd.clear();
if(score==2137){

lcd.setCursor(4,0);
lcd.print("You Won :)");

  
}
else{
    lcd.setCursor(0, 1);
    lcd.print("SCORE:");
    lcd.print(score);
    lcd.print("    ");
    lcd.setCursor(0, 0);
    lcd.print("TryAgain");

    //check for high score
    if (score > hiScore) hiScore = score;
}
    //delay so new game doesnt start

    delay(1000);
  }
  delay(150);
}

int game(int startinglvl) {
  int whatBoss = 0;
  int bossModifier = 1;
  bool negation = 0;
  int8_t previous; //what was previously added obstacle

  int row; //how many spikes there were in a row
  int level = startinglvl; //difficulty. 4 is lowest possible

  //setting default values to gameBoard

  for (int8_t j = 0; j < 2; j++) {
    for (int8_t i = 0; i < 16; i++) {
      gameBoard[i][j] = 32;
    }
  }
  //assigning number alue to enemies for random order of their first appearance
  int questionNumberArray[] = {
    0,
    1,
    2
  };

  const size_t n = sizeof(questionNumberArray) / sizeof(questionNumberArray[0]);

  for (size_t i = 0; i < n - 1; i++) {
    size_t j = random(0, n - i);

    int t = questionNumberArray[i];
    questionNumberArray[i] = questionNumberArray[j];
    questionNumberArray[j] = t;
  }

  przyciagacz = questionNumberArray[0];
  zamieniacz = questionNumberArray[1];
  przyspieszacz = questionNumberArray[2];

  //the game loop itself

  while (true) {

    bossTimer--;
    int puste = 10, moneta = 15, kolec = level * level - 4, hardMoneta = 0, przeciwnik = 0, coinSpike = 1 + level / 10;

    if (level > 30) przeciwnik = 5+level/2;
    if (level > 20) {
      moneta = 5;
      hardMoneta = 15+level*2;
    }
    if (level < 14) coinSpike = 0;

    int nowe = random(puste + moneta + kolec + hardMoneta + przeciwnik + 1);
 

    if (nowe > coinSpike + puste + moneta + kolec + hardMoneta)

    {

      spawn(random(whatBoss));
      goto koniecPrzypisania;
    }
    if (nowe > coinSpike + puste + moneta + kolec)

    {
      if (previous != 2) { //can't be two in a row

        gameBoard[13][random(2)] = 3; //create coin in either upper or lower lane at random
        previous = 2; //set gem as last one
 
      } else previous = 0;
      row = 0; //reset spikes in the row
      goto koniecPrzypisania;
    }

    if (nowe > coinSpike + puste + moneta) {

      if (previous == 1) {
        row++;
      } //increase, how much spikes there were in a row
      if (row < level * 2) { //sets max amount of spikes in a row

        gameBoard[14][random(2)] = 2; //creates spike in either upper or lower lane

     
        previous = 1; //sets spike as last one
      } else {
        previous = 0;
        row = 0;

      } //resets row

      goto koniecPrzypisania;
    }
    if (nowe > coinSpike + puste)

    {
      if (previous != 2) { //can't be two in a row

        gameBoard[14][random(2)] = 3; //create coin in either upper or lower lane at random
        previous = 2; //set gem as last one

      } else {
        previous = 0;
   
      }
      row = 0; //reset spikes in the row
      goto koniecPrzypisania;
    }

    if (nowe > puste) {

      gameBoard[14][0] = 3;
      gameBoard[14][1] = 3;
      gameBoard[14][random(2)] = 2;

    }

    /*spawn puste */
 

    koniecPrzypisania:

      if (level == 10) {
        whatBoss++;
        level++;

        spawn(0);

      }

    if (level == 30) {
      whatBoss++;
      level++;

      spawn(1);

    }

    if (level == 45) {
      whatBoss++;
      level++;

      spawn(2);

    }

    if (gameBoard[15][0] == 35 && bossTimer == 0) {
      gameBoard[15][0] = 32;
      negation = 0;
      gameBoard[charX][0] = 32;
      gameBoard[charX][1] = 32;
    }
    if (gameBoard[15][0] == 35 && bossTimer == 10) {
      negation = 1;
      gameBoard[charX][0] = 32;
      gameBoard[charX][1] = 32;
      tone(buzzer, 100, 50);
    }
    if (gameBoard[15][0] == 36 && bossTimer == 0) {
      charX++;
      gameBoard[15][0] = 32;
      tone(buzzer, 100, 50);
    }

    if (gameBoard[15][0] == 38 && bossTimer == 20) {

      bossModifier = 2;
    }

    if (gameBoard[15][0] == 38 && bossTimer == 2) {
      gameBoard[15][0] = 32;
      bossModifier = 1;
    }

    if (charX > 8) { //mocve character back to left
      charX = 0;
      gameBoard[0][0] = 32;
      gameBoard[0][1] = 32;
      gameBoard[1][1] = 32;
      gameBoard[1][0] = 32;
      gameBoard[2][1] = 32;
      gameBoard[2][0] = 32;
    }

    //rendering current boardstate

    for (int8_t j = 0; j < 2; j++) {
      for (int8_t i = 0; i < 16; i++) {
        lcd.setCursor(i, j);
        lcd.write(gameBoard[i][j]);
      }
    }

    tone(buzzer, 40, 50); //small tone each frame to add mood

    //detecting colision

    switch (gameBoard[charX][abs(character() - negation)]) { //whatever is in place, character is at
    case 2: //spike
      tone(buzzer, 1000, 400); //crete game lost sound
      delay(400);
      tone(buzzer, 950, 100);
      return level - startinglvl; //give lever as a score
      break;
    case 3: //gem
      tone(buzzer, 10000, 250); //playfull sound
      level++; //add to score

      break;
    }

    //rendering character
renderCharacter(  0 , negation, level, bossModifier);


moveLeft();
    //clear rightmost row

    gameBoard[14][1] = 32;
    gameBoard[14][0] = 32;

    //rendering current boardstate

    for (int8_t j = 0; j < 2; j++) {
      for (int8_t i = 0; i < 16; i++) {
        lcd.setCursor(i, j);
        lcd.write(gameBoard[i][j]);
      }
    }
    tone(buzzer, 40, 50);

    //detecting colision

    switch (gameBoard[charX][abs(character() - negation)]) { //whatever is in place, character is at
    case 2: //spike
      tone(buzzer, 1000, 400); //crete game lost sound
      delay(400);
      tone(buzzer, 950, 100);
      return level - startinglvl; //give lever as a score
      break;
    case 3: //gem
      tone(buzzer, 10000, 250); //playfull sound
      level++; //add to score

      break;
    }
    //rendering character
renderCharacter(  1 , negation, level, bossModifier);

moveLeft();
    //clear rightmost row

    gameBoard[14][1] = 32;
    gameBoard[14][0] = 32;
  }

if(level==100){
 tone(11, 700, 150);
    delay(200);
    tone(11, 300, 400);
delay(400);
tone(buzzer, 440, 250);
delay(250);
tone(buzzer, 733, 800);
delay(800);
return(2137);



}
  
}

int8_t character() { //checking whenever character is on top or on bottom
  return !digitalRead(button);
}

void spawn(int8_t opponent) {
  if (gameBoard[15][0] == 32) {

    if (opponent == przyciagacz) { //przeciwnik przyciągacz inicjacja
      gameBoard[15][0] = 36;
      bossTimer = 5;
      tone(buzzer, 15000, 250);

    }
    if (opponent == zamieniacz) {
      gameBoard[15][0] = 35;
      bossTimer = 15;
      tone(buzzer, 15500, 250);
    }
    if (opponent == przyspieszacz) {
      gameBoard[15][0] = 38;
      bossTimer = 30;
      tone(buzzer, 15600, 210);
    }

  }
}




  

