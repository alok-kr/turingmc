/*
   turing.h

   Author : Alok Kumar
   Date   : 30 Aug 2015

   Desc   : Simulate turing machine.

*/

#ifndef __AL_TURING_H_
#define __AL_TURING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#include "util.h"
#include "dlist.h"
#include "tape.h"

/* Structure Definition */

struct transTable {
	char *inState, *tape, *outState, *newTape, *tapeDirection;
};

struct turing {
	int state;		/* 0 - Not Stop, 1 - Undefined at Final, 2 - Undefined at non-Final */

	struct node *tapes;
	int tapeNum;

	char *currState, *initialState;

	struct node *finalStates, *alphabets, *states;
	int numFinalStates, numAlphabets, numStates;

	struct node *transitionTable;
	int numTransition;

	char *workTape; 					/* Used only for temporary storage of tape data */
	char blankChar;
};

/* Function Declaration */

struct turing *loadTuring(FILE *fp);		/* Load turing from a YAML Formatted file 	*/

void step(struct turing *ptr);				/* Perform one step of turing machine 		*/
void run(struct turing *ptr, long maxStep);	/* Run turing mac until it halt or maxStep	*/
void reset(struct turing *ptr);				/* Reset machine 							*/

void deleteTuring(struct turing *ptr);		/* Free all allocated resorces 				*/
void printTuringConf(struct turing *ptr);	/* Print Turing machine config 				*/

/* Function Definitions */

struct turing *loadTuring(FILE *fp) {
	
	if(fp==NULL) {
		printf("\nUnable to open file...\nExiting\n\n");
		exit(1);
	}

	struct turing *ptr = (struct turing *)myAllocate(sizeof(struct turing));

	ptr->state = 0;
	ptr->currState = ptr->initialState = NULL;
	ptr->tapes = ptr->finalStates = ptr->alphabets = ptr->states = ptr->transitionTable = NULL;
	ptr->tapeNum = ptr->numFinalStates = ptr->numAlphabets = ptr->numStates = ptr->numTransition = 0;
	ptr->blankChar = 'b';

	yaml_parser_t parser;
	yaml_event_t  event;

	int state = 0, i = 0;
	char *dup, *str;

	/* Initialize parser */
	if(!yaml_parser_initialize(&parser)) {
		fputs("Error : Failed to initialize parser!\n", stderr);
		exit(1);
	}

	/* Set input file */
	yaml_parser_set_input_file(&parser, fp);

