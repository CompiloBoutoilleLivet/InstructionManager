#ifndef LABEL_H
#define LABEL_H

struct label
{
	char *name;
};

struct label_table
{
	unsigned int count;
	unsigned int size;
	struct label **labels;
};

struct label_table *label_table_init(int size);

#endif