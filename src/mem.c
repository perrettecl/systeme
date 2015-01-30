/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
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


// fonction qui retourne 2 puissance (exp)
// 0 <= exp < 32 (pour les archi 32 bits)
unsigned long f2puiss(unsigned int exp)
{
  assert(exp >= 0 && exp < 32);
  return 1 << exp;
}


//fonction qui retourne la partie entiere du logarithme base 2
// puiss > 0
unsigned int mylog2(unsigned long puiss)
{
  assert(puiss > 0);
  unsigned int log = 0;
  while(puiss != 1) {
    puiss = puiss >> 1;
    log++;
  }

  return log;
}

/* Fonction permettant de decouper un bloc en deux dans la TZL */
void splitBloc(unsigned int indice)
{
  assert(indice <= BUDDY_MAX_INDEX && TZL[indice] != NULL);
  
  unsigned long nvBloc1 = (unsigned long) TZL[indice];
  unsigned long nvBloc2 = nvBloc1 + f2puiss(indice-1);

  //on supprime de la liste le bloc qu'on split
  TZL[indice] = ((s_header*)TZL[indice])->suiv;

  //on chaine le bloc qu'on split dans la TZL a l'indice i-1
  void* prmBlocIm1 = TZL[indice-1];

  ((s_header*)(nvBloc1))->suiv = (void*) nvBloc2;
  ((s_header*)(nvBloc2))->suiv = (void*) prmBlocIm1;

  TZL[indice-1] = (void*)nvBloc1;
}

/* Fonction retournant l'adresse d'un compagnon d'une zone memoire */
void*
getBuddy(void* origin, unsigned long size)
{
  //on aligne notre zone memoire sur l'adresse 0
  unsigned long origin_normalisee = (unsigned long)(origin) - (unsigned long)(zone_memoire);

  //cas du premier bloc de la zone
  if(origin_normalisee == 0)
    return (void*)((unsigned long)(origin) + size);


  // on cherche la fin du bloc
  unsigned long fin_norm = origin_normalisee + size;
  void* buddy;
  //on cherche sur quel bloc on est
  if(f2puiss(mylog2(fin_norm) == fin_norm))
  {
    //On est sur le second segment
    buddy =  (void*)((unsigned long)(origin) - size);
  }
  else
  {
    //On est sur le premier bloc
    buddy = (void*)((unsigned long)(origin) + size);
  }

  return buddy; 
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
  ((s_header*)(zone_memoire))->suiv = NULL;

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

  if(zone_memoire == 0)
          return NULL;

  if(ALLOC_MEM_SIZE < size)
    return (void*) TAILLE_TROP_GRANDE;
  
  unsigned int indice = mylog2(size);
  if(f2puiss(indice) < size)
      indice++;

  unsigned int i = indice;
  while(TZL[i] == NULL) {
    if(i == BUDDY_MAX_INDEX)
        return (void*) PLUS_DE_MEMOIRE;
    i++;
  }
  
  //on peut split le bloc a l'indice i, nbSplit fois
  unsigned int nbSplit = i - indice;
  for(int j=0; j<nbSplit; j++) {
      //on split le bloc a l'indice i
      splitBloc(i);
      i--;
  }
 //on retourne le bloc TZL[indice] et on le supprime de la liste
  void* blocAAllouer = TZL[indice];
  TZL[indice] = ((s_header*)(TZL[indice]))->suiv;

  return blocAAllouer;  
}

int 
mem_free(void *ptr, unsigned long size)
{
  bool fusion = true;

  //on cherche à fusionner des blocs libres
  void* bloc_actuel = ptr;

  unsigned long taille_actuel = mylog2(size);
  if(f2puiss(taille_actuel) < size)
          taille_actuel++;

  while (fusion){
    void* buddy = getBuddy(bloc_actuel,f2puiss(taille_actuel));

    //on recherche si le buddy est dans la liste des blocs libres
    bool trouver = false;
    void* bloc_courrant = TZL[taille_actuel];
    void* bloc_prec = NULL;
    while(bloc_courrant != NULL)
    {
      if(bloc_courrant == buddy){
        trouver = true;
        break;
      }

      bloc_prec = bloc_courrant;
      bloc_courrant = ((s_header*)bloc_courrant)->suiv;
    }

    //on fusionne si on a trouvé le bloc
    if(trouver)
    {
      //on regarde le bloc d'avant le bloc trouvé
      //Et on supprime ce bloc de la liste
      if(bloc_prec != NULL)
      {
        ((s_header*)bloc_prec)->suiv = ((s_header*)buddy)->suiv;
      } else {
        TZL[taille_actuel] = ((s_header*)buddy)->suiv;
      }

      if((unsigned long)(buddy) < (unsigned long)(bloc_actuel))
      {
        //le buddy est avant le bloc actuel
        bloc_actuel = buddy; 
      }

      //on insert le bloc dans la TZl
      taille_actuel = taille_actuel * 2;
      ((s_header*)bloc_actuel)->suiv = TZL[taille_actuel];
      TZL[taille_actuel] = bloc_actuel;

      /*
      //on regarde si on est a la taille max
      if(taille_actuel == ALLOC_MEM_SIZE)
        fusion = false;
      */
    } else {
      fusion = false;
    }
    
  }

  return 0;
}


int
mem_destroy()
{
  free(zone_memoire);
  zone_memoire = 0;
  return 0;
}

