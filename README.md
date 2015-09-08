Turing Machine


-- Required --

	libyaml must be installed (used for file parsing)
	input file must exists

-- Compile Steps --

	gcc main.c -lyaml -o main.out

-- Executing Steps --

	./main.out

-- Project Structure --

	main.c			- Main File
		turing.h		- Header file define turing machine
		tape.h			- Header file define tape stucture
		dlist.h			- Header file defines double-linked list structure and operations
		util.h			- Header file for other functions

	anbncn.tm	- Example turing machine config file
	output.txt	- Output of anbncn.tm

	README.md	- Project Description
