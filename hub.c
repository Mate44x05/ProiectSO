#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

int monitor_pipe_fd[2];
pid_t monitor_pid = -1;
int monitor_alive = 0;

void sigchld_handler(int signum) 
{
    waitpid(monitor_pid, NULL, 0);
    monitor_alive = 0;
    printf("Monitor s-a închis cu succes.\n");
}

void start_monitor() 
{
    if (monitor_alive) 
    {
        printf("Monitor deja pornit.\n");
        return;
    }
    if (pipe(monitor_pipe_fd) == -1) 
    {   
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) 
    {
        perror("fork");
        exit(1);
    }

    char fd_str[10];
    sprintf(fd_str, "%d", monitor_pipe_fd[1]);

    if (pid == 0)
    {
        execl("./monitor", "monitor", fd_str, NULL);
        perror("execl");
        exit(1);
    }    
    else 
    {
        monitor_pid = pid;
        monitor_alive = 1;
        printf("Monitor pornit cu succes.\n");
    }
    close(monitor_pipe_fd[1]);
}

void send_command(const char* cmd) 
{
    if (!monitor_alive && strcmp(cmd, "stop_monitor") != 0) 
    {
        printf("Monitorul nu e activ.\n");
        return;
    }

    // scrie comanda în fișier
    int fd = open("cmd", O_WRONLY | O_CREAT, 0666);
    if (fd == -1) 
    {
        perror("Eroare la deschiderea fișierului pentru comenzi.");
        exit(1);
    }
    write(fd, cmd, strlen(cmd));
    write(fd, "\n", 1);
    if (strcmp(cmd, "list_treasures") == 0 || strcmp(cmd, "view_treasure") == 0) 
    {
        char input[128];
        printf("Introduceți numele hunt-ului: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        write(fd, input, strlen(input));
        write(fd, "\n", 1);
    
        if (strcmp(cmd, "view_treasure") == 0) {
            printf("Introduceți ID-ul comorii: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            write(fd, input, strlen(input));
            write(fd, "\n", 1);
        }
    }
    close(fd);

    // trimite semnal la monitor
    kill(monitor_pid, SIGUSR1);
    char buffer[1024];
    ssize_t len = read(monitor_pipe_fd[0], buffer, sizeof(buffer) - 1);
    if (len > 0) 
    {
        buffer[len] = '\0';
        printf("%s", buffer);
    }
}

int main() 
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    char input[128];

    while (1) 
    {
        usleep(1000000);
        printf("Introduceti comanda (start_monitor, stop_monitor, list_hunts, calculate_scores, list_treasures, view_treasure, exit): \n");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "start_monitor") == 0)
        {
            start_monitor();
        }
        else if(strcmp(input, "exit") == 0) 
        {
            if (monitor_alive) 
            {
                printf("Eroare: Monitorul încă rulează.\n");
            } 
            else 
            {
                break;
            }
        } 
        else 
        {
            send_command(input);
        }
    }

    return 0;
}