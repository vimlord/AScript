#include "list.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct node;
typedef struct node* Node;

struct node {
    void* val;
    Node next;
};

struct linked_list {
    Node front;
};

List makeList() {
    List l = (List) malloc(sizeof(struct linked_list));
    l->front = NULL;
    return l;
}

Node makeNode(char* item) {
    Node n = (Node) malloc(sizeof(struct node));
    n->val = item;
    n->next = NULL;
    return n;
}

void addToList(List lst, void* val) {
    if(lst->front == NULL)
        lst->front = makeNode(val);
    else {
        Node curr = lst->front;
        while(curr->next) curr = curr->next;
        
        curr->next = makeNode(val);
    }
}

void* getFromList(List lst, int idx) {
    int i = 0;
    Node n = lst->front;
    while(i++ < idx) n = n->next;

    return n->val;
}

void* remFromList(List lst, int idx) {
    int i = 0;
    Node n = lst->front;
    void* res;

    if(!idx) {
        //Front of list
        lst->front = n->next;
    } else {
        Node prev;
        while(i++ < idx) {
            prev = n;
            n = n->next;
        }
        
        //Relink
        prev->next = n->next;
        
    }
    
    res = n->val;

    n->next = NULL;
    n->val = NULL;

    free(n);
    return res;
}

int listIndexOfStr(List lst, char* str) {
    int idx = 0;
    Node curr = lst->front;

    while(curr) {
        if(strcmp(str, (char*) curr->val) == 0)
            return idx;

        curr = curr->next;
        idx++;
    }

    return -1;

}

int listSize(List lst) {
    int len = 0;
    Node curr = lst->front;
    while(curr != NULL) {
        len++;
        curr = curr->next;
    }
    return len;
}

void printListStr(List lst) {
    
    Node curr = lst->front;
    while(curr) {
        printf("%s\n", (char*) curr->val);
        curr = curr->next;
    }

}



