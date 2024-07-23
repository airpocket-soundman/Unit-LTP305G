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
#include <map>
#include <vector>
#include <string>
#include <iostream>

// 型定義
using Matrix = std::vector<std::vector<int>>;

// I2C
#define PIN_SDA 13
#define PIN_SCL 15
#define I2C_DEV_ADDR 0x59

// LEDマトリクス制御ピン
#define PIN_CA1    0
#define PIN_CA2    3
#define PIN_CA3   42
#define PIN_CA4   39
#define PIN_CA5    4
#define PIN_CA6    5
#define PIN_CA7    6
#define PIN_AN1   41
#define PIN_AN2   46
#define PIN_AN3    7
#define PIN_AN4    1      
#define PIN_AN5    2

const int NUM_ROWS = 5;
const int NUM_COLS = 4;
const int NUM_TOTAL_COLS = 7;

const int rowPins[NUM_ROWS] = {PIN_AN1, PIN_AN2, PIN_AN3, PIN_AN4, PIN_AN5};
const int totalColPins[NUM_TOTAL_COLS] = {PIN_CA1, PIN_CA2, PIN_CA3, PIN_CA4, PIN_CA5, PIN_CA6, PIN_CA7};


// グローバル変数としてマトリクスを宣言
Matrix m00, m01, m02, m03, m04, m05, m06, m07, m08, m09, m10, m11, m12, m13, m14, m15, m16, m17, m18, m19, m20, m21, m22, m23, m24, m25, m26, m27, m28, m29, m30, m31, m32, m33, m34,  m35, m36, m37, m38, m39, m40;
Matrix m100, m101, m102, m103, m104, m105, m106, m107, m108, m109, m110, m111, m112, m113, m114, m115, m116, m117 ,m118, m119, m120, m121, m122, m123, m124, m125, m126, m127, m128, m129, m130, m131, m132, m133, m134, m135, m136, m137, m138, m139, m140;
Matrix s00, s01, s02, s03, s04;

volatile bool displayRunning = false;

void displayAlphabet(const Matrix& matrix, int repeatCount = -1);
void stopDisplay();
void setupI2CSlaveDevice();
void slideText(const std::string& text);
void slideAlphabet(const Matrix& matrix);
void setMatrix(const Matrix& matrix, int repeatCount);
void initializeMatrices();
void displayAnimation(int num);

