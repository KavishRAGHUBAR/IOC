# Title : IOC_T04 - Programmation ESP32 / Arduino
M1 SESI
Binôme : Kavish RAGHUBAR et Weiyi GONG


<Big>**Gestion des tâches standard périodiques**</Big>

1. Que contient le tableau waitForTimer[] et à quoi sert-il ?

Le tableau waitForTimer[] sert à contenir la durée d'attente pour les tâches. Dans une tâche, il permet de mettre le programme en attente pendant cette durée et donc de bloquer une tâche quand on appelle la fonction waitFor().
waitFor() va ensuite comparer le temps courant avec cette durée d'attente qui correspond à la tâche courante dans le tableau.

2. Si on a deux tâches indépendantes avec la même période, pourquoi ne peut-on pas utiliser le même timer dans waitFor() ?

Dans le cas des tâches indépendantes avec la même durée, on ne pourra pas utiliser le même timer WaitFor() car on risque de lancer en même temps ces deux tâches ce qui causera un conflit.


3. Dans quel cas la fonction waitFor() peut rendre 2 ?
Si nous avons la tâche qui se termine plus tôt que prévu, c'est à dire avant la fin de la durée, la fonction waitFor() retourne 2.

**Utilisation de l'écran OLED**

Dans cette partie nous avons utilisé un exemple afin de mieux comprendre comment fonctionne l'écean OLED.
![](https://i.imgur.com/6hj3zs7.jpg)

![](https://i.imgur.com/UldTuL4.jpg)







<Big>**Communications inter-tâches**</Big>
1. Dans le texte précédent, quel est le nom de la boîte à lettre et comment est-elle initialisée ?
Le nom de la boite est 'mb' et elle est initialisée à l'état "EMPTY" au début du programme.



Dans cette partie, on utilise  un écran OLED et le capteur d'intensité de lumière. Nous allons afficher cette intensité, en pourcentage, sur l'écran OLED comme ci-dessous.
![](https://i.imgur.com/d5i47oq.png)

![](https://i.imgur.com/sVxozCS.png)




<Big>**Gestion des interruptions**</Big>
(cf code)
Dans cette partie, on utilise la fonction serialEvent() qui est un fonction de callback et qui a pour but de gérer les données reçues sur le port série de manière asynchrone.

Nous avons ensuite ajouté une nouvelle variable d'état dans la structure led : blink_led qui va gérer si la led doit clignoter ou pas. Le mailbox partage le message entre serialEvent et ledLum.

Pour réaliser la fonctionnalité d'appuyer sur une autre touche dans le terminal afin de faire clignoter la LED, nous avons ajouter une condition 'else if' dans serialEvent(). Nous avons rencontré des soucis de fonctionnement quand on appuie sur 's' et pour résoudre ce problème, nous abont dû ajouter un autre port de broche à attacher pour rendre cette activation/desactivation de clignottement fonctionnel. C'est pour cela, dans la boucle, nous avons deux ports attachInterrupt.

<Big>**Tout ensemble**</Big>
(cf code)
En ce qui concerne le buzzer et le bouton poussoir, nous avons utilisé une struction. Le mailbox partage le message entre les deux.
Nous avons le même principe que la LED et l'interrupteur. On utilise une variable de controle/d'état pour savoir si le buzzer doit sonner ou non.

Dans bp, on utilise la même logique qu'auparavant sur le Raspberry Pi, avec une valeur précedente et une valeur courante.
Si prec =1 et nouv =0, cela signifie que le bp est enfoncé. A chaque fois que la boucle est exécutée, ces deux valeurs doivent être surveillées, modifiées et envoyer pour indiquer si le buzzer peut sonner maintenant. 

Il existe deux méthodes pour permettre au buzzer de sonner, WaitFor(méthode du timer) et pwm. Par conséquent, nous pouvons facilement constater si le son de la méthode waitFor n'est pas continu, mais que la méthode pwn l'est.









 