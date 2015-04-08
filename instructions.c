#include <stdio.h>
#include <stdlib.h>
#include "label.h"
#include "instructions.h"

struct instr_manager *instr_manager = NULL;

void instr_manager_init()
{
	instr_manager = malloc(sizeof(struct instr_manager));
	if(instr_manager != NULL)
	{
		instr_manager->count = 0;
		instr_manager->resolved = 0;
		instr_manager->first = NULL;
		instr_manager->last = NULL;
		label_table_init(255);
	}
}

char *instr_int_to_reg(int reg)
{
	switch(reg)
	{
		case EBP_REG:
			return "ebp";
			break;

		case ESP_REG:
			return "esp";
			break;

		default:
			return "UNKNOWN";
			break;

	}
}

struct instr_manager *instr_manager_get()
{
	return instr_manager;
}

void instr_manager_print_textual(int color)
{
	instr_manager_print_textual_file(stdout, color);
}

void instr_manager_print_textual_no_color()
{
	instr_manager_print_textual(0);
}

void instr_manager_print_textual_file(FILE *f, int color)
{
	struct instr *instr = NULL;
	if(instr_manager != NULL)
	{
		instr = instr_manager->first;
		while(instr != NULL)
		{
			instr_manager_print_instr_file(f, instr, color);
			instr = instr->next;
		}
	}
}

void instr_manager_print_textual_file_no_color(FILE *f)
{
	instr_manager_print_textual_file(f, 0);
}

void instr_manager_print_instr(struct instr *instr, int color)
{
	instr_manager_print_instr_file(stdout, instr, color);	
}

void instr_manager_print_instr_no_color(struct instr *instr)
{
	instr_manager_print_instr(instr, 0);	
}


void instr_manager_print_instr_file_no_color(FILE *f, struct instr *instr)
{
	instr_manager_print_instr_file(f, instr, 0);
}

