#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
		case BP_REG:
			return "bp";
			break;

		case SP_REG:
			return "sp";
			break;

		case RT_REG:
			return "rt";
			break;

		default:
			return "UNKNOWN";
			break;

	}
}

int instr_reg_to_int(char *name)
{
	if(strcmp(name, "bp") == 0)
	{
		return BP_REG;
	} else if(strcmp(name, "sp") == 0)
	{
		return SP_REG;
	} else if(strcmp(name, "rt") == 0)
	{
		return RT_REG;
	} else {
		return -1;
	}
}

struct instr_manager *instr_manager_get()
{
	return instr_manager;
}

struct instr *instr_manager_get_last_instr()
{
	return instr_manager->last;
}

void instr_manager_print_bytecode_file(FILE *f)
{

	struct instr *instr = NULL;
	if(instr_manager != NULL)
	{

		/* we need resolved jumps in order to generate bytecode */
		if(instr_manager->resolved != 1)
		{
			instr_manager_resolve_jumps();
		}

		instr = instr_manager->first;
		while(instr != NULL)
		{
			instr_manager_print_bytecode_instr_file(f, instr);
			instr = instr->next;
		}

	}

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

void instr_manager_print_bytecode_instr_file(FILE *f, struct instr *instr)
{
	int op0 = 0;
	int op1 = 0;
	int op2 = 0;

	if(instr->type != LABEL_INSTR)
	{

		switch(instr->type)
		{

			case ADD_INSTR:
			case MUL_INSTR:
			case SOU_INSTR:
			case DIV_INSTR:
			case INF_INSTR:
			case SUP_INSTR:
			case EQU_INSTR:
				op0 = instr->params[0];
				op1 = instr->params[1];
				op2 = instr->params[2];
			break;

			case COP_INSTR:
			case AFC_INSTR:
				op0 = instr->params[0];
				op1 = instr->params[1];
			break;

			case JMF_INSTR:
				op0 = instr->params[0];
				op1 = instr->params[1];
				op2 = 0; // on saute plus bas

				if(op1 < 0)
				{
					op1 = -1 * op1;
					op2 = 1; //on saute plus haut
				}

			break;

			case JMP_INSTR:

				op0 = instr->params[0];
				op1 = 0; // on saute plus bas

				if(op0 < 0)
				{
					op0 = -1 * op0;
					op1 = 1; // saute plus haut
				}

			break;

			case PRI_INSTR:
				op0 = instr->params[0];
			break;

			case STOP_INSTR:
			break;

			default:
				printf("Instruction cannot be transformed as opcode :(\n");
				exit(EXIT_FAILURE);
				break;

		}

		fprintf(f, "%d %d %d %d\n", instr->type, op0, op1, op2);
	}
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

		case COP_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("cop") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[2]),
					instr->params[3]);
			} else {
				fprintf(f, "\tcop [%s+%d], [%s+%d]\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[2]),
					instr->params[3]);
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

		case AFC_REG_MEM_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("afc") " " C_REGISTER("%s") ", [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n",
					instr_int_to_reg(instr->params[0]),
					instr_int_to_reg(instr->params[1]),
					instr->params[2]);
			} else {
				fprintf(f, "\tafc %s, [%s+%d]\n",
					instr_int_to_reg(instr->params[0]),
					instr_int_to_reg(instr->params[1]),
					instr->params[2]);
			}
			break;

		case AFC_MEM_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("afc") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], " C_REGISTER("%s") "\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[2]));
			} else {
				fprintf(f, "\tafc [%s+%d], %s\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[2]));
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

		case AFC_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("afc") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], " C_NUMBER("%d") " \n",
					instr_int_to_reg(instr->params[0]), instr->params[1], instr->params[2]);
			} else {
				fprintf(f, "\tafc [%s+%d], %d\n", instr_int_to_reg(instr->params[0]), instr->params[1], instr->params[2]);
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

		case ADD_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("add") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[0]),
					instr->params[2],
					instr_int_to_reg(instr->params[0]),
					instr->params[3]
				);
			} else {
				fprintf(f, "\tadd [%s+%d], [%s+%d], [%s+%d]\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[0]),
					instr->params[2],
					instr_int_to_reg(instr->params[0]),
					instr->params[3]
				);
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

		case SOU_REG_VAL_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("sou") " " C_REGISTER("%s") ", " C_REGISTER("%s") ", " C_NUMBER("%d") "\n", instr_int_to_reg(instr->params[0]), instr_int_to_reg(instr->params[1]), instr->params[2]);
			} else {
				fprintf(f, "\tsou %s, %s, %d\n", instr_int_to_reg(instr->params[0]), instr_int_to_reg(instr->params[1]), instr->params[2]);
			}
			break;

		case SOU_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("sou") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[0]),
					instr->params[2],
					instr_int_to_reg(instr->params[0]),
					instr->params[3]
				);
			} else {
				fprintf(f, "\tsou [%s+%d], [%s+%d], [%s+%d]\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[0]),
					instr->params[2],
					instr_int_to_reg(instr->params[0]),
					instr->params[3]
				);
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

		case MUL_REG_VAL_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("mul") " " C_REGISTER("%s") ", " C_REGISTER("%s") ", " C_NUMBER("%d") "\n", instr_int_to_reg(instr->params[0]), instr_int_to_reg(instr->params[1]), instr->params[2]);
			} else {
				fprintf(f, "\tmul %s, %s, %d\n", instr_int_to_reg(instr->params[0]), instr_int_to_reg(instr->params[1]), instr->params[2]);
			}
			break;

		case MUL_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("mul") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[0]),
					instr->params[2],
					instr_int_to_reg(instr->params[0]),
					instr->params[3]
				);
			} else {
				fprintf(f, "\tmul [%s+%d], [%s+%d], [%s+%d]\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[0]),
					instr->params[2],
					instr_int_to_reg(instr->params[0]),
					instr->params[3]
				);
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

		case DIV_REG_VAL_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("div") " " C_REGISTER("%s") ", " C_REGISTER("%s") ", " C_NUMBER("%d") "\n", instr_int_to_reg(instr->params[0]), instr_int_to_reg(instr->params[1]), instr->params[2]);
			} else {
				fprintf(f, "\tdiv %s, %s, %d\n", instr_int_to_reg(instr->params[0]), instr_int_to_reg(instr->params[1]), instr->params[2]);
			}
			break;

		case DIV_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("div") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[0]),
					instr->params[2],
					instr_int_to_reg(instr->params[0]),
					instr->params[3]
				);
			} else {
				fprintf(f, "\tdiv [%s+%d], [%s+%d], [%s+%d]\n",
					instr_int_to_reg(instr->params[0]),
					instr->params[1],
					instr_int_to_reg(instr->params[0]),
					instr->params[2],
					instr_int_to_reg(instr->params[0]),
					instr->params[3]
				);
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

		case EQU_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("equ") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n",
				instr_int_to_reg(instr->params[0]),
				instr->params[1],
				instr_int_to_reg(instr->params[0]),
				instr->params[2],
				instr_int_to_reg(instr->params[0]),
				instr->params[3]);
			} else {
				fprintf(f, "\tequ [%s+%d], [%s+%d], [%s+%d]\n",
				instr_int_to_reg(instr->params[0]),
				instr->params[1],
				instr_int_to_reg(instr->params[0]),
				instr->params[2],
				instr_int_to_reg(instr->params[0]),
				instr->params[3]);
			}
			break;

		case INF_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("inf") " " C_ADDRESS("%d") ", " C_ADDRESS("%d") ", " C_ADDRESS("%d") "\n", instr->params[0], instr->params[1], instr->params[2]);
			} else {
				fprintf(f, "\tinf [$%d], [$%d], [$%d]\n", instr->params[0], instr->params[1], instr->params[2]);
			}
			break;

		case INF_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("inf") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n",
				instr_int_to_reg(instr->params[0]),
				instr->params[1],
				instr_int_to_reg(instr->params[0]),
				instr->params[2],
				instr_int_to_reg(instr->params[0]),
				instr->params[3]);
			} else {
				fprintf(f, "\tinf [%s+%d], [%s+%d], [%s+%d]\n",
				instr_int_to_reg(instr->params[0]),
				instr->params[1],
				instr_int_to_reg(instr->params[0]),
				instr->params[2],
				instr_int_to_reg(instr->params[0]),
				instr->params[3]);
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

		case SUP_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("sup") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n",
				instr_int_to_reg(instr->params[0]),
				instr->params[1],
				instr_int_to_reg(instr->params[0]),
				instr->params[2],
				instr_int_to_reg(instr->params[0]),
				instr->params[3]);
			} else {
				fprintf(f, "\tsup [%s+%d], [%s+%d], [%s+%d]\n",
				instr_int_to_reg(instr->params[0]),
				instr->params[1],
				instr_int_to_reg(instr->params[0]),
				instr->params[2],
				instr_int_to_reg(instr->params[0]),
				instr->params[3]);
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

		case PRI_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("pri") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n",
					instr_int_to_reg(instr->params[0]), instr->params[1]);
			} else {
				fprintf(f, "\tpri [%s+%d]\n", instr_int_to_reg(instr->params[0]), instr->params[1]);
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

		case JMF_REL_REG_INSTR:
			if(instr_manager->resolved)
			{
				if(color) {
					fprintf(f, "\t" C_OPERATOR("jmf") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], " C_NUMBER("%d") "\n",
						instr_int_to_reg(instr->params[0]),
						instr->params[1],
						instr->params[2]);
				} else {
					fprintf(f, "\tjmf [%s+%d], %d\n",
						instr_int_to_reg(instr->params[0]),
						instr->params[1],
						instr->params[2]);
				}
			} else {
				if(color) {
					fprintf(f, "\t" C_OPERATOR("jmf") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "], " C_LABEL("%s") "\n",
						instr_int_to_reg(instr->params[0]),
						instr->params[1],
						label_table_get_label(instr->params[2])->name);
				} else {
					fprintf(f, "\tjmf [%s+%d], %s\n",
						instr_int_to_reg(instr->params[0]),
						instr->params[1],
						label_table_get_label(instr->params[2])->name);
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

		case PUSH_REL_REG_INSTR:
			if(color)
			{
				fprintf(f, "\t" C_OPERATOR("push") " [" C_REGISTER("%s") "+" C_NUMBER_OFFSET("%d") "]\n", instr_int_to_reg(instr->params[0]), instr->params[1]);
			} else {
				fprintf(f, "\tpush [%s+%d]\n", instr_int_to_reg(instr->params[0]), instr->params[1]);
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

void instr_insert_instr(struct instr *parent, struct instr *child)
{
	if(instr_manager != NULL)
	{
		if(parent == instr_manager->last) // we juste have to call emit_instr ...
		{
			instr_emit_instr(child);
		} else {
			child->next = parent->next;
			parent->next = child;
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

void instr_emit_cop_rel_reg(int reg_dest, int dest, int reg_source, int source)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(COP_REL_REG_INSTR, 4)) != NULL)
	{
		instr->params[0] = reg_dest;
		instr->params[1] = dest;
		instr->params[2] = reg_source;
		instr->params[3] = source;
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

void instr_emit_afc_reg_mem(int reg, int reg2, int off)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(AFC_REG_MEM_INSTR, 3)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = reg2;
		instr->params[2] = off;
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

void instr_emit_afc_rel_reg(int reg, int off, int value)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(AFC_REL_REG_INSTR, 3)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = off;
		instr->params[2] = value;
		instr_emit_instr(instr);
	}
}

void instr_emit_afc_mem_reg(int reg, int off, int reg2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(AFC_MEM_REG_INSTR, 3)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = off;
		instr->params[2] = reg2;
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

void instr_emit_add_rel_reg(int reg, int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(ADD_REL_REG_INSTR, 4)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = dest;
		instr->params[2] = op1;
		instr->params[3] = op2;
		instr_emit_instr(instr);
	}
}

void instr_insert_add_reg_val(struct instr *parent, int reg_dst, int reg_src, int val)
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(ADD_REG_VAL_INSTR, 3)) != NULL)
	{
		instr->params[0] = reg_dst;
		instr->params[1] = reg_src;
		instr->params[2] = val;
		instr_insert_instr(parent, instr);
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

void instr_emit_sou_reg_val(int reg_dst, int reg_src, int val)
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(SOU_REG_VAL_INSTR, 3)) != NULL)
	{
		instr->params[0] = reg_dst;
		instr->params[1] = reg_src;
		instr->params[2] = val;
		instr_emit_instr(instr);
	}
}

