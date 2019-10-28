#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "helper_funcs.h"
#include "symbols.h"

//Take a look at the numeric values of the definitions for this two constant arrays
const char *node_types[] = {"+", "-", "*", "/", "%", "<", ">", "<=", ">=",
 "==", "!=", "&&", "||", "CONSTANT", "IN_TEMP_MEMORY", "ID", "PROGRAM", "DECLS", "DECL", 
  "STATEMENT", "STATEMENTS", "=", "*=", "/=", "+=", "-=", "%=", "PRINT", ";",
 "NOT", "NEGATIVE", "BLOCK", "ELSE-BLC", "IF", "WHILE", "FOR", "BREAK", "CONTINUE"};

const char *opcodes_jumps[] = {"JL", "JG", "JLE", "JGE", "JE", "JNE" };

int check_declare = 0;
int stack_top = 0;

int loop_counter = -1;
int general_loop_counter = -1;
int if_counter = -1;
int and_counter = -1;
int or_counter = -1;

node *
print_tree(node* root, int depth){
	if (root  == NULL) return 0;

	depth++;

	for (int i = 0; i < depth; ++i)
	{
		printf(" ");
	}
	if (root->type != CONSTANT && root->type != ID)
		printf("@%s\n", node_types[root->type]);
	else{
		if (root->type == ID)
			printf("   ID=%s\n", root->symbol);
		else
			printf("   #%d\n", root->value);
			
	}

	for (int i = 0; i < 4; i++)
		print_tree(root->_node_[i], depth);
}


node *
newnode (int type, node *left, node *right, node *mid, node *mid2, int value, char *symbol)
{
	node *to_ret = (node *) calloc(1 ,sizeof(node));
	check_null(to_ret);

	//if number 
	if (value > 1073741823){
		yyerror(" the max int is 1.073.741.823\n");
		exit(1);
	}

	to_ret->value = value;

	//if is a variable variable name
	to_ret->symbol = symbol;

	// node common stuff
	to_ret->type = type;
	to_ret->_node_[LEFT] = left;
	to_ret->_node_[RIGHT] = right;
	to_ret->_node_[MID] = mid;
	to_ret->_node_[MID2] = mid2;

	return to_ret;
}


node * 
newnode_2(int type, node *left, node *right)
{
	return newnode(type, left, right, NULL, NULL, 0, '\0');
}

node * 
newnode_3(int type, node *left, node *mid, node *right)
{
	return newnode(type, left, right, mid, NULL, 0, '\0');
}

node * 
newnode_4(int type, node *left, node *mid, node *mid2, node *right)
{
	return newnode(type, left, right, mid, mid2, 0, '\0');
}


node *
newnode_num(int value)
{
	return newnode(CONSTANT, NULL, NULL, NULL, NULL, value, '\0');
}


node *
newnode_symbol(node *left, node *right ,char *symbol)
{
	return newnode(ID, left, right, NULL, NULL, 0, symbol);
}


void
save_or_not_valueof_A(node *this_node)
{
	if (this_node->type == IN_TEMP_MEMORY){
		if (stack_top == 0)
			printf("\tSTA STACK\n");
		else
			printf("\tSTA STACK+%d\n", stack_top);

		stack_top++;
	}
}


void
load_to_A(node *this_node)
{
	if (this_node->type == CONSTANT)
		constant_load_big_number(this_node->value);
	else if (this_node->type == ID)
		printf("\tLDA VAR%d\n", getSymbolId(this_node->symbol));
}


void 
constant_load_big_number(int value){
	if (value <= 4095)
		printf("\tENTA %d\n", value);
	else if (value < 999999999)
		printf("\tLDA =%d=\n", value);
	else{
		printf("\tLDA =999999999=\n");
		printf("\tADD =%d=\n", value - 999999999);

	}
}


