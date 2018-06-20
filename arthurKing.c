/******************************************************************************/
/******************************** arthurKing.c ********************************/
/*                                                                            */
/*            Auteur : Corentin TROADEC - Anthony Vuillemin                   */
/*                              DATE : Juin 2018                              */
/******************************************************************************/


/********** LIB **********/
#include "arthurKing.h"


/********** CONST *********/
sem_t semChevaliersDispo,semPaysansEnJugement,semTimerGraal,semJugement;
int nb_chevaliersDispo,nb_paysansEnJugement,timerGraal = 0;
int status;


/********** MAIN *********/
int main(int argc,char ** argv)
{
  /******** VARIABLE ********/
  //Paramater requierements
  printf("[PARAM PAYSANS] - %d\n",NB_PAYSANS);
  int nb_paysans = NB_PAYSANS;

  //pthread
  pthread_t pking; //King

  pthread_t pChevalier[11]; //KNIGHTS
  int id_chevalier[11] = {0,1,2,3,4,5,6,7,8,9,10};

  pthread_t pPaysans[NB_PAYSANS]; //Farmers
  int id_paysans[NB_PAYSANS] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
  // --> a remplir dynamniquement comme au dessus

  //semaphores
  sem_init(&semChevaliersDispo, 0, 14); //11 chevalier + Roi + marge
  sem_init(&semPaysansEnJugement, 0,3); //3 paysans en meme temps
  sem_init(&semTimerGraal, 0, 1);
  sem_init(&semJugement,0,3);

  /******** CODE ********/
  printf("[INFO] - Lancement du programme.\n");

  pthread_create (&pking, NULL, (void *) &king, (void *) &id_chevalier[0]);

  //PTHREAD KNIGHTS
  for(int i=0;i<11;i++)
  {
     pthread_create (&pChevalier[i], NULL, (void *) &chevalier, (void *) &id_chevalier[i]);
  }

  //PTHREAD FARMER
  for(int i=0;i<nb_paysans;i++)
  {
    pthread_create (&pPaysans[i], NULL, (void *) &paysans, (void *) &id_paysans[i]);
  }

  //SYNCHRO KNIGHTS & FARMERS
  for(int i=0;i<11;i++){pthread_join(pChevalier[i], NULL);}
  for(int i=0;i<11;i++){pthread_join(pPaysans[i], NULL);}

  printf("[INFO] - Fin du programme.\n");
  return 0;

}

/**
* Processus Fils Chevalier
* 1) -> Lance un processus fils rendreCompte()
* 2) -> Attends la fin de ce processus et lance chercherGraal()
*/
void chevalier(void *ptr)
{
    sleep( rand() % MAX_TIMER_KNIGHTS);
    rendreCompte(ptr);
}

void rendreCompte(void *ptr)
{
  int x;
  x = *((int *) ptr);
  printf("[INFO] - [CHEVALIER %d] - My king I've complete my quest.\n",x );
  sem_wait(&semChevaliersDispo);
  nb_chevaliersDispo++;
  printf("[INFO] - Knigths ready for Graal : %d\n", nb_chevaliersDispo);
  sem_post(&semChevaliersDispo);
}

void paysans(void *ptr)
{
  int x;
  x = *((int *) ptr);
  sleep( rand() % MAX_TIMER_FARMER);
  printf("[PAYSANS %d] - Se rend à la cours.\n",x );
  sem_wait(&semPaysansEnJugement);
  nb_paysansEnJugement++;
  printf("[INFO] - Waiting farmers for the King : %d\n", nb_paysansEnJugement);
  sem_wait(&semJugement);
  int juge = 0;
  while(juge == 0)
  {
      if(nb_paysansEnJugement == 3)
      {
        printf("[PAYSANS %d] - Jugé.\n",x);
        juge = 1;
      }

  }

  //TENTE D ACCEDER AU SEM DU ROI JUGEMENT
  sem_post(&semPaysansEnJugement);
  sem_post(&semJugement);
}

void king(void * ptr)
{

  while(1)
  {
    //Test si tout les chevaliers sont dispos
    sleep(2);
    sem_wait(&semChevaliersDispo);
    if(nb_chevaliersDispo == 11)
    {
      sem_post(&semChevaliersDispo);
      invoqueMerlin();
    }

    else
    {
      //sem_wait(&semPaysansEnJugement);
      int placePaysans;
      sem_getvalue(&semPaysansEnJugement,&placePaysans);

      printf("[WARNING] - PLACE SEM FARMERS -> %d \n",placePaysans);
      if(placePaysans <= 0)
      {
        //sem_post(&semPaysansEnJugement);
        jugement();
      }

    }

    sem_post(&semPaysansEnJugement);
    sem_post(&semChevaliersDispo);
  }
}

void invoqueMerlin()
{
  sem_wait(&semChevaliersDispo);
  nb_chevaliersDispo = 0;
  sem_post(&semChevaliersDispo);
  printf("[INFO] - Merlin ! I need some help !\n");
  printf("[INFO] - Knights RESET : %d\n", nb_chevaliersDispo);
}

void jugement()
{
  sem_wait(&semPaysansEnJugement);
  nb_paysansEnJugement = 0;
  sem_post(&semPaysansEnJugement);
  printf("[INFO] - AND THIS IS MY JUGEMENT !!!\n");
}
