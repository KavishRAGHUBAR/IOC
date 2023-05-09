# Title : IOC_T03 - Pilotage d'un écran LCD en mode utilisateur et par un driver
M1 SESI
Binôme : Kavish RAGHUBAR et Weiyi GONG

<Big>**1. Configuration des GPIO pour le pilotage de l'écran**</Big>

1. Comment faut-il configurer les GPIOs pour les différents signaux de l'afficheur LCD ?
Nous avons les pins GPIO du Raspberry Pi qui sont connectés à l'afficheur LCD. On les définit de cette manière : 


| Signal LCD | GPIO | 
| -------- | -------- | 
| RS     | 7     | 
| E     | 27     | 
| D4, D5, D6, D7     | 22, 23, 24, 25     |

RS (Register Select Signal) est connecté au MPU (Memory Protection Unit). Son role est de selectionner le bon registre dans les cas : si les données envoyés au controleur LCD sont des commandes ou des données à afficher sur l'écran.
Si RS = 0, nous avons les données transférées qui sont des commandes.
Si RS = 1, nous avons les données qui vont être afficher à l'écran.

Le role du signal E (enable), connecté au MPU permet d'activer l'afficheur LCD. Ce signal est sensible au front déscendant, c'est à dire de H vers L. On utilise ce signal pour informer, activer et eventuellement synchroniser l'afficheur dès que la donnée est disponible et peut être affichée.

Nous avons D4-D7 qui sont un vecteur de données(bits) bi-directionnelles et fonctionnent en Tri-state. Ce sont ces 4 bits qui vont être transmises comme donnée/commande. Et comme ce vecteur de bits est bi-directionnel, nous pouvons les configurer en entrée ou sortie.

2. Comment écrire des valeurs vers le LCD ?
Nous avons la fonction lcd_write4bits() qui peut écrire des valeurs en envoyant 2 paquets de 4 bits vers la MPU.
data est décalé à droite pour chaque Dx de manière à déconcatener data pour l'affecter au pins correspondants. gpio_write() va envoyer ces données associées aux pins vers l'écran LCD. 
On fera ensuite un lcd_strobe() afin d'activer E(Enable).


3. Quelles valeurs doivent être envoyées vers l'afficheur pour réaliser l'initialisation ?
On utilise la fonction lcd_init() qui va d'abord desactiver le signal E. Ensuite appeler lcd_init(cmd), cmd étant un mot de 8 bits chacun définit en codage One-Hot au début du code. Par exemple on pourra effacer l'ecran avec le bit 0 de cmd à 1. Parmis d'autres options d'init qu'on explorera par la suite. 
#define LCD_CLEARDISPLAY        0b00000001
#define LCD_RETURNHOME          0b00000010
#define LCD_ENTRYMODESET        0b00000100
#define LCD_DISPLAYCONTROL      0b00001000
#define LCD_CURSORSHIFT         0b00010000
#define LCD_FUNCTIONSET         0b00100000
#define LCD_SETCGRAMADDR        0b01000000
#define LCD_SETDDRAMADDR        0b10000000


4. Comment demander l'affichage d'un caractère ?
Nous allons dans un premier temps positioner le curseur à (0,0) par exemple et ensuite utiliser lcd_command(LCD_SETDDRAMADDR). Par la suite avec lcd_data(txt[i]) on écrira notre message sur l'écran LCD.

5. Comment envoyer des commandes telles que : l'effacement de l'écran, le déplacement du curseur, etc. ?
Pour envoyer des commandes nous allons d'abord mettre RS(Register select) à 0 (mode commande). Nous allons ensuite effacer l'écran en faisant lcd_command(LCD_CLEARDISPLAY).

On a créer une fonction pour déplacer le curseur. Nous aurons comme paramettre [x,y] x ayant comme dynamique 0 à 19 pour les colonnes et y de 0 à 3 pour les lignes. Nous allons convertir ces données pour avoir la position souhaitée(entre 0 et 79). De cette manière, l'utilisateur aura juste à entrer la position x,y sans calculer les offsets. On pensera aussi à mettre LCD_CURSORSHIFT et LCD_DC_CURSORON en mask dans cmd, pour ne citer que quelques uns.



<Big>**2. Fonctionnement de l'écran et fonctions de base**</Big>
//cf code fourni pour les réponses aux questions

![](https://i.imgur.com/XrC0lom.jpg)



<Big>**4. Driver pour le LCD**</Big>

Pour cette partie, nous allons piloter l'écran LCD avec un module qui sera insérer dans /dev. On récupère le numéro major. 

La partie ioctl (Input Output ConTroL) dans la structure 'file_operations' va definir la fonction appeler quand l'utilisateur utilise le module lcd_driver_RG. Ainsi, à chaque syscall de categorie ioctl, on aura notre ioctl_lcd qui sera appelé. On utilise ioctl car les appels système du type "read" ou "write" ne peuvent pas effectuer certaines operations specifiques telles que mettre une broche en entrée/sortie.

ioctl_lcd(struct file *file, unsigned int cmd, unsigned long arg) prend 3 arguments : 
-pointeur vers struct file
-cmd qui indique le type d'operation de controle à effectuer
-arg étant un pointeur vers une zone de mémoire buffer afin de transmettre les arguments spécifiques à une commande ioctl. De cette manière lors d'un appel system, le noyau pourra extraire correctement ces données tranmises.


Nous avons d'autres fonctions comme lcd_pulse_enable() qui activera le pin E afin d'assurer que les données ont été lues par l'afficheur. lcd_write_nibble() est utilisé pour écrire un half-word (4 bits) sur l'écran LCD. lcd_write_byte() pour écrire un octet (8 bits) sur l'écran.

Enfin, dans le code du driver, nous avons mis un \b juste après le 'e' de "Hello" pour l'effacer et un \n pour un retour à la ligne suivante. Nous avons l'affichage comme ci-dessous.

![](https://i.imgur.com/OJlYgfc.jpg)
