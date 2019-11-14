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
bool wolfGameEnded;  // let loop know whether game is ended or not
bool shouldWolfMove; // boolean which lets snake sit in STOP position until player pushes first button
int wolfScore;
enum eWolfPosition { STAP = 0, LEFT_BOTTOM, LEFT_TOP, RIGHT_BOTTOM, RIGHT_TOP}; // ENUM letting game know which position wolf is.
eWolfPosition pos; // position holding enum value
Ticker wolfSpeed; // ticker dictates how fast snake travels
void wolfSetup() {
  wolfGameEnded = false;
  pos = STAP;
  wolfScore = 0;
  wolfSpeed.attach_ms(1000, moveWolf);
}

void moveWolf() {
  shouldWolfMove = true;
}

void wolfGameOver() {
  // If game ends, display game over and final score
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 4, "GAME OVER!");
  display.drawString(64, 24, "FINAL SCORE");
  display.drawString(64, 44, String(wolfScore));
  display.display();
  delay(5000);
  if (wolfScore > 200) {
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
      snakeUrl = defaultSnakeUrl;
      snakeUrl += wolfScore;
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

void wolfDraw() {
  display.clear();
  //display.drawLine(0, 53, 128, 53);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 54, "Score:");
  display.drawString(30, 54, String(wolfScore));
  display.drawXbm(43, 0, test_vlk_width, test_vlk_height, test_vlk_bits);
  display.drawXbm(86, 0, test_kurnik_width, test_kurnik_height, test_kurnik_bits);
  //display.drawRect(fruitX*4,fruitY*4,2,2); // draw fruit
  //display.drawRect(x*4,y*4,2,2); // draw snake head
  display.display();
}

void wolfInput() {
  char key = customKeypad.getKey();
  if (key == '1') pos = LEFT_TOP;
  if (key == '3') pos = RIGHT_TOP;
  if (key == '4') pos = LEFT_BOTTOM;
  if (key == '6') pos = RIGHT_BOTTOM;
  if (key == '#') wolfGameEnded = true;
}

void wolfLogic() {
  switch (pos)
  {
  case LEFT_TOP:
      //x--;
      break;
  case RIGHT_TOP:
      //x++;
      break;
  case LEFT_BOTTOM:
      //y--;
      break;
  case RIGHT_BOTTOM:
      //y++;
      break;
  default:
      break;
  }
}

void wolfGame() {
  display.clear();
  display.drawXbm(17, 0, snakelogo_width, snakelogo_height, snakelogo_bits);
  display.display();
  delay(3000);
  wolfSetup();
  while (!wolfGameEnded) {
    wolfDraw();
    wolfInput();
    if (shouldWolfMove) {
      shouldWolfMove = false;
      wolfLogic();
    }
  }
  gameMenu();
  return;
}
