#include <stddef.h>
#include "ll_cycle.h"

int ll_has_cycle(node *head) {
    if (head == NULL) {
        return 0;
    }
    node *p1 = head;
    node *p2 = head;

    while (p1 != NULL && p2 != NULL && p2->next != NULL) {
        p1 = p1->next;
        p2 = p2->next->next;

        if (p1 == p2) {
            return 1;
        }
    }

    return 0;
}