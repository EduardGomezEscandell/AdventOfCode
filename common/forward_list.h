#ifndef FORWARD_LIST_INCLUDED_H
#define FORWARD_LIST_INCLUDED_H

#include <stdlib.h>
#include <stdio.h>

typedef struct struct_node_{
	char data;
	struct struct_node_ * next;
} Node;

typedef struct {
	Node * begin;
} List;

Node * NewNode(char data);
List NewList();
List ListFromString(const char * const line);

void ListInstert(Node * prev, Node * inserted);
Node * ListEmplace(Node * prev, char data);

void ListClear(List * list);

void ListPrint(const List * const list);

#endif


