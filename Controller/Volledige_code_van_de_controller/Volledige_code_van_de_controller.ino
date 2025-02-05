#include "WiFi.h" //Bibliotheek nodig voor de wifi.
#include <PubSubClient.h> //bibliotheek nodig voor MQTT.
// In de library manager moet PubSubClient geïnstalleerd zijn.

//duidelijke namen geven aant de pinnen
#define PosXL A2
#define PosYL A3
#define ButtonL 17
#define PosXR A4
#define PosYR 32
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

//Wifi instellingen
const char* ssid = "ICW";           //SSID van het te gebruiken wifi-netwerk.
const char* paswoord = "ICW6_ICW6"; //Paswoord van het te gebruiken wifi-netwerk
WiFiClient WiFiClient1;             //Een object wifiClient1 van de klasse wifiClient.

//Instellingen MQTT-broker
const char* mqttBroker = "broker.hivemq.com";     // De te gebruiken MQTT-broker.
const char* mqttClientName = "Robot_Wout_Noah ";  //Naam van de MQTT-client
const char* PubRichting = "Robot/Richting";       //Naam van de topic waarop data verstuurd wordt (publish).
const char* PubDraaien = "Robot/Draaien";         //Naam van de topic waarop data verstuurd wordt (publish).
const char* PubSnelheid = "Robot/Snelheid";       //Naam van de topic waarop data verstuurd wordt (publish).
const char* mqttPwd = "mqtt paswoord";            //Paswoord van de MQTT-broker.
const char* clientID = "username0001";            //Client id usernamen+0001
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
  Serial.begin(115200);                     //Instellen serieële communicatie.

  initWiFi();                               //Instellen van de wifi.
  mqttClient1.setServer(mqttBroker, 1883);  //Instellen van de MQTT broker.
  initINPUTS();                             //de inputs instellen als input
}

void initWiFi()
{
  WiFi.begin(ssid,paswoord);// Verbinding maken met het draadloze netwerk, met het meegegeven SSID en paswoord.
  Serial.println("Verbindt met WiFi ..");
  while (WiFi.status() != WL_CONNECTED)//Controle of er verbinding is.
  {
    Serial.print('.');//Zolang er geen verbinding is print een puntje.
    delay(500);// Een halve seconde wachten.
  }
  // Verbonden met wifi. De nodige info naar de serial monitor sturen.
  Serial.print("Verbonden met");
  Serial.println(ssid);
  Serial.print("Het gekregen IP-adres van het netwerk is: ");
  Serial.println(WiFi.localIP());//Print het gekregen IP-adres.
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());//Geef de signaalsterkte met het wifi netwerk.
  WiFi.setHostname("ESP32 Node Tom"); //Instellen van de hostname
  Serial.print("Het MAC-adres is: ");
  Serial.println(WiFi.macAddress());
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
void mqttConnect()
{
  while (!mqttClient1.connected())
  {
    Serial.print("Probeer te verbinden met MQTT-broker...");
    if (mqttClient1.connect(mqttClientName))
    {
      // Als de microcontroller verbonden is word er een bericht gestuurd naar de MQTT-broker en seriële monitor 
      Serial.println("Verbonden");
      mqttClient1.publish("Hallo", mqttClientName);
    }
    else
    {
      // Niet verbonden wordt er een een bericht gestuurd in de seriële monitor
      Serial.print("Mislukt, rc=");
      Serial.print(mqttClient1.state());
      Serial.println(" Probeer opnieuw binnen 5 seconden");
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
  Serial.println("sneheid: " + intPWMPercent);
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

    Serial.println("Rij LinksVooruit");
    mqttClient1.publish(PubRichting, "Rij LinksVooruit");
  }
  if((blVooruit == true)&&(blRechts == true))
  {
    blRechtsVooruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blRechts = false;
    blVooruit = false;

    Serial.println("Rij RechtsVooruit");
    mqttClient1.publish(PubRichting, "Rij RechtsVooruit");
  }
  if((blAchteruit == true)&&(blLinks == true))
  {
    blLinksAchteruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blLinks = false;
    blAchteruit = false;

    Serial.println("Rij LinksAchteruit");
    mqttClient1.publish(PubRichting, "Rij LinksAchteruit");
  }
  if((blAchteruit == true)&&(blRechts == true))
  {
    blRechtsAchteruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blRechts = false;
    blAchteruit = false;

    Serial.println("Rij RechtsAchteruit");
    mqttClient1.publish(PubRichting, "Rij RechtsAchteruit");
  }

  if(blLinks == true)
  {
    Serial.println("Rij naar links");
    mqttClient1.publish(PubRichting, "Rij links");
  }
  else if(blRechts == true)
  {
    Serial.println("Rij naar rechts");
    mqttClient1.publish(PubRichting, "Rij rechts");
  }
  if(blVooruit == true)
  {
    Serial.println("Rij vooruit");
    mqttClient1.publish(PubRichting, "Rij vooruit");
  }
  else if(blAchteruit == true)
  {
    Serial.println("Rij achteruit");
    mqttClient1.publish(PubRichting, "Rij achteruit");
  }
  if(blDraaiLinks == true)
  {
    Serial.println("Draai links");
    mqttClient1.publish(PubDraaien, "Draai links");
  }
  else if(blDraaiRechts == true)
  {
    Serial.println("Draai rechts");
    mqttClient1.publish(PubDraaien, "Draai rechts");
  }

  if((blRechtsAchteruit == false)&&(blLinksAchteruit == false)&&(blRechtsVooruit == false)&&(blLinksVooruit == false)&&(blDraaiRechts == false)&&(blDraaiLinks == false)&&(blAchteruit == false)&&(blVooruit == false)&&(blRechts == false)&&(blLinks == false))
  {
    Serial.println("STOP");
    mqttClient1.publish(PubRichting, "STOP");
    mqttClient1.publish(PubDraaien, "STOP");
  }
  delay(1000);
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