void instr_emit_sou_rel_reg(int reg, int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(SOU_REL_REG_INSTR, 4)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = dest;
		instr->params[2] = op1;
		instr->params[3] = op2;
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

void instr_emit_mul_reg_val(int reg_dst, int reg_src, int val)
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(MUL_REG_VAL_INSTR, 3)) != NULL)
	{
		instr->params[0] = reg_dst;
		instr->params[1] = reg_src;
		instr->params[2] = val;
		instr_emit_instr(instr);
	}
}

void instr_emit_mul_rel_reg(int reg, int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(MUL_REL_REG_INSTR, 4)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = dest;
		instr->params[2] = op1;
		instr->params[3] = op2;
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

void instr_emit_div_reg_val(int reg_dst, int reg_src, int val)
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(DIV_REG_VAL_INSTR, 3)) != NULL)
	{
		instr->params[0] = reg_dst;
		instr->params[1] = reg_src;
		instr->params[2] = val;
		instr_emit_instr(instr);
	}
}

void instr_emit_div_rel_reg(int reg, int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(DIV_REL_REG_INSTR, 4)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = dest;
		instr->params[2] = op1;
		instr->params[3] = op2;
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

void instr_emit_pri_rel_reg(int reg, int off)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(PRI_REL_REG_INSTR, 2)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = off;
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

void instr_emit_equ_rel_reg(int reg, int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(EQU_REL_REG_INSTR, 4)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = dest;
		instr->params[2] = op1;
		instr->params[3] = op2;
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

void instr_emit_sup_rel_reg(int reg, int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(SUP_REL_REG_INSTR, 4)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = dest;
		instr->params[2] = op1;
		instr->params[3] = op2;
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

void instr_emit_inf_rel_reg(int reg, int dest, int op1, int op2)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(INF_REL_REG_INSTR, 4)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = dest;
		instr->params[2] = op1;
		instr->params[3] = op2;
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

void instr_emit_jmf_rel_reg(int reg, int addr, int label)
{
	struct instr *instr = NULL;
	if((instr = instr_init_instr(JMF_REL_REG_INSTR, 3)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = addr;
		instr->params[2] = label;
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

void instr_emit_push_rel_reg(int reg, int off)
{
	struct instr *instr = NULL;

	if((instr = instr_init_instr(PUSH_REL_REG_INSTR, 2)) != NULL)
	{
		instr->params[0] = reg;
		instr->params[1] = off;
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

			if(instr->type == JMF_REL_REG_INSTR)
			{
				label = label_table_get_label(instr->params[2]);
				instr->params[2] = label->instr->instr_number - instr->instr_number;
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
