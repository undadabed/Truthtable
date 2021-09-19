#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

typedef enum {NOT, AND, OR, NAND, NOR, XOR, DECODER, MULTIPLEXER, PASS} kind;

typedef struct gate {
	kind type;
	int size;
	int *inputs;
	int *outputs;
} gate;

char *temp; // for whatever i may need
char **all; // for all the variables
int *value; // for all the values of said inputs, will correspond to input of same index
gate *help; // to hold a gate temporarily
gate **waiting; // array of gates that have variables not yet found
gate **current; // array of gates that have variables already found
int r, i, o, t, l, m, cptr, wptr, c, fix;
bool completed;

void not() {
	if (value[help->inputs[0]] == 0) {
		value[help->outputs[0]] = 1;
	}
	else {
		value[help->outputs[0]] = 0;
	}
}

void and() {
	if ((value[help->inputs[0]] == 1) && (value[help->inputs[1]] == 1)) {
		value[help->outputs[0]] = 1;
	}
	else {
		value[help->outputs[0]] = 0;
	}
}

void or() {
	if ((value[help->inputs[0]] == 1) || (value[help->inputs[1]] == 1)) {
		value[help->outputs[0]] = 1;
	}
	else {
		value[help->outputs[0]] = 0;
	}
}

void nand() {
	if ((value[help->inputs[0]] == 1) && (value[help->inputs[1]] == 1)) {
		value[help->outputs[0]] = 0;
	}
	else {
		value[help->outputs[0]] = 1;
	}
}

void nor() {
	if ((value[help->inputs[0]] == 1) || (value[help->inputs[1]] == 1)) {
		value[help->outputs[0]] = 0;
	}
	else {
		value[help->outputs[0]] = 1;
	}
}

void xor() {
	if (value[help->inputs[0]] != value[help->inputs[1]]) {
		value[help->outputs[0]] = 1;
	}
	else {
		value[help->outputs[0]] = 0;
	}
}

void decoder() {
	int total = 0;
	int powa = 1;
	for (fix = 0; fix < help->size; fix++) {
		powa = powa*2;
	}
	for (fix = 0; fix < help->size; fix++) {
		int twopow = 1;
		for (m = 0; m < (help->size - fix - 1); m++) {
			twopow = twopow*2;
		}
		total += twopow * value[help->inputs[fix]];
	}
	for (fix = 0; fix < powa; fix++) {
		value[help->outputs[fix]] = 0;
	}
	value[help->outputs[total]] = 1;
}

void multiplexer() {
	int total = 0;
	int twopow = 1;
	int twopowtwo = 1;
	// Find the full size of standard inputs
	for (fix = 0; fix < help->size; fix++) {
		twopow = twopow*2;
	}
	// go to end of selectors and go backwards giving each one another power of 2 and adding to total
	for (fix = twopow + help->size - 1; fix >= twopow; fix--) {
		total += value[help->inputs[fix]] *twopowtwo;
		twopowtwo = twopowtwo*2;
	}
	// Use value
	value[help->outputs[0]] = value[help->inputs[total]];
}

void pass() {
	value[help->outputs[0]] = value[help->inputs[0]];
}