	char *tok = NULL;
	struct transTable *t = NULL;
	/* PARSING */
	do {
		if (!yaml_parser_parse(&parser, &event)) {
			printf("Parser error %d\n", parser.error);
			exit(EXIT_FAILURE);
    		}

		switch(event.type)
		{
			case YAML_NO_EVENT: puts("No event!"); break;

			/* Stream start/end */
			case YAML_STREAM_START_EVENT: printf("\nReading Config..."); break;
			case YAML_STREAM_END_EVENT:   printf("\t\t[COMPLETED]\n\n");   break;

			/* Block delimeters */
			case YAML_DOCUMENT_START_EVENT: state = 0; break;
			case YAML_DOCUMENT_END_EVENT:   state = 0; break;

			case YAML_SEQUENCE_START_EVENT: break;
			case YAML_SEQUENCE_END_EVENT:   state = 0; break;

			case YAML_MAPPING_START_EVENT:  break;
			case YAML_MAPPING_END_EVENT:    state = 0; break;

			/* Data */
			case YAML_ALIAS_EVENT:          break;
			case YAML_SCALAR_EVENT:
				tok = event.data.scalar.value;
				strip(tok);
				if(state == 0) {

					if     ( !strcmp(tok, "Q") )	state = 1;
					else if( !strcmp(tok, "A") )	state = 2;
					else if( !strcmp(tok, "i") )	state = 3;
					else if( !strcmp(tok, "F") )	state = 4;
					else if( !strcmp(tok, "D") )	state = 5;
					else if( !strcmp(tok, "BLANK")) state = 6;
					else				state = 7;

				} else if(state < 11) {
					if     (state == 1) 		state = 11;
					else if(state == 2) 	 	state = 21;
					else if(state == 3) {
						/* Initial State */
						ptr->initialState = (char *)myAllocate(strlen(tok)+1);
						strcpy(ptr->initialState, tok);
						state = 0;
					}
					else if(state == 4)  		state = 41;
					else if(state == 5) 		state = 51;
					else if(state == 6) {
						/* Set Blank Character */
						ptr->blankChar = *tok;
						state = 0;
					}
					else {
						printf("\nError : Undefined key in file\n\n");
						exit(1);
					}
				}

				if(state == 11) {
					/* Set of States */
					if(ptr->states==NULL) {
						ptr->states = newNode();
					} else {
						ptr->states->previous = newNode();
						ptr->states->previous->next = ptr->states;
						ptr->states = ptr->states->previous;
					}
					ptr->states->data = (char *)myAllocate(strlen(tok)+1);
					strcpy(ptr->states->data, tok);
					ptr->numStates++;
					state = 1;
				}

				if(state == 21) {
					/* Set of Alphabets */
					if(ptr->alphabets==NULL) {
						ptr->alphabets = newNode();
					} else {
						ptr->alphabets->previous = newNode();
						ptr->alphabets->previous->next = ptr->alphabets;
						ptr->alphabets = ptr->alphabets->previous;
					}
					ptr->alphabets->data = (char *)myAllocate(strlen(tok)+1);
					strcpy(ptr->alphabets->data, tok);
					ptr->numAlphabets++;

					state = 2;
				}

				if(state == 41) {
					/* Set of Final States */
					if(ptr->finalStates==NULL) {
						ptr->finalStates = newNode();
					} else {
						ptr->finalStates->previous = newNode();
						ptr->finalStates->previous->next = ptr->finalStates;
						ptr->finalStates = ptr->finalStates->previous;
					}
					ptr->finalStates->data = (char *)myAllocate(strlen(tok)+1);
					strcpy(ptr->finalStates->data, tok);
					ptr->numFinalStates++;
					state = 4;
				}

				if(state == 51) {
					/* Transitions */
					t = (struct transTable *)myAllocate(sizeof(struct transTable));
					ptr->numTransition++;
					state = 511;
				} 
				if(state == 511) {
					/* Transition Left Part */

					/* Number of tapes */
					if(ptr->tapeNum==0) {
						dup = strndup(tok+2, strlen(tok)-3);
						str = strtok(dup, ",");

						while(str=strtok(NULL,",")) {
							ptr->tapeNum++;
						}
						free(dup);
					}
					dup = strndup(tok+2, strlen(tok)-3);

					str = strtok(dup, ",");
					t->inState = (char *)myAllocate(strlen(str)+1);
					strcpy(t->inState, str);
					t->tape = (char *)myAllocate(ptr->tapeNum+1);
					i = 0;
					while(str=strtok(NULL,",")) {
						*(t->tape + i) = *str;
						i++;
					}
					*(t->tape + i) = 0;
					free(dup);

					if(ptr->transitionTable==NULL) {
						ptr->transitionTable = newNode();
					} else {
						ptr->transitionTable->previous = newNode();
						ptr->transitionTable->previous->next = ptr->transitionTable;
						ptr->transitionTable = ptr->transitionTable->previous;
					}
					ptr->transitionTable->data = (char *)t;
					state = 512;
				} else if(state == 512) {
					/* Transition Right Part */
					dup = strndup(tok+1, strlen(tok)-2);

					str = strtok(dup, ",");
					t->outState = (char *)myAllocate(strlen(str)+1);
					strcpy(t->outState, str);
					t->newTape = (char *)myAllocate(ptr->tapeNum+1);
					t->tapeDirection = (char *)myAllocate(ptr->tapeNum+1);
					i = 0;
					while(str=strtok(NULL,"}")) {
						str = strchr(str, '{');
						*(t->newTape + i) = *(str+1);
						*(t->tapeDirection + i) = *(str+3);
						i++;
					}
					*(t->newTape + i) = 0;
					*(t->tapeDirection + i ) = 0;
					free(dup);

					state = 5;
				}

				break;
		}
		if(event.type != YAML_STREAM_END_EVENT)
			yaml_event_delete(&event);
	} while(event.type != YAML_STREAM_END_EVENT);

	yaml_event_delete(&event);

	/* Cleanup */
	yaml_parser_delete(&parser);

	/* Adding tape to turing machine */

	i = 0;
	while(i < ptr->tapeNum) {
		if(ptr->tapes==NULL) {
			ptr->tapes = newNode();
		} else {
			ptr->tapes->previous = newNode();
			ptr->tapes->previous->next = ptr->tapes;
			ptr->tapes = ptr->tapes->previous;
		}
		ptr->tapes->data = (char *)newTape();
		i++;
	}

	ptr->workTape = (char *)myAllocate(sizeof(ptr->tapeNum+1));
	ptr->currState = ptr->initialState;

	reverseDList(&ptr->states);
	reverseDList(&ptr->finalStates);
	reverseDList(&ptr->alphabets);
	reverseDList(&ptr->transitionTable);
        return ptr;

}

