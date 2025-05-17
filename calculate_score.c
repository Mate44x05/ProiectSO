#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "treasure.h"

// Exemplu simplu: fiecare fișier = un item cu user și valoare
int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        perror("Numar invalid de argumente");
        exit(-1);
    }

    // Verificarea existentei directorului
    char path[64];
    snprintf(path, sizeof(path), "./%s", argv[1]);

    DIR* dir = opendir(path);
    if (!dir) 
    {
        perror("Eroare la deschiderea directorului");
        exit(0);
    }
    //

    // Parcurgerea fiecarui fisier din director pentru a calcula scorurile
    struct dirent* entry;
    struct
    {
        char user[32];
        int score;
    } users[100];

    int user_count = 0;

    while ((entry = readdir(dir)) != NULL) 
    {
        if (entry->d_type != DT_REG || strcmp(entry->d_name,"logged_hunt") == 0) continue;

        char filepath[320];
        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

        int fd = open(filepath, O_RDONLY);
        if(fd == -1)
        {
            perror("Eroare la deschiderea fisierului");
            continue;
        }

        struct treasure t;
        while(read(fd, &t, sizeof(t)) == sizeof(t)) 
        {
            // Verificare existentei user-ului in list + Calcularea scorului
            if (t.value > 0)
            {
                int found = 0;
                for (int i = 0; i < user_count; ++i)
                {
                    if (strcmp(users[i].user, t.user) == 0) 
                    {
                        users[i].score += t.value;
                        found = 1;
                        break;
                    }
                }
                if (!found) 
                {
                    strcpy(users[user_count].user, t.user);
                    users[user_count].score = t.value;
                    user_count++;
                }
            }
            //
        }

        close(fd);

    }
    //

    closedir(dir);

    // Afisarea scorurilor
    printf("Scoruri %s:\n", argv[1]);
    for (int i = 0; i < user_count; ++i)
    {
        printf("User: %s - Score: %d\n", users[i].user, users[i].score);
    }
    printf("\n\n");
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
    snprintf(mesaj, sizeof(mesaj), "%s: Scorurile au fost afisate.\n", stime);
    if(write(log, mesaj, strlen(mesaj)) == -1)
    {
        write(log, "Eroare la scrierea in log.\n", strlen("Eroare la scrierea in log.\n"));
        perror("Eroare la scrierea in log.\n");
        close(log);
        closedir(dir);
        exit(-1);
    }
    //
    return 0;
}
