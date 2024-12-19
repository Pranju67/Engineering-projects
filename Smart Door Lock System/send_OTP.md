```
#include <ESP8266WiFi.h>
const char* ssid = "VIRUS";
const char* password = "pssssbbb";

const char* host = "api.callmebot.com";
const int httpsPort = 443;

WiFiClient client;

int F = 0;
String passW = "";
String ID = "";

void setup()
{
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
}

void SendData()
{
  // Sending data over telegram
  WiFiClientSecure client;
  //client.setInsecure(); //
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort))
  {
    Serial.println("connection failed");
    return;
  }

//  String url = "/text.php?user=@Kirannaik92&text=OTP+for+ATM+System+is+:"; // -> For Telegram app
  String url = "/text.php?user=";
  url += ID;
  url += "&text=OTP+for+ATM+System+is:+";
  url += passW;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected())
  {
    String line = client.readStringUntil('y'); //checking for a not relevant character, apparently some timing issue
    if (line == "\r")
    {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("Apikey"))
  {
    Serial.println("esp8266/Arduino CI successfull!");//irrelevant
  }
  else
  {
    Serial.println("esp8266/Arduino CI has failed");//irrelevant
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
  F = 1;
}


void loop()
{
  if(Serial.available() > 0)
  {
    String InData = Serial.readString();
    delay(500);
    
    int InDataL = InData.length();
    char val = ',';
    int firstIndex = InData.indexOf(val);
    
    ID = InData.substring(0, firstIndex);
    passW = InData.substring(firstIndex + 1, InDataL);
    Serial.print("ID:");Serial.println(ID);
    Serial.print("passW:");Serial.println(passW);
    SendData();
  }
}

```