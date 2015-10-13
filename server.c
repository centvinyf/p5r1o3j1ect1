/*
** selectserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9034"   // port we're listening on
#define BUFLEN 1024
#define MAX_WAITING 20
// get sockaddr, IPv4 or IPv6:
    int flag = -1;    // the indicator of clients waiting for chat
    int *partner_fd;
    int *hate_fd;
    int blacklist[20];
    int number_of_blacked=0;
    fd_set master;    // master file descriptor list
    fd_set read_fds;
    fd_set adm;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
    int flagged_number=0;
    int number_of_clients=0;
    int number_of_clients_in_chatting=0;
    int chatting[20];

    int waitingfd;
    int wait_number;
    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];
    char buf2[256];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;
    pthread_t threadforadmin;

void *get_in_addr(struct sockaddr *sa)
{     
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int intIsinSet(int a[],int x,int n)
{   
    for(int w =0;w<n;w++)
    {
        if(a[w]==x) return 1;
    }
    return 0;
}
void*  admin()
{
    for(int i =0;i<20;i++)
      {
        blacklist[i]=-1;
        chatting[i]=-1;
      }  

    while(scanf("%s",buf2)!=EOF)
    {
        // // if(fgets(buf2,256,stdin) != NULL)
        // //     printf("%s\n",buf2);
        // // else
        // //     printf("test");
        // FD_SET(0,&adm); // copy it
        // // if (select(fdmax+1, &adm, NULL, NULL, NULL) == -1) {
        // //     perror("select");
        // //     exit(4);
        // // }
        //  if (select(1, &adm, NULL, NULL, NULL) == -1) {
        //     perror("select");
        //     exit(4);
        // }
        // if(FD_ISSET(0,&adm))
        // {
        //     bzero(buf2,BUFLEN);
        //     fgets(buf2,BUFLEN,stdin);
            if(!strncasecmp(buf2,"STATS",5))
            {
                printf("The number of clients are: %d\n",number_of_clients);
                printf("The number of clients in chatting are: %d\n",number_of_clients_in_chatting);
                for(int i=0;i<=fdmax;i++){
                    if(hate_fd[i]!=-1)
                    printf("User %d flagged user %d\n",i,hate_fd[i]);
                    if(i==fdmax&&flagged_number==0)
                        printf("NOBODY has been flagged yet!\n");
                }
                    
            }
            else if(!strncasecmp(buf2,"START",5))
            {
                // FD_SET(listener, &master);
            }
            else if(!strncasecmp(buf2,"BLOCK",5))
            {
                int tem;
                printf("Please enter the socket you want to block: ");
                scanf("%d",&tem);
                blacklist[number_of_blacked++]=tem;
                printf("Socket %d has been blocked successfully!\n",tem);
            }
            else if(!strncasecmp(buf2,"UNBLOCK",7))
            {
                int tem;
                printf("The BLACKLIST is as follows:\n");
                for(int i =0;i<20;i++)
                if(blacklist[i]!=-1)
                    printf("%d\n",blacklist[i]);
                printf("Please enter the socket you want to unblock\n");
                scanf("%d",&tem);
                for(int i =0;i<20;i++)
                if(blacklist[i]==tem)
                {
                    blacklist[i]=-1;
                    printf("Socket %d has been unblocked successfully!\n",tem);
                    break;
                }
            }
            else if(!strncasecmp(buf2,"THROWOUT",8))
            {
                int tem;
                
                if(number_of_clients_in_chatting>0)
                {
                    printf("The current chattings in session are:\n");
                for(int i=0;i<number_of_clients_in_chatting;i+=2)
                    printf("User %d is chatting with user %d\n",chatting[i],chatting[i+1]);
                printf("Please enter the socket of user that you want to throwout:\n");
                    scanf("%d",&tem);
                if(!intIsinSet(chatting,tem,number_of_clients_in_chatting))
                {
                    printf("Invalid Input,Command QUIT!\n");
                    break;
                }
                    
                printf("User %d has been thrown out successfully!\n",tem);
                send(partner_fd[tem],"[FROM SERVER]Your chatting ends because your chatting mate has been thrown out by the ADMIN\n",256,0);
                send(tem,"[FROM SERVER]Your chatting ends because you have been thrown out by the ADMIN\n",256,0);
                for(int i=0;i<number_of_clients_in_chatting;i++)
                {
                    if(chatting[i]==partner_fd[tem])
                         {
                                int temp=chatting[number_of_clients_in_chatting-1];
                                chatting[number_of_clients_in_chatting-1]=chatting[i];
                                chatting[i]=temp;break;
                         }
                }
                for(int j=0;j<number_of_clients_in_chatting;j++)
                {
                    if(chatting[j]==tem)
                         {
                                int temp=chatting[number_of_clients_in_chatting-2];
                                chatting[number_of_clients_in_chatting-2]=chatting[j];
                                chatting[j]=temp;break;
                         }
                }   
                partner_fd[partner_fd[tem]]=-1;
                partner_fd[tem]=-1;
                number_of_clients_in_chatting-=2;
                }
                else
                {
                    printf("No current chatting is in session!\n");
                }
                
               
            }
            else if(!strncasecmp(buf2,"CLEAR",5))
            {
                printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

            }
            else if(!strncasecmp(buf2,"HELP",4))
            {
                printf("[BLOCK] To block a client so that he cannot be chosen to have a chat!\n");
                printf("[CLEAR] To clear the screen\n");
                printf("[HELP] Show all available commands for admins!\n");
                printf("[STATS] To See the detail of clients!\n");
                printf("[THROWOUT] To throw clients out of their chatting room!\n");
                printf("[UNBLOCK] To unblock a client!\n");

            }
            continue;
        }
    // }
    return NULL;
}
int main(void)
{
    
    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);   //Zero the address points to hints
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
    hate_fd = (int *)malloc(100 * sizeof(int));
    for(int i=0;i<100;i++)
        hate_fd[i]=-1;
    pthread_create(&threadforadmin,NULL,admin,(void*)NULL);

	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}
	
	for(p = ai; p != NULL; p = p->ai_next) {
    	listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        // printf("%d\n", listener); //DEBUG
		if (listener < 0) { 
			continue;
		}
		
		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this
 
    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);
    // FD_SET(0,&master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one
    partner_fd = (int *)malloc(fdmax * sizeof(int));
    
    // reset the partner array
    for(i = 0; i <= fdmax; i++) {
        partner_fd[i] = -1;
        hate_fd[i]=-1;
    }

    // printf("%d\n", listener); //DEBUG

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // printf("hello\n");

            
        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                // if(i==0) continue;
                if (i == listener) {    //there is a new
                    // handle new connections
                    addrlen = sizeof remoteaddr;
					newfd = accept(listener,
						(struct sockaddr *)&remoteaddr,
						&addrlen);

					if (newfd == -1) {
                        perror("accept");
                    } 
                    else   //create a new client
                    {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            for (i = fdmax+1; i <= newfd; i++) {
                                partner_fd[i] = -1;
                            }
                            fdmax = newfd;
                        }
                        number_of_clients++;
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                            send(partner_fd[i],"[FROM SERVER]Your chatting partner left the room",256,0);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master);
                        number_of_clients--; // remove from master set
                        // disconnect the chat status of client and its partner
                        if (partner_fd[i] != -1) {
                            partner_fd[partner_fd[i]] = -1;
                            partner_fd[i] = -1; // re
                        }

                    } else {   // we got some data from a client
                        // send to partner client
                        for(j = 0; j <= fdmax; j++) {
                            if (FD_ISSET(j, &master) && (partner_fd[j] == i)) {
                                if (send(j, buf, nbytes, 0) == -1) {
                                    perror("send");
                                }
                            }
                        }
                        // Parsing CHAT command
                        if (!strncasecmp(buf,"CHAT",4)) 
                        {
                            if (flag == -1){
                                if(partner_fd[i]==-1)
                                {
                                flag = 0;
                                waitingfd = i;
                                wait_number++;
                            }   } // one client is waiting for chat
                            else {
                                if(i==waitingfd)
                                    continue;
                                else{
                                    for(int k=0;k<wait_number+1;k++)
                                    {
                                        if(waitingfd!=hate_fd[i]&&hate_fd[waitingfd]!=i&&waitingfd!=-1&&!intIsinSet(blacklist,i,20))
                                {
                                    partner_fd[i] = waitingfd;   
                                partner_fd[waitingfd] = i;
                                chatting[number_of_clients_in_chatting]=i;
                                chatting[number_of_clients_in_chatting+1]=partner_fd[i];
                                waitingfd=-1;
                                send(partner_fd[i],"[FROM SERVER]Your chatting starts, say \"hi\" to your partner:)\n",256,0);
                                send(i,"[FROM SERVER]Your chatting starts, say \"hi\" to your partner:)\n",256,0);
                                flag = -1;
                                wait_number -= 1;number_of_clients_in_chatting+=2; break;
                                }
                                    }
                                    waitingfd=i;
                                    
                                }
                                
                                
                                
                            }// another client wants to chat
                            // printf("%d\n", flag);
                        }

                        if(!strncasecmp(buf,"QUIT",4))
                        {
                            
                            send(partner_fd[i],"[FROM SERVER]Your chatting ends because your chatting mate quits the room.\n",256,0);
                                send(i,"[FROM SERVER]Your chatting ends\n",256,0);
                                for(int i=0;i<number_of_clients_in_chatting;i++)
                {
                    if(chatting[i]==partner_fd[i])
                         {
                                int temp=chatting[number_of_clients_in_chatting-1];
                                chatting[number_of_clients_in_chatting-1]=chatting[i];
                                chatting[i]=temp;break;
                         }
                }
                for(int j=0;j<number_of_clients_in_chatting;j++)
                {
                    if(chatting[j]==i)
                         {
                                int temp=chatting[number_of_clients_in_chatting-2];
                                chatting[number_of_clients_in_chatting-2]=chatting[j];
                                chatting[j]=temp;break;
                         }
                }   
                                partner_fd[partner_fd[i]]=-1;
                            partner_fd[i]=-1;
                            flag=-1;
                            number_of_clients_in_chatting-=2;
                            chatting[number_of_clients_in_chatting]=-1;
                            chatting[number_of_clients_in_chatting+1]=-1;
                        }

                        if(!strncasecmp(buf,"FLAG",4))
                        {
                            hate_fd[i]=partner_fd[i];
                            send(partner_fd[i],"[FROM SERVER]Your chatting ends because your chatting mate FLAGed you.\n",256,0);
                                send(i,"[FROM SERVER]Flag successfully, your chatting ends\n",256,0);
                                for(int i=0;i<number_of_clients_in_chatting;i++)
                {
                    if(chatting[i]==partner_fd[i])
                         {
                                int temp=chatting[number_of_clients_in_chatting-1];
                                chatting[number_of_clients_in_chatting-1]=chatting[i];
                                chatting[i]=temp;break;
                         }
                }
                for(int j=0;j<number_of_clients_in_chatting;j++)
                {
                    if(chatting[j]==i)
                         {
                                int temp=chatting[number_of_clients_in_chatting-2];
                                chatting[number_of_clients_in_chatting-2]=chatting[j];
                                chatting[j]=temp;break;
                         }
                }   
                                partner_fd[partner_fd[i]]=-1;
                                partner_fd[i]=-1;
                                flagged_number++;
                                number_of_clients_in_chatting-=2;
                                chatting[number_of_clients_in_chatting]=-1;
                                chatting[number_of_clients_in_chatting+1]=-1;
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}

