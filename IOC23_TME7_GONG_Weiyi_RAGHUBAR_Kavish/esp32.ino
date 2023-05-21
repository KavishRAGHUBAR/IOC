/*
Sorbonne University (M1 SESI)
Binome : Kavish RAGHUBAR et Weiyi GONG
Date : 21 Mai 2023
Enseignant : Franck Wajsburt
*/

#include <WiFi.h>                 // Library for wifi
#include <PubSubClient.h>         // Library for MQTT communication
  
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "iPhone";        // Username of WIFI
const char* password = "mastersesi21*";   // Password for WIFI
  
const char* mqtt_server = "172.20.10.8";    // IP of MQTT
const char* mqtt_user = "gong";             // Username of MQTT
const char* mqtt_password = "weiyigong";    // Password of MQTT

WiFiClient wifiClient;                      // Creation of wifi client

PubSubClient client(mqtt_server, 1883, wifiClient);   // Create an instance of the PubSubClient class to handle MQTT communication

#define sensor_pin  36

#define MAX_WAIT_FOR_TIMER 5  // Definition of the number of timers

enum {EMPTY, FULL}; // Definition for the flags

struct mailbox_s {    // Structure of the mailbox
  int state;        // State either EMPTY or FULL
  int val;          // The value of the mailbox that needs to be communicated between different tasks
};

struct mailbox_s mb_publish = {.state = EMPTY};     // Initialization of the mailbox for the light intensity

struct mailbox_s mb_led = {.state = EMPTY};

struct lum {
  int timer;
  unsigned long period;
};

struct lum lum_publish;

struct Led {
  int timer;                                              // numéro du timer pour cette tâche utilisé par WaitFor
  unsigned long period;                                   // periode de clignotement
  int pin;                                                // numéro de la broche sur laquelle est la LED
  int etat;                                               // L'etat de led, 1 s'allume, 0 s'etende
  bool blink_led;                                         // Pour dire si le led a besoin de blink
};

struct Led led_blink;

unsigned int waitFor(int timer, unsigned long period) {
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // There are as many timers as periodic tasks
  unsigned long newTime = micros() / period;              // Number of periods modulo 2^32 
  int delta = newTime - waitForTimer[timer];              // Delta between the current period and the recorded one
  if (delta < 0) delta = 1 + newTime;                     // In case the number of periods exceeds 2^32 
  if (delta) waitForTimer[timer] = newTime;               // Recording the new period number
  return delta;
}

void setup_led( struct Led * led, int timer, unsigned long period, byte pin) {//Setup pour led
  led->timer = timer;
  led->period = period;
  led->pin = pin;
  led->etat = 0;
  led->blink_led = true;
  pinMode(pin,OUTPUT);
  digitalWrite(pin, led->etat);
}

void led_blink(struct Led *led_blink, mailbox_s *mb_led){
  if (!(waitFor(led->timer,led->period))) return; //Si le timer pour led n'a pas passe le periode

  if(mb_led -> state == FULL)//Si en cas de flag est FULL, donc il y'a une valeur dans le mailbox
  {
    led_blink -> period = mb_led->val;//Donc on va ecrire la valeur de period de mailbox dans le led
    mb_led -> state = EMPTY;//Laisse le flag en EMPTY
  }

  digitalWrite(LED_BUILTIN,led->etat);//Ecrire la valeur dans le led
  led->etat = 1-(led->etat);//Faire le clignotement
}

void setup_lum(struct lum *lum_publish, int timer, unsigned long period) {
  lum_publish->timer = timer;
  lum_publish->period = period;
}

int readLum() {
  int lum = analogRead(sensor_pin);                        // Read the value from the sensor pin
  lum = map(lum, 0, 4096, 100, 0);
  return lum;
}

void loop_Lum(struct mailbox_s *mb_publish, struct lum *lum_publish) {
  if (!(waitFor(lum_publish->timer, lum_publish->period))) return;  // If the time hasn't passed one period
  int lumvalue = readLum();
  
  if (mb_publish->state != FULL) {
    mb_publish->val = lumvalue;    // The value can be directly sent to OLED
    mb_publish->state = FULL; // Set the mailbox flag to FULL
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  
  WiFi.begin(ssid, password);  // Connect with WIFI
  Serial.println("\nConnecting");
  
  while (WiFi.status() != WL_CONNECTED)    // While waiting for the connection to the WIFI
  {
    Serial.print(".");
    delay(100);
  }
  
  // After connection
  Serial.println("\nConnected to WiFi network");    
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void connect_rasp() {
  while (!client.connected()) {
    if (client.connect("ESP32CLIENT", mqtt_user, mqtt_password))   // Connect to MQTT
    {
      Serial.println("Connected with Raspb");
      //client.subscribe("topic_buzzer",mqtt_user,mqtt_password);
      //client.subscribe("topic_led",mqtt_user,mqtt_password);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

/*
void callback(const char* topic, byte* payload, unsigned int length) {
  String Topic = String(topic);
  Serial.println(Topic);
  Serial.print("length message received in callback");
  Serial.println(length);
  int i;
  for (i = 0; i < length; i++) {
    buf[i] = (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();
  buf[i] = '\0';
  String str = String(buf);

  if (Topic == "topic_led") {
    if(str == "2")
    {
      mb_led.val = 2;
    }
    else if(str == "4")
    {
      mb_led.val = 4;
    }
    else if(str == "6")
    {
      mb_led.val = 6;
    }
    else if(str == "8")
    {
      mb_led.val = 8;
    }
    else if(str == "10")
    {
      mb_led.val = 10;
    }

    mb_led.state = FULL;
    led_blink(&led_blink,&mb_led);
    
  }
}
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(4, 15);
  
  setup_led(&led,1,1000000,LED_BUILTIN);//Timer 2
  setup_lum(&lum_publish, 0, 500000);  // Timer 1 and period 5000000
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void loop() {
  if (!client.connected())   // If MQTT is not connected
  {
    connect_rasp();       // Reconnect MQTT
  }

  loop_Lum(&mb_publish, &lum_publish);   // Loop to read the luminosity

  if (mb_publish.state != EMPTY) {
    String lum = (String)(mb_publish.val);    // Convert the value to string for the message to be sent
    client.publish("topic_lum", lum.c_str());  // Send the message to MQTT
    mb_publish.state = EMPTY;
  }
}
