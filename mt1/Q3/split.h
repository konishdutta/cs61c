typedef struct node {
    char *data;
    struct node *next;
} Node;

void split(Node * words, Node ** consonants, Node ** vowels);