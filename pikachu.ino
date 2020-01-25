char playPikachuAnimation() {
  // Displays blinking animation
  for (int i = 0; i < 7; i++) {
    char eventCustomKey = customKeypad.getKey();
    if (eventCustomKey) {
      screenStateEasterEggShouldPlay = false;
      return eventCustomKey;
    }
    display.clear();
    display.display();
    display.drawXbm(0, 0, blink_width, blink_height, blink_bits);
    delay(200);
    display.display();
  }
  // Displays wild pikachu appeared animation
  for (int i = 128; i >= 0; i--) {
    char eventCustomKey = customKeypad.getKey();
    if (eventCustomKey) {
      screenStateEasterEggShouldPlay = false;
      return eventCustomKey;
    }
    display.clear();
    display.drawXbm(i, 0, border_width, border_height, border_bits);
    display.display();
  }
  // Holds image for a while
  for (int i = 100; i >= 0; i--) {
    char eventCustomKey = customKeypad.getKey();
    if (eventCustomKey) {
      screenStateEasterEggShouldPlay = false;
      return eventCustomKey;
    }
    delay(10);
  }
  // Displays surprised pikachu
  for (int i = 64; i >= 0; i--) {
    char eventCustomKey = customKeypad.getKey();
    if (eventCustomKey) {
      screenStateEasterEggShouldPlay = false;
      return eventCustomKey;
    }
    display.clear();
    display.drawXbm(0, i, pikachu_width, pikachu_height, pikachu_bits);
    display.display();
  }
  // Holds image for a while
  for (int i = 300; i >= 0; i--) {
    char eventCustomKey = customKeypad.getKey();
    if (eventCustomKey) {
      screenStateEasterEggShouldPlay = false;
      return eventCustomKey;
    }
    delay(10);
  }
  screenStateEasterEggShouldPlay = false; 
return 'e';
}
