#ifndef LABEL_H
#define LABEL_H

struct label
{
	char *name;
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

void label_table_init(int size);
struct label *label_table_get_label(int i);
int label_table_hash_string(char *name);
struct label *label_get_next_tmp_label();
int label_add(char *name);

#endif