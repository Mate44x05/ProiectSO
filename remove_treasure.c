#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    // Verificarea numarului de argumente
    if(argc != 3)
    {
        perror("Numar de argumente invalid");
        exit(-1);
    }
    //

    // Crearea caii catre fisierul binar
    char path[80];
    snprintf(path, sizeof(path), "./%s/%s.bin", argv[1], argv[2]);
    //

    // Stergerea fisierului binar
    if(remove(path)==-1)
    {
        perror("Eroare la stergerea fisierului");
        exit(-1);
    }
    //
    
    printf("Treasure sters cu succes!\n");
    return 0;
}