/******************************************************************************/
/***************************** ArthurKing.C ************************************/
/******************************************************************************/

/********** LIB **********/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

/******** FUNCTIONS *******/
void chevalier();
void paysans();
void rendreCompte();


/********** CONST *********/
sem_t semChevaliersDispo,semPaysansEnJugement,semTimerGraal;
int nb_chevaliersDispo,nb_paysansEnJugement,timerGraal = 0;
int status;


/********** MAIN *********/
int main(int argc,char ** argv)
{
  /******** VARIABLE ********/
  //Paramater requierements
  printf("[PARAM PAYSANS] - %s\n",argv[1]);
  int nb_paysans = atoi(argv[1]);

  //semaphores
  sem_init(&semChevaliersDispo, 0, 1);
  sem_init(&semPaysansEnJugement, 0,1);
  sem_init(&semTimerGraal, 0, 1);

  /******** CODE ********/
  printf("[INFO] - Lancement du programme.\n");

  return 0;

}

/**
* Processus Fils Chevalier
* 1) -> Lance un processus fils rendreCompte()
* 2) -> Attends la fin de ce processus et lance chercherGraal()
*/
void chevalier()
{
    pid_t rendreC,chercherG;
    if(!(rendreC = fork()))
      rendreCompte();
    else //if(!(chercherG = fork()))
    {
      waitpid(rendreC,&status,0);
    }
    //exit(0);
}

void rendreCompte()
{
  sleep( rand() % 10);
  printf("[INFO] - [CHEVALIER %d] - My king I've complete my quest.\n",(int) getpid() );
  sem_wait(&semChevaliersDispo);
  nb_chevaliersDispo++;
  sem_post(&semChevaliersDispo);
  exit(0);
}

void paysans()
{
  sleep( rand() % 5);
  printf("[INFO] - [PAYSANS %d] - Se rend à la cours.\n",(int) getpid() );
}
