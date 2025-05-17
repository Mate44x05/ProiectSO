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
    // Așteptarea terminării procesului monitor
    waitpid(monitor_pid, NULL, 0);
    monitor_alive = 0;

    printf("Stop primit. Închidere după 3 sec...\n");
    usleep(3000000);
    printf("Monitor s-a închis cu succes.\n");
    //
}

void start_monitor() 
{
    // Verificarea dacă monitorul este deja activ
    if (monitor_alive) 
    {
        printf("Monitor deja pornit.\n");
        return;
    }
    //

    // Crearea pipe-ului pentru comunicarea cu monitorul
    if (pipe(monitor_pipe_fd) == -1) 
    {   
        perror("pipe");
        exit(1);
    }
    //

    // Crearea procesului pentru monitor
    pid_t pid = fork();
    if (pid == -1) 
    {
        perror("fork");
        exit(1);
    }
    //

    char fd_str[10];
    sprintf(fd_str, "%d", monitor_pipe_fd[1]);

    // Procesul copil va executa monitorul
    if (pid == 0)
    {
        execl("./monitor", "monitor", fd_str, NULL);
        perror("execl");
        exit(1);
    }    
    //
    // Procesul părinte va aștepta ca monitorul să se inițializeze
    else 
    {
        monitor_pid = pid;
        monitor_alive = 1;
        printf("Monitor pornit cu succes.\n");
    }
    //
    // Închiderea pipe-ului de scriere în procesul părinte
    close(monitor_pipe_fd[1]);
    //
}

void send_command(const char* cmd) 
{
    // Verificarea dacă monitorul este activ
    if (!monitor_alive && strcmp(cmd, "stop_monitor") != 0) 
    {
        printf("Monitorul nu e activ.\n");
        return;
    }
    //

    // Deschiderea fișierului pentru comenzi
    int fd = open("cmd", O_WRONLY | O_CREAT, 0666);
    if (fd == -1) 
    {
        perror("Eroare la deschiderea fișierului pentru comenzi.");
        exit(1);
    }
    //

    // Scrierea comenzii în fișier
    write(fd, cmd, strlen(cmd));
    write(fd, "\n", 1);
    //

    // Verificarea comenzilor care necesită argumente
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
    //
    close(fd);


    // Trimiterea semnalului SIGUSR1 pentru a notifica monitorul
    kill(monitor_pid, SIGUSR1);
    //

    // Citirea rezultatelor din pipe-ul monitorului
    char buffer[1024];
    ssize_t len;
    char output[4096] = {0};
    int done = 0;

    while ((len = read(monitor_pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) 
    {
        buffer[len] = '\0';
        strcat(output, buffer);
        if (strstr(buffer, "###DONE###")) 
        {
            done = 1;
            break;
        }
    }

    if (done) 
    {
        // Șterge ###DONE### înainte de afișare
        char* end_marker = strstr(output, "###DONE###");
        if (end_marker) *end_marker = '\0';
        printf("%s", output);
        //
    }   
    //
}

int main() 
{
    // Setarea handler-ului pentru SIGCHLD
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);
    //

    char input[128];

    // Asteptarea comenzilor de la utilizator
    while (1) 
    {
        usleep(1000000);
        // Citirea comenzii de la utilizator
        printf("Introduceti comanda (start_monitor, stop_monitor, list_hunts, calculate_scores, list_treasures, view_treasure, exit): \n");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        //

        // Verificarea comenzii si apelarea functiilor corespunzatoare
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
            // Apelarea functiei de trimitere a comenzii la monitor
            send_command(input);
            //
        }
        //
    }
    //
    close(monitor_pipe_fd[0]);
    return 0;
}