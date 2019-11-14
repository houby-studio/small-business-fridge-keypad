// Shows 'Connecting to:' and SSID underneath on startup and connecting to WiFi
void drawConnectingTo() {
  String ssidString = String(ssid);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Connecting to: ");
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 17, ssidString);
  display.display();
}

// Shows progress bar when device is attempting to connect to WiFi 
void drawProgressBar(int x) {
  int progress = x % 100;
  // draw the progress bar
  display.drawProgressBar(0, 46, 127, 10, progress);
  display.display();
}

// Show 'Powered by' loaded letter by letter
void drawPoweredBy() {
    const int horalign = 0;
    const int veralign = 20; 
    String poweredByArray [9] = { "o", "w", "e", "r", "e", "d", " ", "b", "y" }; 
    String poweredBy = "P";
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);
    display.drawString(horalign, veralign, poweredBy);
    display.display();
    for (String x : poweredByArray) {
      delay(50);
      poweredBy += x;
      display.drawString(horalign, veralign, poweredBy);
      display.display();
    }
    delay(750);
}

// Display Image Prdel Logo
void drawPrdelLogo() {
    display.clear();
    display.drawXbm(0, 0, prdel_width, prdel_height, prdel_bits);
    display.display();
}

void defaultScreenInfo() {
  // If screen is displaying easter egg or if someone is shopping, dont override screen
  if (screenStateEasterEgg == true || screenStateShopping == true) {
    return;  
  }
  screenStateShouldChange = false;
  display.clear();
  if (screenStateAlternate == false) {
    display.drawXbm(0, 0, iotlogo_width, iotlogo_height, iotlogo_bits);
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 34, "IP:");
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 48 , IpAddressStr);    
    screenStateAlternate = true;
  } else {
    display.drawXbm(0, 0, iotlogo_width, iotlogo_height, iotlogo_bits);
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 30, "Pro nákup zvolte");
    display.drawXbm(13, 51, hacek_width, hacek_height, hacek_bits);
    display.drawString(64, 48, "císlo zákazníka");
    screenStateAlternate = false;
    //playPikachuAnimation();
  }
  display.display();
}

void gameMenu() {
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Tajné herní menu");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  if (gameMenuSelect == 1) {
    display.drawString(5, 12, ">");
    display.drawString(10, 12, "Snake");
  } else {
    display.drawString(10, 12, "Snake");
  }
  if (gameMenuSelect == 2) {
    display.drawString(5, 24, ">");
    display.drawString(10, 24, "Pong");
  } else {
    display.drawString(10, 24, "Pong");
  }
  if (gameMenuSelect == 3) {
    display.drawString(5, 36, ">");
    display.drawString(10, 36, "Nu pogodi");
  } else {
    display.drawString(10, 36, "Nu pogodi");
  }
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 48, "2 a 8 pohyb v menu");
  display.display();
}
