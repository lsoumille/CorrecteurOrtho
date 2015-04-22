#include "hash.h"
#include <string.h>
#include <stdlib.h>
#include <search.h>
#include <stdio.h>
#include <ctype.h>

ENTRY e;
ENTRY *ep;

//
//Creation de la table
//
void hash_table_create(void){
	hcreate(HASH_TABLE_SIZE);
}

//
//Destruction de la table
//
void hash_table_destroy(void){
	hdestroy();
}

//
//Test si un mot est présent dans la table
//
int hash_table_is_present(char *word){
	if(hash_table_search(word) != 0){
		return 1;
	}
	return 0;
}

//
//Cherche le mot dans la table et retourne son nombre d'occurence
//
int hash_table_search(char *word){
	e.key = word;
	ep = hsearch(e, FIND);
	if(ep != NULL){
		return (int)(ep->data);
	}
	return 0;
}

//
//retourne le mot,passé en paramètre, en minuscule
//
static char * toMinus(char *word)
{
	char *res = strdup(word);
    for (int i = 0 ; word[i] != '\0' ; i++)
    {
        res[i] = tolower(res[i]);
    }
    return res;
}

//
//ajout d'un nouvel élément dans la table
//
void hash_table_add(char *word){
	e.key = toMinus(word);
	e.data = (void *) 1;
	ep = hsearch(e, FIND);
	if(ep){
		ep->data = ep->data + 1;
	} else {
		ep = hsearch(e, ENTER);
	}
}