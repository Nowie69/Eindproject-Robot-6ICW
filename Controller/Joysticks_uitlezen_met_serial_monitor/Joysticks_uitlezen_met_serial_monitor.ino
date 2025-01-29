


#define PosX1 A2
#define PosY1 A3
#define Button1 17

#define PosX2 A4
#define PosY2 32
#define Button2 21

void afstandsbediening();




void setup()
{
  Serial.begin(115200);
  initWiFi();//Instellen van de wifi.
  pinMode(PosX1, INPUT);
  pinMode(PosY1, INPUT);
  pinMode(Button1, INPUT_PULLUP);
  pinMode(PosX2, INPUT);
  pinMode(PosY2, INPUT);
  pinMode(Button2, INPUT_PULLUP);
}

void loop()
{
  afstandsbediening();


  delay(2500);
}


void afstandsbediening()
{
  int intWaardeX1 = analogRead(PosX1);
  int intWaardeY1 = analogRead(PosY1);
  bool blButton1 = digitalRead(Button1);

  int intWaardeX2 = analogRead(PosX2);
  int intWaardeY2 = analogRead(PosY2);
  bool blButton2 = digitalRead(Button2);


  bool blLinks = false;
  bool blRechts = false;
  bool blVooruit = false;
  bool blAchteruit = false;
  bool blDraaiLinks = false;
  bool blDraaiRechts = false;
  if(intWaardeX1 < 1200)
  {
    blLinks = true;
  }
  else if(intWaardeX1 > 2600)
  {
    blRechts = true;
  }
  if(intWaardeY1 < 1200)
  {
    blAchteruit = true;
  }
  else if(intWaardeY1 > 2600)
  {
    blVooruit = true;
  }
  if(intWaardeX2 < 1200)
  {
    blDraaiLinks = true;
  }
  else if(intWaardeX2 > 2600)
  {
    blDraaiRechts = true;
  }



  bool blLinksVooruit = false;
  bool blRechtsVooruit = false;
  bool blLinksAchteruit = false;
  bool blRechtsAchteruit = false;
  if((blVooruit == true)&&(blLinks == true))
  {
    blLinksVooruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blLinks = false;
    blVooruit = false;

    Serial.println("Rij LinksVooruit");
  }
  if((blVooruit == true)&&(blRechts == true))
  {
    blRechtsVooruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blRechts = false;
    blVooruit = false;

    Serial.println("Rij RechtsVooruit");
  }
  if((blAchteruit == true)&&(blLinks == true))
  {
    blLinksAchteruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blLinks = false;
    blAchteruit = false;

    Serial.println("Rij LinksAchteruit");
  }
  if((blAchteruit == true)&&(blRechts == true))
  {
    blRechtsAchteruit = true;
    blDraaiLinks = false;
    blDraaiRechts = false;
    blRechts = false;
    blAchteruit = false;

    Serial.println("Rij RechtsAchteruit");
  }

  if(blLinks == true)
  {
    Serial.println("Rij naar links");
  }
  else if(blRechts == true)
  {
    Serial.println("Rij naar rechts");
  }
  if(blVooruit == true)
  {
    Serial.println("Rij vooruit");
  }
  else if(blAchteruit == true)
  {
    Serial.println("Rij achteruit");
  }
  if(blDraaiLinks == true)
  {
    Serial.println("Draai links");
  }
  else if(blDraaiRechts == true)
  {
    Serial.println("Draai rechts");
  }

  if((blRechtsAchteruit == false)&&(blLinksAchteruit == false)&&(blRechtsVooruit == false)&&(blLinksVooruit == false)&&(blDraaiRechts == false)&&(blDraaiLinks == false)&&(blAchteruit == false)&&(blVooruit == false)&&(blRechts == false)&&(blLinks == false))
  {
    Serial.println("STOP");
  }
}
