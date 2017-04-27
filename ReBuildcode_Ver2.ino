#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> 
#include <TouchScreen.h>
// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define TS_MINX 204
#define TS_MINY 195
#define TS_MAXX 948
#define TS_MAXY 910

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define DEBUG true

/*********************************************************************************************************************************************************************************************
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*                                                                     TFT Module necessary decelarations end here                                                                          */

/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*********************************************************************************************************************************************************************************************/



/**********************  WiFi SSID Name and Password **************************/

String Host_name = "517";
String password = "z7zhazha-_-||";



/********************** Functions to be performed when touched Button on the screen ****************************/

void resetWifiConfig();
void setButtons();

String TEMP_ABAD();  // live Temperature in Ahmedabad
String Note();      // live Note in India


void setup(void) {
  Serial.begin(9600);
  Serial1.begin(19200);
  tft.reset();
  tft.begin(0x9341);
  tft.fillScreen(BLACK);
  tft.setRotation(1); 
  resetWifiConfig();
  setButtons();  // Caliing the buttons.
}

void loop(void) {

  TSPoint p = ts.getPoint();   //
  pinMode(XP, OUTPUT);         //   For getting the coodinates 
  pinMode(XM, OUTPUT);         //   when we touch on the screen.
  pinMode(YP, OUTPUT);         //
  pinMode(YM, OUTPUT);         //
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)    // if touched
  {  
    int X = map(p.x, TS_MAXX, TS_MINX, 320,0);       // <----------------Serial Touch Detect
    int Y = map(p.y, TS_MINY, TS_MAXY, 240,0);
    if (DEBUG)
    {
      Serial.print("X = "); Serial.print(X);
      Serial.print("\tY = "); Serial.print(Y);
      Serial.print("\tPressure = "); Serial.println(p.z);
    }                                                 // --------------------------->

    if (X < 41 && X > -29 && Y < 260 && Y > 230)   //--------------------------------------------------------RESET button ,reset ESP8266 config
    {
      if (DEBUG)
        Serial.println("RESET");
        resetWifiConfig();
    }

    if (X < 248 && X > 180 && Y < 255 && Y > 220)  //------------------------------------------------------ Temperature Button
    {
      fetchTemp();
      setButtons();
    }

    if (X < 145 && X > 74 && Y < 256 && Y > 226)//------------------------------------------------------ Note Button
    {
     tft.fillRect(0, 55, 320, 185, BLACK);
     if (DEBUG)
     Serial.println("Note");
     tft.setCursor(0, 55);
     tft.setTextColor(WHITE);
     tft.setTextSize(3);
     tft.println( "Fetching note");
     tft.setCursor(0, 55);
     tft.println( Note());
     setButtons();
    }
  }
}

void resetWifiConfig()
{
  //Disconnecting previous networks
  tft.fillScreen(BLACK);
  tft.fillScreen(BLACK);
  tft.setCursor(40, 120);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.println("Resetting");
  sendData("AT+RST\r\n", 2000, DEBUG);
  sendData("AT+CWQAP\r\n", 5000, DEBUG); // Disconnect WiFi if connected.

  //Selecting Mode for WiFi
  tft.fillScreen(BLACK);
  tft.fillScreen(BLACK);
  tft.setCursor(70, 120);
  tft.setTextColor(WHITE);  tft.setTextSize(3);
  tft.println("Mode=3");
  sendData("AT+CWMODE=3\r\n", 1000, DEBUG); // configure as  both access point and station mode.

  //Searching for the wifi Connecrions.
  tft.fillScreen(BLACK);
  tft.fillScreen(BLACK);
  tft.setCursor(70, 80);
  tft.setTextColor(WHITE);  tft.setTextSize(2);
  tft.println("Searching");
  tft.setCursor(50, 120);
  tft.setTextColor(WHITE);  tft.setTextSize(2);
  tft.println("WiFi Networks");
  char inv = '"';
  delay(3000);
  
  sendData("AT+CWQAP\r\n", 4000, DEBUG); // again quit the connection if any.
  
  String conn = "AT+CWJAP";
  conn += "=";
  conn += inv;
  conn += Host_name;  // Host name
  conn += inv;
  conn += ",";
  conn += inv;
  conn += password; // Password
  conn += inv;
  conn += "\r\n\r\n";
  sendData(conn, 7000, DEBUG); // Join the connection.( AT+CWJAP = "Host_name","password" )
  
  tft.fillScreen(BLACK);
  tft.fillScreen(BLACK);
  tft.setCursor(70, 120);
  tft.setTextColor(WHITE);  tft.setTextSize(2);
  tft.println("Connected");
  setButtons();  // again calling buttons.
}