void 
generate_code(node *root)
{
	printf("OUTBUFF\tEQU 51\n");
	printf("OUTBUFF1\tEQU 52\n");
	printf("OUTBUFF2\tEQU 53\n\n");

	printf("TEMP\tEQU 3500\n");
	printf("TEMP2\tEQU 3501\n");
	printf("STACK\tEQU 3502\n");
	
	printf("START\tEQU 80\n");
	printf("\tORIG   START\n");
	printf("\n");

	eval(root);
	
	printf("\tENTA 0\n"); // Simulate linux exit codes 
	printf("\tHLT\n");	   // If all good return 0
	printf("ERROR\tENTA 1\n");
	printf("\tHLT\n");
	printf("\tEND START\n");
}

void
check_overflow(char is_mul){
	//If not Zero so not a div(check the docs) 
	if (!is_mul)
		printf("\tJOV ERROR\n");
	else
		printf("\tJANZ ERROR\n");

}

void
generate_instr(node *this_node, const char *op1, const char *op2)
{
	if (this_node->type == IN_TEMP_MEMORY){
		stack_top--;
		if (stack_top == 0)
			printf("\t%s STACK\n", op1);
		else
			printf("\t%s STACK+%d\n", op1, stack_top);
	}else if (this_node->type == CONSTANT){
			if (this_node->value <= 4095){
				if (strcmp(op2,"MUL") == 0 || strcmp(op2,"DIV") == 0 || strcmp(op2,"CMPA") == 0)
					printf("\t%s =%d=\n", op2, this_node->value);
				else
					printf("\t%s %d\n", op2, this_node->value);
			// Manipulate large values
			}else if (this_node->value < 999999999){
				printf("\tLDX =%d=\n", this_node->value);
				printf("\tSTX TEMP\n");
				printf("\t%s TEMP\n",op1 );
			}else{
				printf("\tSTA TEMP\n");
				printf("\tLDA =999999999=\n");
				printf("\tADD =%d=\n", this_node->value - 999999999);
				printf("\tSTA TEMP2\n");
				printf("\tLDA TEMP\n");
				printf("\t%s TEMP2\n",op1 );
			}
	}else if (this_node->type == ID){
		printf("\t%s VAR%d\n", op1, getSymbolId(this_node->symbol));
	}
}


