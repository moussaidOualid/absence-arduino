#include <dht.h>
//#include <Ethernet.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
 WiFiClient client;
 dht DHT; // Create  DHT object
    #define DHT11_PIN D2//h et t
    #define HC D1//presence
    #define GAZ  A0 //gaz
const int LED = D0;
float t;
float h;
float g;
long previousMillis = 0;
unsigned long currentMillis = 0;
/*long interval = 3000; // READING INTERVAL
long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second =  1000; // 1000 milliseconds in a second
 */

const char* ssid     = "AndroidAP3678";      // SSID
const char* password = "44abcd96";      // Password
const char* host = "192.168.43.227";  // IP serveur 
const int   port = 80;            // Port serveur 
const int   watchdog = 5000;
int calibrationTime     = 30;
int pirState            = LOW;
int val                 = 0;
int p=0;
void setup() { 
  
  pinMode(LED, OUTPUT);
  pinMode(GAZ, INPUT);
  pinMode(DHT11_PIN, INPUT);
  pinMode(HC, INPUT);
  Serial.begin(115200);
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
  
  Serial.print("Calibration du détecteur (30 secondes) ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }

 
}

void loop(){
val=0;

   unsigned long currentMillis = millis();

  if ( currentMillis - previousMillis > watchdog ) {
    previousMillis = currentMillis;
    WiFiClient client;
  
    if (!client.connect(host, port)) {
      Serial.println("connection failed");
      return;
    }

    
      int readData = DHT.read11(DHT11_PIN); // Lire Les donnees
      t = DHT.temperature; // Prendre la valeur de la temperature
      h = DHT.humidity; // Prendre la valeur de l'humidite
       g = analogRead(GAZ); // Recevoir la valeur de gaz
  val = digitalRead(HC);
  p=0;
 
  if (val == HIGH) {
    delay(150);
    if (pirState == LOW) {
      pirState = HIGH;
      p=val;
    }
  } else {
    delay(300);
    if (pirState == HIGH){ 
      p=val;
      pirState = LOW;
    }
  }
 
    digitalWrite(LED, HIGH); //allumer L1
delay(2000); // attendre 2 seconde
digitalWrite(LED, LOW);
  
  
// Envoyer la requete au serveur 
String url="GET http://192.168.43.227/projet/add.php?t=";
url+=String(t);
url+="&h=";
url += String(h);
url+="&g=";
url += String(g);
url+="&p=";
url += String(p);
    client.println(url);
     client.println(" HTTP/1.1");
     client.println("Host: green-storms.info");
     client.println("Content-Length: 12");
     client.println();
     
    
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
    }
     
  }
  

 

  delay(2000); 
}
/*String  time(){
long timeNow = millis();
 
int days = timeNow / day ;                                //number of days
int hours = (timeNow % day) / hour;                       //the remainder from days division (in milliseconds) divided by hours, this gives the full hours
int minutes = ((timeNow % day) % hour) / minute ;         //and so on...
//int seconds = (((timeNow % day) % hour) % minute) / second;
String tm=String(hours)+":"+String(minutes);
return tm;
}*/
