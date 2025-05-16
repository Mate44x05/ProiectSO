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

int write_fd = -1;


void calculate_scores()
{
    dprintf(write_fd, "[monitor] Calculare scoruri...\n\n");

    // Deschiderea directorului curent
    DIR* dir = opendir(".");
    if (!dir) 
    {
        dprintf(write_fd, "[monitor] Eroare la deschiderea directorului\n");
        exit(0);
    }
    //

    struct dirent* entry;
    // Parcurgerea fiecarui fisier din director
    while ((entry = readdir(dir)) != NULL) 
    {
        // Verificarea tipului de fisier si excluderea directorului curent, parinte si alte directoare
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0 &&
            strcmp(entry->d_name, ".git") != 0 &&
            strcmp(entry->d_name, ".vscode") != 0) 
        //
        {
            int fd[2];
            pipe(fd);

            // Crearea procesului copil
            pid_t pid = fork();
            if (pid == 0) 
            {
                // Execuția comenzii în procesul copil
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                execl("./treasure_manager", "treasure_manager", "--score", entry->d_name, NULL);
                perror("exec");
                exit(1);
                //
            }
            else 
            {
                // Procesul părinte citește rezultatul
                close(fd[1]);
                char buffer[512];
                ssize_t len;
                while ((len = read(fd[0], buffer, sizeof(buffer))) > 0) 
                {
                    write(write_fd, buffer, len);  // trimite către hub
                }
                close(fd[0]);
                waitpid(pid, NULL, 0);
                //
            }
            //
        }
    }
    //

    closedir(dir);

    // Mesaj de finalizare
    dprintf(write_fd, "###DONE###\n");
    //
}

void list_hunts() 
{
    dprintf(write_fd, "[monitor] Listare hunt-uri...\n\n");

    // Deschiderea directorului curent
    DIR* dir = opendir(".");
    if (!dir) 
    {
        dprintf(write_fd, "[monitor] Eroare la deschiderea directorului\n");
        exit(0);
    }
    //

    struct dirent* entry;
    // Parcurgerea fiecarui fisier din director
    while ((entry = readdir(dir)) != NULL) 
    {
        // Verificarea tipului de fisier si excluderea directorului curent, parinte si alte directoare
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0 &&
            strcmp(entry->d_name, ".git") != 0 &&
            strcmp(entry->d_name, ".vscode") != 0) 
        //
        {
            int fd[2];
            pipe(fd);

            // Crearea procesului copil
            pid_t pid = fork();
            if (pid == 0) 
            {
                // Execuția comenzii în procesul copil
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                execl("./treasure_manager", "treasure_manager", "--list", entry->d_name, NULL);
                perror("exec");
                exit(1);
                //
            }
            else 
            {
                // Procesul părinte citește rezultatul
                close(fd[1]);
                char buffer[512];
                ssize_t len;
                while ((len = read(fd[0], buffer, sizeof(buffer))) > 0) 
                {
                    write(write_fd, buffer, len);  // trimite către hub
                }
                close(fd[0]);
                waitpid(pid, NULL, 0);
                //
            }
            //
        }
    }
    //

    closedir(dir);

    // Mesaj de finalizare
    dprintf(write_fd, "###DONE###\n");
    //
}

void list_treasures(const char* hunt_name)
{
    int fd[2];
    pipe(fd);

    // Crearea procesului copil
    pid_t pid = fork();
    if (pid == 0)
    {
        // Execuția comenzii în procesul copil
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execl("./treasure_manager", "treasure_manager", "--list", hunt_name, NULL);
        perror("exec");
        exit(1);
        //
    }
    else
    {
        // Procesul părinte citește rezultatul
        close(fd[1]);
        char buffer[512];
        ssize_t len;
        while ((len = read(fd[0], buffer, sizeof(buffer))) > 0)
        {
            write(write_fd, buffer, len);
        }
        close(fd[0]);
        waitpid(pid, NULL, 0);
        //
    }
    //

    // Mesaj de finalizare
    dprintf(write_fd, "###DONE###\n");
    //
}

void view_treasure(const char* hunt_name, const char* treasure_id)
{
    int fd[2];
    pipe(fd);

    // Crearea procesului copil
    pid_t pid = fork();
    if (pid == 0)
    {
        // Execuția comenzii în procesul copil
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execl("./treasure_manager", "treasure_manager", "--view", hunt_name, treasure_id, NULL);
        perror("exec");
        exit(1);
        //
    } 
    else
    {
        // Procesul părinte citește rezultatul
        close(fd[1]);
        char buffer[512];
        ssize_t len;
        while ((len = read(fd[0], buffer, sizeof(buffer))) > 0)
        {
            write(write_fd, buffer, len);
        }
        close(fd[0]);
        waitpid(pid, NULL, 0);
        //
    }
    //

    // Mesaj de finalizare
    dprintf(write_fd, "###DONE###\n");
    //

}


void handle_sigusr1(int sig) 
{
    char buffer[256];
    char cmd[64], hunt_name[64], treasure_id[64];

    // Deschiderea fișierului pentru comenzi
    int fd = open("cmd", O_RDONLY);
    if (fd == -1) 
    {
        perror("monitor: Eroare la deschiderea fișierului cmd");
        return;
    }
    //

    // Citirea comenzii si a eventualelor argumente din fișier
    ssize_t len = read(fd, buffer, sizeof(buffer) - 1);
    if (len <= 0) 
    {
        perror("monitor: Eroare la citire");
        close(fd);
        return;
    }
    buffer[len] = '\0';
    //

    close(fd);

    // Împărțirea buffer-ului în linii
    char* lines[3] = {NULL, NULL, NULL};
    int i = 0;
    char* token = strtok(buffer, "\n");
    while (token && i < 3) 
    {
        lines[i++] = token;
        token = strtok(NULL, "\n");
    }
    //

    if (!lines[0]) return;

    // Verificarea comenzii și apelarea funcțiilor corespunzătoare
    if (strcmp(lines[0], "list_hunts") == 0) 
    {
        list_hunts();
    }
    else if (strcmp(lines[0], "calculate_scores") == 0) 
    {
        calculate_scores();
    }
    else if (strcmp(lines[0], "list_treasures") == 0 && lines[1]) 
    {
        strncpy(hunt_name, lines[1], sizeof(hunt_name));
        list_treasures(hunt_name);
    }
    else if (strcmp(lines[0], "view_treasure") == 0 && lines[1] && lines[2]) 
    {
        strncpy(hunt_name, lines[1], sizeof(hunt_name));
        strncpy(treasure_id, lines[2], sizeof(treasure_id));
        view_treasure(hunt_name, treasure_id);
    }
    else if (strcmp(lines[0], "stop_monitor") == 0) 
    {
        exit(0);
    }
    else 
    {
        dprintf(write_fd, "[monitor] Comandă necunoscută: %s\n", lines[0]);
    }
    //
}

int main(int argc, char** argv) 
{
    // Preluarea file descriptorului de scriere
    if (argc == 2) 
    {
        write_fd = atoi(argv[1]);
    }
    //

    // Setarea semnalului SIGUSR1 pentru a apela funcția handle_sigusr1
    struct sigaction sa;
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);
    //

    printf("[monitor] Monitor activ. Aștept comenzi...\n");

    // Așteptarea semnalului SIGUSR1
    while (1) 
    {
        pause();
    }
    //

    close(write_fd);
    return 0;
}