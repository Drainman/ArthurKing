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
int nb_chevaliersDispo ,nb_paysansEnJugement =0,timerGraal,roiJuge = 0;
char tag;
int status;
pthread_mutex_t semJugement2;

/********** MAIN *********/
int main(int argc,char ** argv)
{
  /******** VARIABLES ********/
  printf("[PARAM PAYSANS] - %d\n",NB_PAYSANS);
  int nb_paysans = NB_PAYSANS;

  /******** THREADS *********/
  //KING
  pthread_t pking;

  //KNIGHTS
  pthread_t pChevalier[11];
  int id_chevalier[11];

  //FARMERS
  pthread_t pPaysans[NB_PAYSANS];
  int id_paysans[NB_PAYSANS];

  /******** SEMAPHORES *********/
  sem_init(&semChevaliersDispo, 0, 11); //11 chevaliers
  sem_init(&semPaysansEnJugement, 0,3); //Porte chateau
  sem_init(&semJugement,0,1); //Attente dans la salle de réception
  sem_init(&semTimerGraal, 0, 1);
  sem_init(&semTag,0,12); // 11 chevalier + le roi

  /******** CODE ********/
  printf("[INFO] - Lancement du programme.\n");

  pthread_create (&pking, NULL, (void *) &king, NULL);

  //PTHREAD KNIGHTS
  /*
  for(int i=0;i<11;i++)
  {
     id_chevalier[i] = i;
     pthread_create (&pChevalier[i], NULL, (void *) &chevalier, (void *) &id_chevalier[i]);
  }
  */

  //PTHREAD FARMERS
  for(int i=0;i<nb_paysans;i++)
  {
    id_paysans[i] = i;
    pthread_create (&pPaysans[i], NULL, (void *) &paysans, (void *) &id_paysans[i]);
  }

  //SYNCHRO KNIGHTS & FARMERS
  pthread_join(pking, NULL);
  for(int i=0;i<11;i++){pthread_join(pChevalier[i], NULL);}
  for(int i=0;i<11;i++){pthread_join(pPaysans[i], NULL);}

  printf("[INFO] - Fin du programme.\n");
  return 0;
}


void chevalier(void *ptr)
{
    //ID
    int x;
    x = *((int *) ptr);

    //TIMER
    sleep( rand() % MAX_TIMER_KNIGHTS);

    //EVENT
    sem_wait(&semChevaliersDispo);
    rendreCompte(ptr);

    //WAIT FOR KING
    while(tag != 'G'){}

    chercherGraal(ptr);
    sem_post(&semChevaliersDispo);
    printf("[CHEVALIER %d] - Come back for my quest.\n",x);
}

void chercherGraal(void * ptr)
{
  //ID
  int x;
  x = *((int *) ptr);

  //AFTER WAIT
  printf("[CHEVALIER %d] - ALRIGHT LET'S GO TO FIND THIS BOOK !\n",x);
  //Wait timer
  sleep( timerGraal);
}

void rendreCompte(void *ptr)
{
  int x;
  x = *((int *) ptr);
  printf("[CHEVALIER %d] - My king I've complete my quest.\n",x );
}

void paysans(void *ptr)
{
  //ID
  int x;
  x = *((int *) ptr);

  //TIMER
  sleep( rand() % MAX_TIMER_FARMER) + 1;
  printf("[PAYSANS %d] - Se rend à la cours.\n",x );

  //SEMAPHORE
  sem_wait(&semPaysansEnJugement); //y a de la place

  //Attend que le roi ouvre la porte
  pthread_mutex_lock(&semJugement2);

  printf("[PAYSANS %d] - Jugé.\n",x);

  pthread_mutex_unlock(&semJugement2);

  //Laisse la place à un autre paysant
  sem_post(&semPaysansEnJugement);
}

void king(void * ptr)
{
  //Bloque l'accés au palais
  printf("[KING] - I'm alive ! \n");
  pthread_mutex_lock(&semJugement2);

  while(1)
  {
    //Get rest
    sleep(2);

    //Test si tout les chevaliers sont dispos
    int placeChevalier;
    sem_getvalue(&semChevaliersDispo,&placeChevalier);
    printf("[INFO/KING] - WAITING KNIGHTS -> %d \n",placeChevalier);

    if(placeChevalier <= 0)
    {
      sem_wait(&semTimerGraal);
      timerGraal =    rand() % 25  ;
      sem_post(&semTimerGraal);
      sem_wait(&semTag);
      tag = 'G';
      sem_post(&semTag);
      invoqueMerlin();
      printf("[KING] - I'm back to my kingdom !\n");
      tag = ' ';
    }

    else
    {
      //nb paysans dispo
      int placePaysans;
      sem_getvalue(&semPaysansEnJugement,&placePaysans);

      printf("[WARNING] - PLACE SEM FARMERS -> %d \n",placePaysans);
      //3 FARMERS HERE
      if(placePaysans <= 0 && nb_paysansEnJugement == 3)
      {
        jugement();
      }

    }

  }
}

void invoqueMerlin()
{
  printf("[MERLIN] - Merlin ! I need some help !\n");
  //Attendre la fin du timer protégé en commun avec les chevaliers
  sleep( timerGraal);
}

void jugement()
{
  printf("[KING] - AND THIS IS MY JUGEMENT !!!\n");

  pthread_mutex_unlock(&semJugement2);
  usleep(400);
  pthread_mutex_lock(&semJugement2);
}
