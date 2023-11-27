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
    int sockfd = 0, n = 0;
    char sendBuffer[1024];
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    double curTime; 
    int numSent = 0;

    if(argc != 3) {
        printf("\n Usage: %s <port number> <ip of server> \n", argv[0]);
        return 1;
    } 

    //specify socket type, SOCK_STREAM for TCP
    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1])); 

    //convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0) {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    //connect to server
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       printf("\n Error : Connect failed \n");
       return 1;
    } 

    //get pid and hostname
    char temp[1024];
    char hostNameString[1024];

    pid_t pid = getpid();
    sprintf(temp, "%d", pid);

    if(gethostname(hostNameString, sizeof(hostNameString) - 1) != 0) {
        perror("gethostname failed");
        exit(1);
    }
    
    strcat(hostNameString, "-");
    strcat(hostNameString, temp);

    printf("Using port: %s\n", argv[1]);
    printf("Using server adress: %s\n", argv[2]);
    printf("Host: %s\n\n", hostNameString);
 
    char jobType;
    int jobTime;

    //run a loop that reads from stdin until EOF
    while(scanf(" %c%d",&jobType,&jobTime) == 2) {

        sprintf(sendBuffer, "%c%d", jobType, jobTime);

        if(gethostname(temp, sizeof(temp) - 1) != 0) {
                perror("gethostname failed");
                exit(1);
        }

        strcat(sendBuffer, hostNameString);

        //send message to server
        n = write(sockfd, sendBuffer, strlen(sendBuffer) + 1);
        if (n < 0)
        {
            perror("write failed");
            exit(1);
        }

        curTime = getCurrentTimeInSeconds();
        printf("%.2f: Send (%c%3d)\n", curTime, jobType, jobTime);
        numSent++;

       
        //read response from server, specifies how many transactions have been processed by the server
        n = read(sockfd, recvBuff, sizeof(recvBuff) - 1);

        if (n < 0)
        {
            perror("fscanf failed");
            exit(1);
        }

        curTime = getCurrentTimeInSeconds();
        printf("%.2f: Recv (%c%3d)\n", curTime, recvBuff[0], atoi(recvBuff + 1));
    }

    //send quit message
    sprintf(sendBuffer, "Q");
    n = write(sockfd, sendBuffer, strlen(sendBuffer) + 1);
    if (n < 0)
    {
        perror("write failed");
        exit(1);
    }

    printf("Sent %d transactions\n", numSent);

    close(sockfd);
    return 0;
}
