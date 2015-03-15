#ifndef LABEL_H
#define LABEL_H

struct label
{
	char *name;
	char is_emitted;
	int numero;
	struct instr *instr;
};

struct label_table
{
	unsigned int size;
	unsigned int count;
	struct label **labels;
	unsigned int tmp_count;
	char **tmp_labels;
};

struct label_table *label_table_init(int size);
struct label *label_get_next_tmp_label();

#endif