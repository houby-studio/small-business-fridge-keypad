// Shows 'Connecting to:' and SSID underneath on startup and connecting to WiFi on power on
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

// Show 'Powered by' loaded letter by letter - intro on power on
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

// Display Image Prdel Logo after 'Powered on' on power on
void drawPrdelLogo() {
    display.clear();
    display.drawXbm(0, 0, prdel_width, prdel_height, prdel_bits);
    display.display();
}

// Handle displaying default alternating screen
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
  }
  display.display();
}

// Displays super secret game menu
void gameMenu() {
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Tajné herní menu");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  if (gameMenuSelect == 1) display.drawString(5, 12, ">");
  display.drawString(10, 12, "Spustit hru Snake");
  if (gameMenuSelect == 2) display.drawString(5, 24, ">");
  display.drawString(10, 24, "High scores");
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  //display.drawString(64, 36, "Storno stiskem     #");
  //display.drawString(64, 36, "Spust *       Storno #");
  display.drawString(64, 36, "Potvrdit           Návrat #");
  display.drawString(64, 48, "2 / 8      pohyb v menu");
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(49, 36, "*");
  display.display();
}

// Displays HTTP Request to show customer something is happening
void drawHttpRequest() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 20, "HTTP dotaz");
  display.display();
}

// Displays product choice menu after customer has been set
void drawChooseProduct(bool makeHttpRequest) {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Zvolte ID produktu");
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 48, "Storno stiskem    #");
  if (makeHttpRequest) {
    // Invokes GET customerName API to retrieve name - webservice may reset everything if customer is not found
    display.drawString(0, 36, "Zákazník: ");
    display.drawString(48, 36, getCustomerName());
  } else {
    // If customer name started with '0' do not invoke web request
    display.drawString(0, 36, "Zákazník: " + customerNumber);
  }
  display.display();
}

// Displays customer selection screen
void drawEnterCustomerNumber() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "ID zákazníka");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 18, customerNumber);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 36, "Potvrd' stiskem");
  display.setFont(ArialMT_Plain_24);
  display.drawString(77, 36, "*");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 48, "Storno stiskem    #");
  display.display();  
}

// Displays 'Connected!' when device connects to Wi-fi
void drawWifiConnected() {
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 20, "Connected!");
  display.display();
}

// Displays 'Customer/Product not found or Product not on stock' when standard API returns 404 return value
void drawApiStandardError(int errorType) {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  if (errorType == 1) {
    display.drawString(64, 10, "Zákazník");
    display.drawString(64, 30, "nenalezen");
  } else if (errorType == 2) {
    display.drawString(64, 10, "Produkt");
    display.drawString(64, 30, "nenalezen");
  } else {
    display.drawString(64, 10, "Produkt");
    display.drawString(64, 30, "není skladem");
  }
  display.display();    
}

// Displays 'Error' when HTTP Request terminates with non-standard error i.e. website or connection is down
void drawTerminatingError() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 10, "Hasta");
  display.drawString(64, 30, "la vista");
  display.display();
}

// Displays success message with product name and price
void drawProductBought(const char* productName, const int productPrice) {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Zakoupil jste");
  display.drawString(64, 20, productName);
  display.drawString(64, 40, String(productPrice) + ",-");
  display.display();
}
