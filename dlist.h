#ifndef __AL_DLIST_H_
#define __AL_DLIST_H_

struct node {
	char *data;
	struct node *next, *previous;
};

/* Function Declaration */

struct node *newNode();					/* Return new Node 				*/
void deleteNode(struct node *ptr);		/* Delete node 					*/
void reverseDList(struct node **ptr);	/* Reverse double-linked list 	*/
void deleteDList(struct node *ptr);		/* Delete double-linked list 	*/

/* Function Definition */

struct node *newNode() {
	struct node *ptr = NULL;
	ptr = (struct node *)myAllocate(sizeof(struct node));
	ptr->next = ptr->previous = NULL;
	return ptr;
}

void deleteNode(struct node *ptr) {
	free(ptr->data);
	free(ptr);
}

void deleteDList(struct node *ptr) {
	struct node *temp = NULL;
	while(ptr) {
		temp = ptr;
		ptr = ptr->next;
		deleteNode(temp);

	}
}

void reverseDList(struct node **start) {
	struct node *temp = NULL, *curr = *start;

	while(curr)
	{
		temp = curr->previous;
		curr->previous = curr->next;
		curr->next = temp;
		curr = curr->previous;
	}
	if(temp!=NULL)
		*start = temp->previous;
}

#endif
