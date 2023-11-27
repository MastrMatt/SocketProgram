#include "mainHeader.h"

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 

    if(argc != 3) {
        printf("\n Usage: %s <port number> <ip of server> \n", argv[0]);
        return 1;
    } 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1])); 

    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0) {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       printf("\n Error : Connect failed \n");
       return 1;
    } 

    printf("connection made\n");


    n=read(sockfd, recvBuff, sizeof(recvBuff)-1);
    if(n<0) {
    	printf("Read problem %d\n",n);
	return 1;
    }
    recvBuff[n] = 0;
    printf( "data received: %s\n", recvBuff );

    return 0;

    char jobType;
    int jobTime;

    // while(scanf(" %c%d",&jobType,&jobTime) == 2) {

    //     printf("sending: %c\n",jobType);
    //     // n = write(sockfd, inputBuffer, strlen(inputBuffer));
    //     // if(n<0) {
    //     //     printf("Write problem %d\n",n);
    //     //     return 1;
    //     // }
    //     // printf("sent: %s\n",inputBuffer);
    //     // n=read(sockfd, recvBuff, sizeof(recvBuff)-1);
    //     // if(n<0) {
    //     //     printf("Read problem %d\n",n);
    //     //     return 1;
    //     // }
    //     // recvBuff[n] = 0;
    //     // printf( "data received: %s\n", recvBuff );
    // }

}
