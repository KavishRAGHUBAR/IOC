# Title : IOC_T02 : **Driver Linux sur Raspberrypi**
M1 SESI
Binôme : Kavish RAGHUBAR et Weiyi GONG

**Étape 1 : Création et test d'un module noyau**
1. Quelle fonction est exécutée lorsqu'on insère le module du noyau ? 

Lorsque on insère le module du noyau, nous avons la fonction module_init(mon_module_init) qui est exécutée. Cela nous affichera "Hello World <raghubar_gong> !" sur le terminal de commande.

Nous avons l'affichage sur KERN_DEBUG : "Hello world", quand la fonction module_init() est lancée.

KERN_DEBUG est un log utiliser par le noyau linux afin de  donner à l'utilisateur des messages avec dmesg.

2. Quelle fonction est exécutée lorsqu'on enlève le module du noyau ?

Lorsque on enlève le module du noyau, on aura module_exit(mon_module_cleanup) qui sera exécutée. Donc on aura l'affichage : "Goodbye World!"



**Étape 2 : Ajout des paramètres au module**
1. Comment voir que le paramètre a bien été lu ?
On utilise la commande "dmesg" après avoir inséré le module avec la commande "sudo insmod ./module.ko leds=4,17".
On aura comme affichage sur la console : 
Hello World!
LED 0 = 4
LED 1 = 17


**Étape 3 : Création d'un driver qui ne fait rien, mais qui le fait dans le noyau**

1. Comment savoir que le device a été créé ?
En utilisant la command "ls -l /dev", on peut afficher les devices présent dans /dev. On pourra ensuite utiliser la commande "dmesg" pour avoir des informations Kernel.

2. Expliquer comment insdev récupère le numéro major.
Le numero de major pour la led_RG est 246.
* crw-r--r-- 1 root root    246,   0 Jul 20 14:34 led_RG
num major = 246
num minor = 0

Avec la ligne de code 
major = register_chrdev(0, "led", &fops_led);
qui se trouve dans le code du driver, nous allons récupérer le numéro major dans la variable major. 

Le script ./insdev va récupérer ce numéro, lorsqu'il est exécuté avec l'argument led_RG.


**Étape 4 : accès aux GPIO depuis les fonctions du pilote**

(voir code pour les commentaires et plus d'explications)

Globalement, la structure "gpio_s" contiendra les entrées/sorties GPIO. 

La fonction gpio_fsel permet de configurer le mode de fonctionnement des pins GPIO.

Les fonctions open_led, read_led, write_led et release_led vont gérer l'ouverture, la lecture, l'écriture et la fermeture du module.

La fonction gpio_write écrira un '0' ou '1' dans le pin passé en argument de la fonction.

module_init(mon_module_init); //initialiser le module et affecter les pins des LEDs en mode driver

module_exit(mon_module_cleanup); //supprimer le module et idéalement éteindre les LEDs



**Réalisation de driver pour les deux leds et un bouton**

Pour piloter les trois composants, on a écrit trois fichiers drivers.

led_RG et led1_RG pour les deux leds, bp_RG pour le bouton. Il y a juste les fonctions de read et de write qu'on modifie pour assurer le bon fonctionnement du driver.

Par exemple, il faut modifier la fonction de write pour les leds, ici on a converti les paramètres de type char* en int, donc on a proposé deux possibilités, soit '0' et '1'. En faisant ainsi, il peut bien identifier les deux cas, et après, appelle la fonction de write dans GPIO de leds.

S'il est en mode de fonctionnement avec le bouton, il faut modifier la fonction de read, on doit convertir les données lues par la fonction de read gpio du bp vers les données de type de char*, donc on a fait un cast pour convertir.

Dans le fichier de test.c, il faut d'abord open le fichier du module, et ensuite, on peut directement utiliser les fonctions de write et read, et on pensera à fermer les fichiers à la fin.
