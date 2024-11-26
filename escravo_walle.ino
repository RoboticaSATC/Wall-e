#include <esp_now.h>
#include <WiFi.h>


const uint8_t ESQ_L_PWM = 15;
const uint8_t ESQ_R_PWM = 13;
const uint8_t DIR_L_PWM = 12;
const uint8_t DIR_R_PWM = 14;

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
PacketData receiverData;

int speed = 0;

void ativa_motores() {
  if (receiverData.switch1Value == 1) {
    ledcWrite(ESQ_L_PWM, 0);
    ledcWrite(ESQ_R_PWM, 0);
    ledcWrite(DIR_L_PWM, 0);
    ledcWrite(DIR_R_PWM, 0);
  } else {
    if ((receiverData.lxAxisValue >= 125) && (receiverData.lxAxisValue <= 129)) {
      if ((receiverData.lyAxisValue >= 125) && (receiverData.lyAxisValue <= 129)) {
        ledcWrite(ESQ_L_PWM, 0);
        ledcWrite(ESQ_R_PWM, 0);
        ledcWrite(DIR_L_PWM, 0);
        ledcWrite(DIR_R_PWM, 0);
      } else if (receiverData.lyAxisValue > 129) {
        //esquerda tanque
        speed = map(receiverData.lyAxisValue, 130, 254, 0, 255);
        ledcWrite(ESQ_L_PWM, speed);
        ledcWrite(ESQ_R_PWM, 0);
        ledcWrite(DIR_L_PWM, 0);
        ledcWrite(DIR_R_PWM, speed);
      } else if (receiverData.lyAxisValue < 125) {
        //direita tanque
        speed = map(receiverData.lyAxisValue, 0, 124, 255, 0);
        ledcWrite(ESQ_L_PWM, 0);
        ledcWrite(ESQ_R_PWM, speed);
        ledcWrite(DIR_L_PWM, speed);
        ledcWrite(DIR_R_PWM, 0);
      }
    } else if (receiverData.lxAxisValue > 129) {
      speed = map(receiverData.lxAxisValue, 130, 254, 0, 255);
      if ((receiverData.lyAxisValue >= 125) && (receiverData.lyAxisValue <= 129)) {
        //frente
        ledcWrite(1, speed);
        ledcWrite(2, 0);
        ledcWrite(3, speed);
        ledcWrite(4, 0);
      } else if (receiverData.lyAxisValue > 129) {
        //frente esquerda
        ledcWrite(ESQ_L_PWM, speed);
        ledcWrite(ESQ_R_PWM, 0);
        speed = speed - map(receiverData.lyAxisValue, 130, 254, 0, 100);
        ledcWrite(DIR_L_PWM, speed);
        ledcWrite(DIR_R_PWM, 0);
      } else if (receiverData.lyAxisValue < 125) {
        //frente direita
        ledcWrite(DIR_L_PWM, speed);
        ledcWrite(DIR_R_PWM, 0);
        speed = speed - map(receiverData.lyAxisValue, 0, 124, 100, 0);
        ledcWrite(ESQ_L_PWM, speed);
        ledcWrite(ESQ_R_PWM, 0);
      }
    } else if (receiverData.lxAxisValue < 125) {
      speed = map(receiverData.lxAxisValue, 0, 124, 255, 0);
      if ((receiverData.lyAxisValue >= 125) && (receiverData.lyAxisValue <= 129)) {
        //tras
        ledcWrite(ESQ_L_PWM, 0);
        ledcWrite(ESQ_R_PWM, speed);
        ledcWrite(DIR_L_PWM, 0);
        ledcWrite(DIR_R_PWM, speed);
      } else if (receiverData.lyAxisValue > 129) {
        //tras esquerda
        ledcWrite(ESQ_L_PWM, 0);
        ledcWrite(ESQ_R_PWM, speed);
        speed = speed - map(receiverData.lyAxisValue, 130, 254, 0, 100);
        ledcWrite(DIR_L_PWM, 0);
        ledcWrite(DIR_R_PWM, speed);
      } else if (receiverData.lyAxisValue < 125) {
        //tras direita
        ledcWrite(DIR_L_PWM, 0);
        ledcWrite(DIR_R_PWM, speed);
        speed = speed - map(receiverData.lyAxisValue, 0, 124, 100, 0);
        ledcWrite(ESQ_L_PWM, 0);
        ledcWrite(ESQ_R_PWM, speed);
      }
    }
  }
  Serial.println(speed);
}


// callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&receiverData, incomingData, sizeof(receiverData));
  ativa_motores();
  char inputValuesString[100];
    Serial.println("Dados recebidos:");
  sprintf(inputValuesString,
          "%3d,%3d,%3d",
          receiverData.lxAxisValue,
          receiverData.lyAxisValue,
          //receiverData.rxAxisValue,
          //receiverData.ryAxisValue,
          receiverData.switch1Value
          //receiverData.switch2Value,
          //receiverData.switch3Value,
          //receiverData.switch4Value,
          //receiverData.switch5Value,
          //receiverData.switch6Value
  );
  Serial.println(inputValuesString);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ledcAttach(ESQ_L_PWM, 5000, 8);
  ledcAttach(ESQ_R_PWM, 5000, 8);
  ledcAttach(DIR_L_PWM, 5000, 8);
  ledcAttach(DIR_R_PWM, 5000, 8);

  //ledcAttachPin(ESQ_L_PWM, 1);
  //ledcAttachPin(ESQ_R_PWM, 2);
  //ledcAttachPin(DIR_L_PWM, 3);
  //ledcAttachPin(DIR_R_PWM, 4);


  //ledcSetup(1, 5000, 8);
  //ledcSetup(2, 5000, 8);
  //ledcSetup(3, 5000, 8);
  //ledcSetup(4, 5000, 8);


  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}



void loop() {
}
