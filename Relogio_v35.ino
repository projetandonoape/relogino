#include <SPI.h>
#include <DS3231.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_BME280.h>
#include <MD_MAX72xx.h>
#include "MD_EyePair.h"
#include "Font_Data.h"
#include "Sprits.h"

#define	MAX_DEVICES	12
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define	CS_PIN 10  // or SS

uint8_t hora, minuto, segundo;

Adafruit_BME280 sensor_bme;
DS3231 Mod_relogio;
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#define CHAR_SPACING    1   // pixels between characters
#define ANIMATION_DELAY 60  // milliseconds
#define MAX_FRAMES      4   // number of animation frames
#define DELAYTIME       25  // in milliseconds
#define scrollDelay     30  // in milliseconds
#define BUF_SIZE        75
#define MAX_BRILHO      15

//uint8_t newMessage[BUF_SIZE+1] = { 'C', 'a', 's', 'a', ' ', 'd', 'a', ' ', 'V', 162, ' ', 'M', 'a', 'r', 'y', ' ', 3, 3, 3};
uint8_t newMessage[BUF_SIZE+1] = {3};
uint8_t indexChr = 0, vez = 0;
bool newMessageAvailable = true; //por false
uint8_t efeitoMsgCampo;

#define  MAX_EYE_PAIR  (MAX_DEVICES/2)
MD_EyePair E[MAX_EYE_PAIR];
#define  DELAYTIME_EYES  400  // in milliseconds

//const uint8_t DATA_WIDTH = (sizeof(pacman[0])/sizeof(pacman[0][0]));

byte brilho = 9;
bool brilho_auto = true;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!sensor_bme.begin(0x76)) sensor_bme.begin(0x77);
  brilho = EEPROM.read(0);
  brilho_auto = EEPROM.read(1);
  mx.begin();
  
  mx.clear();
  mx.begin();
  mx.setFont(numeric7Seg);
  relogio(0);
  randomSeed(segundo * minuto);
}

void loop() {
  uint8_t efeito = 0;
  static uint8_t frase[13];
  
  relogio(0);
  mostraHora();
  if (efeitoMsgCampo == 0) {rotina_brilho(); indexChr = 0; vez = 0; efeitoMsgCampo = 1;}
  else if (efeitoMsgCampo == 1) {
    dtostrf(sensor_bme.readTemperature(), 3, 1, frase);
    frase[4] = 247;
    frase[5] = 'C';
    frase[6] = ' ';
    frase[7] = ' ';
    frase[8] = ' ';
    frase[9] = 0;
    efeitoMsgCampo++;
  }
  else if (efeitoMsgCampo == 2) msgCampo(frase);
  else if (efeitoMsgCampo == 3) desliza(0);
  else if (efeitoMsgCampo == 4) {
    dtostrf(sensor_bme.readHumidity(), 3, 1, frase);
    frase[4] = '%';
    frase[5] = ' ';
    frase[6] = ' ';
    frase[7] = ' ';
    frase[8] = ' ';
    frase[9] = 0;
    efeitoMsgCampo++;    
  }
  else if (efeitoMsgCampo == 5) msgCampo(frase);
  else if (efeitoMsgCampo == 6) desliza(1);
  else if (efeitoMsgCampo == 7) dow2str(Mod_relogio.getDoW(), frase);
  else if (efeitoMsgCampo == 8) msgCampo(frase);
  else if (efeitoMsgCampo == 9) desliza(0);
  else if (efeitoMsgCampo == 10) calend2str(frase, BUF_SIZE);
  else if (efeitoMsgCampo == 11) msgCampo(frase);
  else if (efeitoMsgCampo == 12) desliza(1);
  else if (efeitoMsgCampo == 13) {
    if (newMessageAvailable) msgCampo(newMessage);
    else efeitoMsgCampo = 0;
  }
  else if (efeitoMsgCampo == 14) deslizaMsg();
  else if (efeitoMsgCampo == 15) efeitoMsgCampo = 0;
  else if (efeitoMsgCampo == 16) graphicArrow(); 

//if (segundo % 10 == 0) ovni();
//  if (segundo % 12 == 0) {efeito = random(14); animSprit(sprite[efeito].data, sprite[efeito].width, sprite[efeito].frames);}

  if (segundo == 0 && minuto % 5 == 0) {
      efeito = random(21); 
      if (efeito < 13) {
         animSprit(sprite[efeito].data, sprite[efeito].width, sprite[efeito].frames);
        efeitoMsgCampo = 0;
      }
      else if (efeito == 13) transformation();
      else if (efeito == 14) wrapText();
      else if (efeito == 15) sorteio(); 
      else if (efeito == 16) aleatorio();
      else if (efeito == 17) preenche();
      else if (efeito == 18) olhos();
      else if (efeito == 19) efeitoMsgCampo = 16;
      else ovni();
    }
}

