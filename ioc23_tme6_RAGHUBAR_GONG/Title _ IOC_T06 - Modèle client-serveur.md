# Title : IOC_T06 - Modèle client-serveur
M1 SESI
Binôme : Kavish RAGHUBAR et Weiyi GONG

Dans ce TME, nous allons voir l'interaction entre le client et le serveur.

**`server.c`**

Le code `server.c` est un serveur TCP simple qui va écouter les connection entrantes sur un port d'écoute specifique. Il va afficher les messages envoyer par les clients de cette manière : 
si val = 1, affichage "hiver"
si val = 2, affichage "été"

Ce programme vérifie aussi s'il y a un numéro de port fourni en argument et va créer une socket avec le protocol TCP/IP. De cette manière, la socket sera associée au numéro de port fourni et cette socket sera mise en mode écoute pour les connexions entrantes.

Enfin, dans la boucle while(1), nous avons une acceptation des connections entrantes et on lit les messages envoyés par le client.
Quant on quitte le programme, la socket qu'on venait de créer sera fermée.

Nous avons eut le serveur et le client qui sont fonctionnels.
Nous pouvons voir un exemple dans la figure ci-dessous : 

![](https://i.imgur.com/uBgspri.png)


**`client.c`**

Pour cette partie du code qui s'agit d'implémenter un client TCP/IP qui va se connecter avec le serveur en spécifiant son adresse IP et le numéro de port.

Le client envoie un message (saisi au clavier), au serveur. Nous savons que ce message peut être 1 ou 2 pour afficher "hiver" ou "été" dans le code server.c.
Et en cas d'erreur de saisi au clavier, on quite le programme.

Dans le main(), le programme vérifie si nous avons l'adresse IP et le numéro de port qui ont été fourni. Si ce n'est pas le cas, on quitte le programme.

Ensuite avec la fonction socket(), qui a comme arguments AF_INET (famille d'adresse Internet) et SOCK_STREAM (type de socket TCP), nous allons créer une socket.
Dans le cas ou cette création de socket échoue, nous avons l'appel de la fonction error().

La fonction gethostbyname() qui aura comme argument le nom d'hôte (fourni sur le terminal), va obtenir les informations sur le server. En cas d'échec, on aura un message d'erreur et on quitte le programme.


Nous avons ensuite la configuration de la structure sockaddr_in en utilisant justement les informations sur le serveur telles que : @ IP, numéro de port. En utilisant bzero() on initialise cette structure à 0. Avec bcopy() nous allons copier l'adresse IP du serveur dans la structure sockaddr_in. La fonction htons() va convertir le numéro de port de l'ordre des octets de l'hôte vers celui du réseau.

En utilisant la fonction connect() avec la socket et la structure sockaddr_in, le client peut se connecter au server. En cas d'échec de connection, error() est appelée.

Le programme demande à l'utilisateur de saisir au clavier un entier qui sera stocké dans un buffer, qui sera ensuite écrit dans le socket en appelant la fonction write(). Si write() n'arrive pas à écrire tous les octets du buffer dans le socket, la fonction error() est appelée.

Quand on quite le programme, la socket est fermée à l'aide de la fonction close().