void instr_manager_print_instr_file(FILE *f, struct instr *instr, int color)
{
	switch(instr->type)
	{

		case COP_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("cop") " " C_ADDRESS("%d") ", " C_ADDRESS("%d") "\n", instr->params[0], instr->params[1]);
			} else {
				fprintf(f, "\tcop [$%d], [$%d]\n", instr->params[0], instr->params[1]);
			}
			
			break;

		case COP_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("cop") " " C_REGISTER("%s") ", " C_REGISTER("%s") "\n", instr_int_to_reg(instr->params[0]), instr_int_to_reg(instr->params[1]));
			} else {
				fprintf(f, "\tcop %s, %s\n", instr_int_to_reg(instr->params[0]), instr_int_to_reg(instr->params[1]));
			}
			break;

		case AFC_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("afc") " " C_ADDRESS("%d") ", " C_NUMBER("%d") "\n", instr->params[0], instr->params[1]);
			} else {
				fprintf(f, "\tafc [$%d], %d\n", instr->params[0], instr->params[1]);
			}
			break;

		case AFC_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("afc") " " C_REGISTER("%s") ", " C_NUMBER("%d") "\n", instr_int_to_reg(instr->params[0]), instr->params[1]);
			} else {
				fprintf(f, "\tafc %s, %d\n", instr_int_to_reg(instr->params[0]), instr->params[1]);
			}
			break;

		case ADD_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("add") " " C_ADDRESS("%d") ", " C_ADDRESS("%d") ", " C_ADDRESS("%d") "\n", instr->params[0], instr->params[1], instr->params[2]);
			} else {
				fprintf(f, "\tadd [$%d], [$%d], [$%d]\n", instr->params[0], instr->params[1], instr->params[2]);
			}
			break;

		case ADD_REG_VAL_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("add") " " C_REGISTER("%s") ", " C_REGISTER("%s") ", " C_NUMBER("%d") "\n", instr_int_to_reg(instr->params[0]), instr_int_to_reg(instr->params[1]), instr->params[2]);
			} else {
				fprintf(f, "\tadd %s, %s, %d\n", instr_int_to_reg(instr->params[0]), instr_int_to_reg(instr->params[1]), instr->params[2]);
			}
			break;

		case SOU_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("sou") " " C_ADDRESS("%d") ", " C_ADDRESS("%d") ", " C_ADDRESS("%d") "\n", instr->params[0], instr->params[1], instr->params[2]);
			} else {
				fprintf(f, "\tsou [$%d], [$%d], [$%d]\n", instr->params[0], instr->params[1], instr->params[2]);
			}
			break;

		case MUL_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("mul") " " C_ADDRESS("%d") ", " C_ADDRESS("%d") ", " C_ADDRESS("%d") "\n", instr->params[0], instr->params[1], instr->params[2]);
			} else {
				fprintf(f, "\tmul [$%d], [$%d], [$%d]\n", instr->params[0], instr->params[1], instr->params[2]);
			}
			break;

		case DIV_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("div") " " C_ADDRESS("%d") ", " C_ADDRESS("%d") ", " C_ADDRESS("%d") "\n", instr->params[0], instr->params[1], instr->params[2]);
			} else {
				fprintf(f, "\tdiv [$%d], [$%d], [$%d]\n", instr->params[0], instr->params[1], instr->params[2]);
			}
			break;

		case EQU_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("equ") " " C_ADDRESS("%d") ", " C_ADDRESS("%d") ", " C_ADDRESS("%d") "\n", instr->params[0], instr->params[1], instr->params[2]);
			} else {
				fprintf(f, "\tequ [$%d], [$%d], [$%d]\n", instr->params[0], instr->params[1], instr->params[2]);
			}
			break;

		case INF_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("inf") " " C_ADDRESS("%d") "], " C_ADDRESS("%d") "], " C_ADDRESS("%d") "\n", instr->params[0], instr->params[1], instr->params[2]);
			} else {
				fprintf(f, "\tinf [$%d], [$%d], [$%d]\n", instr->params[0], instr->params[1], instr->params[2]);
			}
			break;

		case SUP_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("sup") " " C_ADDRESS("%d") ", " C_ADDRESS("%d") ", " C_ADDRESS("%d") "\n", instr->params[0], instr->params[1], instr->params[2]);
			} else {
				fprintf(f, "\tsup [$%d], [$%d], [$%d]\n", instr->params[0], instr->params[1], instr->params[2]);
			}
			break;

		case PRI_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("pri") " " C_ADDRESS("%d") "\n", instr->params[0]);
			} else {
				fprintf(f, "\tpri [$%d]\n", instr->params[0]);
			}
			break;

		case JMP_INSTR:
			if(instr_manager->resolved)
			{
				if(color)
				{
					fprintf(f, "\t" C_OPERATOR("jmp") " " C_NUMBER("%d") "\n", instr->params[0]);
				} else {
					fprintf(f, "\tjmp %d\n", instr->params[0]);
				}
			} else {
				if(color)
				{
					fprintf(f, "\t" C_OPERATOR("jmp") " " C_LABEL("%s") "\n", label_table_get_label(instr->params[0])->name);
				} else {
					fprintf(f, "\tjmp %s\n", label_table_get_label(instr->params[0])->name);
				}
			}
			
			break;

		case JMF_INSTR:
			if(instr_manager->resolved)
			{
				if(color)
				{
					fprintf(f, "\t" C_OPERATOR("jmf") " " C_ADDRESS("%d") ", " C_NUMBER("%d") "\n", instr->params[0], instr->params[1]);
				} else {
					fprintf(f, "\tjmf [$%d], %d\n", instr->params[0], instr->params[1]);
				}
			} else {
				if(color)
				{
					fprintf(f, "\t" C_OPERATOR("jmf") " " C_ADDRESS("%d") ", " C_LABEL("%s") "\n", instr->params[0], label_table_get_label(instr->params[1])->name);
				} else {
					fprintf(f, "\tjmf [$%d], %s\n", instr->params[0], label_table_get_label(instr->params[1])->name);
				}
			}
			break;

		case LABEL_INSTR:
			if(!instr_manager->resolved)
			{
				if(color)
				{
					fprintf(f, C_LABEL("%s") ":\n", label_table_get_label(instr->params[0])->name);
				} else {
					fprintf(f, "%s:\n", label_table_get_label(instr->params[0])->name);
				}
			}
			break;

		case CALL_INSTR:
			if(!instr_manager->resolved)
			{
				if(color)
				{
					fprintf(f, "\t" C_OPERATOR("call") " " C_LABEL("%s") "\n", label_table_get_label(instr->params[0])->name);
				} else {
					fprintf(f, "\tcall %s\n", label_table_get_label(instr->params[0])->name);
				}
			} else {
				if(color)
				{
					fprintf(f, "\t" C_OPERATOR("call") " " C_NUMBER("%d") "\n", instr->params[0]);
				} else {
					fprintf(f, "\tcall %d\n", instr->params[0]);
				}
			}
			break;

		case PUSH_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("push") " " C_NUMBER("%d") "\n", instr->params[0]);
			} else {
				fprintf(f, "\tpush %d\n", instr->params[0]);
			}
			break;

		case PUSH_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("push") " " C_REGISTER("%s") "\n", instr_int_to_reg(instr->params[0]));
			} else {
				fprintf(f, "\tpush %s\n", instr_int_to_reg(instr->params[0]));
			}
			break;

		case POP_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("pop") " " C_ADDRESS("%d") "\n", instr->params[0]);
			} else {
				fprintf(f, "\tpush [$%d]\n", instr->params[0]);
			}
			break;

		case STOP_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("stop") "\n");
			} else {
				fprintf(f, "\tstop\n");
			}
			break;

		case LEAVE_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("leave") "\n");
			} else {
				fprintf(f, "\t" "leave\n");
			}
			break;

		case RET_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("ret") "\n");
			} else {
				fprintf(f, "\tret\n");
			}
			break;
	}
}