std::map<char, Matrix> alphabet = {
    {'A', {
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 1, 1, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1}
    }},
    {'B', {
        {1, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 1, 1, 0}
    }},
    {'C', {
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 0, 0, 0},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    }},
    {'D', {
        {1, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 1, 1, 0}
    }},
    {'E', {
        {1, 1, 1, 1},
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {1, 0, 0, 0},
        {1, 1, 1, 1}
    }},
    {'F', {
        {1, 1, 1, 1},
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 0}
    }},
    {'G', {
        {0, 1, 1, 1},
        {1, 0, 0, 0},
        {1, 0, 1, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    }},
    {'H', {
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 1, 1, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1}
    }},
    {'I', {
        {1, 1, 1},
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0},
        {1, 1, 1}
    }},
    {'J', {
        {0, 1, 1, 1},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {1, 0, 1, 0},
        {0, 1, 0, 0}
    }},
    {'K', {
        {1, 0, 0, 1},
        {1, 0, 1, 0},
        {1, 1, 0, 0},
        {1, 0, 1, 0},
        {1, 0, 0, 1}
    }},
    {'L', {
        {1, 0, 0, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 0},
        {1, 1, 1, 1}
    }},
    {'M', {
        {1, 0, 0, 0, 1},
        {1, 1, 0, 1, 1},
        {1, 1, 0, 1, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1}
    }},
    {'N', {
        {1, 0, 0, 1},
        {1, 1, 0, 1},
        {1, 0, 1, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1}
    }},
    {'O', {
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    }},
    {'P', {
        {1, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 1, 1, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 0}
    }},
    {'Q', {
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 1, 0},
        {0, 1, 0, 1}
    }},
    {'R', {
        {1, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 1, 1, 0},
        {1, 0, 1, 0},
        {1, 0, 0, 1}
    }},
    {'S', {
        {0, 1, 1, 1},
        {1, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 1},
        {1, 1, 1, 0}
    }},
    {'T', {
        {1, 1, 1, 1, 1},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0}
    }},
    {'U', {
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    }},
    {'V', {
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0}
    }},
    {'W', {
        {1, 0, 0, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0}
    }},
    {'X', {
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 0, 0, 1}
    }},
    {'Y', {
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0}
    }},
    {'Z', {
        {1, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 1, 1, 0},
        {1, 0, 0, 0},
        {1, 1, 1, 1}
    }},
    {'0', {
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    }},
    {'1', {
        {0, 1},
        {1, 1},
        {0, 1},
        {0, 1},
        {0, 1}
    }},
    {'2', {
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 1, 0, 0},
        {1, 1, 1, 1}
    }},
    {'3', {
        {1, 1, 1, 0},
        {0, 0, 0, 1},
        {0, 1, 1, 0},
        {0, 0, 0, 1},
        {1, 1, 1, 0}
    }},
    {'4', {
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {1, 0, 1, 0},
        {1, 1, 1, 1},
        {0, 0, 1, 0}
    }},
    {'5', {
        {1, 1, 1, 1},
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 1},
        {1, 1, 1, 0}
    }},
    {'6', {
        {0, 1, 1, 0},
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    }},
    {'7', {
        {1, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0}
    }},
    {'8', {
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    }},
    {'9', {
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 1, 1, 0}
    }},
    {' ', {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    }},

    {'!', {
        {1},
        {1},
        {1},
        {0},
        {1}
    }},
    {'"', {
        {1, 0, 1},
        {1, 0, 1},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    }},
    {'#', {
        {0, 1, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 0, 1, 0}
    }},
    {'$', {
        {0, 1, 1, 1, 0},
        {1, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 1},
        {0, 1, 1, 1, 0}
    }},
    {'%', {
        {1, 0, 1},
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 0},
        {1, 0, 1}
    }},
    {'&', {
        {0, 1, 0, 0},
        {1, 0, 1, 0},
        {0, 1, 0, 1},
        {1, 0, 1, 0},
        {1, 1, 0, 1}
    }},
    
    {'\'', {
        {1},
        {1},
        {0},
        {0},
        {0}
    }},
    
    {'(', {
        {0, 1},
        {1, 0},
        {1, 0},
        {1, 0},
        {0, 1}
    }},

    
    {')', {
        {1, 0},
        {0, 1},
        {0, 1},
        {0, 1},
        {1, 0}
    }},
    {'*', {
        {0, 0, 1, 0, 0},
        {1, 0, 1, 0, 1},
        {0, 1, 1, 1, 0},
        {1, 0, 1, 0, 1},
        {0, 0, 1, 0, 0}
    }},
    {'+', {
        {0, 0, 0},
        {0, 1, 0},
        {1, 1, 1},
        {0, 1, 0},
        {0, 0, 0}
    }},
    
    {',', {
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 1},
        {1, 0}
    }},
    {'-', {
        {0, 0, 0},
        {0, 0, 0},
        {1, 1, 1},
        {0, 0, 0},
        {0, 0, 0}
    }},
    {'.', {
        {0},
        {0},
        {0},
        {0},
        {1}
    }},
    {'/', {
        {0, 0, 1},
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 0},
        {1, 0, 0}
    }},
    {':', {
        {0},
        {1},
        {0},
        {1},
        {0}
    }},
    {';', {
        {0},
        {1},
        {0},
        {1},
        {1}
    }},
    {'<', {
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    }},
    {'>', {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 0}
    }},
    {'=', {
        {0, 0, 0},
        {1, 1, 1},
        {0, 0, 0},
        {1, 1, 1},
        {0, 0, 0}
    }},
    {'?', {
        {1, 1, 0},
        {0, 0, 1},
        {0, 1, 0},
        {0, 0, 0},
        {0, 1, 0}
    }},
    {'@', {
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 1, 1, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 1}
    }},
    {'[', {
        {1, 1},
        {1, 0},
        {1, 0},
        {1, 0},
        {1, 1}
    }},{']', {
        {1, 1},
        {0, 1},
        {0, 1},
        {0, 1},
        {1, 1}
    }},{'\\', {
        {1, 0, 0},
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {0, 0, 1}
    }},{'^', {
        {0, 1, 0},
        {1, 0, 1},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    }},{'_', {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {1, 1, 1}
    }},{'`', {
        {1, 0},
        {0, 1},
        {0, 0},
        {0, 0},
        {0, 0}
    }},{'{', {
        {0, 1, 1},
        {0, 1, 0},
        {1, 1, 0},
        {0, 1, 0},
        {0, 1, 1}
    }},{'}', {
        {1, 1, 0},
        {0, 1, 0},
        {0, 1, 1},
        {0, 1, 0},
        {1, 1, 0}
    }},{'~', {
        {0, 1, 0, 0, 0},
        {1, 0, 1, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    }},
    
    // 他のアルファベットも同様に定義する
};


