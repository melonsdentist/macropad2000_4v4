# macropad2000_4v4

ESP32C3 based macropad intended for use with Krita

4 clickable encoders, 4 buttons

<img src="https://github.com/melonsdentist/macropad2000_4v4/assets/52012382/91053c6c-13a9-45d1-a5ea-b423da43d9c0" height="200">
<img src="https://github.com/melonsdentist/macropad2000_4v4/assets/52012382/58c303aa-8916-4819-992c-d7d99c09f230" height="200">

Known issues:

- need to update local lib file BLEKeyboard.cpp in order to get it working on relaunches (found it somewhere online)
```
BLESecurity* pSecurity = new BLESecurity();
// pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
```
- sometimes BLE does not load on relaunch with particular positions of encoder #2 or #3 , need to rotate knob a little and relaunch the device, probably has something to do with ESP32 internal pin wiring
