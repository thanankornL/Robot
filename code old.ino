#include <POP32.h>
#include <Adafruit_TCS34725.h>
Adafruit_TCS34725 tcs = Adafruit_TCS34725();
unsigned long myTime,DistanTime;
#define L A0
#define R2 A3
#define BL A4
#define BR A5
#define Swicth A7
#define ServoYR 1
#define ServoBG 2
#define flag 3
bool box_R = 0, box_G  = 0, box_B  = 0,box_Y  = 0 ;
int Slide_TimeSet = 530 ; //เวลาจากตรงกลางถึงเช็คหลุม +20
int FD_Check_Time = 1100;
int Slide_Haf_Block = 300 ; //เวลาเส้นขอบมาตรงกลาง
int Slide_Full_Block = 600 ; //เวลาสไลด์ 1 ช่อง
int TurnLTime = 850 ; //เวลาเลี้ยว
int TurnRTime = 850 ; //เวลาเลี้ยว
int Block_FD_Pass_Time = 1100 ; //เวลาตรง 1 ช่อง
int TrimEror = 300 ; //เลี้ยวเกินเวลา
int Turnaround = 1430 ;  // เวลากลับหลัง

int SensorR2 = 800 ;  //ค่าแสงเซนเซอร์
int SensorL = 800 ;
int SensorBL = 800 ;
int SensorBR = 800 ;

int SensorSwicth = 1100 ;
int Speed = 40; //ความเร็ว
int TurnSpeed = 40; //ความเร็วตอนเลี้ยว
int Program ;
int box = 0 ; //เก็บค่าการปล่อยลูกเต๋า
bool Tack = 0; ; // เก็บค่าเจอเส้น
bool Bride = 0; 
bool BK_Loop = 0; //ลูปถอยหลัง
bool FD_Loop = 1; 

