#include <Adafruit_TCS34725.h>
#include <POP32.h>
Adafruit_TCS34725 tcs = Adafruit_TCS34725();
#define L A0  // หน้า ซ้ายสุด
#define R2 A3 // หน้าขวาสุด
#define BL A4
#define BR A5
#define Swicth A7
#define ServoRG 1
#define ServoBY 2
#define flag 3
int readswith = digitalRead(Swicth);
int SensorL = 2500;  // A0
int SensorR2 = 2350; // A3
int SensorBL = 2400; // A4
int SensorBR = 2350; // A5
int check_state;
int Color_state;
int Color_Count;
int Program;

int a;
int i;
int j;
int k;
int l;
int m;
bool box_R = 0, box_G = 0, box_B = 0, box_Y = 0;
void setup(void)
{

  beep(1000);
  while (1)
  { // กดเลือกรันได้ หมุนเลือก
    oled.text(2, 0, "                     ");
    oled.show();
    if (knob() <= 500)
    {
      oled.text(2, 1, "Program");
      Program = 1;
      oled.text(2, 9, "1 ");
    }
    else if (knob() <= 800)
    {
      oled.text(2, 1, "Program");
      Program = 2;
      oled.text(2, 9, "2 ");
    }
    else if (knob() <= 1100)
    {
      oled.text(2, 1, "Block_FD");
      Program = 3;
    }
    else if (knob() <= 1400)
    {
      oled.text(2, 1, "Block_BK");
      Program = 4;
    }
    else if (knob() <= 1700)
    {
      oled.text(2, 1, " TurnL ");
      Program = 5;
    }
    else if (knob() <= 2000)
    {
      oled.text(2, 1, " TurnR   ");
      Program = 6;
    }
    else if (knob() <= 2300)
    {
      oled.text(2, 1, " Slide_L_check   ");
      Program = 7;
    }
    else if (knob() <= 2700)
    {
      oled.text(2, 1, "  Slide_R_check   ");
      Program = 8;
    }
    oled.show();
    if (SW_OK())
    { // ปุ่มเหลืองทำงานโปรแกรมหลัก

      break;
    }
    else if (SW_A())
    { // ปุ่ม A อ่านค่าเซนเซอร์
      oled.clear();
      oled.textSize(1);
      Read_Sensor();
    }
    else if (SW_B())
    { // ปุ่ม ฺอ่านค่าสี
      oled.clear();
      oled.textSize(1);
      Check_Color();
    }
  }
  beep(400);
  delay(500);
  beep(400);
  oled.clear();
  oled.textSize(1);
  if (Program == 1)
  {
    Program1();
  }
  else if (Program == 2)
  {
    Program2();
  }
  else if (Program == 3)
  {
    Block_FD_Pass(); // เดิน 1 ช่อง
  }
  else if (Program == 4)
  {
    Block_BK_Pass(); // ถอย 1 ช่อง
  }
  else if (Program == 5)
  {
    SpinL(); // เลี้ยวซ้าย
  }
  else if (Program == 6)
  {
    SpinR(); // เลี้ยวขวา
  }
  else if (Program == 7)
  {
    Slide_L_check(); // ทริมซ้าย
  }
  else if (Program == 8)
  {
    Slide_R_check(); // ทริมขวา
  }
}

void loop()
{
}
void Program1()
{
  Block_FD_Pass();
  while (box_R == 0 || box_G == 0 || box_B == 0 || box_Y == 0)
  {
    Slide_L_check();
    if (check_state == 0)
    {
      delay(100);
      Slide_R();
      delay(500);
      AO();
      delay(100);
      SpinL();
      forwardline();
      continue;
    }
    else if (check_state == 1)
    {
      delay(100);
      Slide_R();
      delay(250);
      AO();
      delay(100);
      walkline(); // เดินเพื่อเช็กเส้น
      if (check_state == 0)
      {
        continue;
      }
      else if (check_state == 1)
      {
        delay(100);
        Backward();
        delay(285);
        AO();
        delay(100);
        forwardline();
        if (Color_state == 0)
        {
          delay(100);
          Backward();
          delay(285);
          AO();
          delay(200);
          Slide_R_check();
          if (check_state == 0)
          {
            delay(100);
            Slide_L();
            delay(500);
            AO();
            delay(100);
            SpinR();
            forwardline();
            continue;
          }
          else if (check_state == 1)
          {
            delay(100);
            Slide_L();
            delay(250);
            AO();
            delay(100);
            turnaround();
            delay(100);
            continue;
          }
        }
        else if (Color_state == 1)
        {
          Check_Color();
          delay(300);
          continue;
        }
      }
      else if (readswith == 0)
      { // เช็คตะเกียบ
        beep(500);
        beep(500);
        delay(200);
        Block_FD_Pass();
        delay(200);
        continue;
      }
    }
  }
}
void Program2()
{
  while (1)
  {

    oled.clear();
    oled.text(1, 0, "Swicth = ");
    oled.text(1, 8, "%d", readswith);
    oled.show();
    delay(100);
  }
}
void turnright()
{
  motor(1, 50);
  motor(2, 50);
  motor(3, -30);
  motor(4, -30);
}

