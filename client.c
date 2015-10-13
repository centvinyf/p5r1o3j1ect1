<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h> //hostent

#define BUFLEN 1024
#define PORT "9034" // the port client will be connecting to


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char **argv)
{
    int sockfd = 0;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in s_addr;
    socklen_t len;
    unsigned int port;
    char buf[BUFLEN], *hostname, *ip;
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd, rv;
    int connectionStatus = -1;
    char s[INET6_ADDRSTRLEN];


    while(1){
        FD_ZERO(&rfds);
        FD_SET(0, &rfds); //fd for standard input
        FD_SET(sockfd, &rfds); //fd for socket
        maxfd = 0;
        if(maxfd < sockfd) maxfd = sockfd;
        retval = select(maxfd+1, &rfds, NULL, NULL, NULL);
        if(retval == -1){
            printf("[ERROR] select error，server down\n");
            break;
        } else {
            /*A message from the server*/
            if(FD_ISSET(sockfd, &rfds) && (connectionStatus==1)){
                //printf("reachpoint1\n");
                /******Receive message*******/
                bzero(buf,BUFLEN);
                len = recv(sockfd,buf,BUFLEN,0);
                //printf("reachpoint2\n");
                if(len > 0)
                    printf("The other side says: %s\n",buf);
                else{
                    // if(len < 0 )
                    printf("[ERROR]Fail to receive messages\n");
                    // else
                    printf("[SERVER] The server is closed, end of chatting\n");
                    break;
=======
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/time.h>

    #define BUFLEN 1024
    #define PORT 9034


    int main(int argc, char **argv)
    {
        int sockfd;
        struct sockaddr_in s_addr;
        socklen_t len;
        unsigned int port;
        char buf[BUFLEN];
        fd_set rfds;
        struct timeval tv;
        int retval, maxfd; 
        int connectionStatus = -1;
        
        /*create socket*/
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
            perror("socket");
            exit(errno);
        }else
            printf("[STATUS]socket created successfully!\n");

        
        /*setting server ip*/
        memset(&s_addr,0,sizeof(s_addr));
        s_addr.sin_family = AF_INET;
         s_addr.sin_port = htons(PORT);
        if (inet_aton(argv[1], (struct in_addr *)&s_addr.sin_addr.s_addr) == 0) {
            perror(argv[1]);
            exit(errno);
        }
        /*start connection*/ 
       /* if(connect(sockfd,(struct sockaddr*)&s_addr,sizeof(struct sockaddr)) == -1){
            perror("connect");
            exit(errno);
        } else
            printf("[STATUS]conncet successfully!\n"); */
        
        while(1){
            FD_ZERO(&rfds);
            FD_SET(0, &rfds); //fd for standard input
            maxfd = 0;
            FD_SET(sockfd, &rfds); //fd for socket
            if(maxfd < sockfd)
                maxfd = sockfd;
            retval = select(maxfd+1, &rfds, NULL, NULL, NULL);
            if(retval == -1){
                printf("[ERROR]select error，server down\n");
                break;
            } else {
                /*A message from the server*/
                if(FD_ISSET(sockfd,&rfds)){
                    /******Receive message*******/
                    bzero(buf,BUFLEN);
                    len = recv(sockfd,buf,BUFLEN,0);
                    if(len > 0)
                       
                        printf("The other side says: %s\n",buf);
                    else{
                        // if(len < 0 )
                            printf("[ERROR]Fail to receive messages\n");
                        // else
                            printf("[SERVER CLOSED]The server is closed, end of chatting\n");
                    break; 
                    }
>>>>>>> origin/master
                }
            }

            /*Client is typing,handle the comments*/
            if(FD_ISSET(0, &rfds)){
                /******send message*******/
                bzero(buf,BUFLEN);
                fgets(buf,BUFLEN,stdin);

                if((strcmp(buf,"CONNECT\n")==0) && (connectionStatus == -1)){
                    printf("Please enter hostname:\n");
                    connectionStatus = 0;
                    continue;
                }

                else if(connectionStatus == 0) {
                    hostname = strtok(buf, "\n");
                    //printf("%s\n", hostname);

                    /*setting server ip*/
                    memset(&hints,0,sizeof hints);
                    hints.ai_family = AF_INET;
                    hints.ai_socktype = SOCK_STREAM;
                    if ((rv = getaddrinfo(hostname, PORT, &hints , &servinfo)) != 0)
                    {
                        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                        return 1;
                    }

                    /* loop through all the results and connect to the first we can */
                    for(p = servinfo; p != NULL; p = p->ai_next) {
                        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                                p->ai_protocol)) == -1) {
                            perror("client: socket");
                            continue;
                        }

                        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                            close(sockfd);
                            perror("client: connect");
                            continue;
                        }
                        //printf("sockfd %d\n", sockfd);
                        connectionStatus = 1;
                        break;
                    }

                    if (p == NULL) {
                        fprintf(stderr, "client: failed to connect\n");
                        return 2;
                    }

                    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                            s, sizeof s);
                    printf("[SERVER] client connecting to %s\n", s);

                    freeaddrinfo(servinfo); // all done with this structure
                    continue;
                }

                else if((strcmp(buf,"CONNECT\n")==0) && (connectionStatus == 1)) {
                    printf("[WARNING] server has already been connected!\n");
                    continue;
                }

                else if((strcmp(buf,"HELP\n")==0)){
                    printf("[CONNECT] Connect to host server\n");
                    printf("[CLEAR] Clear your screen\n");
                    printf("[CHAT] Ask the server to get a chatting partner\n");
                    printf("[FLAG] In Process of chatting, report your partner and quit the conversation\n");
                    printf("[HELP] Print all the comments that the clients can use\n");
                    printf("[QUIT] Quit the current chatting room\n");
                    printf("[EXIT] Exit the chatting program\n");
                    continue;
                }

                else if((strcmp(buf,"CLEAR\n")==0)){
                    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
                    continue;
                }

                else if((strcmp(buf,"EXIT\n")==0)){
                    printf("[STATUS] exit the program.\n");
                    break;
                }

                else if(connectionStatus == -1){
                    printf("[WARNING] Please connect to host server first!\n");
                    continue;
                }


                else {
                    len = send(sockfd,buf,strlen(buf),0);
                    if(len > 0){
                        printf("You: %s",buf);
                    }
                    // printf("\t消息发送成功：%s\n",buf);
                    else
                    {
                        printf("[ERROR] Fail to send messages!\n");
                        break;
                    }
                }
            }

        }

    }

    /*关闭连接*/
    close(sockfd);

    return 0;
}