void setup(void) {
   beep(1000);
   oled.clear();
   set_servo();
  if (tcs.begin()) {
    oled.text(0,3,"OK Color sensor");
    oled.show();
  } else {
    oled.text(0,0,"No TCS34725 check your connections");
    oled.show();
     while (1);
   }
   oled.textSize(2);  
  while(1){    //กดเลือกรันได้ หมุนเลือก
    oled.text(2,0,"                     ");
    oled.show();
   if( knob()<=500){
     oled.text(2,1,"Program");
     Program = 1;
     oled.text(2,9,"1 ");
   }else  if( knob()<= 800){
     oled.text(2,1,"Program");
     Program = 2;
     oled.text(2,9,"2 ");
   }else  if( knob()<= 1100){
     oled.text(2,1,"Block_FD");
     Program = 3;
   
   }else  if( knob()<= 1400){
     oled.text(2,1,"Block_BK");
     Program = 4;
     
   }else  if( knob()<=1700){
     oled.text(2,1," TurnL ");
     Program = 5;
   }else  if( knob()<= 2000){
     oled.text(2,1," TurnR   ");
     Program = 6;
     
   }else  if( knob()<= 2300){
     oled.text(2,1," TrimL   ");
     Program = 7;
    
   }else  if( knob()<= 2700){
     oled.text(2,1," TrimR   ");
     Program = 8;
    
   }
   oled.show();
   if(SW_OK()){  //ปุ่มเหลืองทำงานโปรแกรมหลัก
      break;
    }else if(SW_A()){ //ปุ่ม A อ่านค่าเซนเซอร์ 
      oled.clear();
      oled.textSize(1);
      Read_Sensor();
    }else if(SW_B()){ //ปุ่ม ฺอ่านค่าสี
      oled.clear();
      oled.textSize(1);
      Read_Color();
    }
  }
  beep(400);
  delay(500);
  beep(400);
  oled.clear();
  oled.textSize(1);
  if(Program == 1){
    Program1();
  }else if(Program == 2){
    Program2();
  }else if(Program == 3){
    Block_FD_Pass(); //เดิน 1 ช่อง
  }else if(Program == 4){
    Block_BK_Pass(); //ถอย 1 ช่อง
  }else if(Program == 5){
    TurnL(); //เลี้ยวซ้าย
  }else if(Program == 6){
    TurnR();   //เลี้ยวขวา
  }else if(Program == 7){
    Slide_L_Check (); //ทริมซ้าย
  }else if(Program == 8){
    Slide_R_Check (); //ทริมขวา
  }

}
void Program1(){ //โปรแกรม 1
  
}
void Program2(){//โปรแกรม 2
    Block_FD_Pass(); //เดิน 1 ช่อง
  while( box_R  == 0 || box_G == 0 || box_B == 0 || box_Y == 0){
     if( Bride == 1 ){
         Bride_Pass();
     }else{
        Read_Color_Box();//เช็คสีครงนี้ 
     }
    if(FD_Loop == 1){
      if( Bride == 1 ){
         Bride_Pass();
     }else{
       Slide_L_Check (); //เช็คซ้าย
     }
     delay(150);
     if(Tack == 0 && FD_Loop == 1 ){ //ไม่เจอเส้น
       
       TurnL(); Block_FD_Pass();//เลี้ยวก่อนไป
  
       FD_Loop =  0 ; //ออกจากลูปเดินหน้า
     }else if(Tack == 1 && FD_Loop == 1 ){   //เจอ
       FD_Check(); //เช็คหน้า
     }
     delay(150);
     if(Tack == 0 && FD_Loop == 1){ //ไม่เจอเส้น
       FD_Loop =  0 ; //ออกจากลูปเดินหน้า
     }else if(Tack == 1 && FD_Loop == 1 ){   //เจอ
       Slide_R_Check ();
     }
     delay(150);
     if(Tack == 0 && FD_Loop == 1){ //ไม่เจอเส้น
       TurnR(); Block_FD_Pass();//เลี้ยวก่อนไป

       FD_Loop =  0 ; //ออกจากลูปเดินหน้า
     }else if(Tack == 1 && FD_Loop == 1 ){   //เจอ เข้าสู่วนถอยหลัง
      turnaround (); //เข้าลูปถอยหลัง
      FD_Loop =  1 ; //ออกจากลูปเดินหน้า
     }
   } //FD_Block
  } //box
}

void FD_Check(){
   AO(); 
   delay(100);
   myTime=millis();
   FD(Speed);
    while(1){ 
        if(millis()- myTime >= FD_Check_Time || (analogRead(L)<SensorL || analogRead(R2)< SensorR2)){ //หาเช็คระยะข้างเลยเวลา
         DistanTime = millis()- myTime;
         break ;
        }
      }
    AO();
   if(DistanTime >= FD_Check_Time){
     FD(Speed); //ไม่เจอไปต่อ
     delay(Block_FD_Pass_Time-FD_Check_Time);
     Tack = 0;
   }else{  //เจอ
     FD_Trim();
     BK(Speed);
     delay(Slide_Haf_Block);
     Tack = 1;
   }
   AO();
}
void Bride_Pass(){
    myTime=millis();  
       FD(Speed);
       while(1){
       if((millis()-myTime) >=(Block_FD_Pass_Time) ){ AO(); break; }  //แก้ตัวเลขให้เดิน 1 ช่องพอดี
       }
         Bride = 0 ;
         FD_Loop = 1; 
         Tack = 0 ;
         FD_Trim();
         BK(Speed);
         delay(Slide_Haf_Block);
         AO();
       while(1){
       if((millis()-myTime) >=(Block_FD_Pass_Time) ){ AO(); break; }  //แก้ตัวเลขให้เดิน 1 ช่องพอดี
       }
}
void Block_FD_Pass(){
  delay(100);
  myTime=millis();
  FD(Speed);
  while(1){
    if( digitalRead(Swicth) == SensorSwicth){    //เช็คสพาน
         Bride = 1 ;
         FD_Loop = 0; 
         Tack = 1 ;
         
         AO(); break;
       }else if((millis()-myTime) >= Block_FD_Pass_Time ){
         AO(); break; 
       }  //แก้ตัวเลขให้เดิน 1 ช่องพอดี
    
  }
  
  AO();
}

