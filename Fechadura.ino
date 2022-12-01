#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

// RFID
#define SS_PIN 4
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

// LED RGB
#define LED_R 14
#define LED_G 27
#define LED_B 26

String ID;

int reley = 25;

const char* ssid = "fari";
const char* password = "pimba12345";
String servidor = "http://5aca-2804-14c-8793-8e03-85f9-a558-fec4-c077.sa.ngrok.io/api/auth/";

void setup()
{
  pinMode(RST_PIN, OUTPUT);
  pinMode(reley, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  Serial.begin(9600);  // Initiate a serial communication
  SPI.begin();         // Initiate  SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  
  WiFi.begin(ssid, password);
  Serial.print("Conectando Wifi ");
  
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  };
    
  Serial.println("");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.println();
}

void loop() 
{
  if (!mfrc522.PICC_IsNewCardPresent()) // Look for new cards
  {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) // Select one of the cards
  {
    return;
  }

  //Show UID on serial monitor
  Serial.print("UID tag :");
  String tag_code = "";
  byte letter;
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    tag_code.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    tag_code.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  tag_code.toUpperCase();

  
  if(WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    
    http.begin(servidor);
    http.addHeader("Content-Type", "application/json");
    int post_return = http.POST("{\"mac\":\"" + WiFi.macAddress() + "\", \"cartao\":\"" + tag_code + "\"}");
    
    Serial.println();
    Serial.print("Post: ");
    Serial.println(post_return);
    
    http.end();


    Serial.println();
    Serial.print("Message : ");
    
    if (post_return == 200 ||  tag_code.substring(1) == ID)
    {
      ID = tag_code.substring(1);

      Serial.print("ID: ");
      Serial.println(ID);
      
      Serial.println("Authorized access");
      Serial.println();
      delay(500);  //Aguarda 5 segundos
      digitalWrite(reley, HIGH);
      digitalWrite(LED_G, HIGH);
      delay(3000);
      digitalWrite(reley, LOW);
      digitalWrite(LED_G, LOW);
    }
    else
    {
      Serial.print("ID: ");
      Serial.println(ID);
      Serial.println(" Access denied");
      digitalWrite(LED_R, HIGH);
      delay(3000);
      digitalWrite(LED_R, LOW);
    }
  }
  else
  {
    pinMode(LED_B, HIGH);
    delay(200);
    pinMode(LED_B, LOW);
  }
}
