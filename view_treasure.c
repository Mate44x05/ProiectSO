#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "treasure.h"

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        perror("Numar de argumente invalid");
        exit(-1);
    }
    // Crearea caii catre fisierul binar
    char path[80];
    snprintf(path, sizeof(path), "./%s/%s.bin", argv[1], argv[2]);
    // 
    
    // Deschiderea fisierului binar pentru citire
    int FD = open(path, O_RDONLY);
    if(FD == -1)
    {
        perror("Eroare la deschiderea fisierului");
        exit(-1);
    }
    //

    printf("Treasure ID: %s\n\n", argv[2]);

    // Citirea datelor din fisierul binar + afisarea fiecarei structuri
    struct treasure t;
    ssize_t bytesRead;
    while ((bytesRead = read(FD, &t, sizeof(t))) == sizeof(t))
    {
        printf("User: %s\n", t.user);
        printf("Latitude: %f\n", t.latitude);
        printf("Longitude: %f\n", t.longitude);
        printf("Clue: %s\n", t.clue);
        printf("Value: %d\n\n", t.value);
    }
    //
    close(FD);
    return 0;
}