struct instr *instr_init_instr(enum instr_type type, int psize)
{
	struct instr *ret = NULL;

	ret = malloc(sizeof(struct instr));
	if(ret != NULL)
	{
		ret->type = type;
		ret->params = malloc(sizeof(int) * psize);
		if(ret->params == NULL)
		{
			free(ret);
			ret = NULL;
		}
	}

	return ret;
}

void instr_emit_instr(struct instr *instr)
{
	if(instr_manager != NULL)
	{
		instr->next = NULL;

		if(instr->type != LABEL_INSTR)
		{
			instr->instr_number = instr_manager->count++;
		}

		if(instr_manager->first == NULL)
		{
			instr_manager->first = instr;
			instr_manager->last = instr;
		} else {
			instr_manager->last->next = instr;
			instr_manager->last = instr;
		}
	}
}

void instr_emit_cop(int dest, int source)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(COP_INSTR, 2)) != NULL)
	{
		instr->params[0] = dest;
		instr->params[1] = source;
		instr_emit_instr(instr);
	}
}

void instr_emit_cop_reg(int reg1, int reg2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(COP_REG_INSTR, 2)) != NULL)
	{
		instr->params[0] = reg1;
		instr->params[1] = reg2;
		instr_emit_instr(instr);
	}
}

void instr_emit_afc(int dest, int value)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(AFC_INSTR, 2)) != NULL)
	{
		instr->params[0] = dest;
		instr->params[1] = value;
		instr_emit_instr(instr);
	}
}

void instr_emit_afc_reg(int reg, int value)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(AFC_REG_INSTR, 2)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = value;
		instr_emit_instr(instr);
	}
}

void instr_emit_add(int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(ADD_INSTR, 3)) != NULL)
	{
		instr->params[0] = dest;
		instr->params[1] = op1;
		instr->params[2] = op2;
		instr_emit_instr(instr);
	}
}

void instr_emit_add_reg_val(int reg_dst, int reg_src, int val)
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(ADD_REG_VAL_INSTR, 3)) != NULL)
	{
		instr->params[0] = reg_dst;
		instr->params[1] = reg_src;
		instr->params[2] = val;
		instr_emit_instr(instr);
	}
}

void instr_emit_sou(int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(SOU_INSTR, 3)) != NULL)
	{
		instr->params[0] = dest;
		instr->params[1] = op1;
		instr->params[2] = op2;
		instr_emit_instr(instr);
	}
}

void instr_emit_mul(int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(MUL_INSTR, 3)) != NULL)
	{
		instr->params[0] = dest;
		instr->params[1] = op1;
		instr->params[2] = op2;
		instr_emit_instr(instr);
	}
}

void instr_emit_div(int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(DIV_INSTR, 3)) != NULL)
	{
		instr->params[0] = dest;
		instr->params[1] = op1;
		instr->params[2] = op2;
		instr_emit_instr(instr);
	}
}

void instr_emit_pri(int what)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(PRI_INSTR, 1)) != NULL)
	{
		instr->params[0] = what;
		instr_emit_instr(instr);
	}
}

