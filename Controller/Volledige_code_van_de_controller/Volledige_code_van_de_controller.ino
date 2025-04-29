#include "WiFi.h"
#include <PubSubClient.h>
// In de library manager moet PubSubClient geïnstalleerd zijn.

//duidelijke namen geven aant de pinnen
#define PosYL A2
#define PosXL A3
#define ButtonL 17
#define PosYR A4
#define PosXR 32
#define ButtonR 21
#define PotSnelheid 33

const int DeadzoneMIN = 1200; //dit zijn de deadzones van de joysticks zodat de eventuele afwijkingen genegeerd worden
const int DeadzoneMAX = 2300;

//Declaratie van de methoden
void mqttConnect();
void initWiFi();
void afstandsbediening();
void initINPUTS();
void readINPUTS();





// WiFi instellingen
const char* ssid = "ICW";
const char* paswoord = "ICW6_ICW6";
WiFiClient WiFiClient1;

// MQTT instellingen
const char* mqttBroker = "192.168.0.245";
const char* mqttClientName = "wout";
const char* PubRichting = "Robot/Richting";       //Naam van de topic waarop data verstuurd wordt (publish).
const char* PubDraaien = "Robot/Draaien";         //Naam van de topic waarop data verstuurd wordt (publish).
const char* PubSnelheid = "Robot/Snelheid";       //Naam van de topic waarop data verstuurd wordt (publish).
const char* mqttPwd = "Test";
const char* clientID = "Test";
PubSubClient mqttClient1(WiFiClient1);





//variabelen van de joysticks
int intWaardeXL;
int intWaardeYL;
bool blButtonL;
int intWaardeXR;
int intWaardeYR;
bool blButtonR;
int intAnalogSnelheid;


void setup()
{
  initWiFi();                               //Instellen van de wifi.
  mqttClient1.setServer(mqttBroker, 1883);  //Instellen van de MQTT broker.
  initINPUTS();                             //de inputs instellen als input
}

void initWiFi()
{
  WiFi.begin(ssid,paswoord);// Verbinding maken met het draadloze netwerk, met het meegegeven SSID en paswoord.
  while (WiFi.status() != WL_CONNECTED)//Controle of er verbinding is.
  {
    delay(500);// Een halve seconde wachten.
  }
  // Verbonden met wifi. De nodige info naar de serial monitor sturen.
  WiFi.setHostname("ESP32 Node Wout"); //Instellen van de hostname
}

//de inputs instellen als input
void initINPUTS()
{
  pinMode(PosXL, INPUT);
  pinMode(PosYL, INPUT);
  pinMode(ButtonL, INPUT_PULLUP);
  pinMode(PosXR, INPUT);
  pinMode(PosYR, INPUT);
  pinMode(ButtonR, INPUT_PULLUP);
  pinMode(PotSnelheid, INPUT);
}

void loop()
{
  //Controle of de ESP32 nog verbonden is met de MQTT-broker.
  if(!mqttClient1.connected())
  {
    mqttConnect();
  }
  //Dit moet reggelmatig worden aangeroepen om de client in staat te stellen
  // inkomende berichten te verwerken en de verbinding met de server te behouden.
  mqttClient1.loop();
  
  afstandsbediening();
}

// Verbinding maken met de mosquitto broker
void mqttConnect() {
    while (!mqttClient1.connected()) {
        if (mqttClient1.connect(mqttClientName, clientID, mqttPwd))
        {
        } else {
            delay(5000);
        }
    }
}

void afstandsbediening()
{
  // Alle variabelen aanmaken en op false zetten
  bool blLinks = false;
  bool blRechts = false;
  bool blVooruit = false;
  bool blAchteruit = false;
  bool blDraaiLinks = false;
  bool blDraaiRechts = false;
  bool blLinksVooruit = false;
  bool blRechtsVooruit = false;
  bool blLinksAchteruit = false;
  bool blRechtsAchteruit = false;
  readINPUTS(); // laten lezen van inputs

  //Snelheid van de robot inlezen
  int intPWMPercent = map(intAnalogSnelheid, 0, 4095, 0, 100);
  String strPWMPercent = String(intPWMPercent);
  mqttClient1.publish(PubSnelheid, strPWMPercent.c_str());

  //Kijken naar welke kant de robot moet rijden of draaien
  if(intWaardeXL < DeadzoneMIN)
  {
    blLinks = true;
  }
  else if(intWaardeXL > DeadzoneMAX)
  {
    blRechts = true;
  }
  if(intWaardeYL < DeadzoneMIN)
  {
    blAchteruit = true;
  }
  else if(intWaardeYL > DeadzoneMAX)
  {
    blVooruit = true;
  }
  if(intWaardeXR < DeadzoneMIN)
  {
    blDraaiLinks = true;
  }
  else if(intWaardeXR > DeadzoneMAX)
  {
    blDraaiRechts = true;
  }

  // Bij elke rijrichting een bericht naar de MQTT-broker versturen om te zeggen welke kant de robot gaat
  if((blVooruit == true)&&(blLinks == true))
  {
    blLinksVooruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blLinks = false;
    blVooruit = false;

    mqttClient1.publish(PubRichting, "Rij LinksVooruit");
  }
  if((blVooruit == true)&&(blRechts == true))
  {
    blRechtsVooruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blRechts = false;
    blVooruit = false;

    mqttClient1.publish(PubRichting, "Rij RechtsVooruit");
  }
  if((blAchteruit == true)&&(blLinks == true))
  {
    blLinksAchteruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blLinks = false;
    blAchteruit = false;

    mqttClient1.publish(PubRichting, "Rij LinksAchteruit");
  }
  if((blAchteruit == true)&&(blRechts == true))
  {
    blRechtsAchteruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blRechts = false;
    blAchteruit = false;

    mqttClient1.publish(PubRichting, "Rij RechtsAchteruit");
  }

  if(blLinks == true)
  {
    mqttClient1.publish(PubRichting, "Rij links");
  }
  else if(blRechts == true)
  {
    mqttClient1.publish(PubRichting, "Rij rechts");
  }
  if(blVooruit == true)
  {
    Serial.println("Rij vooruit");
    mqttClient1.publish(PubRichting, "Rij vooruit");
  }
  else if(blAchteruit == true)
  {
    mqttClient1.publish(PubRichting, "Rij achteruit");
  }
  if(blDraaiLinks == true)
  {
    mqttClient1.publish(PubDraaien, "Draai links");
  }
  else if(blDraaiRechts == true)
  {
    mqttClient1.publish(PubDraaien, "Draai rechts");
  }

  if((blRechtsAchteruit == false)&&(blLinksAchteruit == false)&&(blRechtsVooruit == false)&&(blLinksVooruit == false)&&(blDraaiRechts == false)&&(blDraaiLinks == false)&&(blAchteruit == false)&&(blVooruit == false)&&(blRechts == false)&&(blLinks == false))
  {
    mqttClient1.publish(PubRichting, "STOP");
    mqttClient1.publish(PubDraaien, "STOP");
  }
  delay(200);
}

void readINPUTS()
{
  // Inlezen van alle inputs
  intWaardeXL = analogRead(PosXL);
  intWaardeYL = analogRead(PosYL);
  blButtonL = digitalRead(ButtonL);

  intWaardeXR = analogRead(PosXR);
  intWaardeYR = analogRead(PosYR);
  blButtonR = digitalRead(ButtonR);
  intAnalogSnelheid = analogRead(PotSnelheid);
}