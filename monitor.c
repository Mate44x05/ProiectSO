#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "treasure.h"

void list_hunts()
{
    printf("[monitor] Listare hunt-uri...\n\n");

    DIR* dir = opendir(".");
    if (dir == NULL) 
    {
        perror("monitor: Eroare la deschiderea directorului");
        exit(1);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) 
    {
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0 &&
            strcmp(entry->d_name, ".git") != 0 &&
            strcmp(entry->d_name, ".vscode") != 0) 
        {
            pid_t pid = fork();
            if (pid == 0) // copil
            {
                execl("./treasure_manager", "treasure_manager", "--list", entry->d_name, NULL);
                perror("monitor: exec esuat");
                exit(1);
            }
            else if (pid < 0)
            {
                perror("monitor: Eroare la fork");
            }
        }
        
    }

    closedir(dir);

    int status;
    while (wait(&status) > 0);
}

void list_treasures(const char* hunt_name)
{
    pid_t pid = fork();
    if (pid == 0) // copil
    {
        execl("./treasure_manager", "treasure_manager", "--list", hunt_name, NULL);
        perror("monitor: exec esuat");
        exit(1);
    }
    else if (pid < 0)
    {
        perror("monitor: Eroare la fork");
    }
}

void view_treasure(const char* hunt_name, const char* treasure_id)
{
    pid_t pid = fork();
    if (pid == 0) // copil
    {
        execl("./treasure_manager", "treasure_manager", "--view", hunt_name, treasure_id, NULL);
        perror("monitor: exec esuat");
        exit(1);
    }
    else if (pid < 0)
    {
        perror("monitor: Eroare la fork");
    }
}


void handle_sigusr1(int sig) {
    char cmd[128];
    int fd = open("cmd", O_RDONLY);
    if (fd == -1) 
    {
        perror("monitor: Eroare la deschiderea fișierului cmd");
        exit(1);
    }

    read(fd, cmd, sizeof(cmd) - 1);
    close(fd);

    cmd[strcspn(cmd, "\n")] = 0;

    if (strcmp(cmd, "list_hunts") == 0) 
    {
        list_hunts();
    }
    else if(strcmp(cmd, "list_treasures") == 0) 
    {
        char hunt_name[64];
        printf("[monitor] Introduceți numele hunt-ului: ");
        fgets(hunt_name, sizeof(hunt_name), stdin);
        hunt_name[strcspn(hunt_name, "\n")] = 0;
        list_treasures(hunt_name);
    }
    else if (strcmp(cmd, "view_treasure") == 0) 
    {
        char hunt_name[64];
        char treasure_id[64];
        printf("[monitor] Introduceți numele hunt-ului: ");
        fgets(hunt_name, sizeof(hunt_name), stdin);
        hunt_name[strcspn(hunt_name, "\n")] = 0;
        printf("[monitor] Introduceți ID-ul comorii: ");
        fgets(treasure_id, sizeof(treasure_id), stdin);
        treasure_id[strcspn(treasure_id, "\n")] = 0;
        view_treasure(hunt_name, treasure_id);
    }
    else if (strcmp(cmd, "stop_monitor") == 0) 
    {
        printf("[monitor] Stop primit. Închidere după 3 sec...\n");
        usleep(3000000);  // 3 secunde
        exit(0);
    }
    else 
    {
        printf("[monitor] Comandă necunoscută: %s\n", cmd);
    }
}

int main() 
{
    struct sigaction sa;
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);

    printf("[monitor] Monitor activ. Aștept comenzi...\n");

    while (1) 
    {
        pause(); // așteaptă semnal
    }

    return 0;
}