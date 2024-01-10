#include <stdio.h>

#define MAX_LEN 80

int main(int argc, char *argv[]) {
    FILE *ifp;
    char a_word[MAX_LEN];
    printf("What's your name?\n");

    ifp = fopen("in.txt", "r");

    if (ifp == NULL) {
        fgets(a_word, MAX_LEN, stdin);
    } else {
        fgets(a_word, MAX_LEN, ifp);
        fclose(ifp);
    }

    printf("Hey, %sI just really wanted to say hello to you.\nI hope you have a wonderful day.", a_word);

    return 0;
}