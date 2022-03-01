#include <EEPROM.h>

int green_main = 7, yellow_main = 12, red_main = 5;
int button_main = 3, state_main = 0, state_emergency = 0, count = 0;
int green_pre = 8, yellow_pre = 9;
int red_emergency = 11, button_emergency = 2;

bool start = true, first = true, press_detected = false, firstChange = true;
bool m_stop = false, m_speed_change = false;
bool logo_continue = true;
int event_interval = 5, prev_time = 0;

int last_button_state = 1, hold_time = 0, end_pressed = 0, start_pressed = 0;

unsigned long previousMillis = 0, currentMillis = 0;
int ELedState = LOW;
int address = 0;
int speed_limit, prev_speed_limit, speed_count;

int curTmp = 0, prevTmp = 0;

int DIN = 6, CS = 10, CLK = 4;

int ThermistorPin = A0;
int Vo;
float R1 = 10000; // value of R1 on board
float logR2, R2, T;
float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741; //steinhart-hart coeficients for thermistor

int m_leds[8];

byte image[8][53] = {
  {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
  {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

void setup()
{
  pinMode(green_main, OUTPUT);
  pinMode(yellow_main, OUTPUT);
  pinMode(red_main, OUTPUT);
  pinMode(green_pre, OUTPUT);
  pinMode(yellow_pre, OUTPUT);
  pinMode(red_emergency, OUTPUT);
  pinMode(button_main, INPUT_PULLUP);
  pinMode(button_emergency, INPUT_PULLUP);
  pinMode(DIN, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(CLK, OUTPUT);
  Serial.begin(9600);

  //  writeWord(0xC01); // Normal operation (not shutdown)
  for (int i = 0; i < 4; i++) writeBit(LOW);
  for (int i = 0; i < 2; i++) writeBit(HIGH);
  for (int i = 0; i < 2; i++) writeBit(LOW);
  for (int i = 0; i < 7; i++) writeBit(LOW);
  writeBit(HIGH);
  latchBuf();

  //  writeWord(0xB07); // Scan limit
  for (int i = 0; i < 4; i++) writeBit(LOW);
  writeBit(HIGH);
  writeBit(LOW);
  writeBit(HIGH);
  writeBit(HIGH);
  for (int i = 0; i < 5; i++) writeBit(LOW);
  for (int i = 0; i < 3; i++) writeBit(HIGH);
  latchBuf();
}

void loop()
{
  if (start)
  {
    start = false;
    speed_limit = EEPROM.read(address);

    if (speed_limit < 1 || speed_limit > 12)
    {
      EEPROM.write(address, 10);
      speed_limit = 10;
    }

    prev_speed_limit = speed_limit;
    dissplay(speed_limit);
    initilize(); // turn all leds on
  }

  readTemp();

  if (curTmp == 1)
    blinkELed();

  state_emergency = digitalRead(button_emergency);

  if (m_stop)
  {
    FadeOut();
    FadeIn();

    state_emergency = digitalRead(button_emergency);// read the button input
    if (state_emergency != last_button_state)// button state changed
      updateStateEmergency();
    last_button_state = state_emergency; // save state for next loop
  }
  else if (state_emergency == LOW)
  {
    m_stop = true;
    EStop(); //emergency stop
  }
  else if (m_speed_change)
    speedChange();
  else if (!m_stop && !m_speed_change)
    Seminor(); //normal state

}

void readTemp()
{
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculate resistance on thermistor
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)); // temperature in Kelvin
  T = T - 273.15; //convert Kelvin to Celcius

  /*Serial.print("Temperature: ");
    Serial.print(T);
    Serial.println(" C");
  */

  if (T > 30)
  {
    curTmp = 1;
    if (curTmp != prevTmp)
    {
      if (speed_limit > 8)
        speed_limit = 8;

      prevTmp = 1;
    }
    dissplay(speed_limit);
  }

  else
  {
    curTmp = 0;
    if (curTmp != prevTmp)
    {
      speed_limit = EEPROM.read(address);
      dissplay(speed_limit);
      digitalWrite(red_emergency, LOW);
      prevTmp = 0;
    }
  }
}

void blinkELed()
{
  digitalWrite(red_emergency, HIGH);
  delay(200);
  digitalWrite(red_emergency, LOW);
  delay(200);
}

void dissplay(int speed_limit)
{
  turnOffAll();
  switch (speed_limit)
  {
    case 1:
      display1();
      break;
    case 2:
      display2();
      break;
    case 3:
      display3();
      break;
    case 4:
      display4();
      break;
    case 5:
      display5();
      break;
    case 6:
      display6();
      break;
    case 7:
      display7();
      break;
    case 8:
      display8();
      break;
    case 9:
      display9();
      break;
    case 10:
      display10();
      break;
    case 11:
      display11();
      break;
    case 12:
      display12();
      break;
    default:
      break;
  }
}

void updateStateSpeedChange()
{
  // the button has been just pressed
  if (state_main == LOW) {
    press_detected = true;
    start_pressed = millis();
  }
  // the button has been just released
  else
  {
    end_pressed = millis();
    hold_time = end_pressed - start_pressed;

    if (hold_time >= 2000)
    {
      m_speed_change = false;
      if (speed_count % 12 == 0)
        speed_limit = 12;
      else
        speed_limit = speed_count % 12;
      EEPROM.write(address, speed_limit);
      dissplay(EEPROM.read(address));
      Serial.println(speed_limit);
      Serial.println(EEPROM.read(address));

    }
    else if (press_detected)
    {
      Serial.println("short press");
      speed_count++;
      press_detected = false;
    }
  }
}

void speedChange()
{
  state_main = digitalRead(button_main);
  if (state_main != last_button_state)// button state changed
    updateStateSpeedChange();

  last_button_state = state_main; // save state for next loop
  switch (speed_count % 12)
  {
    case 1:
      turnOffAll();
      delay(100);
      display1();
      delay(100);
      break;
    case 2:
      turnOffAll();
      delay(100);
      display2();
      delay(100);
      break;
    case 3:
      turnOffAll();
      delay(100);
      display3();
      delay(100);
      break;
    case 4:
      turnOffAll();
      delay(100);
      display4();
      delay(100);
      break;
    case 5:
      turnOffAll();
      delay(100);
      display5();
      delay(100);
      break;
    case 6:
      turnOffAll();
      delay(100);
      display6();
      delay(100);
      break;
    case 7:
      turnOffAll();
      delay(100);
      display7();
      delay(100);
      break;
    case 8:
      turnOffAll();
      delay(100);
      display8();
      delay(100);
      break;
    case 9:
      turnOffAll();
      delay(100);
      display9();
      delay(100);
      break;
    case 10:
      turnOffAll();
      delay(100);
      display10();
      delay(100);
      break;
    case 11:
      turnOffAll();
      delay(100);
      display11();
      delay(100);
      break;
    case 0:
      turnOffAll();
      delay(100);
      display12();
      delay(100);
      break;
    default:
      break;
  }

}

void writeBit(bool b) // Write 1 bit to the buffer
{
  digitalWrite(DIN, b);
  digitalWrite(CLK, LOW);
  digitalWrite(CLK, HIGH);
  //Serial.print(b);
  //Serial.print(".");
}

void latchBuf() // Latch the entire buffer
{
  digitalWrite(CS, LOW);
  digitalWrite(CS, HIGH);
  //Serial.println();
}

void updateStateEmergency()
{
  // the button has been just pressed
  if (state_emergency == LOW) {
    start_pressed = millis();
  }
  // the button has been just released
  else {
    end_pressed = millis();
    hold_time = end_pressed - start_pressed;

    if (hold_time >= 2000)
    {
      m_stop = false;
      ReleaseEmergency();
    }
  }
}

void initilize()
{
  digitalWrite(green_main, HIGH);
  digitalWrite(yellow_main, HIGH);
  digitalWrite(red_main, HIGH);
  digitalWrite(green_pre, HIGH);
  digitalWrite(yellow_pre, HIGH);
  digitalWrite(red_emergency, HIGH);
  delay(2000);
  digitalWrite(green_main, LOW);
  digitalWrite(yellow_main, LOW);
  digitalWrite(red_main, LOW);
  digitalWrite(green_pre, LOW);
  digitalWrite(yellow_pre, LOW);
  digitalWrite(red_emergency, LOW);
  delay(1000);
}

void turnOnCol(byte col, byte leds)
{
  byte colNum[8], ledsNum[8];
  for (int i = 0; i < 8; i++)
  {
    colNum[7 - i] = ((col << 7 - i)) >> 7 & 1;
  }
  for (int i = 0; i < 8; i++)
  {
    if (colNum[i] == 1)
      writeBit(HIGH);
    else
      writeBit(LOW);
  }
  for (int i = 0; i < 8; i++)
  {
    ledsNum[7 - i] = ((leds << 7 - i)) >> 7 & 1;
  }
  for (int i = 0; i < 8; i++)
  {
    if (ledsNum[i] == 1)
      writeBit(HIGH);
    else
      writeBit(LOW);
  }
  latchBuf();
}

void checkFirst()
{
  if (first)
  {
    turnOffAll();
    first = false;
  }
}

void display1()
{
  checkFirst();
  turnOnCol(0x03, 0x22);
  turnOnCol(0x04, 0x3E);
  turnOnCol(0x05, 0x20);
}

void display2()
{
  checkFirst();
  turnOnCol(0x03, 0x3A);
  turnOnCol(0x04, 0x2A);
  turnOnCol(0x05, 0x2E);
}

void display3()
{
  checkFirst();
  turnOnCol(0x03, 0x2A);
  turnOnCol(0x04, 0x2A);
  turnOnCol(0x05, 0x3E);
}

void display4()
{
  checkFirst();
  turnOnCol(0x03, 0x0E);
  turnOnCol(0x04, 0x08);
  turnOnCol(0x05, 0x3E);
}

void display5()
{
  checkFirst();
  turnOnCol(0x03, 0x2E);
  turnOnCol(0x04, 0x2A);
  turnOnCol(0x05, 0x3A);
}

void display6()
{
  checkFirst();
  turnOnCol(0x03, 0x3E);
  turnOnCol(0x04, 0x2A);
  turnOnCol(0x05, 0x3A);
}

void display7()
{
  checkFirst();
  turnOnCol(0x03, 0x02);
  turnOnCol(0x04, 0x02);
  turnOnCol(0x05, 0x3E);
}

void display8()
{
  checkFirst();
  turnOnCol(0x03, 0x3E);
  turnOnCol(0x04, 0x2A);
  turnOnCol(0x05, 0x3E);
}

void display9()
{
  checkFirst();
  turnOnCol(0x03, 0x2E);
  turnOnCol(0x04, 0x2A);
  turnOnCol(0x05, 0x3E);
}

void display10()
{
  checkFirst();
  turnOnCol(0x02, 0x22);
  turnOnCol(0x03, 0x3E);
  turnOnCol(0x04, 0x20);

  turnOnCol(0x06, 0x3E);
  turnOnCol(0x07, 0x22);
  turnOnCol(0x08, 0x3E);
}

void display11()
{
  checkFirst();
  turnOnCol(0x02, 0x22);
  turnOnCol(0x03, 0x3E);
  turnOnCol(0x04, 0x20);

  turnOnCol(0x06, 0x22);
  turnOnCol(0x07, 0x3E);
  turnOnCol(0x08, 0x20);
}

void display12()
{
  checkFirst();
  turnOnCol(0x02, 0x22);
  turnOnCol(0x03, 0x3E);
  turnOnCol(0x04, 0x20);

  turnOnCol(0x06, 0x3A);
  turnOnCol(0x07, 0x2A);
  turnOnCol(0x08, 0x2E);
}

void EStop()
{
  m_stop = true;
  digitalWrite(green_main, LOW);
  digitalWrite(yellow_main, LOW);
  digitalWrite(red_main, HIGH);
  digitalWrite(green_pre, LOW);
  digitalWrite(yellow_pre, HIGH);
  digitalWrite(red_emergency, HIGH);
}

void ReleaseEmergency()
{
  digitalWrite(green_main, LOW);
  digitalWrite(yellow_main, LOW);
  digitalWrite(red_main, LOW);
  digitalWrite(green_pre, LOW);
  digitalWrite(yellow_pre, LOW);
  digitalWrite(red_emergency, LOW);
}

void LogoDisplay()
{
  for (int i = 46; i > 0 && logo_continue; i--)
  {
    LogoPart(image, i);
    delay(80);

    state_main = digitalRead(button_main);
    state_emergency = digitalRead(button_emergency);

    if (state_main == LOW)
      return;

    else if (state_emergency == LOW)
    {
      m_stop = true;
      return;
    }
  }
  if (logo_continue)
    delay(1000);
}

void LogoPart(byte image[8][53], int offset)
{
  for (int j = 0; j < 8; j++)
  {
    for (int i = 0; i < 8; i++)
      m_leds[8 - i] = image[i][j + offset];
    turnOnColLogo(j + 1, m_leds);
  }

}

void turnOnColLogo(byte col, int leds[8])
{
  byte colNum[8];
  for (int i = 0; i < 8; i++)
  {
    colNum[7 - i] = ((col << 7 - i)) >> 7 & 1;
  }
  for (int i = 0; i < 8; i++)
  {
    if (colNum[i] == 1)
      writeBit(HIGH);
    else
      writeBit(LOW);
  }

  for (int i = 0; i < 8; i++)
  {
    if (leds[i] == 1)
      writeBit(HIGH);
    else
      writeBit(LOW);
  }
  latchBuf();
}

void Seminor()
{
  digitalWrite(red_emergency, LOW);

  state_main = digitalRead(button_main);
  if (count % 3 != 1)
  {
    if (state_main == LOW)
      count++;
  }

  switch (count % 3)
  {
    case 0: //RED
      if (T < 30)
        dissplay(EEPROM.read(address));
      digitalWrite(green_main, LOW);
      digitalWrite(red_main, HIGH);

      digitalWrite(green_pre, LOW);
      digitalWrite(yellow_pre, HIGH);
      count = 0;
      break;

    case 1: //YELLOW
      digitalWrite(red_main, LOW);
      digitalWrite(yellow_main, HIGH);

      digitalWrite(green_pre, HIGH);
      digitalWrite(yellow_pre, HIGH);
      if (firstChange) 
      {
        state_main = HIGH;
        firstChange = false;
      }

      if (state_main != last_button_state)// button state changed
        updateStateSeminor();

      last_button_state = state_main; // save state for next loop
      break;

    case 2: //GREEN
      digitalWrite(yellow_main, LOW);
      digitalWrite(green_main, HIGH);

      digitalWrite(green_pre, HIGH);
      digitalWrite(yellow_pre, LOW);
      LogoDisplay();
      break;
      
    default:
      break;
  }
  delay(200);
}

void updateStateSeminor()
{
  // the button has been just pressed
  if (state_main == LOW) {
    press_detected = true;
    start_pressed = millis();
  }
  // the button has been just released
  else
  {
    end_pressed = millis();
    hold_time = end_pressed - start_pressed;

    if (hold_time >= 2000)
    {
      firstChange = true;
      m_speed_change = true;
      speed_count = speed_limit;
    }
    else if (press_detected)
    {
      count++;
      press_detected = false;
      firstChange = true;
    }
  }
}

void FadeIn()
{
  int cur_time;
  int i = 0;
  while (i < 255)
  {
    cur_time = millis();
    if (cur_time - prev_time >= event_interval)
    {
      i += 5;
      //i++;
      analogWrite(red_emergency, i);
      prev_time = cur_time;
    }
  }
}

void FadeOut()
{
  int cur_time;
  int i = 255;
  while (i >= 0)
  {
    cur_time = millis();
    if (cur_time - prev_time >= event_interval)
    {
      i -= 5;
      //i--;
      analogWrite(red_emergency, i);
      prev_time = cur_time;
    }
  }
}

void turnOffAll()
{
  //1
  for (int i = 0; i < 7; i++) writeBit(LOW);
  for (int i = 0; i < 1; i++) writeBit(HIGH);
  for (int i = 0; i < 8; i++) writeBit(LOW);
  latchBuf();
  //2
  for (int i = 0; i < 6; i++) writeBit(LOW);
  for (int i = 0; i < 1; i++) writeBit(HIGH);
  for (int i = 0; i < 1; i++) writeBit(LOW);
  for (int i = 0; i < 8; i++) writeBit(LOW);
  latchBuf();
  //3
  for (int i = 0; i < 6; i++) writeBit(LOW);
  for (int i = 0; i < 2; i++) writeBit(HIGH);
  for (int i = 0; i < 8; i++) writeBit(LOW);
  latchBuf();
  //4
  for (int i = 0; i < 5; i++) writeBit(LOW);
  for (int i = 0; i < 1; i++) writeBit(HIGH);
  for (int i = 0; i < 2; i++) writeBit(LOW);
  for (int i = 0; i < 8; i++) writeBit(LOW);
  latchBuf();
  //5
  for (int i = 0; i < 5; i++) writeBit(LOW);
  for (int i = 0; i < 1; i++) writeBit(HIGH);
  for (int i = 0; i < 1; i++) writeBit(LOW);
  for (int i = 0; i < 1; i++) writeBit(HIGH);
  for (int i = 0; i < 8; i++) writeBit(LOW);
  latchBuf();
  //6
  for (int i = 0; i < 5; i++) writeBit(LOW);
  for (int i = 0; i < 2; i++) writeBit(HIGH);
  for (int i = 0; i < 1; i++) writeBit(LOW);
  for (int i = 0; i < 8; i++) writeBit(LOW);
  latchBuf();
  //7
  for (int i = 0; i < 5; i++) writeBit(LOW);
  for (int i = 0; i < 3; i++) writeBit(HIGH);
  for (int i = 0; i < 8; i++) writeBit(LOW);
  latchBuf();
  //8
  for (int i = 0; i < 4; i++) writeBit(LOW);
  for (int i = 0; i < 1; i++) writeBit(HIGH);
  for (int i = 0; i < 3; i++) writeBit(LOW);
  for (int i = 0; i < 8; i++) writeBit(LOW);
  latchBuf();
}
