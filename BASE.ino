
#include <SoftwareSerial.h>
#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <DHT.h>
//#define _SS_MAX_RX_BUFF 256 
#define ESP8266_rxPin 3
#define ESP8266_txPin 4

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22 // DHT 22  (AM2302)

// Define color code for TFT display <Easy to use>
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF


#include <avr/power.h>

//SSID + KEY
const char SSID_ESP[] = "Dialog 4G";
const char SSID_KEY[] = "J21H9H3GJJ9";
//"J21H9H3GJJ9"
// URLs
String URL_webhost = "GET http://weatherstation.dx.am/Index.php?humidity=";
//String URL_webhost = "GET http://192.168.197.1/SensorValues/from_micro.php?humidity=";
//MODES
const char CWMODE = '1';//CWMODE 1=STATION, 2=APMODE, 3=BOTH
const char CIPMUX = '1';//CWMODE 0=Single Connection, 1=Multiple Connections

SoftwareSerial ESP8266(ESP8266_rxPin, ESP8266_txPin);// rx tx

//DEFINE ALL FUNCTIONS HERE
boolean setup_ESP();
boolean read_until_ESP(const char keyword1[], int key_size, int timeout_val, byte mode);
void timeout_start();
boolean timeout_check(int timeout_ms);
void serial_dump_ESP();
boolean connect_ESP();
void connect_webhost();

//DEFINE ALL GLOBAL VAARIABLES HERE
//int unit_id = 1;
//int sensor_value = 123;
//int sensor_readBack = 0;
unsigned long time_now = 0;
int cycleTime = 2000;
unsigned long timeout_start_val;
char scratch_data_from_ESP[20];//first byte is the length of bytes
char payload[150];
byte payload_size=0, counter=0;
char ip_address[16];
String URL_withPacket = "                                                                                                    ";
String payload_closer = " HTTP/1.0\r\n\r\n\r\n";
unsigned long multiplier[] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
//int LED_value = 0;

char t1_from_ESP[5];//time
char d1_from_ESP[11];//temp
char d2_from_ESP[11];//humidity
char d3_from_ESP[11];//rainfall

//DEFINE KEYWORDS HERE
const char keyword_OK[] = "OK";
const char keyword_Ready[] = "Ready";
const char keyword_no_change[] = "no change";
const char keyword_blank[] = "#&";
const char keyword_ip[] = "192.";
const char keyword_rn[] = "\r\n";
const char keyword_quote[] = "\"";
const char keyword_carrot[] = ">";
const char keyword_sendok[] = "SEND OK";
const char keyword_linkdisc[] = "Unlink";
//const char keyword_t1[] = "t1";
//const char keyword_d1[] = "d1";
//const char keyword_d2[] = "d2";
//const char keyword_d3[] = "d3";
//const char keyword_doublehash[] = "##";

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);
DHT dht(DHTPIN, DHTTYPE);



// char array to print to the screen
char tempPrintout[6];
char humPrintout[6];
char raindropPrintout[6];
float h;
float t;
String raindropVal; 

void setup(){//        SETUP     START
  //Pin Modes for ESP TX/RX
  pinMode(ESP8266_rxPin, INPUT);
  pinMode(ESP8266_txPin, OUTPUT);
  
 
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);
 
 // write the static text to the screen
 // set the font color to white
 TFTscreen.stroke(255,255,255);
 // set the font size
 TFTscreen.setTextSize(1.5);
 // write the text to the top left corner of the screen
 TFTscreen.text("Temp (F)",0,20);
   // write the text to the top left corner of the screen
 TFTscreen.text("Humidity (%)",0,40);
 // write the text to the top left corner of the screen
 TFTscreen.text("RainDrops Value",0,60);
 // ste the font size very large for the loop
 TFTscreen.setTextSize(2);
  dht.begin();
  ESP8266.begin(9600);//default baudrate for ESP
  ESP8266.listen();//not needed unless using other software serial instances
  Serial.begin(9600); //for status and debug
  
  //delay(5000);//delay before kicking things off
  //setup_ESP();//go setup the ESP 
}//                    SETUP     END

void loop(){//         LOOP     START
  // Read the value of the temp/humidity sensor on D2
  //setup_ESP();
  
 // Reading temperature or humidity takes about 250 milliseconds!
 // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
 h = dht.readHumidity();
 t = dht.readTemperature();
 t = (t*1.8)+32; //C to F conversion
 raindropVal = String(analogRead(A0));

   
  connect_webhost();
  
 String tempVal = doubleToString(t, 2);
 String humVal = doubleToString(h, 0);

 
// String sensorVal = String(1.234);

 // convert the reading to a char array
tempVal.toCharArray(tempPrintout, 6);
 humVal.toCharArray(humPrintout, 6);
 raindropVal.toCharArray(raindropPrintout, 6);
 TFTscreen.setTextSize(1.5);
 // set the font color
 TFTscreen.stroke(244,66,66);
 // print the sensor value
 TFTscreen.text(tempPrintout, 0, 30);
 TFTscreen.text(humPrintout, 0, 50);
 TFTscreen.text(raindropPrintout, 0, 70);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" Heat index: ");
  Serial.print(t);
  Serial.println("°F");
  Serial.print("Raindrops sensor value =");
  Serial.println(raindropVal);
  
  delay(7000);
 // erase the text you just wrote
 TFTscreen.stroke(0,0,0);
 TFTscreen.text(tempPrintout, 0, 10);
 TFTscreen.text(humPrintout, 0, 30);
 TFTscreen.text(raindropPrintout, 0, 50);
  
  
  
  
  
}//                    LOOP     END
//Rounds down (via intermediary integer conversion truncation)
String doubleToString(double input,int decimalPlaces){
if(decimalPlaces!=0){
String string = String((int)(input*pow(10,decimalPlaces)));
if(abs(input)<1){
if(input>0)
string = "0"+string;
else if(input<0)
string = string.substring(0,1)+"0"+string.substring(1);
}
return string.substring(0,string.length()-decimalPlaces)+"."+string.substring(string.length()-decimalPlaces);
}
else {
return String((int)input);
}
}
