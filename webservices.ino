// Webservice to obtain customer's name
String getCustomerName() {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;
  String payload;
  // Make request to target host with customerName call and customerNumber
  if (https.begin(*client, "https://" + host + getCustomerNameApi + customerNumber)) {
    Serial.print("[HTTPS] GET...\n");
    // start connection and send HTTP header with API secret
    https.addHeader("sbf-API-secret", apiSecret);
    int httpCode = https.GET();
  
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        // Get result and remove first and last characters ""
        payload = https.getString();
        payload.remove(0,1);
        payload.remove(payload.length()-1);
        Serial.println(payload);
      } else if (httpCode == HTTP_CODE_NOT_FOUND) {
        // User with specified ID not found - error https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h
        drawApiStandardError(1);
        delay(2000);
        resetState();
      } else {
        // Either bad secret or not a number - error https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h
        drawTerminatingError();
        delay(2000);
        resetState();
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      // Received HTTP Error Code https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h
      drawTerminatingError();
      delay(2000);
      resetState();
    }
    https.end();
    return payload;
  } else {
    // Error making HTTP request
    drawTerminatingError();
    delay(2000);
    resetState();
  }
}

// Webservice to make product order
String postKeypadOrder(char productNumber) {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;
  String payload;
  DynamicJsonDocument doc(1024);
  // Make request to target host and api url
  if (https.begin(*client, "https://" + host + postKeypadOrderApi)) {
    Serial.print("[HTTPS] POST...\n");
    // Start connection and send HTTP header with API secret and content type
    https.addHeader("sbf-API-secret", apiSecret);
    https.addHeader("Content-Type", "application/json");
    // Make HTTP POST request with body containing customerNumber and productNumber
    int httpCode = https.POST("{\"customer\":" + customerNumber + ",\"product\":" + productNumber + "}" );
  
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
  
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        // Get result and parse JSON
        payload = https.getString();
        auto error = deserializeJson(doc, payload);
        if (error) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(error.c_str());
            return "ERROR";
        }
        const char* productName = doc["product"]["name"];
        const int productPrice = doc["product"]["price"];
        drawProductBought(productName, productPrice);
        delay(3000);
        resetState();
      } else if (httpCode == HTTP_CODE_NOT_FOUND) {
        // User with specified ID not found - error https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h
        payload = https.getString();
        Serial.println(payload);
        if (payload == "\"USER_NOT_FOUND\"") {
          drawApiStandardError(1);
        } else if (payload == "\"PRODUCT_NOT_FOUND\"") {
          drawApiStandardError(2);
        } else {
          // STOCK_NOT_FOUND
          drawApiStandardError(3);
        }
        delay(2000);
        resetState();
      } else {
        // Either bad secret or different error - error https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h
        drawTerminatingError();
        delay(2000);
        resetState();
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      // Received HTTP Error Code https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h
      drawTerminatingError();
      delay(2000);
      resetState();
    }
    https.end();
    return payload;
  } else {
    // Error making HTTP request
    drawTerminatingError();
    delay(2000);
    resetState();
  }
}
