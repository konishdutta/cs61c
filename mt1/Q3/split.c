//
// Created by Konish Dutta on 1/21/24.
//

#include "split.h"
#include <string.h>

void *CS61C_malloc(size_t size);
void CS61C_free(void *ptr);
/*
 * typedef struct node {
 * char *data;
 * struct node *next;
 * } Node;
 */

void split(Node *words, Node **consonants, Node **vowels) {
    Node *curr = words;

    if (curr->next) {
        split(curr->next, consonants, vowels);
    }
    char v[] = {'a', 'e', 'i', 'o', 'u'};
    int c_or_v = 0;
    for (int i = 0; i < 5; i++) {
        if (curr->data[0] == v[i]) {
            c_or_v = 1;
            break;
        }
    }
    Node *newNode = (Node *) CS61C_malloc(sizeof(Node));
    char *newString = (char *) CS61C_malloc(sizeof(curr->data));
    strcpy(newString, curr->data);
    newNode->data = newString;
    if (c_or_v == 1) {
        newNode->next = *vowels;
        *vowels = newNode;
    } else {
        newNode->next = *consonants;
        *consonants = newNode;
    }
    CS61C_free(curr->data);
    CS61C_free(curr);
    return;
}