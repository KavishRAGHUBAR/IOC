# TME5 Serveur WEB minimaliste

**1. Communication par FIFO**

writer.py
* Dans quel répertoire est créée la fifo?
La fifo est créee dans le répertoire : /tmp

* Quelle est la différence mkfifo et open ?
mkfifo va créer une fifo qui n'existe pas déjà avec le nom(pipe_name) tandis que open va ouvrir ou créer un fichier avec le nom pour n'importe quel type de fifo.
La différence entre les 2 est que la création d'une fifo avec mkfifo est bloquante alors que 'open' est non-bloquante.

* Pourquoi tester que la fifo existe ?
Pour vérifier s'il existe afin de pouvoir faire l'écriture. Dans le cas où la fifo n'existe pas, nous allons créer une fifo.De cette manière on sait que la fifo est disponible et peut être utiliser par des processus

* À quoi sert flush ?
La fonction flush() nous permet de vider le buffer d'écriture et assurer que ce qu'il y avait dans ce buffer soit transmis vers la FIFO. De cette manière le buffer sera disponible pour une nouvelle écriture et assurer que les données qui y étaient ne sont pas perdues/écrasées. Les autre processus pourront donc lire les données à partir de la FIFO.

* Pourquoi ne ferme-t-on pas la fifo ? 
Tant qu'on a une écriture qui peut être faite sur la fifo, on le ferme pas la fifo. Si on le ferme, lors de l'exécution de l'applications, nous allons pas pouvoir écrire dans la fifo et il faudra la réouvrir, ce qui serait moins efficace. C'est seulement lorsque les écritures sont terminées et qu'on ait plus de nouvelles écritures à faire qu'on ferme la fifo.


reader.py

* Que fait readline ? 
La fonction readline() nous permet de lire toute la ligne de la fifo tant qu'on arrive pas à la fin. Dans le code, la fifo est ouverte en mode 'r'. On reste dans une boucle tant qu'on ne lit pas 'end\n'. On recupère donc la donnée sous format str/string et nous pouvons afficher cette chaine de charactère par la suite en faisant un print().
Chaque ligne qui sera lue est affiché sur le terminal.


Vous allez remarquer que lorsque vous lancer un écrivain (en C ou en Python) rien ne se passe tant que vous n'avez pas lancé un lecteur.

* Expliquez le phénomène (... c'est le open(), mais pourquoi ? ...) 

En utilisant mkfifo() nous avons un fichier FIFO qui est inséré dans le système de fichiers. De cette manière n'importe quel processus peut l'ouvrir en mode lecture ou écriture. Toutefois l'ouverture d'un FIFO en lecture par un processus est bloquante tant qu'on a pas un autre processus qui ouvre ce même FIFO en mode écriture, et vice-versa.
Il faudra donc qu'un processus lit la donnée à l'autre extrémité du pipe pour ne pas avoir de blocage sur la fifo.



**2. Création d'un serveur fake**

En utilisant la commande 
'scp -P62220 -r server-fake pi@peri:raghubar_gong/server'
nous pouvons transferer le dossier server-fake vers le raspberry pi

(cf code pour plus de commentaires)

server.py
Nous avons 2 pipes : s2f_fw et f2s_fw qui seront crées s'ils n'existent pas. on les ouvrent avec open().
s2f ouvert en mode écriture et lecture 'w+'
f2s ouvert en mode lecture seule 'r'

Avec write() le programme écrit "hello" dans la FIFO s2f. Flush() va forcer la sortie du tampon. Comme mentionné précedement, cela garantit que la chaine de charactère est bien écrite dans la FIFO.
En utilisant readline() on lit une ligne sur FIFO f2s.
De cette manière ce qui a été écrite sera lue et cela permet la communication non blocante entre les 2 processus : fake et server.

fake.c
Celui-ci lit s2f_fw et écrit sur f2s_fw.
Ce programme va écouter la fifo s2f_fw.

**3. Création d'un serveur web**
(cf code pour plus de commentaires)

Tout d'abord, nous devons ajouter le code que nous avons écrit lors du tme1 concernant la lecture du bouton poussoir, l'initialisation du bouton gpio et écrire la partie du code pour lire la valeur et l'écrire dans la fifo.

Pour l'imprimer sur le site, nous avons vu quelle partie du code pour le site imprimer, le fichier led.py pourrait imprimer la valeur dans la fifo f2s, mais il y a un inconvénient, il imprimerait la valeur à chaque fois que nous essayons de définir une nouvelle valeur de la led comme on peut le voir sur les 2 figures dessous.

![](https://i.imgur.com/2eMyqDr.jpg)

![](https://i.imgur.com/tuEOLSB.jpg)



**Problèmes rencontrés lors du TME**

1. Lorsque nous avons crée le server sur le Raspberry Pi, nous avons constaté qu'il y avait un problème sur la valeur(de la LED) transferée au Raspberry Pi par la fifo. Pour la première fois, la valeur est bien lu. Cependant, le programme se bloque la 2eme fois qu'on tente de changer la valeur de la LED. Nous avons donc essayé de déboguer le code avec des fprintf() dans le fichier led_bp.c. Nous avons constaté que cette condition (ci-dessous) bloquait le code.
 
 ` if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) {fprintf(stderr,"BREAK\n");break;}`

Dans le fichier led.py, nous avons ajouté des print et nous avons constaté que ce bloquage provient de la fonction readline(). En effet cette fonction bloquait et ne pouvait pas lire la ligne suivante. Nous n'avons pas pu régler ce type de problème. 
Nous avons donc modifié le code pour ignorer ce problème en faisant ceci : on change 'break' par 'continue'. Mais le programme reste toujour bloqué.

La cause de ce problème selon nous, est lié à la FIFO. La FIFO pourrait avoir des problèmes de permission ou d'autres types.

2. Après avoir fait plusieurs essaies, nous avons changé de carte Raspberry, nous avons changé l'emplacement du server que nous avions crée ainsi que le nom de la fifo. Au début, nous avons essayé d'utiliser la fifo s2f_rg et f2s_rg, mais sur plusieurs cartes,cela ne fonctionne pas en raison de permission non-accordées. Nous avons donc utilisé les fifos initialisées : s2f_fw et f2s_fw, qui ont fonctionné.
Le programme n'était plus bloqué maintenant et les fifos étaient autorisées à être utilisées.

D'une certaine manière, nous avons changé le port 8100 en 8000, et nous avons changé la carte de 20 à 21. Nous avons changé le nom de la fifo ce qui a permis à notre programme de fonctionner correctement cette fois-ci. Le programme ne bloquait plus quand on faisait nous nouvelle écriture pour une nouvelle valeur de LED. C'était le même code, et juste les noms différents des fifos et les ports différents qui ont put causé ce problème, ce qui est très étrange.
