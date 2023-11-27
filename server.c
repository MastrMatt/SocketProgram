#include "mainHeader.h"

double getCurrentTimeInSeconds() {
    struct timeval time;

    //gettimeofday is thread safe, wrapper around a thread safe system call
    gettimeofday(&time, NULL);

    double seconds = time.tv_sec + (time.tv_usec / 1e6);

    //do not always need to check return value
    return seconds;
}


int main(int argc, char *argv[])
{
        if(argc != 2) {
                printf("\n Usage: %s <port number> \n", argv[0]);
                return 1;
        }

        int transNum = 0;

        int listenfd = 0, connfd = 0;
        struct sockaddr_in serv_addr; 
        int bytes;

        char sendBuff[1025];
        time_t ticks; 

        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&serv_addr, '0', sizeof(serv_addr));
        memset(sendBuff, '0', sizeof(sendBuff)); 

        serv_addr.sin_family = AF_INET;

        //a machine can have different IP addresses depending on network interfaces, ex: wifi, ethernet, etc., so use INADDR_ANY to bind to all of them on the specified port
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        printf("using port %s\n",argv[1]);
        serv_addr.sin_port = htons(atoi(argv[1])); 

        bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

        listen(listenfd, 10); 


        while(1)
        {
        printf( "wait for a connection\n" );

        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        if ( connfd < 0 ) 
        {
                perror( "accept failed" );
                exit( 1 );
        }
        else if ( connfd > 0) {
                transNum++;
                printf("connection made, transaction number: %d\n", transNum);
        }

        //sucessfully connected, now read transaction type and time
        char jobType;
        int jobTime;

        bytes = read(connfd, &jobType, sizeof(jobType));
        if( bytes != sizeof(jobType) ) 
        {
                perror("Read failed");
                exit(1);
        }

        bytes = read(connfd, &jobTime, sizeof(jobTime));
        if( bytes != sizeof(jobTime) ) 
        {
                perror("Read failed");
                exit(1);
        }

        char hostname[256];
        if(gethostname(hostname, sizeof(hostname) - 1) != 0) {
                perror("gethostname failed");
                exit(1);
        }

        pid_t pid = getpid();
        if(pid < 0) {
                perror("getpid failed");
                exit(1);
        }

        double currentTime = getCurrentTimeInSeconds();

        printf("%.2f: #  %-3d  (%c%3d) from %s.%d\n", currentTime, transNum, jobType, jobTime, hostname, pid);


        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        bytes = write(connfd, sendBuff, strlen(sendBuff) );
        if( bytes != strlen(sendBuff) ) 
        {
                perror("Write failed");
                exit(1);
        }
        printf( "data sent: %s\n", sendBuff );

        close(connfd);
        sleep(1);
        }
}
