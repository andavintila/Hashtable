#ifndef HASHTABLE_H
#define	HASHTABLE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(EXIT_FAILURE);				\
		}							\
	} while(0)
typedef struct Hashtable Hashtable;


Hashtable * hashtable_new(unsigned int capacity);


void ht_delete(Hashtable *ht);


int ht_get(Hashtable *ht, char *value);


int ht_put(Hashtable *ht, char *value);

int ht_remove(Hashtable *ht, char *value);

void ht_print_bucket(Hashtable *ht, unsigned int index, FILE* stream);

void ht_print(Hashtable *ht, FILE* stream);

Hashtable* ht_resize(Hashtable *ht, unsigned int type);

#ifdef	__cplusplus
}
#endif

#endif	/* HASHTABLE_H */

