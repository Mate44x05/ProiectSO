#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

int main(int argc, char **argv)
{
    // Verificarea numarului de argumente
    if(argc != 2)
    {
        perror("Numar de argumente invalid");
        exit(-1);
    }
    //

    // Deschidere directorului + Verificarea existentei directorului
    DIR *dir = opendir(argv[1]);
    if(dir == NULL)
    {
        perror("Eroare la deschiderea directorului");
        exit(-1);
    }
    //

    // Parcurgerea fiecarui fisier din director
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[320];
        snprintf(path, sizeof(path), "./%s/%s", argv[1], entry->d_name);
        // Stergerea fiecarui fisier
        if(remove(path) == -1)
        {
            perror("Eroare la stergerea unui treasure");
            exit(-1);
        }
        //
    }
    //

    // Inchidere directoruli + Stergerea directorului
    closedir(dir);
    if(rmdir(argv[1])==-1)
    {
        perror("Eroare la stergerea directorului");
        exit(-1);
    }
    //

    // Stergerea legaturi simbolice
    char link[64];
    snprintf(link, sizeof(link), "./logged_hunt-%s", argv[1]);
    if(remove(link))
    {
        perror("Eroare la stergerea legaturii simbolice");
        exit(-1);
    }
    //

    printf("Hunt sters cu succes!\n");
    return 0;
}