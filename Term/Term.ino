// Accelerometer Parameters: Start
#include "Arduino_BMI270_BMM150.h"
float accel_x, accel_y, accel_z;
// Accelerometer Parameters: End

// Temperature/Humidity Sensor Parameters: Start
#include <Arduino_HS300x.h>
float temperature, humidity;
// Temperature/Humidity Sensor Parameters: End

// Pulse Sensor Parameters: Start
int pulse_pin = 3;
int onboard_red_led = 22;
int ppg_threshold = 550;
int pulse_signal;  // Holding incoming raw data from the PPG sensor, ranging from 0 to 1024.
// Pulse Sensor Parameters: End

// RTC Chip Parameters: Start
#include "Arduino.h"
#include "uRTCLib.h"

uRTCLib rtc(0x68);

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
int hour, minute, second;
// RTC Chip Parameters: End

// Touchscreen Parameters: Start
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

#define TS_CS 3
#define TS_IRQ 4

XPT2046_Touchscreen ts(TS_CS);

#define TFT_RST 5
#define TFT_DC 6
#define TFT_CS 7
#define TFT_MOSI 8
#define TFT_MISO 9
#define TFT_CLK 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

int ts_Xmin = 350;
int ts_Xmax = 3800;
int ts_Ymin = 280;
int ts_Ymax = 3800;

extern uint8_t circle[];
extern uint8_t x_bitmap[];

int pressureThreshold = 999;  // Replacing ts.pressureThreshold in the original code.
int gameScreen = 1;
int moves = 1;
int winner = 0;  // 0: Draw; 1: Human; 2: Computer
bool buttonEnabled = true;

int board[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};  // 0: Blank; 1: Human; 2: Computer
// // Touchscreen Parameters: End

void setup() {
  Serial.begin(9600);

  // RTC Chip Setup: Start
  URTCLIB_WIRE.begin();
  rtc.set(0, 11, 7, 6, 10, 3, 23);
  // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year)
  // dayOfWeek (1=Sunday, 7=Saturday)
  // RTC Chip Setup: End

  // Accelerometer Setup: Start
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  // Serial.print("Accelerometer sample rate = ");
  // Serial.print(IMU.accelerationSampleRate());
  // Serial.println(" Hz");
  // Accelerometer Setup: Start

  // Temperature/Humidity Sensor Setup: Start
  if (!HS300x.begin()) {
    Serial.println("Failed to initialize temperature/humidity sensor!");
  }
  // Temperature/Humidity Sensor Setup: End

  // Touchscreen Setup: Start
  tft.begin(0x9341);
  tft.setRotation(3);
  ts.begin();
  ts.setRotation(1);
  drawStartScreen();
  // Touchscreen Setup: End

  Serial.println("Board Initialized!");
}

void loop() {
  // Pulse Sensor Code: Start
  pulse_signal = analogRead(pulse_pin);  // Read the PulseSensor's value.
  Serial.print("PPG: ");
  Serial.print(pulse_signal); // A good refresh rate is 100 ms.
  Serial.print("; ");

  if (pulse_signal < ppg_threshold) {  // Synchronizing the heartbeat with onboard LED.
    digitalWrite(onboard_red_led, HIGH);
  } else {
    digitalWrite(onboard_red_led, LOW);
  }
  // Pulse Sensor Code: End

  // RTC Chip Code: Start
  rtc.refresh();
  // The optimal refresh is naturally 1 second, maybe with multithreading.
  hour = rtc.hour();
  minute = rtc.minute();
  second = rtc.second();
  Serial.print("Time: ");
  Serial.print(hour);
  Serial.print(": ");
  Serial.print(minute);
  Serial.print(": ");
  Serial.print(second);
  Serial.print("; ");
  // RTC Chip Code: End

  // Accelerometer Code: Start
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(accel_x, accel_y, accel_z);
    Serial.print("Accelerometer: ");
    Serial.print(accel_x);
    Serial.print(", ");
    Serial.print(accel_y);
    Serial.print(", ");
    Serial.print(accel_z);
    Serial.print("; "); // The optimal refresh rate could be 100 ms.
  }
  // Accelerometer Code: End

  // Temperature/Humidity Sensor Code: Start
  temperature = HS300x.readTemperature();
  humidity = HS300x.readHumidity();
  Serial.print("Temperature/Humidity: ");
  Serial.print(temperature);
  Serial.print(", "); // Temperature in Celsius.
  Serial.print(humidity);
  Serial.print("; "); // Humidity in percentage.
  // Temperature/Humidity Sensor Code: End

  // Touchscreen Code: Start
  TS_Point touch = ts.getPoint();
  Serial.print("Pressure = ");
  Serial.print(touch.z);
  Serial.print(" (x = ");
  Serial.print(touch.x);
  Serial.print(", y = ");
  Serial.print(touch.y);
  Serial.println(")");
  // Touchscreen Code: End

  // TicTacToe Code: Start
  TS_Point p = ts.getPoint();  //Get touch point

  if (gameScreen == 3) {
    buttonEnabled = true;
  }

  if (ts.touched()) {
    p.x = map(p.x, ts_Xmin, ts_Xmax, 0, 320);
    p.y = map(p.y, ts_Ymin, ts_Ymax, 0, 240);
    // Serial.print("X = ");
    // Serial.print(p.x);
    // Serial.print("\tY = ");
    // Serial.print(p.y);
    // Serial.print("\n");

    if (p.x > 60 && p.x < 260 && p.y > 180 && p.y < 220 && buttonEnabled) { // The user has pressed inside the red rectangle
      buttonEnabled = false;  //Disable button
      // Serial.println("Button Pressed");
      resetGame();
      // pinMode(XM, OUTPUT); // This is important, because the libraries are sharing pins
      // pinMode(YP, OUTPUT);
      drawGameScreen();
      playGame();
    }
    delay(10);
  }
  // TicTacToe Code: End
}

