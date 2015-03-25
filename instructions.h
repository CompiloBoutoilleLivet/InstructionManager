#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "label.h"

/*
Idée : faire une sorte d'instruction manager
Celui ci contient une liste chainée des instructions émisent par
le parser. A partir de cette liste chainée, il est possible de générer
un fichier avec les instructions sous forme textuelle ou alors
les instructions directement sous forme de bytecode. On pourrait imaginer
interpréter directement la liste d'instructions, on aurait alors un
langage interprété.

Ca pourrait aussi faciliter les jump conditionels ou non

Je ne sais pas si y'a vraiment un intérêt mais c'est cool
*/

/*
LABEL_COND:
	condition
	blabla boucle

LABEL_FIN:

while(a==1){
	print(a);
}

LABEL_COND:
	a == 1 ?
	JMF LABEL_NON
	OUI
	JMP LABEL_COND
LABEL_NON:

*/

enum instr_type {
	ADD_INSTR,
	MUL_INSTR,
	SOU_INSTR,
	DIV_INSTR,
	COP_INSTR,
	AFC_INSTR,
	JMP_INSTR,
	JMF_INSTR,
	INF_INSTR,
	SUP_INSTR,
	EQU_INSTR,
	PRI_INSTR,
	LABEL_INSTR // virtual instruction
};

struct instr
{
	enum instr_type type;
	int *params;
	struct instr *next;
};

struct instr_manager
{
	unsigned int count;
	struct instr *first;
	struct instr *last;
	struct label_table *labels;
};

void instr_manager_init();
struct instr_manager *instr_manager_get();

void instr_manager_print_textual(int color);
void instr_manager_print_textual_no_color();
void instr_manager_print_textual_file(FILE *f, int color);
void instr_manager_print_textual_file_no_color(FILE *f);
void instr_manager_print_instr(struct instr *instr, int color);
void instr_manager_print_instr_no_color(struct instr *instr);
void instr_manager_print_instr_file(FILE *f, struct instr *instr, int color);
void instr_manager_print_instr_file_no_color(FILE *f, struct instr *instr);
void instr_emit_cop(int dest, int source);
void instr_emit_afc(int dest, int value);
void instr_emit_add(int dest, int op1, int op2);
void instr_emit_sou(int dest, int op1, int op2);
void instr_emit_mul(int dest, int op1, int op2);
void instr_emit_div(int dest, int op1, int op2);
void instr_emit_equ(int dest, int op1, int op2);
void instr_emit_inf(int dest, int op1, int op2);
void instr_emit_sup(int dest, int op1, int op2);
void instr_emit_pri(int what);
void instr_emit_jmf(int addr, int label);
void instr_emit_jmp(int label);
void instr_emit_label(int label);


#include "term_colors.h"
// Some define only for the colors :D
#define C_NUMBER(text) COLOR(text, CYAN)
#define C_ADDRESS(text) COLOR(text, BRIGHT_GREEN)
#define C_OPERATOR(text) COLOR(text, YELLOW)
#define C_LABEL(text) COLOR(text, RED)

#endif
