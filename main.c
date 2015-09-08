#include <stdio.h>
#include <stdlib.h>

#include "turing.h"

int main() {

	/* Load Turing Machine from File */
	FILE *fp = fopen("anbncn.tm","r");
	struct turing *mymc = loadTuring(fp);
	fclose(fp);

	/* Print Loaded Configuration */
	printTuringConf(mymc);
	
	/* Load tape with string */
	char str[22] = "aaabbbccc";
	loadString((struct tape *)(mymc->tapes->data), str);

	/* Run machine */
	struct node *tapes = NULL;
	int stepCounter = 0, maxStep = 10000;
	while(mymc->state==0 && maxStep--) {
		printf("\n\nStep : %03d, Current State : %s, Machine State : %d", stepCounter, mymc->currState, mymc->state);
		tapes = mymc->tapes;
		while(tapes) {
			previewTape((struct tape *)(tapes->data), str, 22, 11);
			printf("\n%s\n          ^", str);
			tapes = tapes->next;
		}
		step(mymc);
		stepCounter++;
	}
	if(mymc->state==0)
		printf("\nMachine does not halt on executing 10000 steps");
	else {
		printf("\nStep : %03d, Current State : %s, Machine State : %d",stepCounter, mymc->currState, mymc->state);
		tapes = mymc->tapes;
		while(tapes) {
			previewTape((struct tape *)(tapes->data), str, 22, 11);
			printf("\n%s\n          ^", str);
			tapes = tapes->next;
		}
		
		if(mymc->state==1)
			printf("\n\nMachine halted");
		else
			printf("\n\nMachine crashed");
	}
	printf("\n\n");


	/* Clean Up - Free Memory allocated by Machine */
	deleteTuring(mymc);
	
	return 0;
}
