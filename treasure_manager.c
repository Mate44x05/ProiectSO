#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

void add(char* hunt_name)
{
    char buff[128];
    int bytesRead; 
    char *aux=NULL;
// Scrierea in fisier a fiecarui camp    
    // Crearea fisierului pentru treasure-ul adaugat
    write(STDOUT_FILENO, "Treasure ID: ", strlen("Treasure ID: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    char name[128];
    strcpy(name,"./");strcat(name,hunt_name);
    strcat(name,"/");strcat(name,aux);
    printf("%s",name);
    int FD = open(name, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if(FD==-1)
    {
        // Creare director daca acesta nu exista
        mkdir(hunt_name, 0777);
        FD = open(name, O_WRONLY | O_APPEND | O_CREAT, 0666);
        if(FD==-1)
        {
            perror("Eroare la crearea unui nou treasure.\n");
            exit(-1);
        }
        //
    }
    //
    strcat(aux, ", ");
    write(FD, aux, strlen(aux));

    write(STDOUT_FILENO, "User name: ", strlen("User name: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    strcat(aux, ", ");
    write(FD, aux, strlen(aux));

    write(STDOUT_FILENO, "Latitude: ", strlen("Latitude: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    strcat(aux, ", ");
    write(FD, aux, strlen(aux));

    write(STDOUT_FILENO, "Longitude: ", strlen("Longitude: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    strcat(aux, ", ");
    write(FD, aux, strlen(aux));

    write(STDOUT_FILENO, "Clue: ", strlen("Clue: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    strcat(aux, ", ");
    write(FD, aux, strlen(aux));

    write(STDOUT_FILENO, "Value: ", strlen("Value: "));
    bytesRead = read(STDIN_FILENO, buff, sizeof(buff));
    buff[bytesRead]='\n';
    aux=strtok(buff,"\n");
    strcat(aux, "\n");
    write(FD, aux, strlen(aux));

    close(FD);
//
}

void list(char* nume_director) 
{
// Tiparirea numelui hunt-ului
    write(STDOUT_FILENO, "Nume director: ", strlen("Nume director: "));
    write(STDOUT_FILENO, nume_director, strlen(nume_director));
//

    write(STDOUT_FILENO, "\n", strlen("\n"));

// Tiparirea informatiilor despre director
    struct stat fileStat;
    // Verificarea existentei directorului
    if (stat(nume_director, &fileStat) == -1) 
    {
        perror("Eroare la obtinerea informatiilor despre director");
        exit(-1);
    }
    //
    write(STDOUT_FILENO, "\nSize: ", strlen("\nSize: "));
    char size[20];
    sprintf(size, "%ld", fileStat.st_size);
    write(STDOUT_FILENO, size, strlen(size));
//

    write(STDOUT_FILENO, "\n----------------------------\n", strlen("\n----------------------------\n"));

// Listam treasure-urile din director impreuna cu informatiile despre acestea
    write(STDOUT_FILENO, "List of treasures:\n", strlen("List of treasures:\n"));

    struct dirent *entry;
    char aux[128];
    strcpy(aux, "./");
    strcat(aux, nume_director);
    
    // Deschiderea directorului pentru parcurgerea fiecarui treasure
    DIR *dir = opendir(aux);

    if (dir == NULL) {
        perror("Nu se poate deschide directorul");
        exit(-1);
    }
    //

    // Parcurgerea fiecarui fisier din director
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
        {
            // Tiparirea numelui fiecarui fisier
            write(STDOUT_FILENO, "\nTreasure ID: ", strlen("\nTreasure ID: "));
            write(STDOUT_FILENO, entry->d_name, strlen(entry->d_name));
            //

            // Crearea unui path pentru a obtine informatiile despre fiecare fisier
            char path[160];
            strcpy(path, aux);
            strcat(path, "/");
            strcat(path, entry->d_name);
                // Deschiderea fisierului pentru a obtine informatiile despre acesta
            if (stat(path, &fileStat) == -1) 
            {
                perror("\nEroare la obtinerea informatiilor despre un fisier");
                continue;
            }
                //
            //     
            
            // Tiparirea informatiilor despre fiecare fisier
            write(STDOUT_FILENO, "\nSize: ", strlen("\nSize: "));
            char size[20];
            sprintf(size, "%ld", fileStat.st_size);
            write(STDOUT_FILENO, size, strlen(size));
            
            write(STDOUT_FILENO, "\nLast modified: ", strlen("\nLast modified: "));
            char time[20];
            strftime(time, sizeof(time), "%D %T", gmtime(&fileStat.st_mtime));
            write(STDOUT_FILENO, time, strlen(time));
            //

            write(STDOUT_FILENO, "\n", strlen("\n")); 
        }
    }

    write(STDOUT_FILENO, "\nEnd of list\n", strlen("\nEnd of list\n"));
    closedir(dir);
}

int main(void)
{
    list("test");
    return 0;
}