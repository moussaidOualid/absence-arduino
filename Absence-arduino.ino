#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#define SS_PIN D4
#define RST_PIN D2
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
WiFiClient client;
MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];
byte knownTac[3][4] = {{35,247,239,27},{43,150,90,10},{252,190,186,121}};//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
boolean KNOW = true;               //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//WIFI data
const char* ssid     = "AndroidAP3678";      // SSID
const char* password = "44abcd96";      // Password
const char* host = "192.168.43.227";  // IP serveur 
const int   port = 8080;            // Port serveur 


void setup() { 
  Serial.begin(9600);
  // Connection wifi
   Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");      
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
////////////////////////
  
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  
  
}
 
void loop() {
   if (!client.connect(host, port)) {
      Serial.println("connection failed");
      return;
    }

  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;


 
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    // Serial.println(nuidPICC[i]); 
    }
   
   
   for(int i=0 ;i<4;i++) Serial.println(nuidPICC[i]); 
   
                     

                     //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                     for(int i=0 ;i<3;i++)
                    {
                     
  if (knownTac[i][0] != nuidPICC[0] || 
    knownTac[i][1] != nuidPICC[1] || 
    knownTac[i][2] != nuidPICC[2] || 
    knownTac[i][3] != nuidPICC[3])
    {
      
   KNOW = false;
     
  }else{
   
  KNOW = true; 
  
  }    
                 if(KNOW) break; 
    }
    
  if(KNOW){                            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Serial.print("OPEN THE DOOR");
   
    // Envoyer la requete au serveur 
 
 String data="";
 data = "tag0="+String(nuidPICC[0])+"&tag1=" +String(nuidPICC[1])+"&tag2=" + String(nuidPICC[2]) + "&tag3=" +String(nuidPICC[3]);
if (client.connect("192.168.43.227",8080)) { // REPLACE WITH YOUR SERVER ADDRESS
   Serial.print("CONECTED");
    client.println("POST /save HTTP/1.1"); 
    client.println("Host: 192.168.43.227"); // SERVER ADDRESS HERE TOO
    client.println("Content-Type: application/x-www-form-urlencoded"); 
    client.print("Content-Length: "); 
    client.println(data.length()); 
    client.println(); 
    client.print(data); 
  } 
     unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
       
        return;
      }
    } 
    
      // Lire la reponse du serveur
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
      
  delay(2000); 
    }
  }else{
    Serial.print("UnKNOW ID. NO ACCESS");
  }  
  }
  else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

}

 
