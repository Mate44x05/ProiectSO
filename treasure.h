#define MAX_ID 32
#define MAX_USER 32
#define MAX_CLUE 128

struct treasure
{
    char id[MAX_ID];
    char user[MAX_USER];
    float latitude;
    float longitude;
    char clue[MAX_CLUE];
    int value;
};