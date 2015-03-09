# InstructionManager

L'instruction manager est chargé de gérer une liste chainée des instructions du programme.

Différentes fonctions :
- ```void instr_manager_init()``` : initialiser l'instruction manager
- ```void instr_manager_print_textual()``` : afficher les instructions sous forme textuelles dans stdout
- ```void instr_manager_print_textual_file(FILE *f)``` : afficher les instructions sous forme textuelles dans le fichier ```f```
- ```void instr_emit_INSTR(...)``` : différentes fonctions pour générer les instructions voir https://github.com/CompiloBoutoilleLivet/InstructionManager/blob/master/instructions.h#L72

