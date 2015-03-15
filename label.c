#include <stdio.h>
#include <stdlib.h>
#include "label.h"

int num_label = 0;

struct label_table *label_table_init(int size)
{
	int i;
	struct label_table *ret = malloc(sizeof(struct label_table));
 
	if(ret == NULL)
	{
		return NULL;
	}

	ret->labels = malloc(sizeof(struct label *) * size);
	if(ret->labels == NULL)
	{
		free(ret);
		return NULL;
	}

	ret->tmp_labels = malloc(sizeof(char *) * size);
	if(ret->tmp_labels == NULL)
	{
		free(ret->labels);
		free(ret);
		return NULL;
	}

	ret->count = 0;
	ret->size = size;
	ret->tmp_count = 0;
	for(i=0; i<size; i++)
	{
		ret->labels[i] = NULL;
		ret->tmp_labels[i] = NULL;
	}

	return ret;
}

struct label *label_init()
{
	struct label *ret = malloc(sizeof(struct label));

	if(ret == NULL)
	{
		return NULL;
	}

	ret->name = malloc(sizeof(char)*16);
	if(ret->name == NULL)
	{
		free(ret);
		return NULL;
	}

	ret->is_emitted = 0;
	ret->numero = -1;
	ret->instr = NULL;

	return ret;
}

struct label *label_get_next_tmp_label()
{
	struct label *ret = label_init();
	sprintf(ret->name, "labelx_%04X", num_label);
	ret->is_emitted = 0;
	ret->numero = num_label;

	num_label++;
	return ret;
}

void label_add_temp(char *name)
{

}
