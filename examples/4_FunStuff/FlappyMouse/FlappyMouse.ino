/*
  Flappy Mouse
  Modified to run on FED3 from:

  Created in 2016 by Maximilian Kern
  https://hackaday.io/project/12876-chronio
  Low power Arduino based (smart)watch code

  License: GNU GPLv3

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "FR1";                                  //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object
Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 144, 168);

//Initialize hiscore
byte hiscore;
bool scoreTrigger = true;

//Setup
void setup() {
  display.begin();
  display.setRotation(3); //Rotate 180°
  display.setTextSize(2);
  display.clearDisplay();
  pinMode(RIGHT_POKE, INPUT_PULLUP); //  RIGHT_POKE Pullup
}

//Loop
void loop() {
  Flappy();
}

void Flappy() {
  //  display.clearDisplay();
  static boolean firstRunGames = true;
  static float ySpeed = 0;
  static int py = 20;
  static boolean gameOver = false;
  static byte score = 0;
  static int wallPos[2] = {100, 186};
  static int wallGap[3] = {40, 60, 0};
  static unsigned long lastTime = millis();
  int wallsize = 40;

  if (firstRunGames) {
    ySpeed = score = 0;
    py = 20;
    wallPos[0] = 100;
    wallPos[1] = 186;
    lastTime = millis();
    gameOver = false;
  }

  float deltaTime = float(millis() - lastTime);

  //Mouse dropping speed
  ySpeed += deltaTime / 80;

  //Mouse Position
  py += ySpeed;
  py = constrain(py, 20, 130);

  //Clear mouse column on left side of screen
  display.fillRect(0, 0, 27, 168, WHITE);

  //Draw walls and landscape
  for (int i = 0; i < 2; i++) { // draw walls
    //clear landscape
    display.fillRect(wallPos[i] , wallGap[i] + wallsize , 85, 168, WHITE);

    //Clear walls
    display.fillRect(wallPos[i] , 18, 15, wallGap[i] - 18, WHITE);
    display.fillRect(wallPos[i] , wallGap[i] + wallsize , 15, 168, WHITE);

    //draw mountains
    display.fillTriangle(wallPos[i] - 10, 144, wallPos[i] + 10, wallGap[i] + wallsize + 30, wallPos[i] + 20, 144, BLACK);  //mountain 1
    display.fillTriangle(wallPos[i] + 5, 144, wallPos[i] + 25, wallGap[i] + wallsize + 15, wallPos[i] + 35, 144, BLACK);  //mountain 2
    display.fillTriangle(wallPos[i] + 20, 144, wallPos[i] + 40, wallGap[i] + wallsize + 25, wallPos[i] + 50, 144, BLACK);  //mountain 3
    display.fillTriangle(wallPos[i] + 40, 144, wallPos[i] + 60, wallGap[i] + wallsize + 40, wallPos[i] + 70, 144, BLACK);  //mountain 4

    //Draw walls
    display.fillRect(wallPos[i] - 10, 18, 10, wallGap[i] - 18, BLACK);
    display.fillRect(wallPos[i] - 10, wallGap[i] + wallsize , 10, 168, BLACK);

    //Check if mouse hit wall
    if (wallPos[i] > 5 && wallPos[i] < 20) { // detect wall
      if (wallGap[i] > py - 1 || wallGap[i] < py - wallsize + 7) gameOver = true; //detect gap
    }

    //When wall reaches left side reset it
    if (wallPos[i] <= -40) { // reset wall
      wallPos[i] += 202;
      wallGap[i] = random(30, 100);
      scoreTrigger = true;
    }

    //When wall reaches left side reset it
    if ((wallPos[i] <= 10) and (scoreTrigger == true)) {
      score++;
      tone(BUZZER, 3000, 200);
      delay(200);
      tone(BUZZER, 4000, 400);
      scoreTrigger = false;
    }

    //Draw Mouse
    display.fillRoundRect (5, py, 16, 12, 4, BLACK); //body
    if (py % 2 == 0) {
      display.fillRoundRect (14, py - 1, 11, 6, 2, BLACK); //head
      display.fillRoundRect (12, py - 3, 6, 3, 1, BLACK);  //ear
      display.fillRoundRect (18, py - 1, 2, 1, 1, WHITE);  //eye
      display.drawFastHLine(0, py + 1, 6, BLACK);      //tail
      display.drawFastHLine(0, py, 6, BLACK);      //tail
      display.fillRoundRect (18, py + 9, 4, 4, 1, BLACK); //front foot
      display.fillRoundRect (3, py + 9, 4, 4, 1, BLACK); //back foot
    }
    else {
      display.fillRoundRect (15, py - 2, 11, 7, 2, BLACK); //head
      display.fillRoundRect (13, py - 3, 6, 3, 1, BLACK);  //ear
      display.fillRoundRect (19, py - 1, 2, 1, 1, WHITE);  //eye
      display.drawFastHLine(0, py + 3, 6, BLACK);      //tail
      display.drawFastHLine(0, py + 2, 6, BLACK);    //tail
      display.fillRoundRect (16, py + 9, 4, 4, 1, BLACK); //front foot
      display.fillRoundRect (5, py + 9, 4, 4, 1, BLACK); //back foot
    }

    //Control wall speed  (lower the divisor number to speed up).  This will automatically speed up to a pretty impossible rate as score goes up to 11
    if (score < 4) wallPos[i] -= deltaTime / (20 - (score * 3)); // move walls
    if (score >= 4) wallPos[i] -= deltaTime / 9; // move walls
  }

  //Display scores
  display.setTextColor(BLACK, WHITE);
  display.setCursor(3, 2);
  display.print(F("SCORE:"));
  display.println(score);
  display.setCursor(108, 2);
  display.print(F("HI:"));
  display.println(hiscore);

  //Check for poke
  if (digitalRead(RIGHT_POKE) == LOW) ySpeed = -3;
  lastTime = millis();

  if (gameOver) {
    firstRunGames = true;
    display.clearDisplay();
    display.setCursor(20, 30);
    display.println(F("GAME OVER"));
    display.setCursor(20, 50);
    display.print(F("SCORE:"));
    display.println(score);
    if (score > hiscore) {
      display.setCursor(0, 110);
      display.print(F("NEW HIGH SCORE"));
      hiscore = score;
    }
    display.setCursor(20, 70);
    display.print(F("HIGH: "));
    display.println(hiscore);
    display.refresh();
    delay(3000);
    display.clearDisplay();
  }
  else firstRunGames = false;
  display.refresh();
}
