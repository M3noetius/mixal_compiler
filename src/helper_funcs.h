// LEFT - MID - MID2 - RIGHT
#define LEFT 0
#define RIGHT 1
#define MID 2
#define MID2 3


// Define types
#define ADD 0
#define SUB	1
#define MUL 2
#define DIV 3
#define MOD 4
#define SMALLER 5
#define BIGGER 6
#define SMALLER_EQU 7
#define	BIGGER_EQU	8
#define EQU 9
#define NOT_EQU 10
#define AND 11
#define OR 12

#define CONSTANT 13
#define IN_TEMP_MEMORY 14
#define ID 15

#define PROGRAM 16
#define DECLS 17
#define DECL 18
#define STATEMENT 19
#define STATEMENTS 20

#define EQU_VAR 21
#define MUL_EQU 22
#define DIV_EQU 23
#define ADD_EQU 24
#define SUB_EQU 25
#define MOD_EQU 26

#define PRINT 27
#define EOC_ONLY 28

#define NOT 29
#define NEGATIVE 30

#define BLOCK 31
#define ELSE_BLC 32
#define _IF 33
#define _WHILE 34
#define _FOR 35
#define _BREAK 36
#define _CONTINUE 37

extern const char *node_types[];
extern const char *opcodes_jumps[];
extern int check_declare;
extern int stack_top ;

extern int loop_counter;
extern int general_loop_counter;
extern int if_counter;
extern int and_counter;
extern int or_counter;



typedef struct _node{
	int type;
	int value;
	char *symbol;
	struct _node *_node_[4];
} node;


node* print_tree(node *, int);

node* newnode (int , node *, node *, node *, node *, int , char *);
node* newnode_2(int , node *, node *);
node* newnode_3(int , node *, node *, node *);
node* newnode_4(int , node *, node *, node *, node *);
node* newnode_num(int );
node* newnode_symbol(node *, node *, char *);

void constant_load_big_number(int );

int eval(node *);
void generate_code(node *);
void generate_instr(node *ptr, const char *op1, const char *op2);
void check_overflow(char );
void check_null(void *);
