#include <dht.h>


#include <stdlib.h>
dht DHT;
#define DHT11_PIN 7     // what pin we're connected to 

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
/***********************/

/* change with your data*/

String apiKey = "6EI741MIG67UGHVV";               // Change with your apikey

String ssid = "harsh";                            // change with your wifi name

String password = "rishu2734";                   // Change with your wifi password

String channelID = "1114497"; // Enter your Actuator1 Channel ID here
/***********************/

LiquidCrystal lcd(13, 8, 5, 4, 3, 2);

//Variables of pulse sensor
//int fadePin = 8;                  // pin to do fancy classy fading blink at each beat
//int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

float t=0;
char data = 0;
float temp = 0;
float h=0;
float rt=0;

//  Variables of pulse sensor
//int PulseSensorPurplePin = 4;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
//int LED13 = 13;                    //  The on-board Arduion LED


int Signal;                // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 550;            // Determine which Signal to "count as a beat", and which to ingore.

int ecg_value=0;              //ecg variable

SoftwareSerial ser(10, 12); // RX, TX

void setup()

{
//pinMode(LED13,OUTPUT);
pinMode(8, INPUT);                // Setup for leads off detection LO +    for ECG
pinMode(11, INPUT);               // Setup for leads off detection LO -    for ECG
Serial.begin(9600);

ser.begin(115200);

lcd.begin(16,2);  //16 by 2 character display
lcd.setCursor(0,0);
lcd.print("WELCOME TO DIY");
lcd.setCursor(0,1);
lcd.print("SMART WORLD");
delay(2000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Patient Health");
lcd.setCursor(0,1);
lcd.print("MONITORING");
delay(1000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("INITIALISING....");
delay(2000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("GETTING DATA....");


Serial.println("WELCOME TO DIY SMART WORLD");
Serial.println("");
Serial.println("Patient Health");
delay(3000);
Serial.println("Monitoring");
delay(7000);
Serial.println("");
Serial.println("Initialising...");
Serial.println("");
delay(5000);
Serial.println("Getting Data...");
delay(2000);
ser.println("AT");

delay(3000);

if(ser.find("OK"))

{

Serial.println("AT response is OK");
lcd.clear();
lcd.setCursor(0,0);
lcd.print("AT response OK");

while(connectWiFi() == false);

}
}

void loop()

{
    
t = analogRead(A0);  // Read sensor value and stores in a variable t

  Serial.print("Airquality = ");

  Serial.println(t);


int temp_val = analogRead(A1);
float mv = (temp_val/1024.0)*5000;
float cel = mv/10;
temp = (cel*9)/5 + 32;
Serial.print("Body Temperature: ");
Serial.println(temp);
//Serial.println(" F");

int chk = DHT.read11(DHT11_PIN);
h = DHT.humidity;
rt = DHT.temperature;   // Read temperature as Celsius (the default)
Serial.print("Area Humidity: ");
Serial.println(DHT.humidity);
//Serial.println(" %");
Serial.print("Area Temperature: ");
Serial.println(DHT.temperature);
//Serial.println(" C");

Signal = analogRead(A4);  // Read the PulseSensor's value.
Serial.print("PULSE RATE : ");                                              // Assign this value to the "Signal" variable.
Serial.println(Signal);                    // Send the Signal value to Serial Plotter.
//Serial.println(" BPM");
/*if(Signal > Threshold){                          // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
     digitalWrite(LED13,HIGH);
   } else {
     digitalWrite(LED13,LOW);                //  Else, the sigal must be below "550", so "turn-off" this LED.
   }
delay(10);*/

if((digitalRead(9) == 1)||(digitalRead(12) == 1)){                   //code for ECG
Serial.print("ECG VALUE : ");
Serial.println(ecg_value);
//Serial.println("No Value");
}
else{
// send the value of analog input 0:
ecg_value=analogRead(A3);
Serial.print("ECG VALUE : ");
Serial.println(ecg_value);
}
//Wait for a bit to keep serial data from saturating
delay(2000);                                                        //code end for ECG

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Airquality");
lcd.setCursor(0,1);
lcd.print(t);
delay(1000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Body Temp");
lcd.setCursor(0,1);
lcd.print(temp);
delay(1000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Area Humidity");
lcd.setCursor(0,1);
lcd.print(DHT.humidity);
delay(1000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Area Temp");
lcd.setCursor(0,1);
lcd.print(DHT.temperature);
delay(1000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Pulse Rate");
lcd.setCursor(0,1);
lcd.print(Signal);
delay(1000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("ECG VALUE");
lcd.setCursor(0,1);
lcd.print(ecg_value);



String cmd = "AT+CIPSTART=\"TCP\",\"184.106.153.149\",80";

ser.println(cmd);

delay(2000);

if(ser.find("Error"))

{

Serial.println("AT+CIPSTART error");

return;

}

  int command;
  // preparacao da string GET
  String getStr = "GET /channels/";
  getStr += channelID;
  getStr +="/fields/1/last";
  getStr += "\r\n";
  Serial.println("received from actuator is: ");
  Serial.println(getStr);

  String messageDown = sendThingSpeakGetCmd(getStr);
  if (messageDown[5] == 49)
  {
    command = messageDown[7]-48; 
    Serial.print("Command received: ");
    Serial.println(command);
  }
  

String SndStr = "GET /update?api_key=";

SndStr += apiKey;

SndStr +="&field1=";

SndStr += String(t);
SndStr +="&field2=";
SndStr += String(temp);
SndStr +="&field3=";
SndStr += String(rt);
SndStr +="&field4=";
SndStr += String(h);
SndStr +="&field5=";
SndStr += String(Signal);
SndStr +="&field6=";
SndStr += String(ecg_value);

SndStr += "\r\n\r\n";

cmd = "AT+CIPSEND=";

cmd += String(SndStr.length());

ser.println(cmd);

delay(5000);

if(ser.find(">"))

{

ser.print(SndStr);

}

else

{

ser.println("AT+CIPCLOSE");

Serial.println("AT+CIPCLOSE");

}

delay(1000);

}


String sendThingSpeakGetCmd(String getStr)
{
  String cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  
  Serial.print("enviado ==> lenght cmd: ");
  Serial.println(cmd);

  if(ser.find((char *)">"))
  {
    ser.print(getStr);
    Serial.print("enviado ==> getStr: ");
    Serial.println(getStr);
    delay(500);//tempo para processar o GET, sem este delay apresenta busy no próximo comando

    String messageBody = "";
    while (ser.available()) 
    {
      String line = ser.readStringUntil('\n');
      if (line.length() == 1) 
      { //actual content starts after empty line (that has length 1)
        messageBody = ser.readStringUntil('\n');
      }
    }
    Serial.print("MessageBody received: ");
    Serial.println(messageBody);
    return messageBody;
  }
  else
  {
    ser.println("AT+CIPCLOSE");     // alert user
    Serial.println("ESP8266 CIPSEND ERROR: RESENDING"); //Resend...
  } 
}




boolean connectWiFi(void)

{

ser.println("AT+CWMODE=3");

delay(1000);

String cmd="AT+CWJAP=\"";

cmd+=ssid;

cmd+="\",\"";

cmd+=password;

cmd+="\"";

ser.println(cmd);

delay(1000);

if(ser.find("OK"))

{
lcd.setCursor(0,1);
lcd.print("wifi connected");

Serial.println("wifi connection OK");

return true;

}else

{
lcd.setCursor(0,1);
lcd.print("wifi connection OK");

Serial.println("wifi connection error");

return false;

}
}
