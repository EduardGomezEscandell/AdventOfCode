#include "forward_list.h"

Node * NewNode(char data)
{
	Node * node = malloc(sizeof(Node));
	node->data = data;
	node->next = NULL;
	return node;
}

List NewList()
{
	List list;
	list.begin = NULL;
	return list;
}

void ListInstert(Node * prev, Node * inserted)
{
	if(inserted->next)
	{
		fprintf(stderr, "Inserting non-isolated node!");
		exit(EXIT_FAILURE);
	}
	inserted->next = prev->next;
	prev->next = inserted;
}

Node * ListEmplace(Node * prev, char data)
{
	Node * inserted = NewNode(data);
	ListInstert(prev, inserted);
	return inserted;
}

void ListClear(List * list)
{
	Node * curr = list->begin;
	Node * prev = NULL;
	

	while(curr)
	{
		prev = curr;
		curr = curr->next;
		free(prev);
	}

	list->begin = NULL;
}

void ListPrint(const List * const list)
{
	Node const * curr = list->begin;

	printf("[ ");
	while(curr)
	{
		printf("%c", curr->data);
		curr = curr->next;
	}
	printf(" ]\n");
}

List ListFromString(const char * const line)
{
	List list = NewList();


	Node * prev = NULL;
	for(char const * it=line; *it != '\0' && *it != '\n'; ++it)
	{
		Node * new_node = NewNode(*it);

		if(prev == NULL)
			list.begin = new_node;
		else
			ListInstert(prev, new_node);

		prev = new_node;
	}

	return list;
}