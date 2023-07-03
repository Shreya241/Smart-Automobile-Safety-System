
#include <Adafruit_BMP085.h>
#include <ADXL335.h>
#include <LiquidCrystal.h>
#include <TinyGPS.h>
#include <Wire.h>


Adafruit_BMP085 bmp;

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);
int val;
int temp=A5;
int pres=A4;
const int max_pressure = 50;  
const int rain_threshold = 500;
int rain = A3; 
const int relay_Pin = 2;
const int buzzer_Pin = 3;
const int ir_Sensor = 10;
const int alcohol_Sensor = 11;
const int vibration_Sensor = 12;
const int xpin = A0;
const int ypin = A1;
const int zpin = A2;
const float acceleration_threshold = 1.5;
const float temp_threshold = 35.0;
#define sensitivity_x 0.4
#define sensitivity_y 0.4
#define sensitivity_z 0.4
#define ADC_ref 5 // ADC reference Voltage
#define zero_x 1.799
#define zero_y 1.799
#define zero_z 1.799
//ADXL335 accel(xpin,ypin,zpin);
TinyGPS gps;
long lat,lon;
bool ir_status = LOW;
bool alcohol_Status = LOW;
bool vibration_Status = LOW;
 
void setup() {
  pinMode(relay_Pin, OUTPUT);
  pinMode(buzzer_Pin, OUTPUT);
  pinMode(ir_Sensor, INPUT);
  pinMode(alcohol_Sensor, INPUT);
  pinMode(vibration_Sensor, INPUT);
  pinMode(xpin,INPUT);
  pinMode(ypin,INPUT);
  pinMode(zpin,INPUT);
  //pinMode(temp,INPUT);
  pinMode(rain,INPUT);
  //pinMode(pres,INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("ACCIDENT DETECTION");
  lcd.setCursor(3,2);
  lcd.print("SYSTEM");
 
}

