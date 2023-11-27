#include "mainHeader.h"

double getCurrentTimeInSeconds()
{
       struct timeval time;

       // gettimeofday is thread safe, wrapper around a thread safe system call
       gettimeofday(&time, NULL);

       double seconds = time.tv_sec + (time.tv_usec / 1e6);

       // do not always need to check return value
       return seconds;
}

int main(int argc, char *argv[])
{
       if (argc != 2)
       {
               printf("\n Usage: %s <port number> \n", argv[0]);
               return 1;
       }

       struct sockaddr_in serv_addr;

       char tempBuffer[1024];
       char senderInfo[1024];

       char jobType;

       int transNum = 0;
       int listenfd = 0, connfd = 0;
       int jobTime;
       int numItems;

       listenfd = socket(AF_INET, SOCK_STREAM, 0);
       memset(&serv_addr, '0', sizeof(serv_addr));

       serv_addr.sin_family = AF_INET;

       // a machine can have different IP addresses depending on network interfaces, ex: wifi, ethernet, etc., so use INADDR_ANY to bind to all of them on the specified port
       serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

       printf("using port %s\n", argv[1]);
       serv_addr.sin_port = htons(atoi(argv[1]));

       bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

       listen(listenfd, 10);

       fd_set rfds;
       struct timeval tv;
       int retval;

       while (1)
       {
                //use fd_set to wait for either a client connection or a timeout
               FD_ZERO(&rfds);
               FD_SET(listenfd, &rfds);
               tv.tv_sec = 30;
               tv.tv_usec = 0;

                //select is a blocking call, so it will wait for either a client connection or a timeout
                //the +1 is because the first parameter is the highest file descriptor in the set plus 1 (not the number of file descriptors)
               retval = select(listenfd + 1, &rfds, NULL, NULL, &tv);

               if (retval == -1)
               {
                      perror("select()");
                      exit(1);
               }
               else if (retval)
               {
                      connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
                      if (connfd < 0)
                      {
                              perror("accept failed");
                              exit(1);
                      }

                      int notDone = 1;

                      // create a loop that reads from the socket until the client sends a 'Q' message
                      while (notDone)
                      {
                              transNum++;
                              numItems = read(connfd, tempBuffer, sizeof(tempBuffer));

                              if (tempBuffer[0] == 'Q')
                              {
                                     notDone = 0;
                                     break;
                              }

                              if (numItems < 0)
                              {
                                     perror("read failed");
                                     exit(1);
                              }

                              double currentTime = getCurrentTimeInSeconds();

                              jobType = tempBuffer[0];
                              jobTime = atoi(tempBuffer + 1);
                              strcpy(senderInfo, tempBuffer + 2);

                              printf("%.2f: # %-3d (%c%3d) from %s\n", currentTime, transNum, jobType, jobTime, senderInfo);

                              // perform job received from client
                              if (jobType == 'T')
                              {
                                     Trans(jobTime);
                              }
                              else if (jobType == 'S')
                              {
                                     Sleep(jobTime);
                              }
                              else
                              {
                                     printf("Invalid job type\n");
                                     exit(1);
                              }

                              // print job completed
                              printf("%.2f: # %-3d (Done) from %s\n", currentTime, transNum, senderInfo);

                              // notify client that job is done
                              sprintf(tempBuffer, "D%d", transNum);

                              // write to socket
                              numItems = write(connfd, tempBuffer, strlen(tempBuffer) + 1);
                              if (numItems < 0)
                              {
                                     perror("write failed");
                                     exit(1);
                              }

                              printf("This is message to send: %s\n", tempBuffer);

                              // print job completed
                              printf("%.2f: # %-3d (Done) to  %s\n", currentTime, transNum, senderInfo);
                      }

                      close(connfd);
               }
               else
               {
                      printf("No client connected within thirty seconds. Exiting.\n");
                      exit(0);
               }
       }
}
