
#include <LiquidCrystal.h>
#include <stdio.h>

#include <Wire.h>
#include "dht.h"

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);


//LiquidCrystal lcd(6, 7, 5, 4, 3, 2);



#define dht_apin A2
dht DHT;

int buzzer  = A3;


int tempc=0,humc=0;
char ldrs='x';
int ldrv=0;
int gasv=0;
 
unsigned char rcv,gchr='x';
 
char pastnumber[11];

int sti=0;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete



void okcheck()
{
  unsigned char rcr;
  do{
      rcr = Serial.read();
    }while(rcr != 'K');
}

void beep()
{
  digitalWrite(buzzer, LOW);delay(2000);digitalWrite(buzzer, HIGH);
}
void setup() 
{
 Serial.begin(1200);//serialEvent();

    pinMode(buzzer, OUTPUT);

    digitalWrite(buzzer, HIGH); 
    
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on  
  lcd.print("Grain Monitoring");
  lcd.setCursor(0,1);
  lcd.print("   System  ");
     delay(3000);
  
  gsminit();
  
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("T:");//2,0 
   lcd.setCursor(8,0);
   lcd.print("H:");//10,0

   lcd.setCursor(0,1);
   lcd.print("L:");//2,1
   lcd.setCursor(8,1);
   lcd.print("G:");//10,1
}

int sts1=0;
int cntlmk=0;

void loop() 
{
   DHT.read11(dht_apin);

   tempc = DHT.temperature;
   humc  = DHT.humidity;

   lcd.setCursor(2,0);  convertl(tempc);
   lcd.setCursor(10,0); convertl(humc);

   if(tempc > 40)
     {
      beep();
      lcd.setCursor(15,1);lcd.write('G');
       delay(5000);delay(4000);delay(4000);
      Serial.write("AT+CMGS=\"");
      Serial.write(pastnumber);
      Serial.write("\"\r\n");  delay(3000);  
      Serial.write("High Temp:");Serial.print(tempc);
      Serial.print("-Temperature has been incereased_Try to control");
      Serial.write(0x1A);delay(5000);delay(4000);delay(4000);
      lcd.setCursor(15,1);lcd.write(' ');
     }
   
   if(humc > 70)
     {
      beep();
      lcd.setCursor(15,1);lcd.write('G');
       delay(4000);delay(4000);delay(4000);
      Serial.write("AT+CMGS=\"");
      Serial.write(pastnumber);
      Serial.write("\"\r\n");  delay(3000);  
      Serial.write("High Hum:");Serial.print(humc);
      Serial.print("-Humidity has been incereased_Try to control");
      Serial.write(0x1A);delay(4000);delay(4000);delay(4000);
      lcd.setCursor(15,1);lcd.write(' ');
     }
  /*
   if(digitalRead(ldr) == HIGH)
     {
      lcd.setCursor(2,1);lcd.print("Dark ");
      ldrs='d';
     }
   if(digitalRead(ldr) == LOW)
     {
      lcd.setCursor(2,1);lcd.print("Light");
      ldrs='l';
     }
  */
   ldrv = analogRead(A0);
   ldrv = (1024 - ldrv);   
   lcd.setCursor(2,1);convertl(ldrv);
   
   if(ldrv < 100)
     {  
            beep();
       sts1++;
       if(sts1 >= 2){sts1=2;}
       if(sts1 == 1)
         {
          lcd.setCursor(15,1);lcd.write('G'); 
            delay(4000); delay(4000);  delay(4000);
          Serial.write("AT+CMGS=\"");
          Serial.write(pastnumber);
          Serial.write("\"\r\n");  delay(3000);  
          Serial.write("LDR Dark:");Serial.print(ldrv);
          Serial.print("-Power OFF_Please Turn ON generator");
          Serial.write(0x1A);
            delay(5000);delay(4000);delay(4000); 
          lcd.setCursor(15,1);lcd.write(' ');
         }
     }    
   if(ldrv > 100)
     {
      sts1=0;  
     }
     
   gasv = analogRead(A1);
   lcd.setCursor(10,1);convertl(gasv);
   
   if(gasv > 400)
     {  
            beep();
       lcd.setCursor(15,1);lcd.write('G'); 
        delay(4000); delay(4000);  delay(4000);
       Serial.write("AT+CMGS=\"");
       Serial.write(pastnumber);
       Serial.write("\"\r\n");  delay(3000);  
       Serial.write("High Gas:");Serial.print(gasv);
       Serial.print("-Please Control Gas/Fire");
       Serial.write(0x1A);
        delay(5000);delay(4000);delay(4000); 
       lcd.setCursor(15,1);lcd.write(' ');
     }    
     
   delay(1000);
   cntlmk++;
   if(cntlmk >= 80)
     {cntlmk=0;
       
       lcd.setCursor(15,1);lcd.write('G');
        delay(4000); delay(4000);  delay(4000);
       Serial.write("AT+CMGS=\"");
       Serial.write(pastnumber);
       Serial.write("\"\r\n");  delay(3000);
       Serial.write("Temp:");Serial.print(tempc);  
       Serial.write("_Hum:");Serial.print(humc);
       Serial.write("_LDR:");Serial.print(ldrv);
       Serial.write("_Gas:");Serial.print(gasv);
       Serial.write(0x1A);
        delay(5000);delay(4000);delay(4000); 
       lcd.setCursor(15,1);lcd.write(' ');
     }
}


void serialEvent() 
{
   while(Serial.available()) 
        {         
         char inChar = (char)Serial.read();
           if(inChar == '*')
             {
             gchr = 's';
             }
                 
        }
}



int readSerial(char result[]) 
{
  int i = 0;
  while (1) 
  {
    while (Serial.available() > 0) 
    {
      char inChar = Serial.read();
      if (inChar == '\n') 
         {
          result[i] = '\0';
          Serial.flush();
          return 0;
         }
      if (inChar != '\r') 
         {
          result[i] = inChar;
          i++;
         }
    }
  }
}

void gsminit()
{
  Serial.write("AT\r\n");                   okcheck();
  Serial.write("ATE0\r\n");                 okcheck();
  Serial.write("AT+CMGF=1\r\n");            okcheck();
  Serial.write("AT+CNMI=1,2,0,0\r\n");      okcheck();
  Serial.write("AT+CSMP=17,167,0,0\r\n");   okcheck();
    
  lcd.clear();
  lcd.print("SEND MSG STORE");
  lcd.setCursor(0,1);
  lcd.print("MOBILE NUMBER");  
  do{
     rcv = Serial.read();
    }while(rcv != '*');
     readSerial(pastnumber);
     pastnumber[10]='\0';
     
  lcd.clear();
  lcd.print(pastnumber);

    Serial.write("AT+CMGS=\"");
    Serial.write(pastnumber);
    Serial.write("\"\r\n"); delay(3000);
    Serial.write("Mobile no. registered\r\n");
    Serial.write(0x1A);    
    delay(4000);delay(4000);delay(4000);  
}

void converts(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
   Serial.write(a);
   Serial.write(c);
   Serial.write(e); 
   Serial.write(g);
   Serial.write(h);
}

void convertl(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
//   lcd.write(a);
   lcd.write(c);
   lcd.write(e); 
   lcd.write(g);
   lcd.write(h);
}

void convertk(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
  // lcd.write(a);
  // lcd.write(c);
  // lcd.write(e); 
  // lcd.write(g);
   lcd.write(h);
}
