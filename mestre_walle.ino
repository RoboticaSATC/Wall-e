#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t robo[] = {0xA0, 0xB7, 0x65, 0x5A, 0x15, 0x1C};  //A0:B7:65:5A:15:1C

struct PacketData {
  byte lxAxisValue;
  byte lyAxisValue;
  //byte rxAxisValue;
  //byte ryAxisValue;
  byte switch1Value;
  //byte switch2Value;
  //byte switch3Value;
  //byte switch4Value;
  //byte switch5Value;
  //byte switch6Value;
};
PacketData data;

esp_now_peer_info_t peerInfo;

//This function is used to map 0-4095 joystick value to 0-254. hence 127 is the center value which we send.
//It also adjust the deadband in joystick.
//Jotstick values range from 0-4095. But its center value is not always 2047. It is little different.
//So we need to add some deadband to center value. in our case 1800-2200. Any value in this deadband range is mapped to center 127.
int mapAndAdjustJoystickDeadBandValues(int value, bool reverse) {
  if (value >= 2200) {
    value = map(value, 2200, 4095, 127, 254);
  } else if (value <= 1800) {
    value = (value == 0 ? 0 : map(value, 1800, 0, 127, 0));
  } else {
    value = 127;
  }

  if (reverse) {
    value = 254 - value;
  }
  Serial.println(value);
  return value;
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("Succes: Initialized ESP-NOW");
  }

  esp_now_register_send_cb(OnDataSent);

  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  memcpy(peerInfo.peer_addr, robo, 6);


  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  delay(10);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
}

void loop() {
  data.lxAxisValue = mapAndAdjustJoystickDeadBandValues(analogRead(36), false);
  data.lyAxisValue = mapAndAdjustJoystickDeadBandValues(analogRead(39), false);
  //data.rxAxisValue = !digitalRead(25);
  //data.ryAxisValue = !digitalRead(26);
  data.switch1Value = !digitalRead(16);
  //data.switch2Value = !digitalRead(5);
  //data.switch3Value = !digitalRead(12);
  //data.switch4Value = !digitalRead(13);
  //data.switch5Value = !digitalRead(14);
  //data.switch6Value = !digitalRead(15);
  

  esp_err_t result = esp_now_send(robo, (uint8_t *)&data, sizeof(data));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  delay(50);
}