void turnleft()
{
  motor(1, -30);
  motor(2, -30);
  motor(3, 53);
  motor(4, 53);
}

void turnaround()
{
  motor(1, 52);
  motor(2, 52);
  motor(3, -53);
  motor(4, -53);
  delay(1230);
  AO();
}

void Backward()
{
  motor(1, -50);
  motor(2, -50);
  motor(3, -50);
  motor(4, -50);
}

void Slide_R()
{
  motor(1, -50);
  motor(2, 50);
  motor(3, 50);
  motor(4, -50);
}

void Slide_L()
{
  motor(1, 50);
  motor(2, -50);
  motor(3, -50);
  motor(4, 50);
}

void Block_FD_Pass()
{
  for (i = 1; i <= 820; i++)
  {
    motor(1, 50);
    motor(2, 50);
    motor(3, 50);
    motor(4, 50);
    delay(1);
  }
  AO();
  delay(100);
}

void Block_BK_Pass()
{
  for (j = 1; j <= 820; j++)
  {
    motor(1, -50);
    motor(2, -50);
    motor(3, -50);
    motor(4, -50);
    delay(1);
  }
  AO();
  delay(100);
}

void SpinL()
{
  motor(1, -50);
  motor(2, -50);
  motor(3, 50);
  motor(4, 50);
  delay(620);
  AO();
  delay(100);
}

void SpinR()
{
  motor(1, 50);
  motor(2, 50);
  motor(3, -50);
  motor(4, -50);
  delay(620);
  AO();
  delay(100);
}

void forwardline()
{
  check_state = 0;
  Color_state = 0;
  for (k = 1; k <= 1500; k++)
  {

    if (analogRead(L) < SensorL && analogRead(R2) < SensorR2)
    {
      check_state = 1;
      AO();
      delay(200);
      break;
    }
    else if (analogRead(L) < SensorL)
    {
      turnright();
    }
    else if (analogRead(R2) < SensorR2)
    {
      turnleft();
    } /*
   else if (analogRead(L) < SensorL && analogRead(R2) > SensorR2)
   {

     motor(1, -20);
     motor(2, -20);
     motor(3, 15);
     motor(4, 15);
   }
   else if (analogRead(L) > SensorL && analogRead(R2) < SensorR2)
   {
     motor(1, 15);
     motor(2, 15);
     motor(3, -20);
     motor(4, -20);
   }*/
    else
    {
      motor(1, 50);
      motor(2, 50);
      motor(3, 50);
      motor(4, 50);
    }
  }
  AO();
  delay(200);
  Read_Color();
}
void walkline()
{
  check_state = 0;
  Color_state = 0;
  for (k = 1; k <= 1500; k++)
  {

    if (analogRead(L) < SensorL && analogRead(R2) < SensorR2)
    {
      check_state = 1;
      AO();
      delay(200);
      break;
    }
    else if (analogRead(L) < SensorL)
    {
      turnright();
    }
    else if (analogRead(R2) < SensorR2)
    {
      turnleft();
    }
    else if (readswith == 0)
    {
      AO();
      delay(200);
      break;
    }
    else
    {
      motor(1, 50);
      motor(2, 50);
      motor(3, 50);
      motor(4, 50);
    }
  }
  AO();
}
void Slide_L_check()
{
  check_state = 0;
  for (l = 1; l <= 1500; l++)
  {
    if (analogRead(L) < SensorL && analogRead(BL) < SensorBL)
    {
      check_state = 1;
      AO();
      delay(200);
      break;
    }
    else if (analogRead(L) < SensorL && analogRead(BL) > SensorBL)
    {
    SR(35);
    }
    else if (analogRead(L) > SensorL && analogRead(BL) < SensorBL)
    {
     SL(35);
    }
    else
    {
      Slide_L();
    }
  }
  AO();
}

void Slide_R_check()
{
  check_state = 0;
  for (m = 1; m <= 1500; m++)
  {
    if (analogRead(R2) < SensorR2 && analogRead(BR) < SensorBR)
    {
      check_state = 1;
      AO();
      delay(100);
      break;
    }
    else if (analogRead(R2) < SensorR2 && analogRead(BR) > SensorBR)
    {
  SL(35);
    }
    else if (analogRead(R2) > SensorR2 && analogRead(BR) < SensorBR)
    {
    SR(35);
    }
    else
    {
      Slide_R();
    }
  }
  AO();
}

void Read_Sensor()
{ // อ่าค่าเซนเซอร์แสดงจอ
  beep(1000);
  while (1)
  {
    oled.clear();
    oled.text(0, 0, "L");
    oled.text(0, 5, "%d", analogRead(L));
    oled.text(1, 0, "R2");
    oled.text(1, 5, "%d", analogRead(R2));
    oled.text(2, 0, "BL");
    oled.text(2, 5, "%d", analogRead(BL));
    oled.text(3, 0, "BR");
    oled.text(3, 5, "%d", analogRead(BR));
    oled.show();
    delay(100);
  }
}

