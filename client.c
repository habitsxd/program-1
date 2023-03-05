#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>

int main(){
    //data section
    int fd;
    int fd1;
    char* client = "/tmp/client";
    char client_name[80];
    int pid;
    char* server = "/tmp/server";
    int client_choice;
    int server_call;
    int inpt1;
    char inpt2[10];
    char server_response[80];
    int servrsp2;
    int mem;
    //data section end
    
    start:
    ///begin program
    printf("welcome! connecting to server!\n");

    //create client specific fifo name
    char fifo_name[80];
    strcpy(fifo_name,client);//copy client name into fifo name buffer
    printf("please enter a client name: ");//prompt for client name
    fgets(client_name,80,stdin);
    strcat(fifo_name,client_name);
    //printf("%s",fifo_name);

    //get pid to send to server
    pid = getpid();

    //open server fifo, write client fifo name, client name, and pid to server
    fd = open(server,O_WRONLY);
    if(write(fd,&pid,sizeof(int)) == -1){
        return 3;
    }
    if(write(fd,fifo_name, 80) == -1){
        return 3;
    }
     if(write(fd,client_name, 80) == -1){
        return 3;
    }
    close(fd);
    printf("finished writing client data!\n");

    //make client specific fifo
    if(mkfifo(fifo_name,0777) == -1){
        if(errno != EEXIST){
            printf("Error: Fifo exists.\n");
            return EXIT_FAILURE;
        }
    }
    //open client fifo
    fd1 = open(fifo_name,O_RDONLY);
    char dummy[80];
    read(fd1,dummy,80);
    printf("%s", dummy);
    close(fd1);

    //enter loop to take system calls
    while(1){
        fd = open(server, O_WRONLY);
        printf("Enter next action(1: System Call, 2: EXIT, 3: Terminate Client and Server): ");
        scanf("%d", &client_choice);
        //take system call and send it to server if choice is 1
        if(client_choice == 1){
            printf("What system call would you like to make? (-1-4): ");
            scanf("%d", &server_call);
            write(fd,&server_call,sizeof(int));
            close(fd);

            //send data according to server call
            switch(server_call){
                case 1:
                    //send int value to convert to text
                    fd = open(server, O_WRONLY);
                    printf("Enter a number 0-9: ");
                    scanf("%d", &inpt1);
                    write(fd, &inpt1, sizeof(int));
                    close(fd);
                    fd1 = open(fifo_name, O_RDONLY);
                    read(fd1, server_response, 80);
                    close(fd1);
                    printf("Converted number is: %s\n", server_response);
                    break;
                case 2:
                    //send text value to convert to int
                    fd = open(server, O_WRONLY);
                    printf("Enter a text number: ");
                    scanf("%s", inpt2);
                    write(fd,inpt2,10);
                    close(fd);
                    fd1 = open(fifo_name,O_RDONLY);
                    read(fd1, &servrsp2, sizeof(int));
                    close(fd1);
                    printf("Converted number is: %d\n", servrsp2);
                    break;
                case 3:
                    //send value to store in server
                    fd = open(server, O_WRONLY);
                    printf("Enter an integer value to store: ");
                    scanf("%d", &mem);
                    write(fd, &mem, sizeof(int));
                    close(fd);
                    break;
                case 4:
                    //recieve stored value from server
                    fd1 = open(fifo_name, O_RDONLY);
                    read(fd1, &mem, sizeof(int));
                    close(fd1);
                    printf("Recalled value: %d\n", mem);
                    break;
                default:
                    break;

            }

        }
        //exit client
        else if(client_choice == 2){
            //send exit server call to server
            printf("Exiting the client!\n");
            fd = open(server, O_WRONLY);
            int serv_call = 0;
            write(fd, &serv_call, sizeof(int));
            close(fd);
            exit(0);
        }
        //terminate client and server
        else{
            printf("Exiting the client, and terminating server!\n");
            fd = open(server, O_WRONLY);
            int serv_call = -1;
            write(fd, &serv_call, sizeof(int));
            close(fd);
            exit(0);
        }

    }

}