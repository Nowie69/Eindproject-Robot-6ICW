#define PosYL A2
#define PosXL A3
#define ButtonL 17
#define PosYR A4
#define PosXR 32
#define ButtonR 21
#define PotSnelheid 33

void readINPUTS();

void setup() {
  // put your setup code here, to run once:
  pinMode(PosYL, INPUT);
  pinMode(PosXL, INPUT);
  pinMode(ButtonL, INPUT_PULLUP);
  pinMode(PosYR, INPUT);
  pinMode(PosXR, INPUT);
  pinMode(ButtonR, INPUT_PULLUP);
  pinMode(PotSnelheid, INPUT);

  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

  readINPUTS();
  delay(2000);

}

void readINPUTS()
{
  //lezen van de X en Y waardes

    Serial.print("Y-waarde Links:   ");
  Serial.println(analogRead(PosYL));
  Serial.print("X-waarde Links:   ");
  Serial.println(analogRead(PosXL));
  Serial.print("Y-waarde Rechts:   ");
  Serial.println(analogRead(PosYR));
  Serial.print("X-waarde Rechts:   ");
  Serial.println(analogRead(PosXR));
  //lezen van de buttons
  Serial.print("button Links:   ");
  Serial.println(digitalRead(ButtonL));
  Serial.print("button Rechts:   ");
  Serial.println(digitalRead(ButtonR));
}
