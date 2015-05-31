#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "label.h"

/*

afc esp, 0
afc ebp, 0
call boubi

push ebp
afc ebp, esp
....
leave
ret


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

#define BP_REG 0
#define SP_REG 1
#define RT_REG 2

char *instr_int_to_reg(int reg);
int instr_reg_to_int(char *name);

enum instr_type {
	ADD_INSTR = 0x01,
	MUL_INSTR = 0x02,
	SOU_INSTR = 0x03,
	DIV_INSTR = 0x04,
	COP_INSTR = 0x05,
	AFC_INSTR = 0x06,
	JMP_INSTR = 0x07,
	JMF_INSTR = 0x08,
	INF_INSTR = 0x09,
	SUP_INSTR = 0x0A,
	EQU_INSTR = 0x0B,
	PRI_INSTR = 0x0C,
	STOP_INSTR = 0x0D,
	ADD_REG_VAL_INSTR,
	SOU_REG_VAL_INSTR,
	MUL_REG_VAL_INSTR,
	DIV_REG_VAL_INSTR,
	ADD_REL_REG_INSTR,
	SOU_REL_REG_INSTR,
	MUL_REL_REG_INSTR,
	DIV_REL_REG_INSTR,
	COP_REG_INSTR,
	COP_REL_REG_INSTR,
	AFC_REG_INSTR,
	AFC_REG_MEM_INSTR,
	AFC_MEM_REG_INSTR,
	AFC_REL_REG_INSTR,
	JMF_REL_REG_INSTR,
	EQU_REL_REG_INSTR,
	INF_REL_REG_INSTR,
	SUP_REL_REG_INSTR,
	CALL_INSTR,
	PUSH_INSTR,
	PUSH_REG_INSTR,
	PUSH_REL_REG_INSTR,
	POP_INSTR,
	LEAVE_INSTR,
	RET_INSTR,
	PRI_REL_REG_INSTR,
	LABEL_INSTR // virtual instruction
};

struct instr
{
	enum instr_type type;
	int *params;
	int instr_number;
	struct instr *next;
};

struct instr_manager
{
	unsigned int count;
	int resolved;
	struct instr *first;
	struct instr *last;
	struct label_table *labels;
};

void instr_manager_init();
struct instr_manager *instr_manager_get();
struct instr *instr_manager_get_last_instr();
void instr_manager_print_bytecode_file(FILE *f);
void instr_manager_print_textual(int color);
void instr_manager_print_textual_no_color();
void instr_manager_print_textual_file(FILE *f, int color);
void instr_manager_print_textual_file_no_color(FILE *f);
void instr_manager_print_instr(struct instr *instr, int color);
void instr_manager_print_instr_no_color(struct instr *instr);
void instr_manager_print_instr_file(FILE *f, struct instr *instr, int color);
void instr_manager_print_instr_file_no_color(FILE *f, struct instr *instr);
void instr_manager_print_bytecode_instr_file(FILE *f, struct instr *instr);
void instr_insert_instr(struct instr *parent, struct instr *child);
void instr_emit_cop(int dest, int source);
void instr_emit_cop_reg(int dest, int source);
void instr_emit_cop_rel_reg(int reg_dest, int dest, int reg_source, int source);
void instr_emit_afc(int dest, int value);
void instr_emit_afc_reg_mem(int reg, int reg2, int off);
void instr_emit_afc_reg(int reg, int value);
void instr_emit_afc_rel_reg(int reg, int offset, int value);
void instr_emit_afc_mem_reg(int reg, int off, int reg2);
void instr_emit_add(int dest, int op1, int op2);
void instr_emit_add_reg_val(int dest, int src, int val);
void instr_emit_add_rel_reg(int reg, int dest, int op1, int op2);
void instr_insert_add_reg_val(struct instr *parent, int dest, int src, int val);
void instr_emit_sou(int dest, int op1, int op2);
void instr_emit_sou_reg_val(int dest, int src, int val);
void instr_emit_sou_rel_reg(int reg, int dest, int op1, int op2);
void instr_emit_mul(int dest, int op1, int op2);
void instr_emit_mul_reg_val(int dest, int src, int val);
void instr_emit_mul_rel_reg(int reg, int dest, int op1, int op2);
void instr_emit_div(int dest, int op1, int op2);
void instr_emit_div_reg_val(int dest, int src, int val);
void instr_emit_div_rel_reg(int reg, int dest, int op1, int op2);
void instr_emit_equ(int dest, int op1, int op2);
void instr_emit_equ_rel_reg(int reg, int dest, int op1, int op2);
void instr_emit_inf(int dest, int op1, int op2);
void instr_emit_inf_rel_reg(int reg, int dest, int op1, int op2);
void instr_emit_sup(int dest, int op1, int op2);
void instr_emit_sup_rel_reg(int reg, int dest, int op1, int op2);
void instr_emit_pri(int what);
void instr_emit_pri_rel_reg(int reg, int off);
void instr_emit_jmf(int addr, int label);
void instr_emit_jmf_rel_reg(int reg, int addr, int label);
void instr_emit_jmp(int label);
void instr_emit_label(int label);
void instr_emit_stop();
void instr_emit_call(int label);
void instr_emit_push(int value);
void instr_emit_push_reg(int reg);
void instr_emit_push_rel_reg(int reg, int off);
void instr_emit_pop(int value);
void instr_emit_leave();
void instr_emit_ret();

int instr_manager_check_calls();
void instr_manager_resolve_jumps();

#include "term_colors.h"
// Some define only for the colors :D
#define C_NUMBER(text) COLOR(text, CYAN)
#define C_NUMBER_OFFSET(text) COLOR(text, BRIGHT_GREEN)
#define C_ADDRESS(text) "[$" COLOR(text, BRIGHT_GREEN) "]"
#define C_OPERATOR(text) COLOR(text, YELLOW)
#define C_LABEL(text) COLOR(text, RED)
#define C_REGISTER(text) COLOR(text, BRIGHT_BLUE)

#endif
