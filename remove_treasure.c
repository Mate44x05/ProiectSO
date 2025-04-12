#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "treasure.h"

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

    // Crearea/deschiderea fisierului pentru log
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
    snprintf(mesaj, sizeof(mesaj), "%s: Treasure ID %s a fost sters.\n", stime, argv[2]);
    if(write(log, mesaj, strlen(mesaj)) == -1)
    {
        write(log, "Eroare la scrierea in log.\n", strlen("Eroare la scrierea in log.\n"));
        perror("Eroare la scrierea in log.\n");
        close(log);
        exit(-1);
    }
    //

    close(log);
    return 0;
}