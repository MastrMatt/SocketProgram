#include "mainHeader.h"

int main (int argc, char **argv) {

    if (argc != 4) {
        printf("Usage: %s <port number> <ip of server> <client number>\n", argv[0]);
        exit(1);
    }

    int clientNum = atoi(argv[3]);


    for(int i = 0; i < clientNum; i++) {
        // fork
        int forkResult = fork();
        if (forkResult == -1) {
            perror("fork failed");
            exit(1);
        }
        else if (forkResult == 0) {
            // child process
            char *args[] = {"./client", argv[1], argv[2], NULL};
            execvp(args[0], args);
        }
        else {
            // parent process
            printf("client %d started\n", i);
        }

    }

}

