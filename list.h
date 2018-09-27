#ifndef _LOX_LIST_H_
#define _LOX_LIST_H_

typedef struct node {
    void* data;
    struct node* next;
    struct node* prev;
} Node;

typedef struct list {
    Node* head;
    Node* last;
    unsigned int count;
} List;

#endif