void loop() {
  ir_status = digitalRead(ir_Sensor);
  delay(100);
  if(ir_status == 1)
  {
    digitalWrite(buzzer_Pin, LOW);  
    delay(200);
    lcd.clear();
    lcd.print("Seat Belt");
    lcd.setCursor(3,2);
    lcd.print("Detected");
    delay(200);  
 
    while(1)
    {
      alcohol_Status = digitalRead(alcohol_Sensor);
      delay(100);  
      if(alcohol_Status == 0)
      {
       digitalWrite(buzzer_Pin, LOW);  
       delay(100);
       lcd.clear();
       lcd.print("Alcohol not");
       lcd.setCursor(3,2);
       lcd.print("Detected");
       delay(200);  
       digitalWrite(relay_Pin, HIGH);  
       delay(100);  
       while(1)  
       {
        lcd.clear();
        lcd.print("Vehicle Started");
        delay(200);    
        while(1)
        {  
          vibration_Status = digitalRead(vibration_Sensor);
          delay(100);  
            if(vibration_Status ==  0)
          {
            
            if(bmp.readTemperature() < 7){
            lcd.clear();
            lcd.print("Low temp!!!");
            lcd.setCursor(0,1);
            lcd.print("check tyres");
            delay(200);
            }

            if(bmp.readPressure()<137800){
            lcd.clear();
            lcd.print("Low pressure");
            lcd.setCursor(0,1);
            lcd.print("tyre blowout expected");
            delay(200);
            }
           
            int x_raw = analogRead(xpin);
            int y_raw = analogRead(ypin);
            int z_raw = analogRead(zpin);
            float xaccel = (x_raw - 512) * 0.0039 * 9.81;
            float yaccel = (y_raw - 512) * 0.0039 * 9.81;
            float zaccel = (z_raw - 512) * 0.0039 * 9.81;
            float acceleration = sqrt(xaccel * xaccel + yaccel * yaccel + zaccel * zaccel);
            float xv=(x_raw /1024.0*ADC_ref-zero_x)/sensitivity_x;
            float yv=(y_raw /1024.0*ADC_ref-zero_x)/sensitivity_x;
            float zv=(z_raw /1024.0*ADC_ref-zero_x)/sensitivity_x;
            float angle =atan2(-yv,-zv)*57.2957795+180;
            if (acceleration > acceleration_threshold) {    
              lcd.clear();
              lcd.print("Warning: sudden");
              lcd.setCursor(0,1);
              lcd.print("change in acc");
              digitalWrite(buzzer_Pin, HIGH); 
              delay(500);
            }
            if(angle>45){
              lcd.clear();
              lcd.print("Tilt: ");
              lcd.print(angle);
              lcd.setCursor(0,1);
              lcd.print("Roll over predicted");
              digitalWrite(buzzer_Pin, HIGH); 
              delay(500);       
            }
           
            int rain_analog = analogRead(rain);
            lcd.clear();
            lcd.print("Heavy rain");
            lcd.setCursor(0,1);
            lcd.print("slow down");
            digitalWrite(buzzer_Pin, HIGH); 
            delay(500);
            /*if(rain_analog>200)
            {
              lcd.clear();
              lcd.print("Heavy rain");
              lcd.setCursor(0,1);
              lcd.print("slow down");
              digitalWrite(buzzer_Pin, HIGH); 
              delay(500);
            }*/
            /*val = analogRead(temp);
            float mv = (val/1024.0)*5000;
            float cel  = mv/10;
            if(cel>temp_threshold){
              lcd.clear();
              lcd.print("Warning: ");
              lcd.setCursor(0,1);
              lcd.print("extreme weather");
              digitalWrite(buzzer_Pin, HIGH); 
              delay(500);
            }*/
            
           /*float val = analogRead(pressure);
           float pkPa = (val/(float)1023+0.095)/0.009;
           if(pkPa<17){
            lcd.clear();
            lcd.print(pkPa);
            lcd.setCursor(0,1);
            lcd.print("possible tire blow");
            digitalWrite(buzzer_Pin, HIGH); 
            delay(500);
           }*/

           
           
           /*lcd.clear();
           lcd.print("Accident");
           lcd.setCursor(3,2);
           lcd.print("not detected");
           delay(200);*/
          }
          if(vibration_Status ==  1)
          {
           lcd.clear();
           lcd.print("Accident Detected");
           lcd.setCursor(3,2);
           lcd.print("Sending Msg");
           delay(200);    
           Serial.println("AT+CMGF=1");    
           delay(100);  // Delay of 1000 milli seconds or 1 second
           Serial.println("AT+CMGS=\"+9198688348201\"\r");          
           delay(100);
           Serial.println("Accident Detected ");
           Serial.println("GPS location");
           while(1)
           {
            gps_read();
           }
          }
          else
          {
           /* Do nothing */
          }
        }                
       }
      }
      else
      {
        lcd.clear();
        lcd.print("Alcohol ");
        lcd.setCursor(3,2);
        lcd.print("Detected");
        delay(200);  
        digitalWrite(relay_Pin, LOW);  
        delay(100);              
        digitalWrite(buzzer_Pin, HIGH);  
        delay(100);        
      }
    }
  }
  else
  {
    lcd.clear();
    lcd.print("Seat Belt");
    lcd.setCursor(3,2);
    lcd.print("not Detected");
    digitalWrite(relay_Pin, LOW);  
    delay(100);              
    digitalWrite(buzzer_Pin, HIGH);  
    delay(100);          
  }
}
 
 
void gps_read()
{
 byte a;
 
  if(Serial.available())  
  {
    a=Serial.read();
   
   //Serial.write(a);
   
    while(gps.encode(a))   // encode gps data
    {
      gps.get_position(&lat,&lon); // get latitude and longitude
   
      Serial.println("Position: ");
      Serial.print("lat:");
      Serial.println((lat*0.000001),8);
      Serial.print("log:");
      Serial.println((lon*0.000001),8);
    }
  }
}
 
 