int 
eval(node *root){
	if (root == NULL) return -1;
	
	int if_counter_tmp, if_counter_tmp2;
	int loop_counter_tmp, loop_counter_tmp2, loop_counter_tmp3;
	int and_counter_tmp;

	switch(root->type){
		case MUL_EQU:
		case DIV_EQU:
		case ADD_EQU:
		case SUB_EQU:
		case MOD_EQU:
		case EQU_VAR:
			eval(root->_node_[LEFT]);
			//Random :P
			save_or_not_valueof_A(root->_node_[LEFT]);

			//Is it memmory or a constant ?
			eval(root->_node_[RIGHT]);
			load_to_A(root->_node_[RIGHT]);

		

			printf("\tSTA VAR%d\n", getSymbolId(root->_node_[LEFT]->symbol));

			break;
		
		case ID:
			if (!check_declare){
				gen_variable(root);
				//Only care about the left because this is how we created the ast
				eval(root->_node_[LEFT]);
			}
			break;	
		
		case STATEMENTS:
			check_declare = 1;
			eval(root->_node_[LEFT]);
			eval(root->_node_[RIGHT]);
			break;

		case ADD:

			// Main execution is from the left to the right
			eval(root->_node_[LEFT]);
			// Save value in order to start executing the expression 
			// ont the right branch
			save_or_not_valueof_A(root->_node_[LEFT]);
			
			// Execute the second brach
			eval(root->_node_[RIGHT]);
			// Load value to A reg 
			load_to_A(root->_node_[RIGHT]);

			generate_instr(root->_node_[LEFT], "ADD", "INCA");
			
			check_overflow(0);
			root->type = IN_TEMP_MEMORY;
			break;

		case NEGATIVE:
		case SUB:
			// Main execution is from the right to the left
			eval(root->_node_[RIGHT]);
			// Save value in order to start executing the expression 
			// ont the right branch
			save_or_not_valueof_A(root->_node_[RIGHT]);
			
			// Execute the second brach
			eval(root->_node_[LEFT]);
			// Load value to A reg 
			load_to_A(root->_node_[LEFT]);

			generate_instr(root->_node_[RIGHT], "SUB", "DECA");
			
			check_overflow(0);
			root->type = IN_TEMP_MEMORY;
			break;

		case MUL:
			// Main execution is from the left to the right
			eval(root->_node_[LEFT]);
			// Save value in order to start executing the expression 
			// ont the right branch
			save_or_not_valueof_A(root->_node_[LEFT]);
			
			// Execute the second brach
			eval(root->_node_[RIGHT]);
			// Load value to A reg 
			load_to_A(root->_node_[RIGHT]);

			generate_instr(root->_node_[LEFT], "MUL", "MUL");

			check_overflow(1);
			printf("\tSTX TEMP\n");
			printf("\tLDA TEMP\n");

			root->type = IN_TEMP_MEMORY;
			break;

		case MOD:
		case DIV:
			// Main execution is from the right to the left
			eval(root->_node_[RIGHT]);
			// Save value in order to start executing the expression 
			// ont the right1 branch
			save_or_not_valueof_A(root->_node_[RIGHT]);
			
			// Execute the second brach
			eval(root->_node_[LEFT]);
			// Load value to A reg 
			load_to_A(root->_node_[LEFT]);

			printf("\tSTA TEMP\n");
			printf("\tLDX TEMP\n");
			printf("\tENTA 0\n");

			generate_instr(root->_node_[RIGHT], "DIV", "DIV");

			check_overflow(0);
			if (root->type == MOD){
				printf("\tSTX TEMP\n");
				printf("\tLDA TEMP\n");
			}

			root->type = IN_TEMP_MEMORY;
			break;

		case SMALLER:
		case BIGGER:
		case SMALLER_EQU:
		case BIGGER_EQU:
		case EQU:
		case NOT_EQU:
			// Main execution is from the right to the left
			eval(root->_node_[RIGHT]);
			// Save value in order to start executing the expression 
			// ont the right1 branch
			save_or_not_valueof_A(root->_node_[RIGHT]);
			
			// Execute the second brach
			eval(root->_node_[LEFT]);
			// Load value to A reg 
			load_to_A(root->_node_[LEFT]);

			generate_instr(root->_node_[RIGHT], "CMPA", "CMPA");

			//Take a look at the numeric values of the definitions
			printf("\t%s *+3\n", opcodes_jumps[root->type - SMALLER]);
			printf("\tENTA 0\n");
			printf("\tJMP *+2\n");
			printf("\tENTA 1\n");

			root->type = IN_TEMP_MEMORY;
			break;
		
		case NOT:
			// Main execution is from the right to the left
			eval(root->_node_[RIGHT]);
			// Save value in order to start executing the expression 
			// ont the right1 branch
			save_or_not_valueof_A(root->_node_[RIGHT]);

			printf("\tJAZ *+3\n", opcodes_jumps[root->type - SMALLER]);
			printf("\tENTA 0\n");
			printf("\tJMP *+2\n");
			printf("\tENTA 1\n");

			root->type = IN_TEMP_MEMORY;
			break;

		case AND:
			// Main execution is from the left to the right
			eval(root->_node_[LEFT]);
			// Save value in order to start executing the expression 
			// ont the right branch
			save_or_not_valueof_A(root->_node_[LEFT]);
			load_to_A(root->_node_[LEFT]);
			
			//Next LIA symbol
			and_counter_tmp = ++and_counter;

			printf("\tJAZ LAND%d\n", and_counter);
			// Execute the second brach
			eval(root->_node_[RIGHT]);
			// Load value to A reg 
			load_to_A(root->_node_[RIGHT]);
		
			printf("LAND%d\tNOP\n", and_counter_tmp);
			break;
		case OR:
			// Main execution is from the left to the right
			eval(root->_node_[LEFT]);
			// Save value in order to start executing the expression 
			// ont the right branch
			save_or_not_valueof_A(root->_node_[LEFT]);
			load_to_A(root->_node_[LEFT]);
			
			//Next LIA symbol
			or_counter = ++or_counter;

			printf("\tJANZ LOR%d\n", or_counter);
			// Execute the second brach
			eval(root->_node_[RIGHT]);
			// Load value to A reg 
			load_to_A(root->_node_[RIGHT]);
		
			printf("LOR%d\tNOP\n", or_counter);
			break;
		case _IF:
			//Next LIA symbol
			if_counter_tmp = ++if_counter;
			if (root->_node_[RIGHT] != NULL) if_counter_tmp2 = ++if_counter; 
			
			eval(root->_node_[LEFT]);
			load_to_A(root->_node_[LEFT]);
			printf("\tJAZ LIF%d\n", if_counter_tmp);
			
			eval(root->_node_[MID]);

			if (root->_node_[RIGHT] != NULL)  printf("\tJMP LIF%d\n", if_counter_tmp2);
			printf("LIF%d\tNOP\n", if_counter_tmp);
			eval(root->_node_[RIGHT]);
			if (root->_node_[RIGHT] != NULL)  printf("LIF%d\tNOP\n", if_counter_tmp2);
			
			break;
		case _WHILE:
			loop_counter_tmp = ++general_loop_counter; // start
			loop_counter_tmp2 = ++general_loop_counter; // end
			loop_counter = general_loop_counter;

			printf("LLOOP%d", loop_counter_tmp);
			eval(root->_node_[LEFT]);
			load_to_A(root->_node_[LEFT]);
			printf("\tJAZ LLOOP%d\n", loop_counter_tmp2);
			
			eval(root->_node_[RIGHT]);
			printf("\tJMP LLOOP%d\n", loop_counter_tmp);
			// May need nop
			printf("LLOOP%d\tNOP\n", loop_counter_tmp2); 

			loop_counter -= 2;
			break;
		
		case _BREAK:
			// The last loop counter point to the end of the loop
			printf("\tJMP LLOOP%d\n", loop_counter);
			break;

		case _CONTINUE:
			printf("\tJMP LLOOP%d\n", loop_counter-1);
			break;
		
		case _FOR:
			loop_counter_tmp = ++general_loop_counter; // start
			loop_counter_tmp2 = ++general_loop_counter; // do the ++ continue
			loop_counter_tmp3 = ++general_loop_counter; // end
			loop_counter = general_loop_counter;

			eval(root->_node_[LEFT]);
			load_to_A(root->_node_[LEFT]);

			printf("LLOOP%d", loop_counter_tmp);

			eval(root->_node_[MID]);
			load_to_A(root->_node_[MID]);
			printf("\tJAZ LLOOP%d\n", loop_counter_tmp3);

			eval(root->_node_[RIGHT]); // BLOCK

			printf("LLOOP%d", loop_counter_tmp2); // continue goes here
			eval(root->_node_[MID2]); // do the ++

			printf("\tJMP LLOOP%d\n", loop_counter_tmp);
			printf("LLOOP%d\tNOP\n", loop_counter_tmp3); // break or exit

			loop_counter -= 3;
			break;

		case PRINT:
			eval(root->_node_[RIGHT]);
			load_to_A(root->_node_[RIGHT]);
			// 40="." 41="," 42=( 43=) 44=+ 45=- 
			printf("\tSTA TEMP\n");

			printf("\tENTX 45\n");
			printf("\tJAN *+2\n");
			printf("\tENTX 44\n");
			printf("\tSTX OUTBUFF\n");
			//printf("\tOUT BUFF(18)\n");
			//printf("\tJBUS *(18)\n");
			
			//Print number
			printf("\tLDA TEMP\n");
			printf("\tCHAR\n");
			printf("\tSTA OUTBUFF1\n");
			printf("\tSTX OUTBUFF2\n");
			printf("\tOUT OUTBUFF(19)\n");
			printf("\tJBUS *(19)\n");
			

		default:
			eval(root->_node_[LEFT]);
			eval(root->_node_[RIGHT]);
	}

	return NULL;

}


void check_null(void *check)
{
	if (check == NULL){
		printf("ERROR: Out of memory\n");
		exit(0);
	}
}