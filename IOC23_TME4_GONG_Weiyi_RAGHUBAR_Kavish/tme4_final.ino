#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     16 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define sensor_pin  36


#define MAX_WAIT_FOR_TIMER 5  //Definition du nombre de timer
int i=0;  //Variable globale de compteur


bool stringComplete = false;  // whether the string is complete


enum {EMPTY, FULL}; //Definition pour les flags

struct mailbox_s {    //Structure de mailbox
  int state;        //State soit EMPTY, soit FULL
  int val;          //La valeur de mailbox qui a besoin de communiquer entre les differentes taches
};

struct mailbox_s mb_lum = {.state = EMPTY};     //Initialisation de mailbox pour l'intensité de la lumiere
struct mailbox_s mb_led = {.state = EMPTY};     //Initialisation de mailbox pour la valeur de led
struct mailbox_s mb_isr = {.state = EMPTY};     //Initialisation de mailbox pour l'interrupteur
struct mailbox_s mb_buzzer = {.state = EMPTY};  //Initialisation de mailbox pour le buzzer

struct Led {
  int timer;              // numéro du timer pour cette tâche utilisé par WaitFor
  unsigned long period;   // periode de clignotement
  int pin;                // numéro de la broche sur laquelle nous avons la LED
  int etat;               // L'etat de led, 1 s'allume, 0 s'eteint
  bool blink_led;         // Variable d'état pour dire si la led a besoin de blink
}; 

struct Led led;

struct lum{
  int timer;
  unsigned long period;
};

struct lum lum1;


struct counter{
  int timer;
  unsigned long period;
};

struct counter counter1;


unsigned int waitFor(int timer, unsigned long period){
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // il y a autant de timers que de tâches périodiques
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  int delta = newTime - waitForTimer[timer];              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta = 1 + newTime;                   // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) waitForTimer[timer] = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}

int readLum()                                            // La fonction pour recuperer l'intensité de la lumiere
{
  int lum=analogRead(sensor_pin);                        // Lire la valeur dans le pin du sensor
  return lum;
}



void loop_readLum(struct mailbox_s *mb_lum, struct lum *lum1, struct mailbox_s *mb_led) {// La fonction pour envoyer la valeur d'intensite de la lumiere vers oled et led
  if (!(waitFor(lum1->timer,lum1->period))) return;  //Si le temps ne dépasse pas une periode
  int lum=readLum();
  
  if (mb_lum->state != FULL) {
    mb_lum->val = lum;    // On peut directement envoyer la valeur vers oled
    mb_lum->state = FULL; // Mettre le flag de mailbox en FULL
  }

  if (mb_led->state != FULL)
  {
    mb_led->val = 800000000/lum; // La valeur envoie dans led doit etre inverse
    mb_led->state = FULL;  // Mettre le flag de mailbox en FULL
  }

}

void loop_afficheLum(struct mailbox_s *mb_lum) {//Fonction pour afficher l'intensite de la lumiere dans OLED
  if (mb_lum->state != FULL) return;// attend que le mailbox soit plein
    int tension=mb_lum->val;//Recupere la valeur dans le mailbox
    int rate=map(tension,4095,0,0,100);//On converti l'intensite de la lumiere en pourcentage, donc 0-4095 en 0%-100%
    char str[3];//Initialisation d'une string en forme de 3 cases en char
    sprintf(str,"%02d",rate);//Convertir la valeur de mailbox vers la tableau de char
  // usage de mb->val
    display.fillRect((SCREEN_WIDTH-((strlen(str)+1)*12))/2,(SCREEN_HEIGHT-16)/2,48,16,BLACK);//Remplir la zone de centre en noir
    display.setTextSize(2);// Modifier la taille de mot
    display.setTextColor(WHITE);// Modifier la couleur de mot
    display.cp437(true);//Appeler la tableau de ASCII
    display.setCursor((SCREEN_WIDTH-((strlen(str)+1)*12))/2,(SCREEN_HEIGHT-16)/2);// Mettre le cursor dans la premiere case qu'on veut au centre
    display.print(str);//Afficher la valeur
    display.print('%');//Afficher un symbol de pourcentage apres
    display.display();//Le rendre visible
    //display.clearDisplay();
  mb_lum->state = EMPTY;// Laisse le flag de mailbox en EMPTY, donc il peut recuperer la valeur par la suite
}


void loop_ledLum(struct mailbox_s *mb_led, struct Led *led, struct mailbox_s *mb_isr)//La fonction pour controler le clignotement de led
{
  if (!(waitFor(led->timer,led->period))) return; //Si le timer pour led n'a pas depassé la periode

  if(led->blink_led)//Si la valeur indique il peut clignoter
  {
    if(mb_led -> state == FULL)//Si en cas de flag est FULL, donc il y'a une valeur dans le mailbox
    {
      led -> period = mb_led->val;//Donc on va ecrire la valeur de period de mailbox dans la led
      mb_led -> state = EMPTY;//Et mettre le flag en EMPTY
    }

    digitalWrite(LED_BUILTIN,led->etat);//Ecrire la valeur dans le led
    led->etat = 1-(led->etat);//Faire le clignotement
  }
  if(mb_isr->state == FULL)//Si pour l'interrupteur il existe une valeur
  {
    if(mb_isr -> val==1)//si le mailbox d'interrupteur il a une valeur de 1, donc le interrupteur est ouvert
    {
      led->blink_led = 0;//On dit que la led ne peut pas clignoter apres
      digitalWrite(LED_BUILTIN,0);//Etendre la led
    }
    else if(mb_isr -> val ==0)//Si l'interrupteur il a une valeur 0
    {
      led->blink_led = 1;//On fait la led clignoter
    }
    mb_isr->state = EMPTY;//On laisse le mailbox en EMPTY pour pouvoir recuperer la prochaine valeur
  }

}


void oledCount(counter *counter1) {//La fonction oledCount

  if (!(waitFor(counter1->timer,counter1->period))) return;//Si le timer pour compteur n'a pas dépasser la period
  char cpt[10];//Tableau de char
  display.fillRect(0,0,96,16,BLACK);//Mettre la boite qu'on va utiliser en noir
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);//Mettre le cursor au debut
  //display.cp437(true);

 
  
  if(i==9999999999) {
    i=0;
  }//Si le compteur passe cette valeur, il est remise à 0
  
  sprintf(cpt,"%02d",i);//Ecrire la valeur dans le tableau de char
  display.setCursor(0,0);
  display.print(cpt);
  display.display();
  i++;//Compteur qui augmente chaque seconde
}