void identifyAndSolve() {
	// identify what type it is
	if (help->type == 0) {
		not();
	}
	else if (help->type == 1) {
		and();
	}
	else if (help->type == 2) {
		or();
	}
	else if (help->type == 3) {
		nand();
	}
	else if (help->type == 4) {
		nor();
	}
	else if (help->type == 5) {
		xor();
	}
	else if (help->type == 6) {
		decoder();
	}
	else if (help->type == 7) {
		multiplexer();
	}
	else if (help->type == 8) {
		pass();
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		return EXIT_FAILURE;
	}
	bool first = true;
	bool found;
	bool completed = false;
	// MALLOC
	all = malloc(sizeof(char *) * 512);
	if (!all) {
		return EXIT_FAILURE;
	}
	for (l = 0; l < 512; l++) {
		all[l] = malloc(sizeof(char) * 17);
		if (!all[l]) {
			return EXIT_FAILURE;
		}
	}
	value = malloc(sizeof(int) * 512);
	waiting = malloc(sizeof(gate) * 128);
	if (!waiting) {
		return EXIT_FAILURE;
	}
	current = malloc(sizeof(gate)*128);
	if (!current) {
		return EXIT_FAILURE;
	}
	temp = malloc(sizeof(char) * 128);
	i = 0;
	o = 0;
	// Open the file
	FILE *fp;
	fp = fopen(argv[1], "r");
	if (!fp) {
		perror("fopen");
		return EXIT_FAILURE;
	}
	c = 0;
	t = 0;
	// Loop to read the file ADD LOOP UNTIL OUT OF INPUTS
	while (fscanf(fp, "%16s", temp) != EOF) {
		// Fill in inputs/outputs
		if (strcmp(temp, "INPUT") == 0) {
			r = fscanf(fp, "%d", &c);
			for (l = 0; l < c; l++) {
				r = fscanf(fp, "%16s", all[i]);
				i++;
			}
		}
		else if (strcmp(temp, "OUTPUT") == 0) {
			r = fscanf(fp, "%d", &c);
			for (l = 0; l < c; l++) {
				r = fscanf(fp, "%16s", all[i+o]);
				o++;
			}
		}
		// Create gates
		else if (strcmp(temp, "AND") == 0) {
			first = true;
			help = (gate *)malloc(sizeof(gate));
			help->type = AND;
			help->size = 3;
			int *te = malloc(sizeof(int) * 128);
			if (!te) {
				return EXIT_FAILURE;
			}
			help->inputs = te;
			int *tem = malloc(sizeof(int) * 128);
			if (!tem) {
				return EXIT_FAILURE;
			}
			help->outputs = tem;
			r = fscanf(fp, "%16s", temp);
			found = false;
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[0] = 511;
				}
				else {
					help->inputs[0] = 510;
				}
			}
			else {
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[0] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[0] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[1] = 511;
				}
				else {
					help->inputs[1] = 510;
				}
			}
			else {
				found = false;
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[1] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[1] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			found = false;
			for (l = 0; l < i+o+t; l++) {
				if (strcmp(all[l], temp) == 0) {
					found = true;
					help->outputs[0] = l;
				}
			}
			if (found == false) {
					free(all[i+o+t]);
				all[i+o+t] = temp;
				temp = malloc(sizeof(char) * 128);
				help->outputs[0] = i+o+t;
				t++;
			}
			if (first == true) {
				current[cptr] = help;
				cptr++;
			}
			else {
				waiting[wptr] = help;
				wptr++;
			}
			
		}
		else if (strcmp(temp, "OR") == 0) {
			first = true;
			help = (gate *)malloc(sizeof(gate));
			help->type = OR;
			help->size = 3;
			int *te = malloc(sizeof(int) * 128);
			if (!te) {
				return EXIT_FAILURE;
			}
			help->inputs = te;
			int *tem = malloc(sizeof(int) * 128);
			if (!tem) {
				return EXIT_FAILURE;
			}
			help->outputs = tem;
			r = fscanf(fp, "%16s", temp);
			found = false;
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[0] = 511;
				}
				else {
					help->inputs[0] = 510;
				}
			}
			else {
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[0] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[0] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[1] = 511;
				}
				else {
					help->inputs[1] = 510;
				}
			}
			else {
				found = false;
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[1] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[1] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			found = false;
			for (l = 0; l < i+o+t; l++) {
				if (strcmp(all[l], temp) == 0) {
					found = true;
					help->outputs[0] = l;
				}
			}
			if (found == false) {
					free(all[i+o+t]);
				all[i+o+t] = temp;
				temp = malloc(sizeof(char) * 128);
				help->outputs[0] = i+o+t;
				t++;
			}
			if (first == true) {
				current[cptr] = help;
				cptr++;
			}
			else {
				waiting[wptr] = help;
				wptr++;
			}
			
		}
		else if (strcmp(temp, "NOT") == 0) {
			first = true;
			help = (gate *)malloc(sizeof(gate));
			help->type = NOT;
			help->size = 3;
			int *te = malloc(sizeof(int) * 128);
			if (!te) {
				return EXIT_FAILURE;
			}
			help->inputs = te;
			int *tem = malloc(sizeof(int) * 128);
			if (!tem) {
				return EXIT_FAILURE;
			}
			help->outputs = tem;
			r = fscanf(fp, "%16s", temp);
			found = false;
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[0] = 511;
				}
				else {
					help->inputs[0] = 510;
				}
			}
			else {
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[0] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[0] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			found = false;
			for (l = 0; l < i+o+t; l++) {
				if (strcmp(all[l], temp) == 0) {
					found = true;
					help->outputs[0] = l;
				}
			}
			if (found == false) {
					free(all[i+o+t]);
				all[i+o+t] = temp;
				temp = malloc(sizeof(char) * 128);
				help->outputs[0] = i+o+t;
				t++;
			}
			if (first == true) {
				current[cptr] = help;
				cptr++;
			}
			else {
				waiting[wptr] = help;
				wptr++;
			}
			
		}
		else if (strcmp(temp, "XOR") == 0) {
			first = true;
			help = (gate *)malloc(sizeof(gate));
			help->type = XOR;
			help->size = 3;
			int *te = malloc(sizeof(int) * 128);
			if (!te) {
				return EXIT_FAILURE;
			}
			help->inputs = te;
			int *tem = malloc(sizeof(int) * 128);
			if (!tem) {
				return EXIT_FAILURE;
			}
			help->outputs = tem;
			r = fscanf(fp, "%16s", temp);
			found = false;
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[0] = 511;
				}
				else {
					help->inputs[0] = 510;
				}
			}
			else {
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[0] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[0] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[1] = 511;
				}
				else {
					help->inputs[1] = 510;
				}
			}
			else {
				found = false;
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[1] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[1] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			found = false;
			for (l = 0; l < i+o+t; l++) {
				if (strcmp(all[l], temp) == 0) {
					found = true;
					help->outputs[0] = l;
				}
			}
			if (found == false) {
					free(all[i+o+t]);
				all[i+o+t] = temp;
				temp = malloc(sizeof(char) * 128);
				help->outputs[0] = i+o+t;
				t++;
			}
			if (first == true) {
				current[cptr] = help;
				cptr++;
			}
			else {
				waiting[wptr] = help;
				wptr++;
			}
			
		}
		else if (strcmp(temp, "NAND") == 0) {
			first = true;
			help = (gate *)malloc(sizeof(gate));
			help->type = NAND;
			help->size = 3;
			int *te = malloc(sizeof(int) * 128);
			if (!te) {
				return EXIT_FAILURE;
			}
			help->inputs = te;
			int *tem = malloc(sizeof(int) * 128);
			if (!tem) {
				return EXIT_FAILURE;
			}
			help->outputs = tem;
			r = fscanf(fp, "%16s", temp);
			found = false;
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[0] = 511;
				}
				else {
					help->inputs[0] = 510;
				}
			}
			else {
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[0] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[0] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[1] = 511;
				}
				else {
					help->inputs[1] = 510;
				}
			}
			else {
				found = false;
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[1] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[1] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			found = false;
			for (l = 0; l < i+o+t; l++) {
				if (strcmp(all[l], temp) == 0) {
					found = true;
					help->outputs[0] = l;
				}
			}
			if (found == false) {
					free(all[i+o+t]);
				all[i+o+t] = temp;
				temp = malloc(sizeof(char) * 128);
				help->outputs[0] = i+o+t;
				t++;
			}
			if (first == true) {
				current[cptr] = help;
				cptr++;
			}
			else {
				waiting[wptr] = help;
				wptr++;
			}
			
		}
		else if (strcmp(temp, "NOR") == 0) {
			first = true;
			help = (gate *)malloc(sizeof(gate));
			help->type = NOR;
			help->size = 3;
			int *te = malloc(sizeof(int) * 128);
			if (!te) {
				return EXIT_FAILURE;
			}
			help->inputs = te;
			int *tem = malloc(sizeof(int) * 128);
			if (!tem) {
				return EXIT_FAILURE;
			}
			help->outputs = tem;
			r = fscanf(fp, "%16s", temp);
			found = false;
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[0] = 511;
				}
				else {
					help->inputs[0] = 510;
				}
			}
			else {
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[0] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[0] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[1] = 511;
				}
				else {
					help->inputs[1] = 510;
				}
			}
			else {
				found = false;
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[1] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[1] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			found = false;
			for (l = 0; l < i+o+t; l++) {
				if (strcmp(all[l], temp) == 0) {
					found = true;
					help->outputs[0] = l;
				}
			}
			if (found == false) {
					free(all[i+o+t]);
				all[i+o+t] = temp;
				temp = malloc(sizeof(char) * 128);
				help->outputs[0] = i+o+t;
				t++;
			}
			if (first == true) {
				current[cptr] = help;
				cptr++;
			}
			else {
				waiting[wptr] = help;
				wptr++;
			}
			
		}
		// DECODER
		else if (strcmp(temp, "DECODER") == 0) {
			first = true;
			help = (gate *)malloc(sizeof(gate));
			help->type = DECODER;
			r = fscanf(fp, "%d", &(help->size));
			int twopow = 1;
			for (l = 0; l < help->size; l++) {
				twopow = twopow*2;
			}
			int *te = malloc(sizeof(int) * help->size);
			if (!te) {
				return EXIT_FAILURE;
			}
			help->inputs = te;
			int *tem = malloc(sizeof(int) * twopow);
			if (!tem) {
				return EXIT_FAILURE;
			}
			help->outputs = tem;
			for (m = 0; m < help->size; m++) {
				r = fscanf(fp, "%16s", temp);
				found = false;
				if (isdigit(temp[0])) {
					if (atoi(temp) == 1) {
						help->inputs[m] = 511;
					}
					else {
						help->inputs[m] = 510;
					}
				}
				else {
					for (l = 0; l < i+o+t; l++) {
						if (strcmp(all[l], temp) == 0) {
							found = true;
							help->inputs[m] = l;
							if (l > i) {
								first = false;
							}
						}
					}
					if (found == false) {
						first = false;
					free(all[i+o+t]);
						all[i+o+t] = temp;
						temp = malloc(sizeof(char) * 128);
						help->inputs[m] = i+o+t;
						t++;
					}
				}
			}
			for (m = 0; m < twopow; m++) {
				r = fscanf(fp, "%16s", temp);
				found = false;
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->outputs[m] = l;
					}
				}
				if (found == false) {
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->outputs[m] = i+o+t;
					t++;
				}
			}
			if (first == true) {
				current[cptr] = help;
				cptr++;
			}
			else {
				waiting[wptr] = help;
				wptr++;
			}
		}
		// MULTIPLEXER
		else if (strcmp(temp, "MULTIPLEXER") == 0) {
			first = true;
			help = (gate *)malloc(sizeof(gate));
			help->type = MULTIPLEXER;
			r = fscanf(fp, "%d", &(help->size));
			int twopow = 1;
			for (l = 0; l < help->size; l++) {
				twopow = twopow * 2;
			}
			int *te = malloc(sizeof(int) * (twopow + help->size));
			if (!te) {
				return EXIT_FAILURE;
			}
			help->inputs = te;
			int *tem = malloc(sizeof(int) * 1);
			if (!tem) {
				return EXIT_FAILURE;
			}
			help->outputs = tem;
			for (m = 0; m < (twopow + help->size); m++) {
				r = fscanf(fp, "%16s", temp);
				found = false;
				if (isdigit(temp[0])) {
					if (atoi(temp) == 1) {
						help->inputs[m] = 511;
					}
					else {
						help->inputs[m] = 510;
					}
				}
				else {
					for (l = 0; l < i+o+t; l++) {
						if (strcmp(all[l], temp) == 0) {
							found = true;
							help->inputs[m] = l;
							if (l > i) {
								first = false;
							}
						}
					}
					if (found == false) {
						first = false;
					free(all[i+o+t]);
						all[i+o+t] = temp;
						temp = malloc(sizeof(char) * 128);
						help->inputs[m] = i+o+t;
						t++;
					}
				}
			}
			r = fscanf(fp, "%16s", temp);
			found = false;
			for (l = 0; l < i+o+t; l++) {
				if (strcmp(all[l], temp) == 0) {
					found = true;
					help->outputs[0] = l;
				}
			}
			if (found == false) {
					free(all[i+o+t]);
				all[i+o+t] = temp;
				temp = malloc(sizeof(char) * 128);
				help->outputs[0] = i+o+t;
				t++;
			}
			if (first == true) {
				current[cptr] = help;
				cptr++;
			}
			else {
				waiting[wptr] = help;
				wptr++;
			}
		}
		else if (strcmp(temp, "PASS") == 0) {
			first = true;
			help = (gate *)malloc(sizeof(gate));
			help->type = PASS;
			help->size = 3;
			int *te = malloc(sizeof(int) * 128);
			if (!te) {
				return EXIT_FAILURE;
			}
			help->inputs = te;
			int *tem = malloc(sizeof(int) * 128);
			if (!tem) {
				return EXIT_FAILURE;
			}
			help->outputs = tem;
			r = fscanf(fp, "%16s", temp);
			found = false;
			if (isdigit(temp[0])) {
				if (atoi(temp) == 1) {
				help->inputs[0] = 511;
				}
				else {
					help->inputs[0] = 510;
				}
			}
			else {
				for (l = 0; l < i+o+t; l++) {
					if (strcmp(all[l], temp) == 0) {
						found = true;
						help->inputs[0] = l;
						if (l > i) {
							first = false;
						}
					}
				}
				if (found == false) {
					first = false;
					free(all[i+o+t]);
					all[i+o+t] = temp;
					temp = malloc(sizeof(char) * 128);
					help->inputs[0] = i+o+t;
					t++;
				}
			}
			r = fscanf(fp, "%16s", temp);
			found = false;
			for (l = 0; l < i+o+t; l++) {
				if (strcmp(all[l], temp) == 0) {
					found = true;
					help->outputs[0] = l;
				}
			}
			if (found == false) {
					free(all[i+o+t]);
				all[i+o+t] = temp;
				temp = malloc(sizeof(char) * 128);
				help->outputs[0] = i+o+t;
				t++;
			}
			if (first == true) {
				current[cptr] = help;
				cptr++;
			}
			else {
				waiting[wptr] = help;
				wptr++;
			}
			
		}
	}
	// Finished creating and semi-organizing gates. Now test different possible inputs for the truth table.
	int twopow = 1;
	int just;
	value[511] = 1;
	value[510] = 0;
	for (l = 0; l < i; l++) {
		twopow = twopow * 2;
	}
	for (l = 0; l < 510; l++) {
		value[l] = 2;
	}
	for (just = 0; just < twopow; just++) {
		for (l = 0; l < 510; l++) {
			value[l] = 2;
		}
		// set all inputs based on what just equals at the moment.
		int justsave = just;
		for (l = i-1; l >= 0; l--) {
			value[l] = justsave%2;
			justsave = justsave/2;
		}
		int currentptr = 0;
		// go through all the current gates
		while (currentptr < cptr) {
			help = current[currentptr];
			// Method to help organize and lessen the clunkiness
			identifyAndSolve();
			currentptr++;
		}
		for (l = i; l < i+o; l++) {
			if (value[l] == 2) {
				completed = false;
			}
		}
// while completed == false and remove above
		while (completed == false) {
			// check for gates that can be completed
			for (l = 0; l < wptr; l++) {
				help = waiting[l];
				if (help->type == 0) {
					if (((value[help->outputs[0]] == 2) || (value[help->outputs[0]] == 3)) && (value[help->inputs[0]] != 2)) {
						not();
					}
				}
				else if (help->type == 1) {
					if (((value[help->outputs[0]] == 2) || (value[help->outputs[0]] == 3)) && (value[help->inputs[0]] != 2) && (value[help->inputs[1]] != 2)) {
						and();
					}
				}
				else if (help->type == 2) {
					if (((value[help->outputs[0]] == 2) || (value[help->outputs[0]] == 3)) && (value[help->inputs[0]] != 2) && (value[help->inputs[1]] != 2)) {
						or();
					}
				}
					else if (help->type == 3) {
						if (((value[help->outputs[0]] == 2) || (value[help->outputs[0]] == 3)) && (value[help->inputs[0]] != 2) && (value[help->inputs[1]] != 2)) {
							nand();
					}
				}
				else if (help->type == 4) {
					if (((value[help->outputs[0]] == 2) || (value[help->outputs[0]] == 3)) && (value[help->inputs[0]] != 2) && (value[help->inputs[1]] != 2)) {
						nor();
					}
				}
				else if (help->type == 5) {
					if (((value[help->outputs[0]] == 2) || (value[help->outputs[0]] == 3)) && (value[help->inputs[0]] != 2) && (value[help->inputs[1]] != 2)) {
						xor();
					}
				}
				else if (help->type == 6) {
					bool todo = true;
					int powa = 1;
					for (m = 0; m < waiting[l]->size; m++) {
						powa = powa*2;
					}
					for (m = 0; m < help->size; m++) {
						if (value[help->inputs[m]] == 2) {
							todo = false;
						}
					}
					if (todo) {
						decoder();
					}
				}
				else if (help->type == 7) {
					bool todo = true;
					int powa = 1;
					for (m = 0; m < waiting[l]->size; m++) {
						powa = powa*2;
					}
					if (value[help->outputs[0]] == 2) {
						for (m = 0; m < help->size + powa; m++) {
							if (value[help->inputs[m]] == 2) {
								todo = false;
							}
						}
					}
					if (todo) {
						multiplexer();
					}
				}
				else if (help->type == 8) {
					if (((value[help->outputs[0]] == 2) || (value[help->outputs[0]] == 3)) && (value[help->inputs[0]] != 2)) {
						pass();
					}
				}
			}
			// check if it's done
			completed = true;
			for (l = i; l < i+o; l++) {
				if (value[l] == 2) {
					completed = false;
				}
			}
		}
		for (l = 0; l < i; l++) {
			printf("%d ", value[l]);
		}
		printf("|");
		for (l = 0; l < o; l++) {
			printf(" %d", value[l+i]);
		}
		printf("\n");
	}
	// free everything
	for (l = 0; l < 512; l++) {
		free(all[l]);
	}
	free(all);
	free(value);
	for (l = 0; l < cptr; l++) {
		free(current[l]->inputs);
		free(current[l]->outputs);
		free(current[l]);
	}
	for (l = 0; l < wptr; l++) {
		free(waiting[l]->inputs);
		free(waiting[l]->outputs);
		free(waiting[l]);
	}
	free(current);
	free(waiting);
	free(temp);
	return EXIT_SUCCESS;
}
