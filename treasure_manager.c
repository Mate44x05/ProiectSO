#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void add(char* hunt_name)
{
    char buff[128];
    int bytesRead; 
    char *aux=NULL;

    write(STDOUT_FILENO, "Treasure ID: ", strlen("Treasure ID: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    char name[128];
    strcpy(name,"./");strcat(name,hunt_name);
    strcat(name,"/");strcat(name,aux);
    printf("%s",name);
    int FD = open(name, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if(FD==-1)
    {
        mkdir(hunt_name, 0777);
        FD = open(name, O_WRONLY | O_APPEND | O_CREAT, 0666);
        if(FD==-1)
        {
            perror("Eroare la crearea unui nou treasure.\n");
            exit(-1);
        }
    }
    strcat(aux, ", ");
    write(FD, aux, strlen(aux));

    write(STDOUT_FILENO, "User name: ", strlen("User name: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    strcat(aux, ", ");
    write(FD, aux, strlen(aux));

    write(STDOUT_FILENO, "Latitude: ", strlen("Latitude: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    strcat(aux, ", ");
    write(FD, aux, strlen(aux));

    write(STDOUT_FILENO, "Longitude: ", strlen("Longitude: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    strcat(aux, ", ");
    write(FD, aux, strlen(aux));

    write(STDOUT_FILENO, "Clue: ", strlen("Clue: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    strcat(aux, ", ");
    write(FD, aux, strlen(aux));

    write(STDOUT_FILENO, "Value: ", strlen("Value: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    strcat(aux, "\n");
    write(FD, aux, strlen(aux));

    close(FD);
}

int main(void)
{
    add("test");
    return 0;
}