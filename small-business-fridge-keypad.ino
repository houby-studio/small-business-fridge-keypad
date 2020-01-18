/*
  ESP8266 Smart fridge order system
  Author: Jakub Šindelář (Prdel CEO, coder, scripter, director, graphic designer, analyst, manager, etc.)
  Includes: 
  • NodeMCU v1.0 ESP-12E (ESP8266) chip connected to WiFi which handles main code and peripheries. Sends HTTP requests to webservice
  • Adafruit 4x3 Plastic keybaord used for input. Required for customed ID and product ID.
  • ssd1306 OLED 128x64 display for showing status, pressed numbers and hidden easter egg animations

  Notes from this project:
  • Originally I wanted to use D0-D8 to connect LCD (2 GPIO pins) and 4x3 Keypad (7 GPIO pins)
    Turns out D8 is reserved for RESET so I had to change serial RX pin to standard GPIO input and use it for keypad!
    Also D4 is used for onboard LED, so whenever one presses key on certain row, led blinks. I guess that's not really that big of a deal.
  • LCD use cases use LCD assistant to trasnform bmp image to code, but it doesn't work for me. I had to use Gimp and save fiels as xbm file which works perfectly
  • 
*/
#include <SSD1306Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Keypad.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include "images.h"
#include "animations/pikachu/pikachu.h"

// WiFi connection variables - SSID and Password to your WiFi and IpAddress string, which will be overridden when DHCP assigns IP address to device
const char* ssid     = "DEV";
const char* password = "Heslo321.";
String IpAddressStr = "127.0.0.1"; //Fallback if no IP address assigned - shouldn't really ever get to this scenario tho

// Keypad variables and initialization - depends on keypad. In this case its Adafruit 4x3 Keypad
const byte ROWS = 4; // Four rows (123; 456; 789; *0#)
const byte COLS = 3; //four columns (147*; 2580; 369#)
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
}; // Characters which will be "pressed" when coresponding key is pressed. Can change to your liking, but should probably reflect the characters on physical keypad
// Adafruit hard plastic keypad uses pretty non-standard headache-causing pin layout; Reading from pin 1 to 7 (left to right) on keypad controls those rows/columns: 1. COL2, 2. ROW1, 3. COL1, 4. ROW4, 5. COL3, 6. ROW3, 7. ROW2
byte rowPins[ROWS] = {D1, 3, D7, D4}; // Connect to the row pinouts of the keypad - D1, 3(RX), D7, D3 (Cannot use D8 because it is used by RESET, so we change RX pin to GPIO mode and use that one)
byte colPins[COLS] = {D2, D0, D6}; // Connect to the column pinouts of the keypad - D2 D0 D6
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

// Website variables
const String host = "lednice.prdelka.eu";
const int httpPort = 80;
const String getCustomerNameApi = "/api/customerName?customer=";
const String postKeypadOrderApi = "/api/keypadOrder";
const String getSnakeHighscoresApi = "/api/snakeHighscores";
const String postSnakeHighscoresApi = "/api/snakeHighscores";
const String apiSecret = "secret";

// Easter egg animations array. When easter egg is supposed to be played, one of the following is randomly chosen - DEPRACATED, ONLY ONE GAME AVAILABLE RIGHT NOW
// void (* easterEggArray [])() = {}; 
// Games idea - snake, tetris, pong, space invaders

// Ticker to control display change changes. One for alternating default screen, second for easter egg animations and third one for timeout for purchasing.
Ticker displayEasterEgg;
Ticker displayAlternateDefault;
Ticker shoppingTimeout;

// Booleans holding information about display state
bool screenStateEasterEgg = false;            // Info if easter egg is being played. If that is the case, alternating screen cannot override it.
bool screenStateEasterEggShouldPlay = false;  // When displayEasterEgg ticker ticks, it marks this variable as true so next time loop checks, it plays easter egg (unless someone is shopping).
bool screenStateAlternate = false;            // Info about which state default screen is in. So next time screen is supposed to change, it displays the other one.
bool screenStateShouldChange = false;         // When displayAlternateDefault ticker ticks, it marks this variable as true so next time loop checks, it changes to other default screen (unless someone is shopping or easter egg is playing).
bool screenStateShopping = false;             // Info whether or not someone is shopping. Prevents all other kinds of stuff happening on screen.

// Sources => Examples > ESP8266 and ESP32 Oled Driver for SSD1306 display > SSD1306SimpleDemo
// ##### START LED configure part #####
// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, D3, D5);

// Shopping variables
String customerNumber = "";
bool customerMode = true;

// Secret variables
bool gameMenuMode = false;
int gameMenuSelect = 1;

// Restarts variables to its default state and displays default screen
void resetState() {
  screenStateShopping = false;
  customerNumber = "";
  customerMode = true;
  screenStateEasterEggShouldPlay = false;
  defaultScreenInfo();
}

// Once every couple minutes ticker marks easter egg to play.
void changeEasterEggState() {
  Serial.println("Easter Egg Screen Ticker ticked");
  if (screenStateEasterEggShouldPlay == false) {
    screenStateEasterEggShouldPlay = true;
  }
}

