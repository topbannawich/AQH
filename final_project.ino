#include <ESP8266WiFi.h>
#include <Wire.h>
// Config connect WiFi
#define WIFI_SSID "aqh"
#define WIFI_PASSWORD "12345678"
  
// Line config
#define LINE_TOKEN "18wsSMpVRoSFGhqMssfHj24DhkSNvPDrLdTiC3PuWNX"
int ppm;
int sensorValue;
int sum;
char disp_c[8] ;
const int SCLK_pin=D4;
const int RCLK_pin=D3;
const int DIO_pin =D2;
int  disp[8];
unsigned long start=millis();
unsigned long prev =0;
unsigned long waitMS=0;
void setup() {
  Serial.begin(115200);
  pinMode(RCLK_pin,OUTPUT);
  pinMode(DIO_pin,OUTPUT);
  pinMode(SCLK_pin,OUTPUT);
  WiFi.mode(WIFI_STA);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

}
int b=0;
int count=0;
void loop() {
 
 sensorValue = analogRead(A0); 
 showDisplay();
  if(b==0){
    b++;
    showText('O','P','E','N');
    wait(5000);     
  }
  else{
    if ( millis() > (prev + waitMS))
    {
      ppm=ppm+sensorValue;
      wait(1000);
      count++;
    }else if(count==5){
      sum=ppm/5;
      Serial.println(sum);
      count=0;
      ppm=0;
      showText((sum/1000)%10,(sum/100)%10,(sum/10)%10,sum%10); 
      if(sum>=79){
        Line_Notify("%E0%B8%AA%E0%B8%B8%E0%B9%88%E0%B8%A1%E0%B9%80%E0%B8%AA%E0%B8%B5%E0%B9%88%E0%B8%A2%E0%B8%87%E0%B8%95%E0%B9%88%E0%B8%AD%E0%B8%AA%E0%B8%B8%E0%B8%82%E0%B8%A0%E0%B8%B2%E0%B8%9E%20AQI%3A");
        wait(5000);
      }
      if(sum>100){
        Line_Notify("%E0%B8%A1%E0%B8%B5%E0%B8%9C%E0%B8%A5%E0%B8%81%E0%B8%A3%E0%B8%B0%E0%B8%97%E0%B8%9A%E0%B8%95%E0%B9%88%E0%B8%AD%E0%B8%AA%E0%B8%B8%E0%B8%82%E0%B8%A0%E0%B8%B2%E0%B8%9E%20AQI%3A");
        wait(5000);;
      }
      if(sum>200){
        Line_Notify("%E0%B8%A1%E0%B8%B5%E0%B8%9C%E0%B8%A5%E0%B8%81%E0%B8%A3%E0%B8%B0%E0%B8%97%E0%B8%9A%E0%B8%95%E0%B9%88%E0%B8%AD%E0%B8%AA%E0%B8%B8%E0%B8%82%E0%B8%A0%E0%B8%B2%E0%B8%9E%E0%B8%A1%E0%B8%B2%E0%B8%81%20AQI%3A");
        wait(5000);
      }
      if(sum>300){
        Line_Notify("%E0%B8%AD%E0%B8%B1%E0%B8%99%E0%B8%95%E0%B8%A3%E0%B8%B2%E0%B8%A2%E0%B8%95%E0%B9%88%E0%B8%AD%E0%B8%AA%E0%B8%B8%E0%B8%82%E0%B8%A0%E0%B8%B2%E0%B8%9E%20AQI%3A");
        wait(5000);;
      }
    }
     
  }
 
  
}
void Line_Notify(String message) {
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
   // showText('E','R','O','R');
    return;   
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Connection: close\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message+sum).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message+sum;
  // Serial.println(req);
  client.print(req);
    
  delay(20);

  // Serial.println("-------------");
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  // Serial.println("-------------");
}
void showText(char a, char b , char c,char d){
  disp_c[0] = d;
  disp_c[1] = c;
  disp_c[2] = b;
  disp_c[3] = a;
}
void showDisplay()
{
  setDisp();
  for(int i=0; i<8; i++)
  {
    setDigit(i,disp[i]);
  }
}

void setDigit(int dig, int character)
{
  int digits[]= {
    128,64,32,16,8,4,2,1        };

  //character set (0-9)0-9
  //            (10-19)0.-9.
  //            (20-45)A-Z
  //            (46-71)a-z
  //            (72)- (73) space
  int characters[]= {
    3,159,37,13,153,73,65,31,1,9,
    2,158,36,12,152,72,64,30,0,8,
    17,1,99,3,97,113,67,145,243,135,145,227,85,19,3,49,25,115,73,31,129,129,169,145,137,37,
    5,193,229,133,33,113,9,209,247,143,81,227,85,213,197,49,25,245,73,225,199,199,169,145,137,37,
    253,255        };

  digitalWrite(RCLK_pin, LOW);
  shiftOut(DIO_pin, SCLK_pin, LSBFIRST, characters[character]);
  shiftOut(DIO_pin, SCLK_pin, LSBFIRST, digits[dig]);
  digitalWrite(RCLK_pin, HIGH);
}

void setDisp()
{
  for (int i=0; i<8;i++)
  {
    int val = disp_c[i];
    if((val >= 32)&&(val <= 47)){ 
      switch (val){
      case 45 :
        val = 72;
        break;  
      default :
        val = 73;
        break;  
      }
    }
    else if((val >= 48)&&(val <= 57)) //0-9
    {
      val -= 48;
    }
    else if((val >= 65)&&(val <= 90)) //A-Z
    {
      val -= 45;
    }
    else if((val >= 97)&&(val <= 122)) //a-z
    {
      val -= 51;
    }

    disp[i] = val;

  }
}  
void wait( unsigned long milsec)
{
  prev = millis();
  waitMS = milsec;
}
