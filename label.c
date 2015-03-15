#include <stdio.h>
#include <stdlib.h>
#include "label.h"

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

	ret->count = 0;
	ret->size = size;
	for(i=0; i<size; i++)
	{
		ret->labels[i] = NULL;
	}

	return ret;
}


