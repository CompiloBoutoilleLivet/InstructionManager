#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "label.h"

int num_label = 0;
struct label_table *label_table = NULL;

void label_table_init(int size)
{
	int i;
	label_table = malloc(sizeof(struct label_table));
 
	if(label_table == NULL)
	{
		return;
	}

	label_table->labels = malloc(sizeof(struct label *) * size);
	if(label_table->labels == NULL)
	{
		free(label_table);
		return;
	}


	label_table->count = 0;
	label_table->size = size;
	for(i=0; i<size; i++)
	{
		label_table->labels[i] = NULL;
	}
}

int label_table_hash_string(char *name)
{
	int ret = 0;
	int i;

	for(i=0; i<strlen(name); i++)
	{
		ret += (int) name[i];
	}

	ret = ret%label_table->size;
	return ret;
}

int label_table_add_label(struct label *label)
{
	int ret = label_table_hash_string(label->name);
	if(label_table->labels[ret] != NULL)
	{
		printf("Label already exists ........\n");
		exit(-1);
	}
	label_table->labels[ret] = label;
	label_table->count++;
	return ret;
}

struct label *label_table_get_label(int i)
{
	return label_table->labels[i];
}

struct label *label_init()
{
	struct label *ret = malloc(sizeof(struct label));

	if(ret == NULL)
	{
		return NULL;
	}

	ret->name = malloc(sizeof(char)*32);
	if(ret->name == NULL)
	{
		free(ret);
		return NULL;
	}

	ret->instr = NULL;

	return ret;
}

int label_get_next_tmp_label()
{
	int ret;
	struct label *label = label_init();
	sprintf(label->name, "label_%04X", num_label);
	ret = label_table_add_label(label);
	num_label++;
	return ret;
}

int label_add(char *name)
{
	struct label *l = label_init();
	sprintf(l->name, "%s", name);
	return label_table_add_label(l);
}
