#include <stdio.h>
#include <stdlib.h>
#include "../instructions.h"
#include "../label.h"

int main(int argc, char const *argv[])
{
	int label_ref;

	instr_manager_init();
	
	label_ref = label_add("main");

	instr_emit_label(label_ref);
	instr_emit_afc(1, 10);
	instr_emit_afc(2, 10);
	instr_emit_add(1, 1, 2);
	instr_emit_pri(1);
	instr_emit_jmp(label_ref);

	instr_manager_print_textual(1);
	return 0;
}
