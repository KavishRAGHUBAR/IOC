#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
        perror(msg);                                                // Afficher l'erreurr correspondante
        exit(0);                                                    // Sortie de la boucle
}

int main(int argc, char *argv[])
{
        int sockfd, portno, n;
        struct sockaddr_in serv_addr;                               // Structure avec famille d'adresse, port , adresse IP, remplissage avec une taille
        struct hostent *server;                                     // Structure avec nom de l'hote, tableau de noms alternatifs pour l'hote, type d'adresse de l'hote, longueur de l'adresse de l'hote, tableau d'adresses IP pour l'hote

        char buffer[256];

        // Le client doit connaitre l'adresse IP du serveur, et son numero de port
        if (argc < 3) {
                fprintf(stderr,"usage %s hostname port\n", argv[0]);
                exit(0);
        }
        portno = atoi(argv[2]);                                     // Convertir la valeur de argv[2] en int

        // 1) Création de la socket, INTERNET et TCP

        sockfd = socket(AF_INET, SOCK_STREAM, 0);                   // Creation de nouveau socket
        if (sockfd < 0)                                             // Si la socket est nulle
                error("ERROR opening socket");                      // Print error

        server = gethostbyname(argv[1]);                            // Trouver le serveur par leur nom
        if (server == NULL) {                                       // S'il n'a pas trouve le serveur
                fprintf(stderr,"ERROR, no such host\n");            // Print error
                exit(0);
        }

        // On donne toutes les infos sur le serveur

        bzero((char *) &serv_addr, sizeof(serv_addr));              // Set le serv_addr en zero
        serv_addr.sin_family = AF_INET;                             // Set l'adresse de serv_addr en IPv4
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);        // Copy le server -> h_addr vers serv_addr.sin_addr.s_addr et la taille est h_length
        serv_addr.sin_port = htons(portno);                         // convertit l’entier court non signé portno depuis l’ordre des octets de l’hôte vers celui du réseau.

        // On se connecte. L'OS local nous trouve un numéro de port, grâce auquel le serveur
        // peut nous renvoyer des réponses, le \n permet de garantir que le message ne reste
        // pas en instance dans un buffer d'emission chez l'emetteur (ici c'est le clent).

        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)                   // Connect to the server
                error("ERROR connecting");

        //strcpy(buffer,"Coucou Peri\n");                             // Copie chaine de charactere dans le buffer
        printf("Hiver ou ete");
        int rep = -1;
        while(rep == -1)
        {
            scanf("%d",&rep);
        }
        sprintf(buffer,"%d",rep);
        n = write(sockfd,buffer,strlen(buffer));                    // Ecrire le contenu du buffer dans la socket
        if (n != strlen(buffer))                                    // Si la longeur n'est pas la meme
                error("ERROR message not fully trasmetted");        // message d'erreur - transmission incomplete

        // On ferme la socket

        close(sockfd);                                              // Fermeture de la socket
        return 0;
}