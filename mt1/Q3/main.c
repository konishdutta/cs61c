#include "split.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *CS61C_malloc(size_t size) {
    return malloc(size);
}

void CS61C_free(void *ptr) {
    free(ptr);
}

int main(int argc, char *argv[]) {
    char *str1 = malloc(strlen("zebra") + 1);  // +1 for the null terminator
    strcpy(str1, "zebra");

    char *str2 = malloc(strlen("ant") + 1);
    strcpy(str2, "ant");

    char *str3 = malloc(strlen("walrus") + 1);
    strcpy(str3, "walrus");

    char *str4 = malloc(strlen("bat") + 1);
    strcpy(str4, "bat");

    char *str5 = malloc(strlen("emu") + 1);
    strcpy(str5, "emu");
    Node *words = (Node *) CS61C_malloc(sizeof(Node));
    Node *consonants = NULL;
    Node *vowels = NULL;


    //words = [“zebra” -> “ant” -> “walrus” -> “bat” -> “emu”]
    words->data = str1;
    words->next = (Node *) CS61C_malloc(sizeof(Node));
    words->next->data = str2;
    words->next->next = (Node *) CS61C_malloc(sizeof(Node));
    words->next->next->data = str3;
    words->next->next->next = (Node *) CS61C_malloc(sizeof(Node));
    words->next->next->next->data = str4;
    words->next->next->next->next = (Node *) CS61C_malloc(sizeof(Node));
    words->next->next->next->next->data = str5;

    split(words, &consonants, &vowels);

    //print what's in consonants and vowels
    printf("Consonants\n");
    Node *curr = consonants;
    while (curr != NULL) {
        printf("%s, ", curr->data);
        curr = curr->next;
    }
    printf("\n\nVowels\n");
    curr = vowels;
    while (curr != NULL) {
        printf("%s, ", curr->data);
        curr = curr->next;
    }

    Node *temp;
    while (consonants != NULL) {
        temp = consonants;
        consonants = consonants->next;
        CS61C_free(temp->data);
        CS61C_free(temp);
    }

    while (vowels != NULL) {
        temp = vowels;
        vowels = vowels->next;
        CS61C_free(temp->data);
        CS61C_free(temp);
    }
    return 0;
}