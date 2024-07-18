#if ARDUINO_USB_CDC_ON_BOOT
  #define S3USBSerial Serial
#else
  #if ARDUINO_USB_MODE
    #define S3USBSerial USBSerial
  #else
    #error "Please, board settings -> USB CDC On Boot=Enabled"
  #endif
#endif

#include <stdlib.h>
#include "Wire.h"

//I2C
#define PIN_SDA 13
#define PIN_SCL 15
#define I2C_DEV_ADDR 0x58

//HPDL1414
#define PIN_CA1   39
#define PIN_CA2    3
#define PIN_CA3   42
#define PIN_CA4    0   //44  
#define PIN_CA5    4
#define PIN_CA6    5
#define PIN_CA7    6
#define PIN_ANDE  40
#define PIN_AN1   41
#define PIN_AN2   46
#define PIN_AN3    7
#define PIN_AN4    1      
#define PIN_AN5    2


uint32_t i = 0;

void onRequest() {
  Wire.print(i++);
  Wire.print(" Packets.");
  S3USBSerial.println("onRequest");
}


void onReceive(int len) {
  S3USBSerial.println("");  // 改行
  S3USBSerial.println("onReceive called");  // デバッグメッセージ
  S3USBSerial.printf("onReceive[%d]: ", len);

  char com[3] = {0};  // 2文字を格納するバッファ（+1は終端用）
  char buffer[33] = {0};  // 残りのデータを格納するバッファ（+1は終端用）
  int comIndex = 0;
  int bufferIndex = 0;
  int totalBytesRead = 0;

  while (Wire.available()) {
    char c = Wire.read();
    totalBytesRead++;
    
    if (totalBytesRead == 1) {
      // 最初の1文字を捨てる
      continue;
    } else if (totalBytesRead >= 2 && totalBytesRead <= 3) {
      // 2文字目から3文字目をcomに格納
      if (c >= 32 && c <= 126) {  // 表示可能なASCII文字のみをバッファに格納
        com[comIndex++] = c;
      }
    } else if (bufferIndex < sizeof(buffer) - 1) {
      // 残りのデータをbufferに格納
      if (c >= 32 && c <= 126) {  // 表示可能なASCII文字のみをバッファに格納
        buffer[bufferIndex++] = c;
      }
    } else {
      // バッファサイズを超えたデータを無視（必要に応じてデバッグ出力可能）
      S3USBSerial.print("*");  // 超過データを視覚的に確認するためのデバッグ出力
    }
  }

  //com[comIndex] = '\0';  // comバッファの終端を設定
  //buffer[bufferIndex] = '\0';  // bufferバッファの終端を設定

  // 受信したデータを表示
  S3USBSerial.print("Command: ");
  S3USBSerial.println(com);
  S3USBSerial.print("Data: ");
  S3USBSerial.println(buffer);
  
  // comの内容に応じて関数を呼び出す
  if (strcmp(com, "S:") == 0) {
    //scrollDisplay(buffer);
  } else if (strcmp(com, "F:") == 0) {
    //fixedDisplay(buffer);
  } else if (strcmp(com, "D:") == 0) {
    //setScrollDelay(buffer);
  }
}


void setup() {
  S3USBSerial.begin(115200);
  while (!S3USBSerial) {
    ; // シリアルポートが利用可能になるまで待機
  }

  setupI2CSlaveDevice();


  // 各ピンを出力モードに設定
  pinMode(PIN_CA1, OUTPUT);
  pinMode(PIN_CA2, OUTPUT);
  pinMode(PIN_CA3, OUTPUT);
  pinMode(PIN_CA4, OUTPUT);
  pinMode(PIN_CA5, OUTPUT);
  pinMode(PIN_CA6, OUTPUT);
  pinMode(PIN_CA7, OUTPUT);
  pinMode(PIN_ANDE, OUTPUT);
  pinMode(PIN_AN1, OUTPUT);
  pinMode(PIN_AN2, OUTPUT);
  pinMode(PIN_AN3, OUTPUT);
  pinMode(PIN_AN4, OUTPUT);
  pinMode(PIN_AN5, OUTPUT);

    // 各ピンにデジタルで1を出力
  digitalWrite(PIN_CA1,   1);
  digitalWrite(PIN_CA2,   1);
  digitalWrite(PIN_CA3,   1);
  digitalWrite(PIN_CA4,   1);
  digitalWrite(PIN_CA5,   1);
  digitalWrite(PIN_CA6,   1);
  digitalWrite(PIN_CA7,   1);
  digitalWrite(PIN_ANDE,  1);
  digitalWrite(PIN_AN1,   1);
  digitalWrite(PIN_AN2,   1);
  digitalWrite(PIN_AN3,   1);
  digitalWrite(PIN_AN4,   1);
  digitalWrite(PIN_AN5,   1);

  for (i =0; i< 1000; i++)
  {
    digitalWrite(PIN_CA7,   1);
    digitalWrite(PIN_CA1,   0);
    delay(1000);
    digitalWrite(PIN_CA1,   1);
    digitalWrite(PIN_CA2,   0);
    delay(1000);
    digitalWrite(PIN_CA2,   1);
    digitalWrite(PIN_CA3,   0);
    delay(1000);
    digitalWrite(PIN_CA3,   1);
    digitalWrite(PIN_CA4,   0);
    delay(1000);
    digitalWrite(PIN_CA4,   1);
    digitalWrite(PIN_CA5,   0);
    delay(1000);
    digitalWrite(PIN_CA5,   1);
    digitalWrite(PIN_CA6,   0);
    delay(1000);
    digitalWrite(PIN_CA6,   1);
    digitalWrite(PIN_CA7,   0);
    delay(1000);
    //digitalWrite(PIN_CA7,   1);
    //digitalWrite(PIN_CA1,   0);
    //delay(10);
  }  



}

void loop() {
  S3USBSerial.print(".");
  delay(1000);
}

void setupI2CSlaveDevice(){

  S3USBSerial.println("Setup starting...");
  S3USBSerial.print("Start as I2C 0x");
  S3USBSerial.println(I2C_DEV_ADDR, HEX);
  
  Wire.setPins(PIN_SDA, PIN_SCL);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)I2C_DEV_ADDR);

  S3USBSerial.println("I2C initialized");
}



