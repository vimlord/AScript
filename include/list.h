#ifndef _LIST_H_
#define _LIST_H_

struct linked_list;
typedef struct linked_list* List;

List makeList();

void addToList(List lst, void* val);
void* getFromList(List lst, int idx);
void* remFromList(List lst, int idx);
int listIndexOfStr(List lst, char* str);
int listSize(List lst);

void printListStr(List lst);

#endif

