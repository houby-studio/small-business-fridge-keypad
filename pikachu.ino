char playPikachuAnimation() {
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
    screenStateEasterEggShouldPlay = false; 
    return 'e';
}