void Block_BK_Pass(){
  AO();
  delay(100);
  myTime=millis();
  BK(Speed);
  while(1){
    if((millis()-myTime) >=Block_FD_Pass_Time){ break; AO();}  //แก้ตัวเลขให้เดิน 1 ช่องพอดี
  }
  AO();
}
void Slide_L_Check (){
   AO();
   motor(1, Speed);
   motor(2, -Speed);
   motor(3, -Speed);
   motor(4, Speed);
   delay(100);
   AO();
   delay(100);
   myTime=millis();
   motor(1, -Speed);
   motor(2, Speed);
   motor(3, Speed);
   motor(4, -Speed);
    while(1){ 
        if(millis()- myTime >= Slide_TimeSet || analogRead(L)<SensorL || analogRead(BL)<SensorBL){ //หาเช็คระยะข้างเลยเวลา
         AO();
         DistanTime = millis()- myTime; 
         break ;
        }
      }
    AO();
    oled.text(5,6," DistanTime");
    oled.text(6,6,"%d",DistanTime);
    oled.show();
     delay(200);
   if(DistanTime < Slide_TimeSet){  //เจอสไลด์กลับ
     TrimL();
     delay(100);
     motor(1, Speed);
     motor(2, -Speed);
     motor(3, -Speed);
     motor(4, Speed);
     delay(Slide_Haf_Block);
     Tack = 1 ;
     delay(100);
   }else{      //ไม่เจอกลับมาเพื่อรอเลี้ยว
   AO();
   delay(100);
   motor(1, Speed);
   motor(2, -Speed);
   motor(3, -Speed);
   motor(4, Speed);
   delay(DistanTime);
   AO();
   Tack = 0 ;
   }
   AO();
   
}
void Slide_R_Check (){
   AO();
   motor(1, -Speed);
   motor(2, Speed);
   motor(3, Speed);
   motor(4, -Speed);
   delay(100);
   AO();
   delay(100);
   myTime=millis();
   motor(1, Speed);
   motor(2, -Speed);
   motor(3, -Speed);
   motor(4, Speed);
    while(1){ 
        if(millis()- myTime >= Slide_TimeSet || analogRead(R2)<SensorR2 || analogRead(BR)<SensorBR){ //หาเช็คระยะข้างเลยเวลา
         AO();
         DistanTime = millis()- myTime;
         delay(100);
         break ;
        }
      }
    AO();
    oled.text(5,6," DistanTime");
    oled.text(6,6,"%d",DistanTime);
    oled.show();
    delay(200);
   if(DistanTime <  Slide_TimeSet){  //เจอสไลด์กลับ
     TrimR();
     delay(100);
     motor(1, -Speed);
     motor(2, Speed);
     motor(3, Speed);
     motor(4, -Speed);
     delay(Slide_Haf_Block);
     Tack = 1 ;
   }else{      //ไม่เจอกลับเลี้ยวก่อน
   AO();
   delay(100);
   motor(1, -Speed);
   motor(2, Speed);
   motor(3, Speed);
   motor(4, -Speed);
   delay(DistanTime);
   AO();
   Tack = 0 ;
   }
   AO();
}
void FD_Trim(){
  AO();
  BK(35);
  delay(300);
  AO();
  delay(100);
  FD(30);
  while(1){
  if(analogRead(L)<SensorL && analogRead(R2)<SensorR2){
    AO();
    break;
  }
  else if(analogRead(L)>SensorL && analogRead(R2)<SensorR2){
    SR(25);
  }else if(analogRead(L)<SensorL && analogRead(R2)>SensorR2){
    SL(25);
  }
  }
  AO();

}
void TrimL(){

  delay(100);
       
  while(1){
   if(analogRead(L)<SensorL && analogRead(BL)<SensorBL){
    AO();
    break;
    }
    else if(analogRead(L)>SensorL && analogRead(BL)<SensorBL){
      SL(30);
    
    }else if(analogRead(L)<SensorL && analogRead(BL)>SensorBL){
      SR(30);
      
    }else{
      motor(1, -Speed);
      motor(2, Speed);
      motor(3, Speed);
      motor(4, -Speed);
    }
  }
  
  
  AO();
}
void TrimR(){
  delay(100);
  while(1){
  if(analogRead(R2)<SensorR2  && analogRead(BR)<SensorBR){
    AO();
    break;
  }
  else if(analogRead(R2)>SensorR2 && analogRead(BR)<SensorBR){
    SR(30);
  }else if(analogRead(R2)<SensorR2 && analogRead(BR)>SensorBR){
    SL(30);
  }else{
    motor(1, Speed);
    motor(2, -Speed);
    motor(3, -Speed);
    motor(4, Speed);
  }
 }
  AO();
}

