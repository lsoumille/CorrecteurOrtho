#include "corpus.h"
#include "hash.h"
#include <string.h>
#include <stdio.h>

//
//Permet la creation du corpus a partir du fichier passé en paramètre
//
int init_corpus_from_file(char *filename){
	FILE *fp;
	char str[100] = "";
	fp = fopen(filename, "r");
	
	if(fp == NULL){
		return 0;
	}
	char *word;

	while(fgets(str, 100, fp) != NULL){
		word = strtok(str, SEPARATORS);

		while(word != NULL){
			hash_table_add(word);
			word = strtok(NULL, SEPARATORS);
		}
	}
	return 1;
} 