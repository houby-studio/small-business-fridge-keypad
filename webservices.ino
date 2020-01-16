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
        drawCustomerNotFound();
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
