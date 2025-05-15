#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include "treasure.h"

// Exemplu simplu: fiecare fișier = un item cu user și valoare
int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        perror("Numar invalid de argumente");
        exit(-1);
    }

    char path[64];
    snprintf(path, sizeof(path), "./%s", argv[1]);

    DIR* dir = opendir(path);
    if (!dir) 
    {
        perror("Eroare la deschiderea directorului");
        exit(0);
    }

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
        char owner[64];
        int value;
        if (read(fd, &t, sizeof(t)) == sizeof(t)) 
        {
            strcpy(owner, t.user);
            value = t.value;
        }
        close(fd);

        if (value > 0)
        {
            int found = 0;
            for (int i = 0; i < user_count; ++i)
            {
                if (strcmp(users[i].user, owner) == 0) 
                {
                    users[i].score += value;
                    found = 1;
                    break;
                }
            }
            if (!found) 
            {
                strcpy(users[user_count].user, owner);
                users[user_count].score = value;
                user_count++;
            }
        }
    }

    closedir(dir);

    printf("Scoruri %s:\n", argv[1]);
    for (int i = 0; i < user_count; ++i)
    {
        printf("User: %s - Score: %d\n", users[i].user, users[i].score);
    }
    printf("\n");
    return 0;
}
