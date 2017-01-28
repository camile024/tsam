

#include "libs.h"
#include "defines.h"
#include "main.h"
#include "sockets.h"
#include "files.h"

/* Connects to the server and gets a list of channels. Stores the server's
 * response in 'buffer' 
 * Returns 0 on success, negative value on error 
 */
int net_getinfo(char* buffer){
    bzero(buffer, BUFFER_SIZE);
    int socketfd, errno;
    plog("Connecting to the server...\n");
    socketfd = net_connect();
    if (socketfd < 0)
        return socketfd;
    recv(socketfd, buffer, BUFFER_SIZE, 0);
    login(socketfd);
    plog("Logged in.\n");
    selectServer(socketfd);
    bzero(buffer, BUFFER_SIZE);
    getCommand(socketfd, "channellist", buffer);
    plog("Received data.\n");
    close(socketfd);
    return 0;
}

/* Connects to the server, returns socket's file descriptor */
int net_connect(){
    int socketfd, portnum;
    char temp[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    struct hostent* server;
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0){
        plog("[ERR] Couldn't create socket.\n");
        return -1;
    }
    portnum = atoi(port);
    server = gethostbyname(ip);
    if (server == NULL){
        plog("[ERR] Couldn't find host.\n");
        return -2;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnum);
    memcpy(&server_addr.sin_addr, server->h_addr_list[0], server->h_length);
    if (connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        (void) close(socketfd);
        plog("[ERR] Couldn't connect to the server.\n");
        return -3;
    }
    return socketfd;
}


/* Logs in as server query (or any account specified) */
int login(int socketfd){
    char buffer[BUFFER_SIZE];
    char value[5];
    strcpy(buffer, "login ");
    strcat(buffer, username);
    strcat(buffer, " ");
    strcat(buffer, password);
    strcat(buffer, "\n");
    int errno = send(socketfd, buffer, strlen(buffer), 0);
    if (errno <= 0){
        plog("[ERR] Couldn't log in (writing to socket failure).\n");
        return -1;
    }
    recv(socketfd, buffer, BUFFER_SIZE, 0);
    getValue(buffer, value);
    if (strcmp(value,"0") == 0)
        return 0;
    else {
        plog("[ERR] Couldn't log in. Error code: %s\n", value);
        return -1;
    }
}


/* Selects virtual server sid */
int selectServer(int socketfd){
    char buffer[BUFFER_SIZE] = "use ";;
    char value[3];
    strcat(buffer, sid);
    strcat(buffer, "\n");
    int errno = send(socketfd, buffer, strlen(buffer), 0);
    if (errno <= 0){
        plog("[ERR] Couldn't select server (writing to socket failure).\n");
        return -1;
    }
    recv(socketfd, buffer, BUFFER_SIZE, 0);
    getNextField(buffer, value, "error id=");
    if (strcmp(value,"0") == 0)
        return 0;
    else {
        plog("[ERR] Couldn't select server. Error code: %s\n", value);
        return -1;
    }
}

/* Sends command cmd and stores server's response in response */
int getCommand(int socketfd, char* cmd, char* response){
    int len = strlen(cmd);
    char command[len+1];
    strcpy(command, cmd);
    strcat(command, "\n");
    int errno = send(socketfd, command, strlen(command), 0);
    if (errno <= 0){
        plog("[ERR] Couldn't select server (writing to socket failure).\n");
        return -1;
    }
    int bytesRecv = PACKET_SIZE;
    char tempbuf[PACKET_SIZE] = "";
    /* TS3 servers send packets with strings of size 1448B (unless less data available) and
     the last packet is ended with character 13 (carriage return) */
    while (bytesRecv == PACKET_SIZE){
        bytesRecv = recv(socketfd, tempbuf, PACKET_SIZE, 0);
        tempbuf[bytesRecv] = '\0';
        strcat(response, tempbuf);
        plog("recv: %d\n", bytesRecv);
        /* In case we get less data, but server isn't done sending
         (e.g. poor internet connection)*/
        if (bytesRecv != PACKET_SIZE && tempbuf[bytesRecv-1] != 13){
            bytesRecv = PACKET_SIZE;
        }
        usleep(2500);
    }
    return 0;
}