void resetGame() {
  buttonEnabled = false;
  for (int i = 0; i < 9; i++) {
    board[i] = 0;
  }
  moves = 1;
  winner = 0;
  gameScreen = 2;
}

void drawStartScreen() {
  tft.fillScreen(BLACK);
  tft.drawRect(0, 0, 319, 240, WHITE); //Draw white frame
  tft.setCursor(30, 100); //Print "Tic Tac Toe" Text
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("Tic Tac Toe");
  tft.setCursor(80, 30);   //Print "Arduino" text
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.print("Arduino");
  createStartButton();
}

void createStartButton() {
  tft.fillRect(60, 180, 200, 40, RED); //Create Red Button
  tft.drawRect(60, 180, 200, 40, WHITE);
  tft.setCursor(72, 188);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("Start Game");
}

void drawGameScreen() {
  tft.fillScreen(BLACK);
  tft.drawRect(0, 0, 319, 240, WHITE); //Draw frame
  drawVerticalLine(125);
  drawVerticalLine(195);
  drawHorizontalLine(80);
  drawHorizontalLine(150);
}

void drawGameOverScreen() {
  tft.fillScreen(BLACK);
  tft.drawRect(0, 0, 319, 240, WHITE); //Draw frame
  tft.setCursor(50, 30); //Print "Game Over" Text
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("GAME OVER");

  if (winner == 0) {
    tft.setCursor(110, 100); //Print "DRAW!" text
    tft.setTextColor(YELLOW);
    tft.setTextSize(4);
    tft.print("DRAW");
  }

  if (winner == 1) {
    tft.setCursor(40, 100); //Print "HUMAN WINS!" text
    tft.setTextColor(BLUE);
    tft.setTextSize(4);
    tft.print("HUMAN WINS");
  }

  if (winner == 2) {
    tft.setCursor(60, 100); //Print "CPU WINS!" text
    tft.setTextColor(RED);
    tft.setTextSize(4);
    tft.print("CPU WINS");
  }

  createPlayAgainButton();
  // pinMode(XM, OUTPUT);
  // pinMode(YP, OUTPUT);
}

void createPlayAgainButton() {
  tft.fillRect(60, 180, 200, 40, RED); //Create Red Button
  tft.drawRect(60, 180, 200, 40, WHITE);
  tft.setCursor(72, 188);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("Play Again");
}


void drawHorizontalLine(int y) {
  int i = 0;
  for (i = 0; i < 7; i++) {
    tft.drawLine(60, y + i, 270, y + i, WHITE);
  }
}

void drawVerticalLine(int x) {
  int i = 0;
  for (i = 0; i < 7; i++) {
    tft.drawLine(x + i, 20, x + i, 220, WHITE);
  }
}

void playGame() {
  do {
    if (moves % 2 == 0) { // Changing to having the user go first. 
      arduinoMove();
      printBoard();
      checkWinner();
    } else {
      playerMove();
      printBoard();
      checkWinner();
    }
    moves++;
  } while (winner == 0 && moves < 10);
  
  if (winner == 1) {
    Serial.println("HUMAN WINS");
    delay(3000);
    gameScreen = 3;
    drawGameOverScreen();
  } else if (winner == 2) {
    Serial.println("CPU WINS");
    delay(3000);
    gameScreen = 3;
    drawGameOverScreen();
  } else {
    Serial.println("DRAW");
    delay(3000);
    gameScreen = 3;
    drawGameOverScreen();
  }
}

