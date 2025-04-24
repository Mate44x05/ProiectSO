#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

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
        printf("[monitor] Listing hunts...\n");
        printf("Hunt 1: 3 comori\n");
        printf("Hunt 2: 5 comori\n");
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