void TurnL(){
  delay(100);
  myTime=millis();
  SL(TurnSpeed);
  while(1){
    if((millis()-myTime) >= TurnLTime){ break; AO();}  //เลี้ยวแก้ตัวเลขเวลา
  }
  AO();
}
void TurnR(){
  delay(100);
  myTime=millis();
  SR(TurnSpeed);
  while(1){
    if((millis()-myTime) >=TurnRTime){ break; AO();}  //เลี้ยวแก้ตัวเลขเวลา
  }
  AO();
}
void loop(void) {
  AO();
}
void Read_Color_Box(){ //อ่าค่าสีแสดงจอแบบละเอียด
   
 for(int i = 0 ; i<=10; i++){
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);
  oled.clear();
  oled.text(0,0,"Lux");
  oled.text(0,5,"%d", lux);
  oled.text(1,0,"R");
  oled.text(1,5,"%d", r);
  oled.text(2,0,"G");
  oled.text(2,5,"%d", g);
  oled.text(3,0,"B");
  oled.text(3,5,"%d", b);
  oled.text(4,0,"Temp");
  oled.text(4,5,"%d", colorTemp);
  if(i>=1){
  float x = (r+g+b)/3 ;
  if( r>x &&  r>=g && r>=b && lux <= 1 ){ 
    oled.text(6,0,"Color");
    oled.text(6,6,"Red");
    beep(500);
    box_red();                          //ปล่อยสีแดง
    BK_Loop = 1;
    FD_Loop = 0;
    box_R = 1 ;
    i = 10 ;
  }else if( b>x && b>r && b>=g && lux <= 1){
    oled.text(6,0,"Color");
    oled.text(6,6,"Blue");
    beep(500);
    box_blue();                         //ปล่อยสีน้ำเงิน
    BK_Loop = 1;
    FD_Loop = 0;
    box_R = 1;
    i = 10 ;
  }else if( (b<r && b<g) && lux >= 3 && colorTemp <=4000 ){
    oled.text(6,0,"Color");
    oled.text(6,6,"Yellow");
    beep(500);
    box_yellow();                          //ปล่อยสีเหลือง
    BK_Loop = 1;
    FD_Loop = 0;
    box_Y = 1 ;
    i = 10 ;
  }else if( g>x && g>=r && g>=b && lux >= 2 && colorTemp <=5500){
    oled.text(6,0,"Color");
    oled.text(6,6,"Green");
    beep(500);
    box_green();                          //ปล่อยสีเขียว
    BK_Loop = 1;
    FD_Loop = 0;
    box_G = 1 ;
    i = 10 ;
  }else{
    oled.text(6,0,"Color");
    oled.text(6,6,"White");
    if(Tack== 1 ){ //เจอช่องเช็คพอย
      BK_Loop = 0;
      BK_Loop = 1;
    }else{
      FD_Loop = 1;
      BK_Loop = 0;
    }
    i = 10 ;
  }
  oled.show();
  check_box();
  }
  delay(100);
 }
}