void setup_lum(struct lum *lum1, int timer, unsigned long period)//Setup de timer pour recuperer l'intensite de la lumiere
{
  lum1->timer = timer;
  lum1->period = period;
}

void setup_counter(struct counter *counter1,int timer, unsigned long period)//Setup de timer pour compteur
{
  counter1->timer = timer;
  counter1->period = period;
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





void serialEvent() {//Fonction pour lire la valeur saisi dans le terminal
  if(mb_isr.state != EMPTY) return;//Si le timer n'a pas passe la periode
  String inputString;
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    inputString += inChar;
    serialEvent();
  }
  Serial.println(inputString);
  if(inputString == "s")//Si la valeur saisi dans le terminal est s
  {
    mb_isr.val=1;//On met la valeur de mailbox d'interrupteur en 1
  }
  else if(inputString != "s" || " ")//Sinon, on met la valeur de mailbox d'interrupteur en 0
  {
    mb_isr.val=0;
  }
  mb_isr.state=FULL;//Laisse le flag de mailbox en FULL

}

struct Bp {//structure de bouton
  int timer;                                              
  unsigned long period;                                   
  int pin;                                               
  int prec;   //Valeur de bouton precedent                                            
  int nouv;   //Valeur de bouton courant                                            
}; 

struct Bp bp;

void setup_bp( struct Bp * bp, int timer, unsigned long period) {//Setup pour le bouton
  bp->timer = timer;
  bp->period = period;
  bp->pin = 23;//Pin 23 pour bouton
  bp->prec = 1;//La valeur initialisation de la valeur precedent est 1
  bp->nouv = 1;//La valeur initialisation de la valeur maintenant est 1
  pinMode(23,INPUT_PULLUP); 
}

