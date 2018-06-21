/******************************************************************************/
/******************************** arthurKing.h ********************************/
/*                                                                            */
/*            Auteur : Corentin TROADEC - Anthony Vuillemin                   */
/*                              DATE : Juin 2018                              */
/******************************************************************************/

/*************************/
/********** LIB **********/
/*************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

/******* DEFINE ******/
#define NB_PAYSANS 15
#define MAX_TIMER_KNIGHTS 10
#define MAX_TIMER_FARMER 40


/******* PROTOTYPES ******/
void chevalier(void * ptr);
void paysans(void * ptr);
void rendreCompte(void * ptr);
void king(void * ptr);
void chercherGraal(void * ptr);
void invoqueMerlin();
void jugement();
