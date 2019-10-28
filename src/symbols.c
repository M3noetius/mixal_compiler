#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper_funcs.h"
#include "symbols.h"


int variable_counter = 1;
symbol_data **symbolTable = NULL;


void gen_variable(node *this_node)
{
	printf("VAR%d\tEQU %d   %s\n", getSymbolId(this_node->symbol), 
		getSymbolId(this_node->symbol), this_node->symbol);
}


void initiallize_symbols()
{
	symbolTable =  calloc(1, sizeof(symbol_data) * ('z' - 'A') );
}


void
print_symbol_table(){
	for (int i = 'A'; i <= 'z'; ++i)
	{
		symbol_data *key = symbolTable['z' - i];
		printf("[%c]-> ", i);
		if (key == NULL){
			printf("NULL \n");
			continue;
		}
		
		do{
			printf("%s ", key->var_name);
			if (key->fd == NULL)
				break;
			
			key = key->fd;
		}while (1);
		
		printf("\n");
	}
	printf("\n");
}


symbol_data *  
search_symbol_node(char *input){
	symbol_data *key = symbolTable['z' - input[0]];
	if (key == NULL)
		return NULL;
	
	do{
		if ( strcmp(key->var_name, input) == 0 )
			return key;
		
		if (key->fd == NULL)
			return key;
		
		key = key->fd;
	}while (1);

}


// its a sinlge linked list
symbol_data *
new_symbol_node(int id, char *input){
	symbol_data  *to_ret = calloc(1, sizeof(symbol_data));
	
	to_ret->id = id;
	to_ret->var_name = input;
	to_ret->fd = NULL;
	//fd is null from calloc
	return to_ret;
}


void addSymbol(char *input)
{
	symbol_data *key = search_symbol_node(input); 

	if (key == NULL){
		symbolTable['z' - input[0]] = new_symbol_node(variable_counter, input);
		variable_counter++;
		return;
	}

	if (strcmp(key->var_name, input) != 0){
		key->fd =  new_symbol_node(variable_counter, input);
		variable_counter++;
	}else{
		printf("Multiple times declared variable %s\n", input);
		exit(0);
	}
}


int getSymbolId(char *symbol)
{
	symbol_data *key = search_symbol_node(symbol);
	if (!key || strcmp(symbol, key->var_name) != 0){
		yyerror("non declared variable\n");
		exit(0);
	}
	//printf("Variable %c has id %d\n", symbol, symbolTable['a' - symbol].id );
	return key->id;
}

