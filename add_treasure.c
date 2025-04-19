#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include "treasure.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        perror("Numărul de argumente este incorect\n");
        exit(-1);
    }

    struct stat st;
    if (stat(argv[1], &st) == -1)
    {
        if(mkdir(argv[1], 0777) == -1)
        {
            perror("Eroare la crearea directorului.\n");
            exit(-1);
        }
    } 
    else 
    {
        if(!S_ISDIR(st.st_mode))
        {
            perror("Numele specificat nu este un director.\n");
            exit(-1);
        }
    }


    // Citirea in struct a fiecarui camp 
    struct treasure t;
    int ch;

    printf("Introduceti datele pentru treasure:\n");

    printf("Treasure ID: ");
    fgets(t.id, sizeof(t.id), stdin);
    t.id[strcspn(t.id, "\n")] = '\0';
    printf("User name: ");
    fgets(t.user, sizeof(t.user), stdin);
    t.user[strcspn(t.user, "\n")] = '\0';
    printf("Latitude: ");
    scanf("%f", &t.latitude);
    if (t.latitude < -90 || t.latitude > 90) 
    {
        perror("Latitudine invalida\n");
        exit(-1);
    }
    while ((ch = getchar()) != '\n' && ch != EOF);
    printf("Longitude: ");
    scanf("%f", &t.longitude);
    if (t.longitude < -180 || t.longitude > 180) 
    {
        perror("Longitudine invalida\n");
        exit(-1);
    }
    while ((ch = getchar()) != '\n' && ch != EOF);
    printf("Clue: ");
    fgets(t.clue, sizeof(t.clue), stdin);
    t.clue[strcspn(t.clue, "\n")] = '\0';
    printf("Value: ");
    scanf("%d", &t.value);
    while ((ch = getchar()) != '\n' && ch != EOF);
    //

    // Crearea/deschiderea fisierului pentru treasure-ul adaugat
    char aux[10];
    char name[128];
    snprintf(name, sizeof(name), "./%s/%s.bin", argv[1], t.id);
    int FD = open(name, O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (FD == -1) 
    {
        perror("Eroare la deschiderea fisierului.\n");
        exit(-1);
    }

    strcpy(aux, (lseek(FD, 0, SEEK_END) == 0) ? "adaugat" : "modificat");
    //

    ssize_t n = write(FD, &t, sizeof(t));
    if (n != sizeof(t)) 
    {
        perror("Eroare la scrierea completă a structurii");
        close(FD);
        exit(-1);
    }  

    // Crearea/deschiderea fisierului pentru log
    char log_path[128];
    snprintf(log_path, sizeof(log_path), "./%s/logged_hunt", argv[1]);
    int log = open(log_path, O_WRONLY | O_APPEND);
    if(log == -1)
    {
        log = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0777);
        if(log == -1)
        {
            perror("Eroare la deschiderea fisierului de log.\n");
            close(FD);
            exit(-1);
        }
        // Crearea unui link simbolic pentru fisierul de log in directorul curent
        char link[64];
        snprintf(link, sizeof(link), "./logged_hunt-%s", argv[1]);
        if (symlink(log_path, link) == -1) 
        {
            if (errno != EEXIST) 
            {
                perror("Eroare la crearea symlink-ului");
                close(log);
                close(FD);
                exit(-1);
            }
        }
        //
    }
    //

    
    // Scrierea in log a treasure-ului adaugat
    char mesaj[128];
    time_t tm;
    struct tm *tm_info;
    char stime[20]; 
    time(&tm);
    tm_info = localtime(&tm);
    strftime(stime, sizeof(stime), "%Y-%m-%d %H:%M:%S", tm_info);
    snprintf(mesaj, sizeof(mesaj), "%s: Treasure ID %s a fost %s.\n", stime, t.id, aux);
    if(write(log, mesaj, strlen(mesaj)) == -1)
    {
        write(log, "Eroare la scrierea in log.\n", strlen("Eroare la scrierea in log.\n"));
        perror("Eroare la scrierea in log.\n");
        close(log);
        close(FD);
        exit(-1);
    }
    //
    
    close(log);
    close(FD);

    return 0;
}