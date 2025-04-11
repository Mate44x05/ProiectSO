#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include "treasure.h"

int main(int argc, char **argv) 
{
    if (argc != 2) 
    {
        perror("Numar invalid de argumente");
        exit(-1);
    }

// Tiparirea numelui hunt-ului
    printf("Hunt name: %s\n", argv[1]);
//

// Tiparirea informatiilor despre director
    struct stat fileStat;
    // Verificarea existentei directorului
    if (stat(argv[1], &fileStat) == -1) 
    {
        perror("Eroare la obtinerea informatiilor despre director");
        exit(-1);
    }
    //
    printf("Size: %ld\n----------------------------\n", fileStat.st_size);
//

// Listam treasure-urile din director impreuna cu informatiile despre acestea
    printf("Treasure list:\n");

    struct dirent *entry;
    char aux[32];
    snprintf(aux, sizeof(aux), "./%s", argv[1]);
    
    // Deschiderea directorului pentru parcurgerea fiecarui treasure
    DIR *dir = opendir(aux);

    if (dir == NULL) {
        perror("Nu se poate deschide directorul");
        exit(-1);
    }
    //

    // Parcurgerea fiecarui fisier din director
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
        {
            // Tiparirea numelui fiecarui fisier
            char nume[256];
            strcpy(nume, entry->d_name);
            char *id = strtok(nume, "\n");
            printf("\nTreasure ID: %s\n", id);
            //

            // Crearea unui path pentru a obtine informatiile despre fiecare fisier
            char path[320];
            snprintf(path, sizeof(path), "%s/%s", aux, entry->d_name);
                // Deschiderea fisierului pentru a obtine informatiile despre acesta
            if (stat(path, &fileStat) == -1) 
            {
                perror("\nEroare la obtinerea informatiilor despre un fisier");
                continue;
            }
                //
            //     
            
            // Tiparirea informatiilor despre fiecare fisier
            char time[20];
            strftime(time, sizeof(time), "%D %T", gmtime(&fileStat.st_mtime));
            printf("Size: %ld\n", fileStat.st_size);
            printf("Last modified: %s\n", time);
            //
        }
    }

    printf("\nEnd of list\n");

    closedir(dir);
}