void Read_Color()
{ // อ่าค่าสีแสดงจอแบบละเอียด
  Color_state = 0;
  beep(1000);
  while (1)
  {
    uint16_t r, g, b, c, colorTemp, lux;
    tcs.getRawData(&r, &g, &b, &c);
    colorTemp = tcs.calculateColorTemperature(r, g, b);
    lux = tcs.calculateLux(r, g, b);
    oled.clear();
    oled.text(0, 0, "Lux");
    oled.text(0, 5, "%d", lux);
    oled.text(1, 0, "R");
    oled.text(1, 5, "%d", r);
    oled.text(2, 0, "G");
    oled.text(2, 5, "%d", g);
    oled.text(3, 0, "B");
    oled.text(3, 5, "%d", b);
    oled.text(4, 0, "Temp");
    oled.text(4, 5, "%d", colorTemp);
    float x = (r + g + b) / 3;
    if (r > x && r >= g && r >= b && lux <= 1)
    {

      Color_state = 1;
      oled.text(6, 0, "Color");
      oled.text(6, 6, "Red");
    }
    else if (b > x && b > r && b >= g && lux <= 1)
    {

      Color_state = 1;
      oled.text(6, 0, "Color");
      oled.text(6, 6, "Blue");
    }
    else if ((b < r && b < g) && lux >= 3 && colorTemp <= 4000)
    {

      Color_state = 1;
      oled.text(6, 0, "Color");
      oled.text(6, 6, "Yellow");
    }
    else if (g > x && g >= r && g >= b && lux >= 4 && colorTemp <= 5500)
    {

      Color_state = 1;
      oled.text(6, 0, "Color");
      oled.text(6, 6, "Green");
    }
    else
    {
      Color_state = 0;
      oled.text(6, 0, "Color");
      oled.text(6, 6, "White");
    }
    oled.show();
    delay(100);
  }
}

void Check_Color()
{ // เช็คสีปล่อย box
  for (a = 1; a <= 10; m++)
  {
    beep(1000);
    uint16_t r, g, b, c, colorTemp, lux;
    tcs.getRawData(&r, &g, &b, &c);
    colorTemp = tcs.calculateColorTemperature(r, g, b);
    lux = tcs.calculateLux(r, g, b);
    oled.clear();
    oled.text(1, 0, "R");
    oled.text(1, 5, "%d", r);
    oled.text(2, 0, "G");
    oled.text(2, 5, "%d", g);
    oled.text(3, 0, "B");
    oled.text(3, 5, "%d", b);
    oled.text(4, 0, "Temp");
    oled.text(4, 5, "%d", colorTemp);
    if (a >= 1)
    {
      float x = (r + g + b) / 3;
      if (r > x && r >= g && r >= b && lux <= 1)
      {
        box_R = 1;
        Color_state = 1;
        oled.text(6, 0, "Color");
        oled.text(6, 6, "Red");
        beep(1000);
        box_Red();
        a = 10;
      }
      else if (b > x && b > r && b >= g && lux <= 1)
      {
        box_B = 1;
        Color_state = 1;
        oled.text(6, 0, "Color");
        oled.text(6, 6, "Blue");
        beep(1000);
        box_Blue();
        a = 10;
      }
      else if ((b < r && b < g) && lux >= 3 && colorTemp <= 4000)
      {
        box_Y = 1;
        Color_state = 1;
        oled.text(6, 0, "Color");
        oled.text(6, 6, "Yellow");
        beep(1000);
        box_Yellow();
        a = 10;
      }
      else if (g > x && g >= r && g >= b && lux >= 2 && colorTemp <= 5500)
      {
        box_G = 1;
        Color_state = 1;
        oled.text(6, 0, "Color");
        oled.text(6, 6, "Green");
        beep(1000);
        box_Green();
        a = 10;
      }
      a = 10;
      oled.show();
      if (Color_state == 1)
      {
        check_allbox();
        delay(200);
        turnaround();
        check_state = 1;
        delay(100);
      }
    }
  }
}

void box_Red()
{
  servo(ServoRG, 40);
  delay(200);
  servo(ServoRG, 90);
  delay(600);
}

void box_Green()
{
  servo(ServoRG, 200);
  delay(200);
  servo(ServoRG, 90);
  delay(600);
}

void box_Blue()
{
  servo(ServoBY, 10);
  delay(200);
  servo(ServoBY, 90);
  delay(600);
}

void box_Yellow()
{
  servo(ServoBY, 180);
  delay(200);
  servo(ServoBY, 90);
  delay(600);
}

void upfleg()
{
  servo(flag, 180);
  delay(200);
  servo(flag, 90);
  delay(600);
}

void check_allbox()
{
  if (box_R == 1 && box_G == 1 && box_B == 1 && box_Y == 1)
  {
    AO();
    upfleg();
    beep(1000);
    beep(1000);
    beep(1000);
    beep(1000);

    while (1)
    {
    }
  }
}
