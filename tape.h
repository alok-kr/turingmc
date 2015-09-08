/*
   tape.h

   Author : Alok Kumar
   Date   : 30 Aug 2015

   Desc   : Implements a tape structure which behave like infinte at both ends.

*/

#ifndef __AL_TAPE_H_
#define __AL_TAPE_H_

#include <string.h>

#include "util.h"
#include "dlist.h"

#define BLOCK_SIZE	4

#define BLANK_CHAR	'_'

/* Structure definition */

struct tape {
	int headpos;
	struct node *list;
};

/* Function declaration */

struct tape *newTape();							/* Create New Tape 						*/
void deleteTape(struct tape *ptr);				/* Delete tape 							*/
void printTapeStructure(struct tape *ptr);		/* Print tape list 						*/
void previewTape(struct tape *ptr, char *dest, int size, int headAt);
												/* Copy a tape portion on dest string	*/
												
void set(struct tape *ptr, char symbol);		/* Set a symbol at tape 				*/
char get(struct tape *ptr);						/* Get a symbol from tape 				*/
void erase(struct tape *ptr);					/* Erase tape block at head 			*/
void eraseAll(struct tape *ptr);				/* Erase complete tape 					*/
void loadString(struct tape *ptr, char *str);	/* Copy a string on tape, head will be not affected */
void moveLeft(struct tape *ptr);				/* Move head to left 					*/
void moveRight(struct tape *ptr);				/* Move head to right 					*/

/* Function definition */

struct tape *newTape() {
	struct tape *ptr = (struct tape *)myAllocate(sizeof(struct tape));
	ptr->headpos = 0;
	ptr->list = newNode();
	ptr->list->data = (char *)myAllocate(BLOCK_SIZE);
	memset(ptr->list->data, BLANK_CHAR, BLOCK_SIZE);
	return ptr;
}

void deleteTape(struct tape *ptr) {
	struct node *temp = NULL;
	while(ptr->list->previous)
		ptr->list = ptr->list->previous;
	while(ptr->list) {
		temp = ptr->list;
		ptr->list = ptr->list->next;
		deleteNode(temp);
	}
	free(ptr);
}

void printTapeStructure(struct tape *ptr) {
	struct node *temp = ptr->list;
	int i = 1, j;
	while(temp->previous)
		temp = temp->previous;
	printf("Head Position : %d, Head Symbol : %c", ptr->headpos, get(ptr));
	while(temp) {
		printf("\nBlock %d :\n    ~", i++);
		for(j=0; j<BLOCK_SIZE; j++)
			printf("%c",*(temp->data + j));
		temp = temp->next;
		printf("~");
	}
	printf("\n");
}

void previewTape(struct tape *ptr, char *dest, int size, int headAt) {
	int i = headAt - 1;
	size--;
	while(i--)
		moveLeft(ptr);
	i = 0;
	while(i<size) {
		*dest = get(ptr);
		dest++;
		moveRight(ptr);
		i++;
	}
	*(dest) = '\0';
	i = size - headAt + 1;
	while(i--)
		moveLeft(ptr);
}

long ai(long x, long y) {
	if(x>=0)
		return x % y;
	else
		return ((x % y) + y) % y;
}

void set(struct tape *ptr, char symbol) {
	*(ptr->list->data + ai(ptr->headpos,BLOCK_SIZE)) = symbol;
}
char get(struct tape *ptr) {
	return *(ptr->list->data + ai(ptr->headpos,BLOCK_SIZE));
}

void erase(struct tape *ptr) {
	set(ptr,BLANK_CHAR);
}

void eraseAll(struct tape *ptr) {
	struct node *temp = NULL;
	while(ptr->list->previous)
		ptr->list = ptr->list->previous;
	while(ptr->list) {
		temp = ptr->list;
		ptr->list = ptr->list->next;
		deleteNode(temp);
	}
	ptr->headpos = 0;
	ptr->list = newNode();
	ptr->list->data = (char *)myAllocate(BLOCK_SIZE);
	memset(ptr->list->data, BLANK_CHAR, BLOCK_SIZE);
}

void loadString(struct tape *ptr, char *str) {
	// TODO - implement fast algo
	int i=0;
	if(*str) {
		while(*(str+1)) {
			set(ptr,*str);
			moveRight(ptr);
			i++;
			str++;
		}
		set(ptr, *str);
	}
	while(i--)
		moveLeft(ptr);
}

void moveRight(struct tape *ptr) {
	int preheadpos = ai(ptr->headpos, BLOCK_SIZE);
	preheadpos++;
	if(preheadpos==BLOCK_SIZE) {
		if(ptr->list->next==NULL) {
			struct node *temp = newNode();
				temp->data = (char *)myAllocate(BLOCK_SIZE);
				memset(temp->data, BLANK_CHAR, BLOCK_SIZE);
			temp->previous = ptr->list;
			ptr->list->next = temp;
		}
		ptr->list = ptr->list->next;
	}
	ptr->headpos++;
}

void moveLeft(struct tape *ptr) {
	int preheadpos = ai(ptr->headpos, BLOCK_SIZE);
	preheadpos--;
	if(preheadpos==-1) {
		if(ptr->list->previous==NULL) {
			struct node *temp = newNode();
				temp->data = (char *)myAllocate(BLOCK_SIZE);
				memset(temp->data, BLANK_CHAR, BLOCK_SIZE);
			temp->next = ptr->list;
			ptr->list->previous = temp;
		}
		ptr->list = ptr->list->previous;

	}
	ptr->headpos--;

}

#endif
