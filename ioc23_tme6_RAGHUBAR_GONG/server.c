/* A simple server in the internet domain using TCP The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>

void error(const char *msg)
{
        perror(msg);
        exit(1);
}

int main(int argc, char *argv[])
{
        int sockfd, newsockfd, portno;
        socklen_t clilen;                                           // Creation de la structure client
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;                     // Structure de socket adresse de serveur et client
        int n;

        if (argc < 2) {
                fprintf(stderr, "ERROR, no port provided\n");
                exit(1);
        }

        // 1) on crée la socket, SOCK_STREAM signifie TCP

        sockfd = socket(AF_INET, SOCK_STREAM, 0);                   // Creation de nouveau socket
        if (sockfd < 0)
                error("ERROR opening socket");

        // 2) on réclame au noyau l'utilisation du port passé en paramètre 
        // INADDR_ANY dit que la socket va être affectée à toutes les interfaces locales

        bzero((char *) &serv_addr, sizeof(serv_addr));              // Set le serv_addr en zero
        portno = atoi(argv[1]);                                     // Convertir la valeur de argv[1] en int
        serv_addr.sin_family = AF_INET;                             // Set l'adresse de serv_addr en IPv4
        serv_addr.sin_addr.s_addr = INADDR_ANY;                     // Tell the socket to listen on all network interfaces available on the host
        serv_addr.sin_port = htons(portno);                         // Convertit l’entier court non signé portno depuis l’ordre des octets de l’hôte vers celui du réseau.
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)        // Affectation d'un nom à une socket
                error("ERROR on binding");


        // On commence à écouter sur la socket. Le 5 est le nombre max
        // de connexions pendantes

        listen(sockfd, 5);                                          // Listen for the connection of sockets
        while (1) {
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);         // Acceptation de la connection
                if (newsockfd < 0)
                    error("ERROR on accept");

                bzero(buffer, 256);                                 // Initialise the buffer to 0
                n = read(newsockfd, buffer, 255);                   // read the value from the socket to the buffer
                if (n < 0)
                    error("ERROR reading from socket");

                printf("Received packet from %s:%d\nData: [%s]\n\n",
                       inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port),
                       buffer);                                     // Show the value written in the buffer and the value origin from the socket


                int val = atoi(buffer);                             // Convert the value from the string to int
                if(val == 1)                                        // If the value is 1
                {
                    printf("hiver\n");                              // Print out it is winter
                }
                if(val == 2)                                        // If the value is 2
                {
                    printf("ete\n");                                // Print out it is summer
                }

                close(newsockfd);                                   // Close the new socket
        }

        close(sockfd);
        return 0;
}