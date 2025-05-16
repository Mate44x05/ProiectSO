#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
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
        printf("Eroare: Fișierul pentru comoară nu există: %s\n", path);
        exit(0);
    }   
    //

    printf("Treasure ID: %s\n\n", argv[2]);

    // Citirea datelor din fisierul binar + afisarea fiecarei structuri
    struct treasure t;
    while (read(FD, &t, sizeof(t)) == sizeof(t))
    {
        printf("User: %s\n", t.user);
        printf("Latitude: %f\n", t.latitude);
        printf("Longitude: %f\n", t.longitude);
        printf("Clue: %s\n", t.clue);
        printf("Value: %d\n\n", t.value);
    }
    //

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
    snprintf(mesaj, sizeof(mesaj), "%s: Treasure ID %s a fost listat.\n", stime, argv[2]);
    if(write(log, mesaj, strlen(mesaj)) == -1)
    {
        write(log, "Eroare la scrierea in log.\n", strlen("Eroare la scrierea in log.\n"));
        perror("Eroare la scrierea in log.\n");
        close(log);
        exit(-1);
    }
    //

    close(log);
    close(FD);
    return 0;
}
