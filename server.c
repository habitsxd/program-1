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

char* num_to_text(int num);
int text_to_num(char* text);

int main(){
    //data section
    int fd;
    int fd1;
    int pid;
    char client_fifo[80];
    char client_name[80];
    int client_pid;
    int server_call;
    char* server = "/tmp/server";
    int client_input;
    char clientinpt[10];
    char server_response[80];
    char* str;
    int mem;
    char mem2[80];
    int type;
    //data section end

    //make server fifo
    if(mkfifo(server,0777) == -1){
        if(errno != EEXIST){
            printf("Error: Fifo exists.\n");
            return EXIT_FAILURE;
        }
    }

    //read client input
    printf("connecting to client!\n");
    fd = open(server,O_RDONLY);
    if(read(fd,&pid,sizeof(int)) == -1){
        return 3;
    }
    if(read(fd,client_fifo,80) == -1){
        return 3;
    }
    if(read(fd,client_name,80) == -1){
        return 3;
    }
    //output client data from server fifo
    printf("finished reading client data!\n");
    printf("Client pid: %d\n", pid);
    printf("Client fifo name: %s\n", client_fifo);
    printf("Client number: %s\n", client_name);
    close(fd);

    //connect to client fifo
    printf("Connecting to client-specific fifo!\n");
    fd1 = open(client_fifo,O_WRONLY);
    //write to client fifo (test only will be deleted)
    char dummy[80];
    printf("please enter a message to client: ");
    fgets(dummy,80,stdin);
    write(fd,dummy,80);
    close(fd1);

    //take system calls
    while(1){
        fd = open(server, O_RDONLY);
        if(read(fd,&server_call,sizeof(int)) == -1){
        return 3;
        }
        close(fd);
        switch(server_call){
            case 1:
                //number to text case
                printf("Recieved syscall 1 (number to text) request from client %s with pid %d.\n", client_name, pid); 
                fd = open(server, O_RDONLY);
                read(fd, &client_input, sizeof(int));
                close(fd);
                //convert integer to text and send back to client
                fd1 = open(client_fifo, O_WRONLY);
                str = num_to_text(client_input);
                strcpy(server_response, str);
                write(fd1, server_response, 80);
                close(fd1);
                printf("Converted number is: %s\n", server_response);
                printf("Waiting for next system call...\n");
                break;
            case 2:
                //text to number case
                printf("Recieved syscall 2 (text to number) request from client %s with pid %d.\n", client_name, pid); 
                fd = open(server, O_RDONLY);
                read(fd,clientinpt, 10);
                close(fd);
                int num = text_to_num(clientinpt);
                fd1 = open(client_fifo, O_WRONLY);
                write(fd1, &num, sizeof(int));
                close(fd1);
                printf("Converted number is: %d\n", num);
                printf("Waiting for next system call...\n");
                break;
            case 3:
                //store value in the server
                printf("Recieved syscall 3 (store) request from client %s with pid %d.\n", client_name, pid);
                fd = open(server, O_RDONLY);
                read(fd, &mem, sizeof(int));
                close(fd);
                printf("Value %d stored!\n", mem);
                printf("Waiting for next system call...\n");
                break;
            case 4:
                //recall stored value
                printf("Recieved syscall 4 (recall) request from client %s with pid %d.\n", client_name, pid);
                fd1 = open(client_fifo, O_WRONLY);
                write(fd, &mem, sizeof(int));
                close(fd1);
                printf("Value %d recalled!\n", mem);
                printf("Waiting for next system call...\n"); 
                break;
            case 0:
                printf("Recieved syscall 0 (EXIT) request from client %s with pid %d.\n", client_name, pid);
                printf("Disconnecting from client!\n");
                printf("Waiting for new client.....");
                break;
            case -1:
                printf("Recieved syscall -1 (TERMINATE) request from client %s with pid %d.\n", client_name, pid);
                printf("Terminating server....");
                exit(0);
            default:
                break;

        }
        


    }

}

char* num_to_text(int num){
    char* server_response;
    switch(num){
            case 0:
                server_response = "zero";
                break;
            case 1:
                server_response = "one";
                break;
            case 2:
                server_response = "two";
                break;     
            case 3:
                server_response = "three";
                break;
            case 4:
                server_response = "four";
                break;
            case 5:
                server_response = "five";
                break;
            case 6:
                server_response = "six";
                break;
            case 7:
                server_response = "seven";
                break;
            case 8:
                server_response = "eight";
                break;
            case 9:
                server_response = "nine";
                break;
            default:
                break;
            }
    return server_response;
}

int text_to_num(char* text){
    int num;
    if(strcmp(text, "zero") == 0){
        num = 0;
    }
    if(strcmp(text, "one") == 0){
        num = 1;
    }
    if(strcmp(text, "two") == 0){
        num = 2;
    }
    if(strcmp(text, "three") == 0){
        num = 3;
    }
    if(strcmp(text, "four") == 0){
        num = 4;
    }
    if(strcmp(text, "five") == 0){
        num = 5;
    }
    if(strcmp(text, "six") == 0){
        num = 6;
    }
    if(strcmp(text, "seven") == 0){
        num = 7;
    }
    if(strcmp(text, "eight") == 0){
        num = 8;
    }
    if(strcmp(text, "nine") == 0){
        num = 9;
    }

    return num;
    
}