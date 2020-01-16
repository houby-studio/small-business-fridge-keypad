/*
  ESP8266 Snake
  Author: Jakub Šindelář (Prdel CEO, coder, scripter, director, graphic designer, analyst, manager, etc.)
  Includes: 
  • NodeMCU v1.0 ESP-12E (ESP8266) chip connected to WiFi which handles main code and peripheries. Sends HTTP requests to webservice
  • Adafruit 4x3 Plastic keybaord used for input. Required for customed ID and product ID.
  • ssd1306 OLED 128x64 display for showing status, pressed numbers and hidden easter egg animations

  Notes from this project:
  • Snake game is a hidden easter egg in IoT smart fridge system
*/
bool snakeGameOver;  // let loop know whether game is ended or not
bool shouldSnakeMove; // boolean which lets snake sit in STOP position until player pushes first button
const int width = 32; // Display width /4 (because we draw 2x2 pixels)
const int height = 13; // Display height /4 (because we draw 2x2 pixels) - 3x4 pixels for score bar
int x, y, fruitX, fruitY, score; // coordinates of head and fruit and score
int tailX[416], tailY[416]; // Array holding values of up to 416 body parts (416 = 32x13 game field)
int nTail; // Integer counting body length
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN}; // ENUM letting game know which direction is snake heading
eDirection dir; // direction holding enum value
Ticker snakeSpeed; // ticker dictates how fast snake travels
String snakeUrl;
void snakeSetup() {
  snakeGameOver = false;
  std::fill_n(tailX, 416, 129); // New array blinked at 0,0 and previous games blinked in previous array position. This way it blinks outside of LCD
  std::fill_n(tailY, 416, 65); // New array blinked at 0,0 and previous games blinked in previous array position. This way it blinks outside of LCD
  nTail = 0;
  dir = STOP;
  x = width / 2;
  y = height / 2;
  fruitX = rand() % width;
  fruitY = rand() % height;
  score = 0;
  snakeSpeed.attach_ms(70, moveSnake);
}

void moveSnake() {
  shouldSnakeMove = true;
}

void gameOver() {
  // If game ends, display game over and final score
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 4, "GAME OVER!");
  display.drawString(64, 24, "FINAL SCORE");
  display.drawString(64, 44, String(score));
  display.display();
  delay(5000);
  if (score > 200) {
    // If score was at least 1010 upload highscore to website
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 4, "HIGHSCORE");
    display.drawString(64, 24, "Uploading score");
    display.drawString(64, 44, "Please wait");
    display.display();
    WiFiClient client;
    if (!client.connect(host, httpPort)) {
        display.clear();
        display.drawString(64, 4, "CONNECTION");
        display.drawString(64, 24, "FAILED");
        display.drawString(64, 44, "SORRY");
        display.display();
        delay(2000);
    } else {
      snakeUrl = getSnakeHighscoresApi;
      snakeUrl += score;
      client.print(String("GET ") + snakeUrl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
      while (client.connected()) {
        String returnLine = client.readStringUntil('\n');
        if (returnLine == "\r") {
          Serial.println("headers received");
          break;
        }
      }
      // JSON LIB https://arduinojson.org/v6/example/http-client/
      // helper https://arduinojson.org/v6/assistant/
      // be vary of v5 vs v6

      // Create buffer calculated by assistant
      const size_t capacity = 2*JSON_OBJECT_SIZE(3) + 100;
      DynamicJsonDocument doc(capacity);

      // Read response and parse it
      String json = client.readStringUntil('\n');
      deserializeJson(doc, json);

      if (doc["status"] != 200) {
        display.clear();
        display.drawString(64, 4, "CONNECTION");
        display.drawString(64, 24, "FAILED");
        display.drawString(64, 44, "SORRY");
        display.display();
        delay(2000); 
      } else {
        JsonObject data = doc["data"];
        String data_beaten = data["beaten"]; // "newrecord"
        String data_currentrecord = data["currentrecord"]; // "current record in highscores: for example 1000"
        String data_score = data["score"]; // "Your highscore, for example (maximum) 4160"
        if (data_beaten == "newrecord") {
          display.clear();
          display.drawString(64, 4, "NEW RECORD!");
          display.drawString(64, 24, "Previous record:");
          display.drawString(64, 44, data_currentrecord);
          display.display();
          delay(2000);
          display.clear();
          display.drawString(64, 4, "NEW RECORD!");
          display.drawString(64, 24, "Your new record:");
          display.drawString(64, 44, data_score);
          display.display();
        } else {
          display.clear();
          display.drawString(64, 4, "MEDIOCRE!");
          display.drawString(64, 24, "Current record:");
          display.drawString(64, 44, data_currentrecord);
          display.display();
          delay(2000);
          display.clear();
          display.drawString(64, 4, "MEDIOCRE!");
          display.drawString(64, 24, "Your score:");
          display.drawString(64, 44, data_score);
          display.display();
        }
      }
    }
    delay(5000);
  }
}

void snakeDraw() {
  display.clear();
  display.drawLine(0, 53, 128, 53);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 54, "Score:");
  display.drawString(30, 54, String(score));
  display.drawRect(fruitX*4,fruitY*4,2,2); // draw fruit
  display.drawRect(x*4,y*4,2,2); // draw snake head
  for (int k = 0; k < nTail; k++) { 
    display.drawRect(tailX[k]*4,tailY[k]*4,2,2); // draw body
  }
  display.display();
}

void snakeInput() {
  char key = customKeypad.getKey();
  if (key == '2') dir = UP;
  if (key == '8') dir = DOWN;
  if (key == '4') dir = LEFT;
  if (key == '6') dir = RIGHT;
  if (key == '#') 
  {
    snakeGameOver = true;
    gameOver();
  }
}

void snakeLogic() {
  int prevX = tailX[0];
  int prevY = tailY[0];
  int prev2X, prev2Y;
  tailX[0] = x;
  tailY[0] = y;
  for (int i = 1; i < nTail; i++)
  {
      prev2X = tailX[i];
      prev2Y = tailY[i];
      tailX[i] = prevX;
      tailY[i] = prevY;
      prevX = prev2X;
      prevY = prev2Y;
  }
  switch (dir)
  {
  case LEFT:
      x--;
      break;
  case RIGHT:
      x++;
      break;
  case UP:
      y--;
      break;
  case DOWN:
      y++;
      break;
  default:
      break;
  }
  //if (x > width || x < 0 || y > height || y < 0)
  //  gameOver = true;
  if (x >= width) x = 0; else if (x < 0) x = width - 1;
  if (y >= height) y = 0; else if (y < 0) y = height - 1;

  for (int i = 0; i < nTail; i++) {
      if (tailX[i] == x && tailY[i] == y) {
          snakeGameOver = true;
          gameOver();
      }
  }
  
  if (x == fruitX && y == fruitY)
  {
      score += 10;
      fruitX = rand() % width;
      fruitY = rand() % height;
      nTail++;
  }
}

void snakeGame() {
  display.clear();
  display.drawXbm(17, 0, snakelogo_width, snakelogo_height, snakelogo_bits);
  display.display();
  delay(3000);
  snakeSetup();
  while (!snakeGameOver) {
    snakeDraw();
    snakeInput();
    if (shouldSnakeMove) {
      shouldSnakeMove = false;
      snakeLogic();
    }
  }
  gameMenu();
  return;
}
