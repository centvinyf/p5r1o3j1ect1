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
    if(connect(sockfd,(struct sockaddr*)&s_addr,sizeof(struct sockaddr)) == -1){
        perror("connect");
        exit(errno);
    }else
        printf("[STATUS]conncet successfully!\n");
    
    while(1){
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        maxfd = 0;
        FD_SET(sockfd, &rfds);
        if(maxfd < sockfd)
            maxfd = sockfd;
        tv.tv_sec = 6;
        tv.tv_usec = 0;
        retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
        if(retval == -1){
            printf("[ERROR]select error，server down\n");
            break;
        }else if(retval == 0){
            // printf("waiting...\n");
            continue;
        }else{
            /*A message from the server*/
            if(FD_ISSET(sockfd,&rfds)){
                /******Receive message*******/
                bzero(buf,BUFLEN);
                len = recv(sockfd,buf,BUFLEN,0);
                if(len > 0)
                   
                    // printf("服务器发来的消息是：%s\n",buf);
                    printf("The other side says: %s\n",buf);
                else{
                    if(len < 0 )
                        printf("[ERROR]Fail to receive messages\n");
                    else
                        printf("[SERVER CLOSED]The server is closed, end of chatting\n");
                break; 
                }
            }
            /*Client is typing,handle the comments*/
            if(FD_ISSET(0, &rfds)){ 
                /******send message*******/ 
                bzero(buf,BUFLEN);
                fgets(buf,BUFLEN,stdin);
               
                // if(!strncasecmp(buf,"quit",4)){
                //     printf("conversation quit successfully!\n");
                //     break;
                // }
                 if(!strncasecmp(buf,"help",4)){
                     printf("[CLEAR] Clear your screen\n");
                     printf("[CHAT] Ask the server to get a chatting partner\n");
                     printf("[FLAG] In Process of chatting, report your partner and quit the conversation\n");
                     printf("[HELP] Print all the comments that the clients can use\n");
                     printf("[QUIT] Quit the current chatting room\n");
                    // continue;
                }
                 if(!strncasecmp(buf,"clear",5)){
                    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
                 }



                else{
                    len = send(sockfd,buf,strlen(buf),0);
                 if(len > 0){
                    printf("You: %s",buf);
                 }
                    // printf("\t消息发送成功：%s\n",buf); 
                    else{
                    printf("[ERROR]Fail to send messages!\n");
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
