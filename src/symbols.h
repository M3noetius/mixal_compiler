#define bool char


extern int varible_counter;

typedef struct _symbol_data{
	int id;
	char is_set;
	char *var_name;	
	struct _symbol_data *fd;
}symbol_data;

extern symbol_data **symbolTable;

void gen_variable(node *);
void addSymbol(char *);
int getSymbolId(char *);
symbol_data *new_symbol_node(int id, char *input);
symbol_data* search_symbol_node(char *input);
void print_symbol_table();