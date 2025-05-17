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
    printf("\n\nHunt name: %s\n", argv[1]);
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
    int totalSize = 0;
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
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, "logged_hunt") != 0)
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
            struct tm tm_info;
            localtime_r(&fileStat.st_mtime, &tm_info);
            strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", &tm_info);
            totalSize += fileStat.st_size;
            printf("Size: %ld\n", fileStat.st_size);
            printf("Last modified: %s\n", time);
            //
        
        }
    }
    //

    printf("\nEnd of list\n");
    printf("Total size: %d bytes\n\n", totalSize);
    
    // Deschiderea fisierului pentru log
    char log_path[128];
    snprintf(log_path, sizeof(log_path), "./%s/logged_hunt", argv[1]);
    int log = open(log_path, O_WRONLY | O_APPEND);
    if(log == -1)
    {
        perror("Eroare la deschiderea fisierului de log.\n");
        exit(-1);
    }
    //

    // Scrierea in log
    char mesaj[128];
    time_t tm;
    struct tm *tm_info;
    char stime[20]; 
    time(&tm);
    tm_info = localtime(&tm);
    strftime(stime, sizeof(stime), "%Y-%m-%d %H:%M:%S", tm_info);
    snprintf(mesaj, sizeof(mesaj), "%s: Hunt-ul a fost listat.\n", stime);
    if(write(log, mesaj, strlen(mesaj)) == -1)
    {
        write(log, "Eroare la scrierea in log.\n", strlen("Eroare la scrierea in log.\n"));
        perror("Eroare la scrierea in log.\n");
        close(log);
        closedir(dir);
        exit(-1);
    }
    //
    
    close(log);
    closedir(dir);

    return 0;
}