void loop_bp(struct Bp *bp, struct mailbox_s *mb_buzzer) {//Fonction pour prendre la valeur de bouton
  if (!(waitFor(bp->timer,bp->period))) return; //Si le timer n'a pas depassé la periode
  if (mb_buzzer->state == EMPTY){ //Si le mailbox est EMPTY
    bp->nouv = digitalRead(bp->pin); //Recupere la nouvelle valeur
    if (bp->nouv!=bp->prec) { //S'il y'a changement d'etat de bouton
      if (bp->nouv == 0) { //Si la valeur maintenant est 0, donc le bouton est pousser
         mb_buzzer->val = 1 - mb_buzzer->val; //On ecrire la valeur de buzzer
      } 
         bp->prec = bp->nouv; //Passe la valeur precedent en valeur maintenant
    }
    mb_buzzer->state = FULL;//Laisse le flag de mailbox en FULL
  }
}

struct Buzzer {//structure de buzzer
  int timer;                                             
  unsigned long period;                                  
  int etat;
  int pin;                                                 
  bool allume; //Indique le buzzer il a besoin de buzz                                              
}; 

struct Buzzer buzzer;
/*//Partie de la fonction piloter en waitFor
void setup_buzzer( struct  Buzzer * buzzer, int timer, unsigned long period) {
  buzzer->timer = timer;
  buzzer->period = period;
  buzzer->etat = 0;
  buzzer->pin = 17;
  buzzer->allume = 0;
  pinMode(17,OUTPUT);
}

void loop_buzzer(struct Buzzer *buzzer, struct mailbox_s *mb_buzzer) {
  if (!(waitFor(buzzer->timer,buzzer->period))) return; 
  
  if (buzzer->allume) {
    digitalWrite(buzzer->pin,buzzer->etat);
    buzzer->etat = 1 - buzzer->etat;
  }
  if (mb_buzzer->state == FULL){ 
    if (mb_buzzer->val) {buzzer->allume=1;}
    else {
      digitalWrite(buzzer->pin,0);
      buzzer->allume = 0;
    }
    mb_buzzer->state = EMPTY;
  }
}
*/
//Partie fonction piloter par PWM
void setup_buzzer( struct  Buzzer * buzzer, int timer, unsigned long period) {
  buzzer->timer = timer;
  buzzer->period = period;
  buzzer->etat = 0;
  buzzer->pin = 17;
  buzzer->allume = 0;
  ledcAttachPin(17, 1);
  ledcSetup(1, 500,8);//Initialisation de pwm
}

void loop_buzzer(struct Buzzer *buzzer, struct mailbox_s *mb_buzzer) {
  if (!(waitFor(buzzer->timer,buzzer->period))) return; //Si le timer n'a pas depassé la periode
  
  if (buzzer->allume) { //Si le buzzer il a besoin de buzz
    buzzer->etat = 1 - buzzer->etat;  //Ecrire la valeur en forme de carre                          
    ledcWrite(1,100);  //Piloter le pwm                                  
  }
  if (mb_buzzer->state == FULL){//Si le mailbox a une valeur
    if (mb_buzzer->val){buzzer->allume=1;}  //Si l'interrupteur est poussé
    else {
      ledcWrite(1,0);//Etendre le pwm
      buzzer->allume = 0; //Dit au buzzer de ne pas buzz
    }
    mb_buzzer->state = EMPTY;//Laisse le mailbox en EMPTY
  }
}


void setup() {//Setup generale
   // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(4,15);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  //pinMode(LED_BUILTIN, OUTPUT);
  //inputString.reserve(200);
  display.clearDisplay();


  setup_lum(&lum1,0,500000);//Timer 1 et periode 5000000
  setup_counter(&counter1,1,1000000); // Timer 1, set up the counter for 1 second
  setup_led(&led,2,1000000,LED_BUILTIN);//Timer 2
  setup_bp(&bp,3,100000);//Timer 3
  setup_buzzer(&buzzer,4,700);//Timer 4
 
}






void loop() {
  // put your main code here, to run repeatedly:
    //Serial.println(1);
    attachInterrupt(digitalPinToInterrupt(2), serialEvent, CHANGE);
    attachInterrupt(digitalPinToInterrupt(3), serialEvent, CHANGE);//Charger 2 fonction pour attach l'interrupteur en boucle chaque fois
    loop_readLum(&mb_lum,&lum1,&mb_led);
    loop_afficheLum(&mb_lum);
    oledCount(&counter1);
    loop_ledLum(&mb_led,&led,&mb_isr);
    loop_bp(&bp,&mb_buzzer);
    loop_buzzer(&buzzer,&mb_buzzer);//Generer les fonctions
    
}
