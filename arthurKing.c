/******************************************************************************/
/******************************** arthurKing.c ********************************/
/*                                                                            */
/*            Auteur : Corentin TROADEC - Anthony Vuillemin                   */
/*                              DATE : Juin 2018                              */
/******************************************************************************/


/********** LIB **********/
#include "arthurKing.h"


/********** CONST *********/
sem_t semChevaliersDispo,semPaysansEnJugement,semTimerGraal,semJugement,semTag;
int nb_chevaliersDispo,nb_paysansEnJugement,timerGraal = 0,roiJuge = 0;
char tag;
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
  // TODO (FONCTION REMPLISSAGE TABLEAU DYNAMIQUE)

  //semaphores
  sem_init(&semChevaliersDispo, 0, 11); //11 chevaliers
  sem_init(&semPaysansEnJugement, 0,3); //Porte chateau
  sem_init(&semJugement,0,3); //Attente dans la salle de réception
  sem_init(&semTimerGraal, 0, 1);


  /******** CODE ********/
  printf("[INFO] - Lancement du programme.\n");

  pthread_create (&pking, NULL, (void *) &king, (void *) &id_chevalier[0]); //TODO

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
  printf("[CHEVALIER %d] - My king I've complete my quest.\n",x );

  sem_wait(&semChevaliersDispo);
  nb_chevaliersDispo++;

  //printf("[INFO] - Knigths ready for Graal : %d\n", nb_chevaliersDispo);

  //TODO - Il attend l'ordre du roi avant de partir en quete 'Deplacement dans chevalier'
  // -> sem_post(&semChevaliersDispo);
}

//TODO fonction grall

void paysans(void *ptr)
{
  //ID
  int x;
  x = *((int *) ptr);

  //TIMER
  sleep( rand() % MAX_TIMER_FARMER);
  printf("[PAYSANS %d] - Se rend à la cours.\n",x );

  //SEMAPHORE
  sem_wait(&semPaysansEnJugement); //y a de la place
  nb_paysansEnJugement++; //TODO getvalue

  printf("[INFO] - Waiting farmers for the King : %d\n", nb_paysansEnJugement);

  //deuxième porte
  sem_wait(&semJugement);
  int juge = 0;

  while(juge == 0)
  {
    //ATTEND ORDRE ROI AVANT DE SORTIR BOUCLE INFI
    int placePaysans;
    sem_getvalue(&semPaysansEnJugement,&placePaysans);

      if(placePaysans <= 0)
      {
        printf("[PAYSANS %d] - Jugé.\n",x);
        juge = 1;
      }

  }

  //TENTE D ACCEDER AU SEM DU ROI JUGEMENT : ORDRE TODO
  sem_post(&semPaysansEnJugement);
  sem_post(&semJugement);
}

void king(void * ptr)
{

  while(1)
  {
    //Test si tout les chevaliers sont dispos
    sleep(2);

    //NB CHEVALIER
    int placeChevalier;
    sem_getvalue(&semChevaliersDispo,&placeChevalier);
    printf("[WARNING] - PLACE SEM CHEVALIER -> %d \n",placeChevalier);

    if(placeChevalier <= 0)
    {
      //DEFINIISSION TIMER COMMUN : TODO
      invoqueMerlin();
      sem_wait(&semTag);
      tag = 'G';
      sem_post(&semTag);

    }

    else
    {
      //sem_wait(&semPaysansEnJugement);
      //nb paysans dispo
      int placePaysans;
      sem_getvalue(&semPaysansEnJugement,&placePaysans);

      printf("[WARNING] - PLACE SEM FARMERS -> %d \n",placePaysans);

      if(placePaysans == 0)
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
  sem_wait(&semTimerGraal);
  timerGraal =   sleep( rand() % 60);
  sem_post(&semTimerGraal);
  printf("[INFO] - Merlin ! I need some help !\n");
  //sem timer

  printf("[INFO] - Knights RESET : %d\n", nb_chevaliersDispo);
}

void jugement()
{
  sem_wait(&semPaysansEnJugement);
  nb_paysansEnJugement = 0;
  sem_post(&semPaysansEnJugement);
  printf("[INFO] - AND THIS IS MY JUGEMENT !!!\n");
}