void playerMove() {
  // pinMode(XM, OUTPUT);
  // pinMode(YP, OUTPUT);
  TS_Point p;
  boolean validMove = false;
  // Serial.print("\nPlayer Move:");
  do {    
    p = ts.getPoint(); //Get touch point  
    if (p.z > pressureThreshold) {
      p.x = map(p.x, ts_Xmin, ts_Xmax, 0, 320);
      p.y = map(p.y, ts_Ymin, ts_Ymax, 0, 240);
      // Serial.println(p.x);
      // Serial.println(p.y);
  
      if((p.x < 115) && (p.y>=150)) { // 6
        if(board[6] == 0) {
          Serial.println("Player Move: 6");
          // pinMode(XM, OUTPUT);
          // pinMode(YP, OUTPUT);
          board[6] = 1;
          drawPlayerMove(6);  
          // Serial.println("Drawing player move");
        }
      } else if ((p.x > 0 && p.x < 115) && (p.y < 150 && p.y > 80)) { // 3
        if (board[3] == 0) {
          Serial.println("Player Move: 3");
          // pinMode(XM, OUTPUT);
          // pinMode(YP, OUTPUT);
          board[3] = 1;
          drawPlayerMove(3);  
          // Serial.println("Drawing player move");
        }
      } else if ((p.x <125) && (p.y < 80)) { // 0
        if(board[0] == 0) {
          Serial.println("Player Move: 0");          
          // pinMode(XM, OUTPUT);
          // pinMode(YP, OUTPUT);
          board[0] = 1;
          drawPlayerMove(0);  
        }
      } else if ((p.x > 125 && p.x <= 195) && (p.y < 80)) { // 1
        if(board[1] == 0) {
          Serial.println("Player Move: 1");          
          // pinMode(XM, OUTPUT);
          // pinMode(YP, OUTPUT);
          board[1] = 1;
          drawPlayerMove(1);  
        }
      } else if ((p.x > 195) && (p.y < 80)) { //2
        if(board[2] == 0) {
          Serial.println("Player Move: 2");          
          // pinMode(XM, OUTPUT);
          // pinMode(YP, OUTPUT);
          board[2] = 1;
          drawPlayerMove(2);  
        }
      } else if ((p.x > 125 && p.x <= 195) && (p.y < 150 && p.y > 80)) { // 4
        if(board[4] == 0) {
          Serial.println("Player Move: 4");          
          // pinMode(XM, OUTPUT);
          // pinMode(YP, OUTPUT);
          board[4] = 1;
          drawPlayerMove(4);  
        }
      } else if ((p.x > 195) && (p.y < 150 && p.y > 80)) { // 5
        if(board[5] == 0) {
          Serial.println("Player Move: 5");          
          // pinMode(XM, OUTPUT);
          // pinMode(YP, OUTPUT);
          board[5] = 1;
          drawPlayerMove(5);  
        }
      } else if ((p.x > 125 && p.x <= 195) && (p.y > 150)) { // 7
        if(board[7] == 0) {
          Serial.println("Player Move: 7");          
          // pinMode(XM, OUTPUT);
          // pinMode(YP, OUTPUT);
          board[7] = 1;
          drawPlayerMove(7);  
        }
      } else if ((p.x > 195) && (p.y > 150)) { // 8
        if(board[8] == 0) {
          Serial.println("Player Move: 8");          
          // pinMode(XM, OUTPUT);
          // pinMode(YP, OUTPUT);
          board[8] = 1;
          drawPlayerMove(8);  
        }
      }
    }
  } while (p.z < pressureThreshold); 
}

void printBoard() {
  int i = 0;
  Serial.print("Board: [");
  for (i = 0; i < 9; i++) {
    Serial.print(board[i]);
    Serial.print(",");
  }
  Serial.println("]");
}

int checkOpponent() {
  if (board[0] == 1 && board[1] == 1 && board[2] == 0)
    return 2;
  else if (board[0] == 1 && board[1] == 0 && board[2] == 1)
    return 1;
  else if (board[1] == 1 && board[2] == 1 && board[0] == 0)
    return 0;
  else if (board[3] == 1 && board[4] == 1 && board[5] == 0)
    return 5;
  else if (board[4] == 1 && board[5] == 1 && board[3] == 0)
    return 3;
  else if (board[3] == 1 && board[4] == 0 && board[5] == 1)
    return 4;
  else if (board[1] == 0 && board[4] == 1 && board[7] == 1)
    return 1;
  else
    return 100;
}