void instr_emit_equ(int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(EQU_INSTR, 3)) != NULL)
	{
		instr->params[0] = dest;
		instr->params[1] = op1;
		instr->params[2] = op2;
		instr_emit_instr(instr);
	}
}

void instr_emit_sup(int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(SUP_INSTR, 3)) != NULL)
	{
		instr->params[0] = dest;
		instr->params[1] = op1;
		instr->params[2] = op2;
		instr_emit_instr(instr);
	}
}

void instr_emit_inf(int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(INF_INSTR, 3)) != NULL)
	{
		instr->params[0] = dest;
		instr->params[1] = op1;
		instr->params[2] = op2;
		instr_emit_instr(instr);
	}
}

void instr_emit_jmf(int addr, int label)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(JMF_INSTR, 2)) != NULL)
	{
		instr->params[0] = addr;
		instr->params[1] = label;
		instr_emit_instr(instr);
	}
}

void instr_emit_jmp(int label)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(JMP_INSTR, 1)) != NULL)
	{
		instr->params[0] = label;
		instr_emit_instr(instr);
	}
}

void instr_emit_stop()
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(STOP_INSTR, 0)) != NULL)
	{
		instr_emit_instr(instr);
		instr->next = NULL;
	}
}

void instr_emit_label(int label)
{
	struct instr *instr = NULL;
	struct label *l = NULL;

	if((instr = instr_init_instr(LABEL_INSTR, 1)) != NULL)
	{
		instr->params[0] = label;
		instr_emit_instr(instr);
		l = label_table_get_label(label);
		l->instr = instr;
	}
}

void instr_emit_call(int symbol)
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(CALL_INSTR, 1)) != NULL)
	{
		instr->params[0] = symbol;
		instr_emit_instr(instr);
	}
}

void instr_emit_leave()
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(LEAVE_INSTR, 0)) != NULL)
	{
		instr_emit_instr(instr);
	}
}

void instr_emit_ret()
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(RET_INSTR, 0)) != NULL)
	{
		instr_emit_instr(instr);
	}
}

void instr_emit_push(int value)
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(PUSH_INSTR, 1)) != NULL)
	{
		instr->params[0] = value;
		instr_emit_instr(instr);
	}
}

void instr_emit_push_reg(int reg)
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(PUSH_REG_INSTR, 1)) != NULL)
	{
		instr->params[0] = reg;
		instr_emit_instr(instr);
	}
}

void instr_emit_pop(int addr)
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(POP_INSTR, 1)) != NULL)
	{
		instr->params[0] = addr;
		instr_emit_instr(instr);
	}
}

int instr_manager_check_calls()
{
	int ret = 0;
	struct instr *instr = NULL;

	if(instr_manager != NULL)
	{
		instr = instr_manager->first;
		while(instr != NULL)
		{
			if(instr->type == CALL_INSTR)
			{
				if(label_table_get_label(instr->params[0]) == NULL)
				{
					printf("call unknown functions !\n");
					ret++;
				}
			}

			instr = instr->next;
		}
	}
	return ret;
}

void instr_manager_resolve_label_next()
{
	int i = 0;
	struct label *label = NULL;
	struct instr *instr = NULL;

	for(i=0; i<label_table_get_size(); i++)
	{
		if((label = label_table_get_label(i)) != NULL)
		{
			instr = label->instr;
			while(instr != NULL && instr->type == LABEL_INSTR)
			{
				instr = instr->next;
			}
			label->instr = instr;
		}
	}

}

void instr_manager_resolve_jumps()
{
	struct instr *instr = NULL;
	struct label *label = NULL;

	if(instr_manager != NULL)
	{
		instr_manager_resolve_label_next();

		instr = instr_manager->first;
				instr_manager->resolved = 1;


		while(instr != NULL)
		{
			label = NULL;

			if(instr->type == JMP_INSTR)
			{
				label = label_table_get_label(instr->params[0]);
				instr->params[0] = label->instr->instr_number - instr->instr_number;
			}

			if(instr->type == JMF_INSTR)
			{
				label = label_table_get_label(instr->params[1]);
				instr->params[1] = label->instr->instr_number - instr->instr_number;
			}

			if(instr->type == CALL_INSTR)
			{
				label = label_table_get_label(instr->params[0]);
				instr->params[0] = label->instr->instr_number - instr->instr_number;
			}

			instr = instr->next;
		}

	}	
}