void Read_Color(){ //อ่าค่าสีแสดงจอแบบละเอียด
   beep(1000);
 while(1){
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);
  oled.clear();
  oled.text(0,0,"Lux");
  oled.text(0,5,"%d", lux);
  oled.text(1,0,"R");
  oled.text(1,5,"%d", r);
  oled.text(2,0,"G");
  oled.text(2,5,"%d", g);
  oled.text(3,0,"B");
  oled.text(3,5,"%d", b);
  oled.text(4,0,"Temp");
  oled.text(4,5,"%d", colorTemp);
  float x = (r+g+b)/3 ;
  if( r>x &&  r>=g && r>=b && lux <= 1 ){ 
    oled.text(6,0,"Color");
    oled.text(6,6,"Red");
  }else if( b>x && b>r && b>=g && lux <= 1){
    oled.text(6,0,"Color");
    oled.text(6,6,"Blue");
  }else if( (b<r && b<g) && lux >= 3 && colorTemp <=4000 ){
    oled.text(6,0,"Color");
    oled.text(6,6,"Yellow");
  }else if( g>x && g>=r && g>=b && lux >= 2 && colorTemp <=5500){
    oled.text(6,0,"Color");
    oled.text(6,6,"Green");
  }else{
    oled.text(6,0,"Color");
    oled.text(6,6,"White");
  }
  oled.show();
  delay(100);
 }
}

void Read_Sensor(){ //อ่าค่าเซนเซอร์แสดงจอ
   beep(1000);
 while(1){
   oled.clear();
   oled.text(0,0,"L");
   oled.text(0,5,"%d", analogRead(L));
   oled.text(1,0,"R2");
   oled.text(1,5,"%d", analogRead(R2));
   oled.text(2,0,"BL");
   oled.text(2,5,"%d", analogRead(BL));
   oled.text(3,0,"BL");
   oled.text(3,5,"%d", analogRead(BR));
   uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);
  float x = (r+g+b)/3 ;
  if( r>x &&  r>=g && r>=b && lux <= 1 ){ 
    oled.text(6,0,"Color");
    oled.text(6,6,"Red");
  }else if( b>x && b>r && b>=g && lux <= 1){
    oled.text(6,0,"Color");
    oled.text(6,6,"Blue");
  }else if( (b<r && b<g) && lux >= 3 && colorTemp <=4000 ){
    oled.text(6,0,"Color");
    oled.text(6,6,"Yellow");
  }else if( g>x && g>=r && g>=b && lux >= 2 && colorTemp <=5500){
    oled.text(6,0,"Color");
    oled.text(6,6,"Green");
  } 
 oled.show();
 delay(100);
 }
}
void box_green() {
  servo(ServoBG, 0);
  delay(500);
  servo(ServoBG, 90);
  delay(500);
}
void box_blue() {
  servo(ServoBG, 180);
  delay(500);
  servo(ServoBG, 90);
  delay(500);
}
void box_yellow() {
  servo(ServoYR, 0);
  delay(500);
  servo(ServoYR, 90);
  delay(500);
}
void box_red() {
  servo(ServoYR, 180);
  delay(500);
  servo(ServoYR, 90);
  delay(500);
}
void set_servo() {
  servo(flag, 90);
  servo(ServoYR, 90);
  servo(ServoBG, 90);
  delay(500);
}
void upflag() {
  servo(flag, 0);
  delay(300);
  servo(flag, 90);
  delay(500);
}
void check_box() {
  if(box_R  == 1 && box_G == 1 && box_B == 1 && box_Y == 1){
    upflag();
    while(1){
      
    }
  }
}

void turnaround (){
  motor(1, Speed);
  motor(2, Speed);
  motor(3, -Speed);
  motor(4, -Speed);   
  delay(Turnaround);
  AO();      

}