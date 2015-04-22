/*
 * correct.c	-- Correction d'un mot
 *
 * Copyright © 2013-2014 Erick Gallesio - Polytech'Nice-Sophia <eg@unice.fr>
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date:  6-Jan-2013 07:28 (eg)
 * Last file update: 18-Dec-2014 00:16 (eg)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "correct.h"
#include "hash.h"

 //
 //Alloue l'espace mémoire neccessaire pour créer toutes
 //les corrections possibles
 //
static char **allocate_corrections(size_t size){
  int nbCases = 54 * size + 25;
  char **str = (char **) malloc(nbCases * sizeof(char *));

  for(int i = 0 ; i < nbCases ; ++i){
    str[i] = (char *) malloc((size + 1)* sizeof(char *)); 
  }

  return str;
}

//
//Désalloue le tableau des corrections 
//
static void destroy_corrections(char **corrections, int index){
  for(int i = 0 ; i < index ; ++i){
    free(corrections[i]);
  }
  free(corrections);
}

//
//Affiche l'ensemble des corrections 
//
static void printCorrection(char **corrections, int index){
  for(int i = 0 ; i < index ; ++i){
    printf("%s\n", corrections[i]);
  }
}

//
//Creation de nouvelles corrections en enlevant une lettre
//
static int deletions(char *word, char **corrections, int index){
  char* temp = malloc(sizeof(word) + 1);
  int pos = 0;

  for(int i = 0 ; word[i] ; ++i, ++index){
    for(int j = 0 ; word[j] ; ++j){
      if(i == j) {
        continue;
      }
      temp[pos++] = word[j]; 
    }
    corrections[index] = strdup(temp);
    pos = 0;
  }

  free(temp);
  return index;
}

//
//Creation de nouvelles corrections en interchangeant 2 lettres
//
static int transpositions(char *word, char **corrections, int index){
  char *temp = malloc(sizeof(word) + 1);
  strcpy(temp, word);

  for(int i = 0 ; word[i+1] ; ++i, ++index){
    temp[i] = word[i+1];
    temp[i+1] = word[i];
    corrections[index] = strdup(temp);

    /* on remet correctement */
    temp[i] = word[i];
    temp[i+1] = word[i+1];
  }

  free(temp);
  return index;
}

//
//Creation de nouvelles corrections en transformant une lettre
//
static int alterations(char *word, char **corrections, int index){
  char *temp = malloc(sizeof(word) + 1);
  strcpy(temp,word);

  for(int i = 0 ; word[i] ; ++i){
    for(int j = 0 ; ALPHABET[j] ; ++j, ++index){
      temp[i] = ALPHABET[j];
      corrections[index] = strdup(temp);
      temp[i] = word[i];
    }
  }

  free(temp);
  return index;
}

//
//Creation de nouvelles corrections en insérant des caractères
//
static int inserts(char *word, char **corrections, int index){
  char *temp = malloc(sizeof(word) + 2) ;
  int kLenght = strlen(word);
  int pos = 0;

  for(int j = 0 ; j < kLenght + 1 ; ++j){
    for(int i = 0 ; ALPHABET[i] ; ++i, ++index){
      temp[j] = ALPHABET[i];
      for(int k = 0; word[k] ; ++k, ++pos){
        if(k == j){
          ++pos;
        }
        temp[pos] = word[k];
      }
      pos = 0;
      corrections[index] = strdup(temp);
    }
  }

  free(temp);
  return index;
}

//
//Creation du tableau des corrections
//
static char **build_corrections(char *word, int *size)
{
  char **corrections = allocate_corrections(strlen(word));
  int index = 0;

  if (corrections == NULL) {
    fprintf(stderr, "cannot allocate the array of corrections");
    exit(EXIT_FAILURE);
  }

  /* construires une liste de mots possibles */
  index = deletions(word, corrections, index);
  index = transpositions(word, corrections, index);
  index = alterations(word, corrections, index);
  index = inserts(word, corrections, index);
  *size = index;
  return corrections;
}

//
//Cherche la correction la plus probable dans le tableau des 
//corrections
//
static char *better_candidate(char *word, char **corrections, int index){
  char *better ="";
  int nbBetter = 0;
  int nbOc = 0;

  for(int i = 0 ; i < index ; ++i){
    nbOc = hash_table_search(corrections[i]);
    if(nbOc >= nbBetter){
      better = strdup(corrections[i]);
      nbBetter = nbOc;
    }
  }

  if(nbBetter == 0){
    return NULL;
  }
  return better;
}

//
//Correction d'un mot avec deux erreurs
//
static char* try_2_errors(char **corrections, int index){
  //creation d'un nouveau tableau de correction avec deux corrections par mot
  char **correctionDispo = allocate_corrections(index);
  int parcoursDispo = 0;

  for(int i = 0 ; i < index ; ++i){
    int sizeDblCorrec = 0;
    char **doubleCorrection = build_corrections(corrections[i], &sizeDblCorrec);//creation de correction de correction

    for(int j = 0 ; j < sizeDblCorrec ; ++j){
      if(hash_table_is_present(doubleCorrection[j])){
        //si present dans la table -> elle est candidate pour être la correction
        correctionDispo[parcoursDispo] = doubleCorrection[j];
        parcoursDispo++;
      }
    }
  }

  return better_candidate(correctionDispo[0], correctionDispo, parcoursDispo);
}

//
//retourne la correction pour le mot passé en paramètre
//
static char *find_correction(char *word)
{
  char *result;
  int index = 0;
  char **corrections = build_corrections(word, &index);
  //printCorrection(corrections, index);
  /* choisir le meilleur candidat entre word et les corrections possibles du mot */
  result = better_candidate(word, corrections, index);

  if (! result) {
    /* Pas de correction avec  une seule erreur pour "word" */
    //if (DEBUG) printf("Pas de correction avec une distance de 1 pour '%s'\n", word);

    /* Pour commencer, écrivez une version de try_2_errors qui renvoie
     * toujours NULL. Ainsi, vous ne proposerez pas de solution à deux
     * erreurs */
    result = try_2_errors(corrections, index);
  }

  /* un peu de ménage avant de renvoyer le meilleur candidat */
  result = strdup(result? result: word);
  destroy_corrections(corrections, index);
  return result;
}

char *correct_word(char *word)
{
  if (hash_table_is_present(word)){
    return word;
  } else {
    char *correct = find_correction(word);
    return correct? correct: word;
  }
}
