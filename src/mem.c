/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mem.h"

/** squelette du TP allocateur memoire */

void *zone_memoire = 0;

/* ecrire votre code ici */

// Erreurs
#define PLUS_DE_MEMOIRE -1
#define TAILLE_TROP_GRANDE -2

//TZL : contient des pointeurs vers les listes de zone libre
void* TZL[BUDDY_MAX_INDEX + 1];

// s_header zone d'entête contenant les informations de la zone libre 
// et un pointeur vers la zone libre suivante
typedef struct
{
  void* suiv;
} s_header;

unsigned int log2(unsigned long puiss)
{
  unsigned int log = 0;
  while(puiss != 1) {
    puiss = puiss / 2;
    log++;
  }

  return log;
}

/* Fonction retournant l'adresse d'un compagnon d'une zone memoire */
void*
getBuddy(void* origin, unsigned long size)
{
  //on aligne notre zone memoire sur l'adresse 0
  void* origin_normalisee = origin - zone_memoire;

   
}

int 
mem_init()
{
  if (! zone_memoire)
    zone_memoire = (void *) malloc(ALLOC_MEM_SIZE);
  if (zone_memoire == 0)
    {
      perror("mem_init:");
      return -1;
    }

  /* ecrire votre code ici */

  /* Creation de la zone libre */
  //mise du pointeur suivant à null
  ((s_header)(*zone_memoire)).suiv = NULL;

  /* init TZL */
  for(int i = 0; i < BUDDY_MAX_INDEX; i++)
    TZL[i] = NULL;

  /* ajout de la zone libre dans TZL */
  TZL[BUDDY_MAX_INDEX] = zone_memoire;

  return 0;
}

void *
mem_alloc(unsigned long size)
{
  /*  ecrire votre code ici */
  return 0;  
}

int 
mem_free(void *ptr, unsigned long size)
{
  /* ecrire votre code ici */
  return 0;
}


int
mem_destroy()
{
  free(zone_memoire);
  zone_memoire = 0;
  return 0;
}