void step(struct turing *ptr) {
	struct node *temp = NULL;
	struct transTable *t = NULL, *f = NULL;
	int i=0;
	char ch;
	
	/* If machine not at halt or crashed state */
	if(ptr->state==0) {
		i = 0;

		/* Set up worktape value */
		temp = ptr->tapes;
		while(temp) {
			ch = get((struct tape *)(temp->data));
			if(ch==BLANK_CHAR)
				*(ptr->workTape + i) = ptr->blankChar;
			else
				*(ptr->workTape + i) = ch;

			i++;
			temp = temp->next;
		}
		*(ptr->workTape + i) = 0;
		temp = ptr->transitionTable;
		f = NULL;
		
		/* Find sutable transition */
		while(temp) {
			t = (struct transTable *)(temp->data);
			if(!strcmp(ptr->currState,t->inState) && !strcmp(ptr->workTape, t->tape)) {
				f = t;
				break;
			}
			temp = temp->next;
		}

		/* If not a transition found */
		if(f==NULL) {
			ptr->state = 2;
			temp = ptr->finalStates;
			while(temp) {
				/* If current state is final state */
				if(!strcmp(ptr->currState, temp->data))
					ptr->state = 1;
				temp = temp->next;
			}
		/* If transition found */
		} else {
			ptr->currState = f->outState;
			i = 0;
			temp = ptr->tapes;
            while(temp) {
				ch = *(f->newTape + i);
				if(ch==ptr->blankChar)
					erase((struct tape *)(temp->data));
				else
	        	    set((struct tape *)(temp->data), ch);
				if( *(f->tapeDirection + i)=='L')
					moveLeft((struct tape *)(temp->data));
				else if( *(f->tapeDirection + i)=='R')
					moveRight((struct tape *)(temp->data));
	            i++;
                temp = temp->next;
        	}
		}
	}
}

void run(struct turing *ptr, long maxStep) {
	while(ptr->state==0 && maxStep--) 
		step(ptr);
}

void reset(struct turing *ptr) {
	ptr->currState = ptr->initialState;
	struct node *temp = ptr->tapes;
	while(temp) {
		eraseAll((struct tape *)temp->data);
		temp = temp->next;
	}
}

void deleteTuring(struct turing *ptr) {
	deleteDList(ptr->finalStates);
	deleteDList(ptr->states);
	deleteDList(ptr->alphabets);

	struct node *temp = NULL;
	struct transTable *t = NULL;
	while(ptr->transitionTable) {
		temp = ptr->transitionTable;
		ptr->transitionTable = ptr->transitionTable->next;
		t = (struct transTable *)temp->data;
		free(t->inState);
		free(t->outState);
		free(t->tape);
		free(t->newTape);
		free(t->tapeDirection);
		free(t);
		free(temp);
	}

	struct tape *tempTape = NULL;
	while(ptr->tapes) {
		temp = ptr->tapes;
		ptr->tapes = ptr->tapes->next;
		deleteTape((struct tape *)temp->data);
		free(temp);
	}

	free(ptr->initialState);
	free(ptr->workTape);

	free(ptr);

}


void printTuringConf(struct turing *ptr) {
	struct node *temp = NULL;
	printf("--- Turing Machine Configuration ---\n\n");
	printf("Initial State : %s\n",ptr->initialState);
	printf("All States    : {");
	temp = ptr->states;
	if(temp) {
		printf("%s",temp->data);
		temp = temp->next;
	}
	while(temp) {
		printf(", %s",temp->data);
		temp = temp->next;
	}
	printf("}\nFinal States  : {");
	temp = ptr->finalStates;
	if(temp) {
		printf("%s",temp->data);
		temp = temp->next;
	}
	while(temp) {
		printf(", %s",temp->data);
		temp = temp->next;
	}
	printf("}\nAlphabets     : {");
	temp = ptr->alphabets;
	if(temp) {
		printf("%s",temp->data);
		temp = temp->next;
	}
	while(temp) {
		printf(", %s",temp->data);
		temp = temp->next;
	}

	printf("}\nTransitions   : \n");
	temp = ptr->transitionTable;
	struct transTable *t = NULL;
	char *ta = NULL, *tb = NULL;;
	int i =0;
	while(temp) {
		t = (struct transTable *)temp->data;
		printf("        d(%s",t->inState);
		ta = t->tape;
		i=0;
		while(i < ptr->tapeNum) {
			printf(",%c",*ta);
			ta++;
			i++;
		}
		printf(") = (%s",t->outState);
		i=0;
		ta = t->newTape;
		tb = t->tapeDirection;
		while(i < ptr->tapeNum) {
			printf(", {%c,%c}",*ta,*tb);
			ta++, tb++, i++;
		}

		printf(")\n");
		temp = temp->next;
	}
	printf("Blank Char    : %c\n",ptr->blankChar);
}

#endif
