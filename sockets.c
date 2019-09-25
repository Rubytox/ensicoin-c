#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Define, qui nous serviront par la suite
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close (s)

// De même
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

int main(int argc, char* argv[])
{
    /*
     * +=========================+
     * | CONNECTION DE LA SOCKET |
     * +=========================+
     */

    SOCKET sock;
    SOCKADDR_IN sin;

    int port = 6667;
    char *ip = "212.83.153.145";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    while (connect(sock, (SOCKADDR *) &sin, sizeof(sin)) == SOCKET_ERROR) {
        fprintf(stderr, "Tentative de connexion à %s sur le port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
        sleep(2);
    }

    fprintf(stderr, "Connecté à %s sur le port %d!\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));

    /*
     * On prépare les messages qu'on va envoyer au serveur IRC
     */
    char *user_cmd = "USER Rubytox * * :Description\r\n";
    char *nick_cmd = "NICK Rubytox\r\n";
    char *privmsg = "PRIVMSG candy !ep2\r\n";

    /*
     * On récupère les deux messages à la connexion.
     */

    char *welcome = malloc(1000 * sizeof(char));
    recv(sock, welcome, 1000 * sizeof(char), 0);
    printf("%s\n", welcome);

    /*
     * On envoie les messages USER et NICK.
     */
    send(sock, user_cmd, strlen(user_cmd) * sizeof(char), 0);
    printf("Sent user\n");
    send(sock, nick_cmd, strlen(nick_cmd) * sizeof(char), 0);
    printf("Sent nick\n");

    /*
     * Cette partie sert à récupérer le gros blabla qu'envoie
     * le serveur lorsqu'on s'est enregistrés.
     * On remarque que le blabla termine en donnant le mode +x
     * à l'utilisateur. Donc si on trouve +x on est à la fin.
     */
    char sockbuff[4096];
    int connected = 0;
    while (connected < 1) {
        memset(&sockbuff, '\0', sizeof(sockbuff));
        recv(sock, sockbuff, 4096, 0);
        /* printf("%s\n", sockbuff); */

        if (strstr(sockbuff, "+x") != NULL)
            connected = 1;
    }

    /*
     * On envoie un message privé pour recevoir le défi.
     */
    send(sock, privmsg, strlen(privmsg) * sizeof(char), 0);
    printf("Sent privmsg\n");

    /*
     * On reçoit la réponse de candy.
     */
    char first_response[500];
    recv(sock, first_response, 4096, 0);

    /*
     * On parse la réponse, qui est envoyée sous la forme
     * :texte: défi
     * On utilise donc ":" comme délimiteur.
     */
    strtok(first_response, ":");
    char *defi = strtok(NULL, ":");
    printf("%s\n", defi);

    /*
     * On décode le défi
     */

    char command[500] = "echo ";
    strncat(command, defi, strlen(defi) - 2);
    strcat(command, " | base64 -d");

    FILE *fp = popen(command, "r");

    char line[4096];
    fgets(line, sizeof(line), fp);

    pclose(fp);

    /*
     * On envoie la réponse.
     */
    char answer[500] = "PRIVMSG candy !ep2 -rep ";
    strncat(answer, line, strlen(line));
    strncat(answer, "\r\n", 2);

    printf("to be sent : %s \n", answer);

    send(sock, answer, strlen(answer), 0);

    /*
     * On récupère la réponse.
     */
    char response[500];
    memset(&response, '\0', sizeof(response));
    recv(sock, response, 500, 0);
    printf("%s\n", response);


    free(welcome);
    closesocket(sock);

    return 0;
}