// Once every couple seconds ticker marks default screen for change to other screen.
void changeDefaultState() {
  Serial.println("Default Screen Ticker ticked");
  if (screenStateShouldChange == false) {
    screenStateShouldChange = true;
  }
}

// Handles control in secret game menu
void gameMenuPress(char menuKeyPress) {
  if (menuKeyPress == '#') {
    screenStateShopping = false;
    screenStateEasterEgg = false;
    gameMenuMode = false;
    customerMode = true;
    screenStateEasterEggShouldPlay = false;
    defaultScreenInfo();
    return;
  }
  if (menuKeyPress == '*') {
    switch ( gameMenuSelect ) {
      case 1 : 
        snakeGame();
        break;
      case 2 : 
        //highScores();
        break;
      default :
        break;
    }
  }
  if (menuKeyPress == '2') {
    if (gameMenuSelect != 1) {
      gameMenuSelect--;
      gameMenu();
    }
  }
  if (menuKeyPress == '8') {
    if (gameMenuSelect != 2) {
      gameMenuSelect++;
      gameMenu();
    }
  }
}

// If current display state is in default screen and easter egg is marked for play, display easter egg
char easterEgg() {
  char eventCustomKey;
  screenStateEasterEgg = true;
  eventCustomKey = playPikachuAnimation();
  screenStateEasterEgg = false;
  return eventCustomKey;
}

// Main keypad key handler
void updateShoppingScreen(char pressedKey) {
  // Change state to Shopping to make other controls disabled (easter eggs and alternate screen)
  if (screenStateShopping == false) {
    screenStateShopping = true;
  }
  // Cancel all. Return all variables to default
  if (pressedKey == '#') {
    resetState();
    return;
  }
  // Enter customer number
  if (customerMode == true) {
    // If pressed Confirm "*", attempt to transition to product order state
    if (pressedKey == '*') {
      if (customerNumber == "") {
        // Empty customer, cannot proceed, nothing happens.
      } else {
        customerMode = false;
        // If customer starts with 0, skip name check
        if ((customerNumber.charAt(0)) != '0') {
          // Standard way - HTTP GET customer name
          drawHttpRequest();
          drawChooseProduct(true);
        } else {
          // Prefix with 0 - Do NOT HTTP GET customer name - saves some time especially on slower network
          customerNumber.remove(0,1);
          drawChooseProduct(false);
        }
      }
    } else {
      // Keep adding additional characters to customerNumber
      customerNumber += pressedKey;
      drawEnterCustomerNumber();
    }
    // Super secret evil game menu. Begone believers in god!
    if (customerNumber == "666") {
      customerNumber = "";
      screenStateEasterEggShouldPlay = false;
      screenStateEasterEgg = true;
      gameMenuMode = true;
      gameMenu();
    }
  } else {
    // Handle key press which launches product order API
    if (pressedKey == '*') {
      // Invalid key, do nothing.
    }
    drawHttpRequest();
    postKeypadOrder(pressedKey);
  }
}

void setup() {
  //GPIO 3 (RX) swap the pin to a GPIO.
  pinMode(3,INPUT_PULLUP);
  Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY); // We can't use RX for serial, so we use TX only mode - debug only, can comment out in release version
  delay(10);
  // Setup display
  display.init();
  display.flipScreenVertically();
  // Display 'Powered by' and Prdel logo
  drawPoweredBy();
  drawPrdelLogo();
  delay(1500);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Variable for progress bar and display 'Connecting to:' info + loading bar so it shows up at least for a second
  int counter = 0;
  drawConnectingTo();
  drawProgressBar(counter);

  // Start connecting to Wifi + show loading bar
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    drawProgressBar(counter);
    counter++;
  }
  
  // Wifi connected - Show great success
  drawWifiConnected();
  delay(1500);
  
  IpAddressStr = WiFi.localIP().toString();
  defaultScreenInfo();

  //Initialize Ticker every 600sec/10min
  //displayEasterEgg.attach(600, easterEgg);
  //Initialize Ticker every 15sec
  //displayAlternateDefault.attach(15, changeDefaultState);
  displayAlternateDefault.attach(10, changeDefaultState);
  displayEasterEgg.attach(35, changeEasterEggState);
}

// Main loop
void loop() {
  char customKey = customKeypad.getKey();
  if (!customKey) {
    // If no key was pressed, check what to display
    // If screenStateShopping is running, return without intervening
    if (screenStateShopping == true) {
      return;
    }
    if (screenStateEasterEggShouldPlay == true) {
      customKey = easterEgg();
      if (customKey == 'e') {
        return;
      }
    }
    // If no key pressed, do typical stuff
    if (screenStateShouldChange == true) {
     defaultScreenInfo();
    }
  }
  if (customKey){
    // Serial.println("Key pressed");
    // Serial.println(customKey);
    // Do stuff when pressed stuff
    if (gameMenuMode) {
      gameMenuPress(customKey);
    } else {
      updateShoppingScreen(customKey);
    }
  }
}
