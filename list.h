#ifndef _LIST_H_
#define _LIST_H_

struct linked_list;
typedef struct linked_list* List;

List makeList();

void addToList(List lst, char* val);
char* getFromList(List lst, int idx);
int listIndexOf(List lst, char* str);
int listSize(List lst);

#endif