std::map<char, Matrix> alphabetL = {
    {'A', {
        {0, 0, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 0},
        {0, 1, 0, 1, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1}
    }},
    {'B', {
        {0, 1, 1, 0, 1, 1, 0},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'C', {
        {0, 1, 0, 0, 0, 1, 0},
        {1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 0}
    }},
    {'D', {
        {0, 1, 1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'E', {
        {1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'F', {
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 1, 0, 0, 0}, 
        {1, 0, 0, 1, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'G', {
        {0, 1, 0, 1, 1, 1, 0},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 0}
    }},
    {'H', {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'I', {
        {1, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1}
    }},
    {'J', {
        {1, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 1, 0}
    }},
    {'K', {
        {1, 0, 0, 0, 0, 0, 1},
        {0, 1, 1, 0, 1, 1, 0},
        {0, 0, 0, 1, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'L', {
        {0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'M', {
        {1, 1, 1, 1, 1, 1, 1},
        {0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0},
        {0, 0, 1, 1, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'N', {
        {1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1, 1, 0},
        {0, 0, 1, 1, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'O', {
        {0, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1},
        {0, 1, 1, 1, 1, 1, 0}
    }},
    {'P', {
        {0, 1, 1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 1, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'Q', {
        {0, 1, 1, 1, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 1},
        {0, 1, 1, 1, 1, 1, 0}
    }},
    {'R', {
        {0, 1, 1, 0, 0, 1, 1},
        {1, 0, 0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}
    }},
    {'S', {
        {0, 1, 0, 0, 1, 1, 0},
        {1, 0, 0, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 1, 0, 0, 1},
        {0, 1, 1, 0, 0, 1, 0}
    }},
    {'T', {
        {1, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0}
    }},
    {'U', {
        {1, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 0}
    }},
    {'V', {
        {1, 1, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 1, 1, 1},
        {0, 0, 0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 0, 0}
    }},
    {'W', {
        {1, 1, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 1, 1},
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 1, 1},
        {1, 1, 1, 1, 1, 0, 0}
    }},
    {'X', {
        {1, 1, 0, 0, 0, 1, 1},
        {1, 1, 1, 0, 1, 0, 0},
        {0, 0, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1},
        {1, 1, 0, 0, 1, 1, 1}
    }},
    {'Y', {
        {1, 1, 1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 1, 1, 1, 1},
        {1, 1, 1, 0, 0, 0, 0}
    }},
    {'Z', {
        {1, 1, 0, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 1, 1}
    }},
    {'0', {
        {0, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {0, 1, 1, 1, 1, 1, 0}
    }},
    {'1', {
        {0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 1, 0, 0, 0, 0, 1}
    }},
    {'2', {
        {0, 1, 1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 0, 1},
        {1, 0, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 1, 1},
        {0, 1, 0, 0, 0, 1, 1}
    }},
    {'3', {
        {0, 1, 1, 0, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {0, 1, 0, 0, 0, 1, 0}
    }},
    {'4', {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 1, 0, 0, 1, 0, 0},
        {0, 0, 1, 0, 1, 0, 0},
        {0, 0, 0, 1, 1, 0, 0}
    }},
    {'5', {
        {1, 0, 0, 0, 1, 1, 0},
        {1, 0, 0, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 1, 0, 0, 1},
        {1, 1, 1, 1, 0, 0, 1}
    }},
    {'6', {
        {0, 1, 0, 0, 1, 1, 0},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 0}
    }},
    {'7', {
        {1, 1, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0, 0, 0},
        {1, 0, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0}
    }},
    {'8', {
        {0, 1, 1, 0, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {0, 1, 1, 0, 1, 1, 0}
    }},
    {'9', {
        {0, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1},
        {0, 1, 1, 0, 0, 1, 0}
    }},
    {' ', {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    }},
};


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
  char buffer[125] = {0};  // 残りのデータを格納するバッファ（+1は終端用）
  int comIndex = 0;
  int bufferIndex = 0;
  int totalBytesRead = 0;

  stopDisplay();  // 既存の表示を停止

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

  // 受信したデータを表示
  S3USBSerial.print("Command: ");
  S3USBSerial.println(com);
  S3USBSerial.print("Data: ");
  S3USBSerial.println(buffer);
  
  // comの内容に応じて関数を呼び出す
  if (strcmp(com, "S:") == 0) {
    slideText(buffer);
  } else if (strcmp(com, "F:") == 0) {
    // bufferの先頭の一文字を固定表示
    if (buffer[0] != '\0' && alphabetL.find(buffer[0]) != alphabetL.end()) {
      displayAlphabetV(alphabetL[buffer[0]],80);
    }
    // bufferをクリア
    memset(buffer, 0, sizeof(buffer));
  } else if (strcmp(com, "A:") == 0) {
    // bufferの先頭の一文字を固定表示
    int animationIndex = static_cast<int>(buffer[0] - '0'); // buffer[0]をint型に変換
    displayAnimation(animationIndex);
    memset(buffer, 0, sizeof(buffer));
  }
}

void setup() {
    S3USBSerial.begin(115200);

    setupI2CSlaveDevice();

    // 各ピンを出力モードに設定
    for (int i = 0; i < NUM_ROWS; i++) {
        pinMode(rowPins[i], OUTPUT);
    }
    for (int i = 0; i < NUM_TOTAL_COLS; i++) {
        pinMode(totalColPins[i], OUTPUT);
    }

    // 初期状態として全てのカソードをHIGHに設定
    for (int i = 0; i < NUM_TOTAL_COLS; i++) {
        digitalWrite(totalColPins[i], HIGH);
    }
      // マトリクスパターンを初期化
    initializeMatrics();
    delay(100);


    /*
    digitalWrite(PIN_AN1, HIGH);
    digitalWrite(PIN_AN2, HIGH);
    digitalWrite(PIN_AN3, HIGH);
    digitalWrite(PIN_AN4, HIGH);
    digitalWrite(PIN_AN5, HIGH);
    digitalWrite(PIN_CA1, LOW);
    digitalWrite(PIN_CA2, LOW);
    digitalWrite(PIN_CA3, LOW);

    digitalWrite(PIN_CA4, LOW);
    digitalWrite(PIN_CA5, LOW);
    digitalWrite(PIN_CA6, LOW);
    digitalWrite(PIN_CA7, LOW);
    */
    

    S3USBSerial.println("displayAnimation");
    // マトリクスを表示（繰り返し回数を指定）
    for (int i = 0; i < 1; i++) {
        displayAnimation(4);
    }

    S3USBSerial.println("slideAlphabet");
    //slideAlphabet(alphabet['A']);



    // アルファベットの表示
    S3USBSerial.println("displayAlphabet");
    displayAlphabetV(alphabetL['M'], 80);
    displayAlphabetV(alphabetL['5'], 80);
    displayAlphabetV(alphabetL['S'], 80);
    displayAlphabetV(alphabetL['T'], 80);
    displayAlphabetV(alphabetL['A'], 80);
    displayAlphabetV(alphabetL['C'], 80);
    displayAlphabetV(alphabetL['K'], 80);

    S3USBSerial.println("slideText");
    slideText("M5STACK JAPAN CREATIVITY CONTEST 2024");
    

}

void initializeMatrics() {
  m00 = {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0}
  };
  m01 = {
      {0, 0, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 1}
  };
  m02 = {
      {0, 0, 0, 0, 0, 1, 1},
      {0, 0, 0, 0, 0, 1, 1},
      {0, 0, 0, 0, 0, 1, 1},
      {0, 0, 0, 0, 0, 1, 1},
      {0, 0, 0, 0, 0, 1, 1}
  };
  m03 = {
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 1, 1, 1}
  };
  m04 = {
      {0, 0, 0, 1, 1, 1, 1},
      {0, 0, 0, 1, 1, 1, 1},
      {0, 0, 0, 1, 1, 1, 1},
      {0, 0, 0, 1, 1, 1, 1},
      {0, 0, 0, 1, 1, 1, 1}
  };
  m05 = {
      {0, 0, 1, 1, 1, 1, 1},
      {0, 0, 1, 1, 1, 1, 1},
      {0, 0, 1, 1, 1, 1, 1},
      {0, 0, 1, 1, 1, 1, 1},
      {0, 0, 1, 1, 1, 1, 1}
  };
  m06 = {
      {0, 1, 1, 1, 1, 1, 1},
      {0, 1, 1, 1, 1, 1, 1},
      {0, 1, 1, 1, 1, 1, 1},
      {0, 1, 1, 1, 1, 1, 1},
      {0, 1, 1, 1, 1, 1, 1}
  };
  m07 = {
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1}
  };
  m08 = {
      {1, 1, 1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0}
  };
  m09 = {
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0}
  };
  m10 = {
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0}
  };
  m11 = {
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0, 0, 0}
  };
  m12 = {
      {1, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 0, 0, 0, 0}
  };
  m13 = {
      {1, 1, 0, 0, 0, 0, 0},
      {1, 1, 0, 0, 0, 0, 0},
      {1, 1, 0, 0, 0, 0, 0},
      {1, 1, 0, 0, 0, 0, 0},
      {1, 1, 0, 0, 0, 0, 0}
  };
  m14 = {
      {1, 1, 1, 0, 0, 0, 0},
      {1, 1, 1, 0, 0, 0, 0},
      {1, 1, 1, 0, 0, 0, 0},
      {1, 1, 1, 0, 0, 0, 0},
      {1, 1, 1, 0, 0, 0, 0}
  };
  m15 = {
      {1, 1, 1, 1, 0, 0, 0},
      {1, 1, 1, 1, 0, 0, 0},
      {1, 1, 1, 1, 0, 0, 0},
      {1, 1, 1, 1, 0, 0, 0},
      {1, 1, 1, 1, 0, 0, 0}
  };
  m16 = {
      {1, 1, 1, 1, 1, 0, 0},
      {1, 1, 1, 1, 1, 0, 0},
      {1, 1, 1, 1, 1, 0, 0},
      {1, 1, 1, 1, 1, 0, 0},
      {1, 1, 1, 1, 1, 0, 0}
  };
  m17 = {
      {1, 1, 1, 1, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 0}
  };
  m18 = {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {1, 1, 1, 1, 1, 1, 1}
  };
  m19 = {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1}
  };
  m20 = {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1}
  };
  m21 = {
      {0, 0, 0, 0, 0, 0, 0},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
  };

  m100 = {
      {0, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
  };
  m101 = {
      {1, 0, 0, 0, 0, 0, 0},
      {1, 1, 0, 0, 0, 0, 0},
      {0, 1, 0, 0, 0, 0, 0},
      {0, 1, 0, 0, 0, 0, 0},
      {1, 0, 0, 0, 0, 0, 0},
  };
  m102 = {
      {1, 1, 0, 0, 0, 0, 0},
      {1, 1, 1, 0, 0, 0, 0},
      {0, 0, 1, 0, 0, 0, 0},
      {0, 0, 1, 0, 0, 0, 0},
      {0, 1, 0, 0, 0, 0, 0},
  };
  m103 = {
      {0, 1, 1, 0, 0, 0, 0},
      {1, 1, 1, 1, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 0},
      {1, 0, 1, 0, 0, 0, 0},
  };
  m104 = {
      {0, 0, 1, 1, 0, 0, 0},
      {0, 1, 1, 1, 1, 0, 0},
      {0, 1, 0, 0, 1, 0, 0},
      {1, 1, 0, 0, 1, 0, 0},
      {1, 1, 0, 1, 0, 0, 0},
  };
  m105 = {
      {0, 0, 0, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 0},
      {0, 0, 1, 0, 0, 1, 0},
      {1, 1, 1, 0, 0, 1, 0},
      {1, 1, 1, 0, 1, 0, 0},
  };
  m106 = {
      {1, 0, 0, 0, 1, 1, 0},
      {1, 0, 0, 1, 1, 1, 1},
      {1, 0, 0, 1, 0, 0, 1},
      {1, 1, 1, 1, 0, 0, 1},
      {1, 1, 1, 1, 0, 1, 0},
  };
  m107 = {
      {0, 1, 0, 0, 0, 1, 0},
      {0, 1, 0, 0, 1, 1, 1},
      {0, 1, 0, 0, 1, 0, 1},
      {0, 1, 1, 1, 1, 0, 1},
      {0, 1, 1, 1, 1, 0, 1},
  };
  m108 = {
      {0, 0, 1, 0, 0, 1, 0},
      {0, 0, 1, 0, 1, 1, 1},
      {0, 0, 1, 0, 1, 0, 1},
      {0, 0, 1, 1, 1, 0, 1},
      {0, 0, 1, 1, 1, 0, 1},
  };
  m109 = {
      {0, 0, 0, 1, 0, 1, 1},
      {0, 0, 0, 1, 0, 1, 1},
      {0, 0, 0, 1, 0, 1, 1},
      {0, 0, 0, 1, 1, 1, 1},
      {0, 0, 0, 1, 1, 1, 1},
  };
  m110 = {
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 1, 1, 1},
  };
  m111 = {
      {0, 0, 0, 0, 0, 1, 1},
      {0, 0, 0, 0, 0, 1, 1},
      {0, 0, 0, 0, 0, 1, 1},
      {0, 0, 0, 0, 0, 1, 1},
      {0, 0, 0, 0, 0, 1, 1},
  };
  m112 = {
      {0, 0, 0, 0, 1, 1, 0},
      {0, 0, 0, 1, 1, 1, 1},
      {0, 0, 0, 1, 0, 0, 1},
      {1, 1, 1, 1, 0, 0, 1},
      {1, 1, 1, 1, 0, 1, 0},
  };
  m113 = {
      {0, 0, 0, 1, 1, 1, 0},
      {0, 0, 1, 1, 1, 1, 1},
      {0, 0, 1, 0, 0, 0, 1},
      {1, 1, 1, 0, 0, 0, 1},
      {1, 1, 1, 0, 1, 1, 0},
  };
  m114 = {
      {1, 0, 0, 0, 1, 1, 0},
      {1, 0, 0, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 0, 0, 1},
      {1, 1, 1, 1, 0, 1, 0},
  };
  m115 = {
      {1, 0, 0, 0, 1, 1, 0},
      {1, 0, 0, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 0, 0, 1},
      {1, 1, 1, 1, 0, 1, 0},
  };
  m116 = {
      {1, 0, 0, 0, 1, 1, 1},
      {1, 0, 0, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 0, 0, 1},
      {1, 1, 1, 1, 0, 1, 0},
  };
  m117 = {
      {1, 0, 0, 0, 1, 1, 1},
      {1, 0, 0, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 0, 0, 1},
      {1, 1, 1, 1, 0, 1, 0},
  };
  m118 = {
      {1, 0, 0, 1, 1, 1, 1},
      {1, 0, 0, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 0, 0, 1},
      {1, 1, 1, 1, 0, 1, 0},
  };
  m119 = {
      {1, 0, 1, 1, 1, 1, 1},
      {1, 0, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 0, 1},
      {1, 1, 1, 1, 1, 1, 0},
  };
  m120 = {
      {1, 1, 1, 1, 1, 1, 1},
      {1, 0, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 0, 1},
      {1, 1, 1, 1, 1, 1, 0},
  };
  m121 = {
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
  };
  m122 = {
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {0, 0, 0, 1, 0, 0, 0},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
  };

  m123 = {
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 0},
      {0, 0, 0, 1, 0, 0, 0},
      {0, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
  };
  m124 = {
      {1, 1, 1, 1, 1, 1, 0},
      {1, 1, 1, 1, 1, 0, 0},
      {0, 0, 0, 1, 0, 0, 0},
      {0, 0, 1, 1, 1, 1, 1},
      {0, 1, 1, 1, 1, 1, 1},
  };
  m125 = {
      {1, 1, 1, 1, 1, 0, 0},
      {1, 1, 1, 1, 0, 0, 0},
      {0, 0, 0, 1, 0, 0, 0},
      {0, 0, 0, 1, 1, 1, 1},
      {0, 0, 1, 1, 1, 1, 1},
  };
  m126 = {
      {1, 1, 1, 1, 0, 0, 0},
      {1, 1, 1, 1, 0, 0, 0},
      {0, 0, 0, 1, 0, 0, 0},
      {0, 0, 0, 1, 1, 1, 1},
      {0, 0, 0, 1, 1, 1, 1},
  };
  m127 = {
      {1, 1, 1, 0, 0, 0, 0},
      {1, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 1, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 1, 1, 1},
  };
  m128 = {
      {1, 1, 0, 0, 0, 0, 0},
      {1, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 1, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 0, 1, 1},
  };
  m129 = {
      {1, 0, 0, 0, 0, 0, 0},
      {1, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 1, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 0, 0, 1},
  };
  m130 = {
      {0, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 1, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0},
  };
  m131 = {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
  };






  s01 = {
    {0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
  };

  s02 = {
    {0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0},
  };

  s03 = {
    {0, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0},
    {0, 0, 1, 1, 1, 0, 0},
  };

  s04 = {
    {0, 1, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0},
  };
}



void loop() {
    // 特定の時間ごとにディスプレイを更新するなどの処理
}

void setupI2CSlaveDevice() {
    S3USBSerial.println("Setup starting...");
    S3USBSerial.print("Start as I2C 0x");
    S3USBSerial.println(I2C_DEV_ADDR, HEX);
  
    Wire.setPins(PIN_SDA, PIN_SCL);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Wire.begin((uint8_t)I2C_DEV_ADDR);

    S3USBSerial.println("I2C initialized");
}

void displayAlphabet(const Matrix& matrix, int repeatCount) {
    displayRunning = true;
    int numCols = matrix[0].size(); // マトリクスの列数を取得

    if (repeatCount > 0) {
        for (int count = 0; count < repeatCount; count++) {
            for (int col = 0; col < numCols; col++) { // NUM_COLSではなくnumColsを使用
                digitalWrite(totalColPins[col], LOW); // 現在の列をLowに設定
                for (int row = 0; row < NUM_ROWS; row++) {
                    digitalWrite(rowPins[row], matrix[row][col]);
                }
                delay(1);  // 少し待つ
                digitalWrite(totalColPins[col], HIGH); // 列をHighに戻す
            }
        }
    } else {
        while (displayRunning) {
            for (int col = 0; col < numCols; col++) { // NUM_COLSではなくnumColsを使用
                digitalWrite(totalColPins[col], LOW); // 現在の列をLowに設定
                for (int row = 0; row < NUM_ROWS; row++) {
                    digitalWrite(rowPins[row], matrix[row][col]);
                }
                delay(1);  // 少し待つ
                digitalWrite(totalColPins[col], HIGH); // 列をHighに戻す
            }
        }
    }
}

void displayAlphabetV(const Matrix& matrix, int repeatCount) {
    displayRunning = true;
    int numCols = matrix[0].size(); // マトリクスの列数を取得

    if (repeatCount > 0) {
        for (int count = 0; count < repeatCount; count++) {
            for (int col = 0; col < numCols; col++) { // NUM_COLSではなくnumColsを使用
                digitalWrite(totalColPins[col], LOW); // 現在の列をLowに設定
                for (int row = NUM_ROWS - 1; row >= 0; row--) { // 行を逆順に走査
                    digitalWrite(rowPins[row], matrix[NUM_ROWS - 1 - row][col]);
                }
                delay(1);  // 少し待つ
                digitalWrite(totalColPins[col], HIGH); // 列をHighに戻す
            }
        }
    } else {
        while (displayRunning) {
            for (int col = 0; col < numCols; col++) { // NUM_COLSではなくnumColsを使用
                digitalWrite(totalColPins[col], LOW); // 現在の列をLowに設定
                for (int row = NUM_ROWS - 1; row >= 0; row--) { // 行を逆順に走査
                    digitalWrite(rowPins[row], matrix[NUM_ROWS - 1 - row][col]);
                }
                delay(1);  // 少し待つ
                digitalWrite(totalColPins[col], HIGH); // 列をHighに戻す
            }
        }
    }
}

void stopDisplay() {
    displayRunning = false;
}

void slideAlphabet(const Matrix& matrix) {
    // アニメーションが右から左にスライドするように設定
    for (int offset = -NUM_COLS; offset < NUM_TOTAL_COLS; offset++) {
        for (int frame = 0; frame < 20; frame++) { // スライドを滑らかにするためのフレーム
            for (int col = 0; col < NUM_TOTAL_COLS; col++) {
                if (col - offset >= 0 && col - offset < NUM_COLS) {
                    digitalWrite(totalColPins[col], LOW); // 現在の列をLowに設定
                    for (int row = 0; row < NUM_ROWS; row++) {
                        digitalWrite(rowPins[row], matrix[row][col - offset]);
                    }
                } else {
                    digitalWrite(totalColPins[col], LOW); // 空の列を表示
                    for (int row = 0; row < NUM_ROWS; row++) {
                        digitalWrite(rowPins[row], LOW);
                    }
                }
                delay(1); // スライド速度の調整
                digitalWrite(totalColPins[col], HIGH); // 列をHighに戻す
            }
        }
    }
}



void slideText(const std::string& text) {
    Matrix displayMatrix(NUM_ROWS, std::vector<int>());

    // 文字列を行列に変換
    for (char c : text) {
        if (alphabet.find(c) != alphabet.end()) {
            const Matrix& charMatrix = alphabet[c];
            for (int row = 0; row < NUM_ROWS; row++) {
                displayMatrix[row].insert(displayMatrix[row].end(), charMatrix[row].begin(), charMatrix[row].end());
                displayMatrix[row].push_back(0); // 文字間の空白列
            }
        }
    }

    // 各行に7列の空白を先頭に追加
    for (int row = 0; row < NUM_ROWS; row++) {
        displayMatrix[row].insert(displayMatrix[row].begin(), NUM_TOTAL_COLS, 0);
    }

    // 行列をシリアルモニタに表示
    S3USBSerial.println("Generated Matrix:");
    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < displayMatrix[row].size(); col++) {
            S3USBSerial.print(displayMatrix[row][col]);
            S3USBSerial.print(" ");
        }
        S3USBSerial.println();
    }

    // 行列をスライド表示
    int totalCols = displayMatrix[0].size();
    for (int offset = 0; offset < totalCols + NUM_TOTAL_COLS; offset++) {
        for (int frame = 0; frame < 10; frame++) { // スライドを滑らかにするためのフレーム
            for (int col = NUM_TOTAL_COLS - 1; col >= 0; col--) { // 逆順にループ
                int displayCol = offset - col;
                if (displayCol >= 0 && displayCol < totalCols) {
                    digitalWrite(totalColPins[col], LOW); // 現在の列をLowに設定
                    for (int row = 0; row < NUM_ROWS; row++) {
                        digitalWrite(rowPins[row], displayMatrix[row][displayCol]);
                    }
                } else {
                    digitalWrite(totalColPins[col], LOW); // 空の列を表示
                    for (int row = 0; row < NUM_ROWS; row++) {
                        digitalWrite(rowPins[row], LOW);
                    }
                }
                delay(1); // 少し待つ
                digitalWrite(totalColPins[col], HIGH); // 列をHighに戻す
            }
        }
    }
}


void setMatrix(const Matrix& matrix, int repeatCount) {
    if (matrix.size() != NUM_ROWS || matrix[0].size() != NUM_TOTAL_COLS) {
        S3USBSerial.println("Matrix must be 5x7");
        return;
    }

    for (int repeat = 0; repeat < repeatCount; repeat++) {
        for (int col = NUM_TOTAL_COLS - 1; col >= 0; col--) { // 逆順にループ
            digitalWrite(totalColPins[col], LOW); // 現在の列をLowに設定
            for (int row = 0; row < NUM_ROWS; row++) {
                digitalWrite(rowPins[row], matrix[row][NUM_TOTAL_COLS - 1 - col]);
            }
            delay(1); // 少し待つ
            digitalWrite(totalColPins[col], HIGH); // 列をHighに戻す
        }
    }
}

void setMatrixV(const Matrix& matrix, int repeatCount) {
    int numRows = matrix.size();
    int numCols = matrix[0].size();
    Matrix rotatedMatrix(numRows, std::vector<int>(numCols));

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            rotatedMatrix[numRows - 1 - row][numCols - 1 - col] = matrix[row][col];
        }
    }

    if (rotatedMatrix.size() != NUM_ROWS || rotatedMatrix[0].size() != NUM_TOTAL_COLS) {
        S3USBSerial.println("Matrix must be 5x7");
        return;
    }

    for (int repeat = 0; repeat < repeatCount; repeat++) {
        for (int col = NUM_TOTAL_COLS - 1; col >= 0; col--) { // 逆順にループ
            digitalWrite(totalColPins[col], LOW); // 現在の列をLowに設定
            for (int row = 0; row < NUM_ROWS; row++) {
                digitalWrite(rowPins[row], rotatedMatrix[row][NUM_TOTAL_COLS - 1 - col]);
            }
            delay(1); // 少し待つ
            digitalWrite(totalColPins[col], HIGH); // 列をHighに戻す
        }
    }
}


void displayAnimation(int num) {
  if (num == 0)
  {
    setMatrix(m00, 1);
    setMatrix(m01, 5);
    setMatrix(m02, 5);
    setMatrix(m03, 5);
    setMatrix(m04, 5);
    setMatrix(m05, 5);
    setMatrix(m06, 5);
    setMatrix(m07, 30);
    setMatrix(m06, 5);
    setMatrix(m05, 5);
    setMatrix(m04, 5);
    setMatrix(m03, 5);
    setMatrix(m02, 5);
    setMatrix(m01, 5);
    setMatrix(m00, 1);
  }

  else if (num == 1)
  {
    setMatrix(m00, 1);
    setMatrix(m08, 5);
    setMatrix(m09, 5);
    setMatrix(m10, 5);
    setMatrix(m11, 5);
    setMatrix(m07, 30);
    setMatrix(m11, 5);
    setMatrix(m10, 5);
    setMatrix(m09, 5);
    setMatrix(m08, 5);
    setMatrix(m00, 1);
  }

  else if (num == 2)
  {
    setMatrix(m00, 1);
    setMatrix(m12, 5);
    setMatrix(m13, 5);
    setMatrix(m14, 5);
    setMatrix(m15, 5);
    setMatrix(m16, 5);
    setMatrix(m17, 5);
    setMatrix(m07, 30);
    setMatrix(m17, 5);
    setMatrix(m16, 5);
    setMatrix(m15, 5);
    setMatrix(m14, 5);
    setMatrix(m13, 5);
    setMatrix(m12, 5);
    setMatrix(m00, 1);
  }
    else if (num == 3)
  {
    setMatrix(m00, 1);
    setMatrix(m18, 5);
    setMatrix(m19, 5);
    setMatrix(m29, 5);
    setMatrix(m21, 5);
    setMatrix(m07, 30);
    setMatrix(m21, 5);
    setMatrix(m20, 5);
    setMatrix(m19, 5);
    setMatrix(m18, 5);
    setMatrix(m00, 1);
  }
    else if (num == 4)
  {
    setMatrixV(m100, 5);
    setMatrixV(m101, 5);
    setMatrixV(m102, 5);
    setMatrixV(m103, 5);
    setMatrixV(m104, 5);
    setMatrixV(m105, 5);
    setMatrixV(m106, 7);
    setMatrixV(m107, 7);
    setMatrixV(m108, 7);
    setMatrixV(m109, 7);
    setMatrixV(m110, 7);
    setMatrixV(m111, 7);
    setMatrixV(m110, 7);
    setMatrixV(m109, 7);
    setMatrixV(m108, 7);
    setMatrixV(m107, 7);
    setMatrixV(m106, 7);
    setMatrixV(m112, 7);
    setMatrixV(m113, 7);
    setMatrixV(m112, 7);
    setMatrixV(m106, 7);
    setMatrixV(m107, 7);
    setMatrixV(m108, 7);
    setMatrixV(m109, 7);
    setMatrixV(m110, 7);
    setMatrixV(m109, 7);
    setMatrixV(m108, 7);
    setMatrixV(m107, 7);
    setMatrixV(m106, 10);
    setMatrixV(m112, 10);
    setMatrixV(m113, 10);
    setMatrixV(m112, 10);
    setMatrixV(m106, 10);
    setMatrixV(m107, 10);
    setMatrixV(m108, 10);
    setMatrixV(m109, 10);
    setMatrixV(m108, 10);
    setMatrixV(m107, 10);
    setMatrixV(m106, 15);
    setMatrixV(m112, 15);
    setMatrixV(m107, 15);
    setMatrixV(m108, 15);
    setMatrixV(m107, 15);
    setMatrixV(m106, 18);
    setMatrixV(m112, 18);
    setMatrixV(m107, 18);
    setMatrixV(m106, 100);
    setMatrixV(m114, 8);
    setMatrixV(m115, 8);
    setMatrixV(m116, 8);
    setMatrixV(m117, 8);
    setMatrixV(m118, 8);
    setMatrixV(m119, 8);
    setMatrixV(m120, 8);
    setMatrixV(m121, 8);
    setMatrixV(m122, 8);
    setMatrixV(m123, 8);
    setMatrixV(m124, 8);
    setMatrixV(m125, 8);
    setMatrixV(m126, 8);
    setMatrixV(m127, 8);
    setMatrixV(m128, 8);
    setMatrixV(m129, 8);
    setMatrixV(m130, 8);
    setMatrixV(m131, 100);

  }
}