void arduinoMove() {
  int b = 0;
  int counter = 0;
  int movesPlayed = 0;
  Serial.print("\nArduino Move: ");

  int firstMoves[] = {0, 2, 6, 8};  // will use these positions first

  for (counter = 0; counter < 4; counter++) { // Count first moves played
    if (board[firstMoves[counter]] != 0) { // First move is played by someone
      movesPlayed++;
    }
  }
  do {
    if (moves <= 2) {
      int randomMove = random(4);
      int c = firstMoves[randomMove];

      if (board[c] == 0) {
        delay(1000);
        board[c] = 2;
        Serial.print(firstMoves[randomMove]);
        Serial.println();
        drawCpuMove(firstMoves[randomMove]);
        b = 1;
      }
    } else {
      int nextMove = checkOpponent();
      if (nextMove == 100) {
        if (movesPlayed == 4) { // All first moves are played
          int randomMove = random(9);
          if (board[randomMove] == 0) {
            delay(1000);
            board[randomMove] = 2;
            Serial.print(randomMove);
            Serial.println();
            drawCpuMove(randomMove);
            b = 1;
          }
        } else {
          int randomMove = random(4);
          int c = firstMoves[randomMove];

          if (board[c] == 0) {
            delay(1000);
            board[c] = 2;
            Serial.print(firstMoves[randomMove]);
            Serial.println();
            drawCpuMove(firstMoves[randomMove]);
            b = 1;
          }
        }
      } else {
        delay(1000);
        board[nextMove] = 2;
        drawCpuMove(nextMove);
        b = 1;
      }
    }
  } while (b < 1);
}
  
void drawCircle (int x, int y) {
  drawBitmap(x, y, circle, 65, 65, RED);
}

void drawX(int x, int y) {
  drawBitmap(x, y, x_bitmap, 65, 65, BLUE);
}

void drawCpuMove (int move) {
  switch (move) {
    case 0: drawCircle(55, 15); break;
    case 1: drawCircle(130, 15); break;
    case 2: drawCircle(205, 15); break;
    case 3: drawCircle(55, 85); break;
    case 4: drawCircle(130, 85); break;
    case 5: drawCircle(205, 85); break;
    case 6: drawCircle(55, 155); break;
    case 7: drawCircle(130, 155); break;
    case 8: drawCircle(205, 155); break;
  }
}

void drawPlayerMove (int move) {
  switch (move) {
    case 0: drawX(55, 15); break;
    case 1: drawX(130, 15); break;
    case 2: drawX(205, 15); break;
    case 3: drawX(55, 85); break;
    case 4: drawX(130, 85); break;
    case 5: drawX(205, 85); break;
    case 6: drawX(55, 155); break;
    case 7: drawX(130, 155); break;
    case 8: drawX(205, 155); break;
  }
}

void checkWinner() {
  int qq = 0;

  if (board[0] == 1 && board[1] == 1 && board[2] == 1) {
    winner = 1;
  }
  if (board[3] == 1 && board[4] == 1 && board[5] == 1) {
    winner = 1;
  }
  if (board[6] == 1 && board[7] == 1 && board[8] == 1) {
    winner = 1;
  }
  if (board[0] == 1 && board[3] == 1 && board[6] == 1) {
    winner = 1;
  }
  if (board[1] == 1 && board[4] == 1 && board[7] == 1) {
    winner = 1;
  }
  if (board[2] == 1 && board[5] == 1 && board[8] == 1) {
    winner = 1;
  }
  if (board[0] == 1 && board[4] == 1 && board[8] == 1) {
    winner = 1;
  }
  if (board[2] == 1 && board[4] == 1 && board[6] == 1) {
    winner = 1;
  }

  if (board[0] == 2 && board[1] == 2 && board[2] == 2) {
    winner = 2;
  }
  if (board[3] == 2 && board[4] == 2 && board[5] == 2) {
    winner = 2;
  }
  if (board[6] == 2 && board[7] == 2 && board[8] == 2) {
    winner = 2;
  }
  if (board[0] == 2 && board[3] == 2 && board[6] == 2) {
    winner = 2;
  }
  if (board[1] == 2 && board[4] == 2 && board[7] == 2) {
    winner = 2;
  }
  if (board[2] == 2 && board[5] == 2 && board[8] == 2) {
    winner = 2;
  }
  if (board[0] == 2 && board[4] == 2 && board[8] == 2) {
    winner = 2;
  }
  if (board[2] == 2 && board[4] == 2 && board[6] == 2) {
    winner = 2;
  }
}
void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (i & 7) byte <<= 1;
      else byte = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if (byte & 0x80) tft.drawPixel(x + i, y + j, color);
    }
  }
}