void ovni(){
int8_t brilhOvni = 0;
bool aumenta = true, atraso = false;
uint8_t cBuf [4][8] = {};
mx.clear();
mx.control(0, MAX_DEVICES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
for (int i = 0; i < 4; i++) mx.getChar(252 + i, 8, cBuf[i]);

delay(500);

while (1) {
  for (uint8_t j = 0; j < 4; j++) {
    if (brilhOvni > 0) mx.control(MD_MAX72XX::INTENSITY, brilhOvni);
    for (uint8_t k = 0; k < 6; k++){
      for (uint8_t i=0; i<8; i++) {
        mx.transform(MD_MAX72XX::TSL);
        mx.setColumn(0, cBuf [j] [i]); //cBuf[i]=leds preenchidos (em decimal) na coluna
      }
      mx.transform(MD_MAX72XX::TSL);
      mx.transform(MD_MAX72XX::TSL);
      mx.transform(MD_MAX72XX::TSL);
      mx.transform(MD_MAX72XX::TSL);
      mx.transform(MD_MAX72XX::TSL);
      mx.transform(MD_MAX72XX::TSL);
      mx.transform(MD_MAX72XX::TSL);
    }
    mx.update();
    if (aumenta & atraso) brilhOvni++;
    else if (!aumenta & atraso) brilhOvni--;
    if (j == 3) atraso = true;
    else if (j == 2) atraso = false;
    
    if (brilhOvni > 11) aumenta = false;
    else if (brilhOvni < 0) {
      rotina_brilho();
      efeitoMsgCampo = 0;
      mx.control(0, MAX_DEVICES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
      mx.clear();  
      return;
    }
  }
}
}

void mostraHora(){
  char psz [9];
  uint8_t horaD = 200 + (hora / 10);
  uint8_t horaU = 200 + (hora % 10);
  uint8_t minD = 200 + (minuto / 10);
  uint8_t minU = 200 + (minuto % 10);
  uint8_t segD = 200 + (segundo / 10);
  uint8_t segU = 200 + (segundo % 10);
  
  sprintf(psz, "%c%c%c%c%c%c%c%c%c", horaD, horaU, 210, minD, minU, 210,  segD, segU, '\0');
  printText(7, 11, psz);
}

void *calend2str(uint8_t *psz, uint8_t len){
  bool Century;
  uint8_t dia, mes, ano;

  dia = Mod_relogio.getDate();
  mes = Mod_relogio.getMonth(Century) - 1;
  ano = Mod_relogio.getYear();

  static const char str[][4]  = {
    "Jan", "Fev", "Mar", "Abr", "Mai", "Jun",
    "Jul", "Ago", "Set", "Out", "Nov", "Dez"
  };

  *psz = '\0';
  
  sprintf(psz, "%d %s %04d %c", dia, str[mes], (ano + 2000), '\0');
  efeitoMsgCampo++;
  return (psz);
}

char *dow2str(uint8_t code, char *psz) {
  static const uint8_t str[][9] PROGMEM = {
    {"Domingo "}, {"Segunda "}, {'T', 'e', 'r', 135, 'a', ' '}, 
    {"Quarta "}, {"Quinta "}, {"Sexta "}, {'S', 134, 'b', 'a', 'd', 'o', ' '}
  };
  *psz = '\0';
  code--;
  strncpy_P(psz, str[code], 13);
  //psz[13] = '\0';
  efeitoMsgCampo++;
  return(psz);
}

void msgCampo(uint8_t *frase) {
  uint8_t p = 0, espaco_caracter = 0;
  static uint8_t cBuf[8];
  static uint8_t showLen = 0;
    
  p = frase[indexChr];

  if (p == 0) {efeitoMsgCampo++; vez = 0; indexChr = 0; return;}
  showLen = mx.getChar(p, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
  
  delay (scrollDelay);
  mx.transform(0, 6, MD_MAX72XX::TSL);
  mx.setColumn(0, cBuf[vez++]);

  if (showLen == vez) {
    indexChr++;
    if (frase[indexChr] != 0) {
      delay (scrollDelay + 38);
      mx.transform(0, 6, MD_MAX72XX::TSL); 
      vez = 0;
    }
  }
}

void printText(uint8_t modStart, uint8_t modEnd, char *pMsg) {

  uint8_t state = 0;
  uint8_t	curLen;
  uint8_t showLen;
  uint8_t	cBuf[8];
  int8_t  col = ((modEnd + 1) * COL_SIZE) - 1;

  do {
    switch(state) {
      case 0:	// Load the next character from the font table
        // if we reached end of message, reset the message pointer
        if (*pMsg == '\0') {
          showLen = col - (modEnd * COL_SIZE);  // padding characters
          state = 2;
          break;
        }
        // retrieve the next character form the font file

        if (*pMsg == '@') *pMsg = 134;
        else if (*pMsg == '$') *pMsg = 135;
        
        showLen = mx.getChar(*pMsg++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state++;
        // !! deliberately fall through to next state to start displaying

      case 1:	// display the next part of the character
        mx.setColumn(col--, cBuf[curLen++]);

        // done with font character, now display the space between chars
        if (curLen == showLen){
          showLen = CHAR_SPACING;
          state = 2;
        }
        break;

      case 2: // initialize state for displaying empty columns
        curLen = 0;
        state++;
        // fall through

      case 3:	// display inter-character spacing or end of message padding (blank columns)
        mx.setColumn(col--, 0);
        curLen++;
        if (curLen == showLen)
          state = 0;
        break;

      default:
        col = -1;   // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));
}

void desliza (bool sobe) {
  if (sobe) mx.transform(0, 6, MD_MAX72XX::TSU);
  else mx.transform(0, 6, MD_MAX72XX::TSD);

  mx.transform(0, 6, MD_MAX72XX::TSL);
  delay (scrollDelay);
  vez++;
  if (vez == 8) {
    vez = 0;
    efeitoMsgCampo++;
    rotina_brilho();
  }
}

void deslizaMsg (){
  mx.transform(0, 6, MD_MAX72XX::TSL);
  vez++;
  if (vez == 56) {
  efeitoMsgCampo = 0;
  rotina_brilho();
  }
}

void serialEvent() {                          //Bluetooth
  uint8_t frase_recebida[BUF_SIZE];
  uint8_t  putIndex = 0;
  uint32_t time = millis() + 4000;
  
  while (1){
    if (Serial.available()){
      frase_recebida[putIndex] = Serial.read();
      if (frase_recebida [putIndex] > 250) {
        separa (frase_recebida, putIndex);
        break;
      }
      putIndex++;
    }
    if (millis() > time) break;
  }
}

void relogio(byte segundos_mais) {
  bool h12, PM;
  
  segundo = Mod_relogio.getSecond() + segundos_mais; //Guarda os segundos
  minuto = Mod_relogio.getMinute(); //Guarda os minutos
  hora = Mod_relogio.getHour(h12, PM); //Guarda as horas
}

void separa (uint8_t *frase, uint8_t tamanho){
  
  uint8_t indice = 0, p = 0;
  uint8_t indiceN = 0;
  uint8_t ultimoChar = 0;

  ultimoChar = frase[tamanho];
  frase[tamanho] = 0;
  
  if (ultimoChar == 255) {
    if (tamanho > 0){
      
      do{
        p = frase[indice++];
        if (p != 195) {
          if (p == 161) p = 134;        //á
          else if (p == 169) p = 130;   //é
          else if (p == 173) p = 161;   //í
          else if (p == 179) p = 162;   //ó
          else if (p == 186) p = 163;   //ú
          else if (p == 167) p = 135;   //ç
      
          else if (p == 162 || p == 163) p = 131; //â
          else if (p == 170) p = 136;             //ê
          else if (p == 180 || p == 181) p = 147; //ô
          else if (p == 94) p = 3;                //coração
          newMessage[indiceN++] = p;
        }
      }while (p != 0);
      newMessageAvailable = true;
      Serial.write(0);
      Serial.write("Recebido");
    }
    else {
      newMessageAvailable = false;
      Serial.write(0);  //enviará mensagem
      Serial.print("Apagado");
    }
  }
  else if (ultimoChar == 254) {
    Mod_relogio.setHour(frase[0]);
    Mod_relogio.setMinute(frase[1]);
    Mod_relogio.setSecond(frase[2]);
    Mod_relogio.setMonth(frase[3]);
    Mod_relogio.setDate(frase[4]);    
    Mod_relogio.setDoW(frase[5]);
    Mod_relogio.setYear(frase[6]);
    Serial.write(2);
    Serial.print("Atualizado");
  }
  else if (ultimoChar == 253) {
    brilho = frase[0];
    EEPROM.write(0, brilho);
  }
  else if (ultimoChar == 252) {
    if      (frase[0] == 0) {
      brilho_auto = false;
      EEPROM.write(1, brilho_auto);
    }
    else if (frase[0] == 1) {
        brilho_auto = true;
        EEPROM.write(1, brilho_auto);
    }
  }
  else if (ultimoChar == 251) {
      Serial.write(1);
      Serial.write(brilho);
      Serial.write(brilho_auto);
    }
  rotina_brilho();
}

void olhos(){

  for (uint8_t i=0; i<MAX_EYE_PAIR; i++) E[i].begin(i*2, &mx, 500);

  for (byte j = 0; j < 150; j++){
    for (uint8_t i = 0; i < MAX_EYE_PAIR; i++) {
      E[i].animate();
      delay(5);
    }
  }
  mx.clear();
  efeitoMsgCampo = 0;
}

void preenche (){
  byte acende, apaga;
  String mes_, semana_;
  
  for (byte i=0; i<12; i++){
    acende = random (12) * 8;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
    
      for(byte j= acende; j < acende + 8;j++) {
        mx.setColumn (j, 255);
        delay(4);
      }
  }
  for (byte i=0; i<125; i++){
    acende = random (12) * 8;
    apaga = random (12) * 8;
    
      for(byte j= acende; j < acende+ 8;j++) {
        mx.setColumn (j, 255);
      }
      delay (1);
      for(byte k= apaga; k < apaga + 8;k++) {
        mx.setColumn (k, 0);
      }
  }
  for (byte i=0; i<8 * MAX_DEVICES; i++){
    mx.setColumn (i, 255);
    delay (1);
  }
  
  relogio(2);
  mostraHora();  
  
  mx.transform(0, MAX_DEVICES - 1, MD_MAX72XX::TINV);
  mx.control(0, MAX_DEVICES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  
  for (uint32_t i=1000000; i>0; i = i - (1 + i / 40)){
    acende = random (12);
    mx.transform(acende, acende, MD_MAX72XX::TINV);
    delayMicroseconds (i);
  }
  for (uint8_t i = 0; i < 7; i++) mx.clear(i);
  efeitoMsgCampo = 0;
}


void sorteio (){

uint8_t cBuf [12][8] = {};
mx.wraparound(MD_MAX72XX::ON);
mx.getChar(36, 8, cBuf [0]);
mx.getChar(24, 8, cBuf [1]);
mx.getChar(6, 8, cBuf [2]);
mx.getChar(42, 8, cBuf [3]);
mx.getChar(231, 8, cBuf [4]);
mx.getChar(2, 8, cBuf [5]);
mx.getChar(14, 8, cBuf [6]);
mx.getChar(15, 8, cBuf [7]);
mx.getChar(16, 8, cBuf [8]);
mx.getChar(17, 8, cBuf [9]);
mx.getChar(18, 8, cBuf [10]);
mx.getChar(3, 8, cBuf [11]);

for (byte j=0; j<12; j++) {
  for (uint8_t i=0; i<8; i++) {
    mx.transform(MD_MAX72XX::TSL);
    mx.setColumn(0, cBuf [j][i]); //cBuf[i]=leds preenchidos (em decimal) na coluna
    delay(5);
  }
}

byte tempo = 0;
for (byte i=0; i<20; i++) {
  tempo = tempo/2 + i;
  mx.transform(10, 10, MD_MAX72XX::TSU);
  delay(tempo);
  
  mx.transform(3, 3, MD_MAX72XX::TSU);
  mx.transform(5, 5, MD_MAX72XX::TSD);
  mx.transform(6, 6, MD_MAX72XX::TSU);
  mx.transform(9, 9, MD_MAX72XX::TSD);
  mx.transform(10, 10, MD_MAX72XX::TSU);
  delay(tempo);
  
  mx.transform(0, 0, MD_MAX72XX::TSU);
  mx.transform(1, 1, MD_MAX72XX::TSD);
  mx.transform(2, 2, MD_MAX72XX::TSD);
  mx.transform(3, 3, MD_MAX72XX::TSU);
  mx.transform(4, 4, MD_MAX72XX::TSU);
  mx.transform(5, 5, MD_MAX72XX::TSD);
  mx.transform(6, 6, MD_MAX72XX::TSU);
  mx.transform(7, 7, MD_MAX72XX::TSD);
  mx.transform(8, 8, MD_MAX72XX::TSU);
  mx.transform(9, 9, MD_MAX72XX::TSD);
  mx.transform(10, 10, MD_MAX72XX::TSU);
  mx.transform(11, 11, MD_MAX72XX::TSU);
  delay (tempo);

  mx.transform(0, 0, MD_MAX72XX::TSU);
  mx.transform(2, 2, MD_MAX72XX::TSD);
  mx.transform(3, 3, MD_MAX72XX::TSU);
  mx.transform(6, 6, MD_MAX72XX::TSU);
  mx.transform(7, 7, MD_MAX72XX::TSD);
  mx.transform(10, 10, MD_MAX72XX::TSU);
  delay (tempo);

  mx.transform(3, 3, MD_MAX72XX::TSU);
  mx.transform(6, 6, MD_MAX72XX::TSU);
  mx.transform(10, 10, MD_MAX72XX::TSU);
  delay (tempo);
}

for (byte i=20; i<44; i++) {
tempo = tempo/2 + i;
  if (i%6 == 0) {
    mx.transform(1, 1, MD_MAX72XX::TSD);
    mx.transform(4, 4, MD_MAX72XX::TSU);
    mx.transform(8, 8, MD_MAX72XX::TSU);
    mx.transform(11, 11, MD_MAX72XX::TSU);
  }
  if (i%60 == 0) {
    mx.transform(1, 1, MD_MAX72XX::TSU);
    mx.transform(1, 1, MD_MAX72XX::TSU);
    mx.transform(1, 1, MD_MAX72XX::TSU);
    mx.transform(1, 1, MD_MAX72XX::TSU);
  }

  mx.transform(3, 3, MD_MAX72XX::TSU);
  mx.transform(6, 6, MD_MAX72XX::TSU);
  if (i%2 == 0) mx.transform(10, 10, MD_MAX72XX::TSU);
  delay (tempo);
}

delay(250);
mx.wraparound(MD_MAX72XX::OFF);
efeitoMsgCampo = 0;
mx.clear();
}

void aleatorio() {
  byte c, value;
  
  for (uint16_t i=0; i<4000; i++){
    c = random (96);
    value = random (256);
    mx.setColumn (c, value);
  }
  efeitoMsgCampo = 0;
  mx.clear();
}

void animSprit(uint8_t *inData, uint8_t inWidth, uint8_t inFrames){
  int8_t idx = 0, frame = 0, deltaFrame;              // current animation frame
  static uint32_t prevTimeAnim = 0;  // remember the millis() value in animations

  deltaFrame = inWidth;
  frame = (inWidth * inFrames) - 1;

  do {

    while (millis()-prevTimeAnim < ANIMATION_DELAY);
    prevTimeAnim = millis();

    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  
    for (uint8_t i=0; i<inWidth; i++) mx.setColumn(idx-inWidth+i, 0);
    idx++;

    for (uint8_t i=0; i<inWidth; i++) {
      mx.setColumn(idx-inWidth+i, inData[frame - i]);
    }

    frame -= deltaFrame;
    if (frame < 0) frame = (inWidth * inFrames) - 1;

    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  } while (idx != mx.getColumnCount()+inWidth);
}

void transformation(){

  MD_MAX72XX::transformType_t  t[] =  {
    MD_MAX72XX::TFLR,
    MD_MAX72XX::TRC,
    MD_MAX72XX::TSD, MD_MAX72XX::TSD, MD_MAX72XX::TSD, MD_MAX72XX::TSD,
    MD_MAX72XX::TSD, MD_MAX72XX::TSD, MD_MAX72XX::TSD, MD_MAX72XX::TSD,
    MD_MAX72XX::TFUD,
    MD_MAX72XX::TSU, MD_MAX72XX::TSU, MD_MAX72XX::TSU, MD_MAX72XX::TSU,
    MD_MAX72XX::TSU, MD_MAX72XX::TSU, MD_MAX72XX::TSU, MD_MAX72XX::TSU,
    MD_MAX72XX::TINV,
    MD_MAX72XX::TRC, MD_MAX72XX::TRC, MD_MAX72XX::TRC, MD_MAX72XX::TRC,
    MD_MAX72XX::TRC, MD_MAX72XX::TRC, MD_MAX72XX::TRC, MD_MAX72XX::TRC,
    MD_MAX72XX::TINV,
    MD_MAX72XX::TRC, MD_MAX72XX::TRC, MD_MAX72XX::TRC, MD_MAX72XX::TRC
  };

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  delay(DELAYTIME);

  mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::ON);
  for (uint8_t i=0; i<(sizeof(t)/sizeof(t[0])); i++)
  {
    mx.transform(t[i]);
    delay(DELAYTIME*4);
  }
  mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::OFF);
  efeitoMsgCampo = 0;
  mx.clear();
}


void wrapText() {
  mx.wraparound(MD_MAX72XX::ON);

  for (uint8_t i=0; i<1*COL_SIZE*MAX_DEVICES; i++) {
    mx.transform(MD_MAX72XX::TSL);
    delay(DELAYTIME/2);
  }
  for (uint8_t i=0; i<1*COL_SIZE*MAX_DEVICES; i++) {
    mx.transform(MD_MAX72XX::TSR);
    delay(DELAYTIME/2);
  }

  relogio(1);
  
  for (uint8_t i=0; i<ROW_SIZE; i++) {
    mx.transform(MD_MAX72XX::TSU);
    delay(DELAYTIME*3);
  }
  for (uint8_t i=0; i<ROW_SIZE; i++) {
    mx.transform(MD_MAX72XX::TSD);
    delay(DELAYTIME*3);
  }

  mx.wraparound(MD_MAX72XX::OFF);
  //mx.clear();
}

void  rotina_brilho() {

  uint16_t sensorValue;
  int8_t brilho_auto_val;

  if (brilho_auto) {  //SENSOR DE LUMINOSIDADE
    sensorValue = analogRead(A0);
    brilho_auto_val = map(sensorValue, 10, 900, 0, MAX_BRILHO);
    (brilho_auto_val < 0 ? brilho_auto_val = 0 : brilho_auto_val > MAX_BRILHO ? brilho_auto_val = MAX_BRILHO : 0);
    mx.control(MD_MAX72XX::INTENSITY, brilho_auto_val);
  }
  else mx.control(MD_MAX72XX::INTENSITY, brilho);
}

void graphicArrow(){
  const uint8_t arrow[] = { 0x3c, 0x66, 0xc3, 0x99 };
  const uint8_t dataSize = (sizeof(arrow)/sizeof(arrow[0]));
  static uint8_t  idx = 0;

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.transform(0, 6, MD_MAX72XX::TSL);
  mx.setColumn(0, arrow[idx++]);
  if (idx == dataSize) idx = 0;
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  vez++;

  if (vez == 150) {
    efeitoMsgCampo = 0;
    for (uint8_t i = 0; i < 7; i++) mx.clear(i);
    }
}
