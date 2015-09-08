#ifndef __AL_UTIL_H_
#define __AL_UTIL_H_

/* Function declaration */

void *myAllocate(long size);	/* Allocate memory 							 */
void strip(char *str);			/* Removes all space, \n, \r, \t from string */

/* Function definition */

void *myAllocate(long size) {
	void *ptr = NULL;
	ptr = malloc(size);
	if(ptr==NULL) {
		printf("\nError: Out of Memory\n");
		exit(1);
	}
	return ptr;
}

void strip(char *str) {
	int i=0;
	char *temp = str;
	while(*temp) {
		if((*temp==' ' || *temp=='\n' || *temp=='\r' || *temp=='\t')) {
			temp++;
			continue;
		}
		*str = *temp;
		str++, temp++;
	}
	*str=0;
}

#endif