void setButtons()
{
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.setCursor(10, 20);
  tft.fillRect(0, 0, 80, 50, YELLOW);
  tft.println("RESET");

  tft.setCursor(100, 20);
  tft.fillRect(90, 0, 80, 50, YELLOW);
  tft.println("NOTE");

  tft.setCursor(190, 20);
  tft.fillRect(180, 0, 80, 50, YELLOW);
  tft.println("TEMP");
  
}

void fetchNote(){
     tft.fillRect(0, 55, 320, 185, BLACK);
     if (DEBUG)
     Serial.println("Note");
     tft.setCursor(0, 55);
     tft.setTextColor(WHITE);
     tft.setTextSize(3);
     tft.println( "Fetching note");
     tft.setCursor(0, 55);
     tft.println( Note());
  }

String TEMP_ABAD()
{
  char inv = '"';
  String add = "AT+CIPSTART";
  add += "=";
  add += inv;
  add += "TCP";   // type of connection
  add += inv;
  add += ",";
  add += inv;
  add += "api.thingspeak.com";  // host server
  add += inv;
  add += ",";
  add += "80";   // port 
  add += "\r\n";
  sendData(add, 3000, 0);

  String rest = "AT+CIPSEND=90";
  rest += "\r\n";   // carriage return and new line
  sendData(rest, 1000, 0);

  String hostt = "GET /apps/thinghttp/send_request?api_key=2RKQ0084TYEY35MC";
  hostt += "\r\n";
  hostt += "Host:api.thingspeak.com";
  hostt += "\r\n\r\n\r\n\r\n\r\n";
  ;
  String Temp = sendData(hostt, 2000, 0);
  int i = 0;
  Serial.println(Temp);

  while (Temp.charAt(i) != ':')
    i++;
  i++;
  Temp = Temp.substring(i);
  Temp = Temp.substring(0, 2);
  if (DEBUG)
    Serial.print(Temp);
  tft.fillRect(0, 55, 320, 185, BLACK);
  return (Temp);
}

void fetchTemp(){
      tft.fillRect(0, 55, 320, 185, BLACK);
      if (DEBUG)
      Serial.println("TEMP");
      tft.setCursor(0, 55);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.println( "Fetching temperature");
      tft.setCursor(0, 55);
      tft.println( TEMP_ABAD());
      tft.setCursor(0, 55);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.println( "deg C");
  }

String Note()
{
 char inv = '"';
  String add = "AT+CIPSTART";
  add += "=";
  add += inv;
  add += "TCP";
  add += inv;
  add += ",";
  add += inv;
  add += "api.thingspeak.com";  // Host_server_name
  add += inv;
  add += ",";
  add += "80";
  add += "\r\n";
  sendData(add, 3000, 0);  // Establishing a TCP Connection. ( AT+CIPSTART = "TCP","Host_server_name",80 )
  String rest = "AT+CIPSEND=90";
  rest += "\r\n";
  sendData(rest, 2000, 0); //Number of characters to send. ( "AT+CIPSEND=90" )
  String hostt = "GET /apps/thinghttp/send_request?api_key=64872MRT5NJ5JWME";
  hostt += "\r\n";
  hostt += "Host:api.thingspeak.com";
  hostt += "\r\n\r\n\r\n\r\n\r\n";
  String Note = sendData(hostt, 2000, 1);  // GET request ( GET /apps/thinghttp/send_request?api_key=XXXXXXXXXXXXXXXX 
                                            //               Host: Host_server_name ) 
/********************** Steps To filter the received data. ****************************/
  int i = 0;        
  if (DEBUG)
    Serial.println(Note);
  while (Note.charAt(i) != ':') // Character before required data starts.
    i++;
  i++;
  Note = Note.substring(i);
  int j = 0;
  while (Note.charAt(j) != 'C') // Character after required data ends.
    j++;
  Note = Note.substring(0, j);
  if (DEBUG)
    Serial.println(Note);
  tft.fillRect(0, 0, 320, 240, BLACK);
  return (Note);
}

String sendData(String command, const int Goldout, boolean debug)
{
  String response = "";
  Serial1.print(command); // send the read character to the Serial1
  long int Gold = millis();
  while ( (Gold + Goldout) > millis())
  {
    while (Serial1.available())
    {

      // The esp has data so display its output to the serial window
      char c = Serial1.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}


