#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN ""


char ssid[] = "";
char pass[] = "";

int emptyTankDistance = 70 ;  
int fullTankDistance =  30 ;  
int triggerPer =   10 ;  

#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <AceButton.h>
using namespace ace_button; 

#define TRIGPIN    27  
#define ECHOPIN    26  
#define wifiLed    2   
#define ButtonPin1 12  
#define BuzzerPin  13 
#define GreenLed   14 

#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define OLED_RESET     -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


float duration;
float distance;
int   waterLevelPer;
bool  toggleBuzzer = HIGH; 

char auth[] = BLYNK_AUTH_TOKEN;

ButtonConfig config1;
AceButton button1(&config1);

void handleEvent1(AceButton*, uint8_t, uint8_t);

BlynkTimer timer;

void checkBlynkStatus() { 

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    digitalWrite(wifiLed, HIGH);
  }
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
}

void displayData(int value){
  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(8,2);
  display.print(value);
  display.print(" ");
  display.print("%");
  display.display();
}

void measureDistance(){
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
 
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(20);
 
  digitalWrite(TRIGPIN, LOW);
 
  duration = pulseIn(ECHOPIN, HIGH);
 

  distance = ((duration / 2) * 0.343)/10;

  if (distance > (fullTankDistance - 10)  && distance < emptyTankDistance ){
    waterLevelPer = map((int)distance ,emptyTankDistance, fullTankDistance, 0, 100);
    displayData(waterLevelPer);
    Blynk.virtualWrite(VPIN_BUTTON_1, waterLevelPer);
    Blynk.virtualWrite(VPIN_BUTTON_2, (String(distance) + " cm"));

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (waterLevelPer < triggerPer){
      digitalWrite(GreenLed, HIGH);
      if (toggleBuzzer == HIGH){
        digitalWrite(BuzzerPin, HIGH);
      }      
    }
    if (distance < fullTankDistance){
      digitalWrite(GreenLed, LOW);
      if (toggleBuzzer == HIGH){
        digitalWrite(BuzzerPin, HIGH);
      } 
    }

    if (distance > (fullTankDistance + 5) && waterLevelPer > (triggerPer + 5)){
      toggleBuzzer = HIGH;
      digitalWrite(BuzzerPin, LOW);
    }        
  }
  
  delay(100);
}

 
void setup() {
  Serial.begin(115200);
 
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(wifiLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);

  pinMode(ButtonPin1, INPUT_PULLUP);

  digitalWrite(wifiLed, LOW);
  digitalWrite(GreenLed, LOW);
  digitalWrite(BuzzerPin, LOW);

  config1.setEventHandler(button1Handler);
  
  button1.init(ButtonPin1);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1000);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();

  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); 
  Blynk.config(auth);
  delay(1000);
 
}
 void loop() {

  measureDistance();

  Blynk.run();
  timer.run(); 

  button1.check();
   
}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(BuzzerPin, LOW);
      toggleBuzzer = LOW;
      break;
  }
}