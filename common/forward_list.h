#ifndef FORWARD_LIST_INCLUDED_H
#define FORWARD_LIST_INCLUDED_H

#include <stdlib.h>
#include <stdio.h>

#define TEMPLATE_LIST(LIST_data_type, LIST_ListName)                          \
                                                                              \
typedef struct LIST_ListName##Node{                                           \
    LIST_data_type data;                                                      \
    struct LIST_ListName##Node * next;                                        \
} LIST_ListName##Node;                                                        \
                                                                              \
typedef struct {                                                              \
    LIST_ListName##Node * begin;                                              \
} LIST_ListName;                                                              \
                                                                              \
LIST_ListName##Node * New##LIST_ListName##Node(LIST_data_type const * data);  \
LIST_ListName New##LIST_ListName();                                           \
void LIST_ListName##Instert(                                                  \
    LIST_ListName##Node * prev,                                               \
    LIST_ListName##Node * inserted);                                          \
LIST_ListName##Node * LIST_ListName##Emplace(                                 \
    LIST_ListName##Node * prev,                                               \
    LIST_data_type const * data);                                             \
void LIST_ListName##Clear(LIST_ListName * list);                              \
void ListPrint(                                                               \
    LIST_ListName##Node const * begin,                                        \
    LIST_ListName##Node const * end,                                          \
    const char * format)

#define LIST_DEFINE_NEW_NODE(LIST_data_type, LIST_ListName)                   \
LIST_ListName##Node * New##LIST_ListName##Node(LIST_data_type const * data) {       \
    LIST_ListName##Node * node = malloc(sizeof(*node));                       \
    node->data = *data;                                                       \
    node->next = NULL;                                                        \
    return node;                                                              \
}

#define LIST_DEFINE_NEW_LIST(LIST_data_type, LIST_ListName)                   \
LIST_ListName New##LIST_ListName() {                                          \
    LIST_ListName list;                                                       \
    list.begin = NULL;                                                        \
    return list;                                                              \
}

#define LIST_DEFINE_INSERT(LIST_data_type, LIST_ListName)                     \
void LIST_ListName##Instert(                                                  \
    LIST_ListName##Node * prev,                                               \
    LIST_ListName##Node * inserted)                                           \
{                                                                             \
    if(inserted->next)                                                        \
    {                                                                         \
        fprintf(stderr, "Inserting non-isolated node!");                      \
        exit(EXIT_FAILURE);                                                   \
    }                                                                         \
    inserted->next = prev->next;                                              \
    prev->next = inserted;                                                    \
}

#define LIST_DEFINE_EMPLACE(LIST_data_type, LIST_ListName)                    \
LIST_ListName##Node * LIST_ListName##Emplace(                                 \
    LIST_ListName##Node * prev, LIST_data_type const * data) {                \
    LIST_ListName##Node * inserted = New##LIST_ListName##Node(data);          \
    LIST_ListName##Instert(prev, inserted);                                   \
    return inserted;                                                          \
}

#define LIST_DEFINE_CLEAR(LIST_data_type, LIST_ListName)                      \
void LIST_ListName##Clear(LIST_ListName * list) {                             \
    LIST_ListName##Node * curr = list->begin;                                 \
    LIST_ListName##Node * prev = NULL;                                        \
    while(curr) {                                                             \
        prev = curr;                                                          \
        curr = curr->next;                                                    \
        free(prev);                                                           \
    }                                                                         \
    list->begin = NULL;                                                       \
}

#define LIST_DEFINE_PRINT(LIST_data_type, LIST_ListName)                      \
void LIST_ListName##Print(                                                    \
    LIST_ListName##Node const * begin,                                        \
    LIST_ListName##Node const * end,                                          \
    const char * format)                                                      \
{                                                                             \
    printf("[ ");                                                             \
    for(; begin != end; begin = begin->next) {                                \
        printf(format, begin->data);                                          \
    }                                                                         \
    printf(" ]\n");                                                           \
}                                                                             \

#endif
