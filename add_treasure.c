#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "treasure.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        perror("Numărul de argumente este incorect.\n");
        exit(-1);
    }

    struct treasure t;
// Scrierea in fisier a fiecarui camp    
    
    printf("Treasure ID: ");
    fgets(t.id, sizeof(t.id), stdin);
    printf("User name: ");
    fgets(t.user, sizeof(t.user), stdin);
    printf("Clue: ");
    fgets(t.clue, sizeof(t.clue), stdin);
    printf("Latitude: ");
    scanf("%f", &t.latitude);
    printf("Longitude: ");
    scanf("%f", &t.longitude);
    printf("Value: ");
    scanf("%d", &t.value);
    // Crearea fisierului pentru treasure-ul adaugat
    char name[128];
    snprintf(name, sizeof(name), "./%s/%s.bin", argv[1], t.id);
    int FD = open(name, O_WRONLY | O_APPEND | O_CREAT, 0777);
    if(FD==-1)
    {
        // Creare director daca acesta nu exista
        mkdir(argv[1], 0777);
        FD = open(name, O_WRONLY | O_APPEND | O_CREAT, 0777);
        if(FD==-1)
        {
            perror("Eroare la crearea unui nou treasure.\n");
            exit(-1);
        }
        //
    }
    //

    ssize_t n = write(FD, &t, sizeof(t));
    if (n != sizeof(t)) 
    {
        perror("Eroare la scrierea completă a structurii");
        close(FD);
        exit(-1);
    }   

    close(FD);
//
    return 0;
}