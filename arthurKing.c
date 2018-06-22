/******************************************************************************/
/******************************** arthurKing.c ********************************/
/*                                                                            */
/*            Auteur : Corentin TROADEC - Anthony Vuillemin                   */
/*                              DATE : Juin 2018                              */
/******************************************************************************/

/*************************/
/********** LIB **********/
/*************************/
#include "arthurKing.h"

/*************************/
/********** CONST *********/
/*************************/
sem_t semChevaliersDispo,semPaysansEnJugement,semJugement;
//Détails plus bas
int timerGraal,roiJuge = 0;
//Timer commun pour la recherche du Graal entre les chevaliers & le Roi.
char tag;
//Utilisé pour prévenir les chevalier qu'il est tant de partir chercher le Graal. (='G' sinon '' )
int status;
//status de retour de threads

/*************************/
/********** MAIN *********/
/*************************/
int main(int argc,char ** argv)
{
  /******** VARIABLES ********/
  printf("[PARAM PAYSANS] - %d\n",NB_PAYSANS);
  int nb_paysans = NB_PAYSANS;

  /******** THREADS *********/
  //THREAD KING
  pthread_t pking;

  //THREADS KNIGHTS
  pthread_t pChevalier[11];
  int id_chevalier[11];

  //THREADS FARMERS
  pthread_t pPaysans[NB_PAYSANS];
  int id_paysans[NB_PAYSANS];

  /******** SEMAPHORES *********/
  sem_init(&semChevaliersDispo, 0, 11);
  //Pour les 11 Chevaliers. Consomme après avoir fait leur quete.
  sem_init(&semPaysansEnJugement, 0,3);
  //Pour empecher les paysans d'entrer à plus de 3 dans le chateau.
  sem_init(&semJugement,0,1);
  //Monopolisé par le Roi jusqu'a son jugement pour laisser le relais aux trois paysans.

  /******** CODE ********/
  printf("[INFO] - Lancement du programme.\n");

  //Création du Roi
  pthread_create (&pking, NULL, (void *) &king, NULL);

  //Création des chevaliers
  for(int i=0;i<11;i++)
  {
     id_chevalier[i] = i;
     pthread_create (&pChevalier[i], NULL, (void *) &chevalier, (void *) &id_chevalier[i]);
  }

  //Création des paysans
  for(int i=0;i<nb_paysans;i++)
  {
    id_paysans[i] = i;
    pthread_create (&pPaysans[i], NULL, (void *) &paysans, (void *) &id_paysans[i]);
  }

  //Synchronisation des touts les threads
  pthread_join(pking, NULL);
  for(int i=0;i<11;i++){pthread_join(pChevalier[i], NULL);}
  for(int i=0;i<11;i++){pthread_join(pPaysans[i], NULL);}

  /********** END *********/
  printf("[INFO] - Fin du programme.\n");
  return 0;
}


/********************************************************/
/********************** CHEVALIER ***********************/
/********************************************************/

/**
* PTHREAD CHEVALIER
* NOMBRE : 11
* Lance un timer, à la attends les autres et le roi pour partir chercher le Graal.
*/
void chevalier(void *ptr)
{
    //ID CHEVALIER
    int x;
    x = *((int *) ptr);

    while(1)
    {
      //TIMER
      sleep( rand() % MAX_TIMER_KNIGHTS);

      //EVENT
      rendreCompte(ptr);

      //WAIT FOR KING
      while(tag != 'G'){}

      //Cherche le Graal
      chercherGraal(ptr);

      //Libère le sémaphore --> Se rend indisponible et repart en quete
      sem_post(&semChevaliersDispo);
      printf("[CHEVALIER %d] - Come back for my quest.\n",x);
      //while --> Reparte en quete
    }

    //Fin Thread King
    pthread_exit(0);
}

/**
* LIE A PTHREAD CHEVALIER
* Indique que le chevalier à fini sa quete.
*/
void rendreCompte(void *ptr)
{
  //ID CHEVALIER
  int x;
  x = *((int *) ptr);

  //Se rend disponible pour le roi --> Consomme le sémaphore
  sem_wait(&semChevaliersDispo);

  printf("[CHEVALIER %d] - My king I've complete my quest.\n",x );
}


/**
* LIE A PTHREAD CHEVALIER
* Le chevalier part en quete $timerGraal secondes
*/
void chercherGraal(void * ptr)
{
  //ID CHEVALIER
  int x;
  x = *((int *) ptr);

  //Préviens de son départ en quete
  printf("[CHEVALIER %d] - ALRIGHT LET'S GO TO FIND THIS BOOK !\n",x);
  //Timer de quete commun avec le Roi
  sleep( timerGraal);
}

/********************************************************/
/************************* ROI **************************/
/********************************************************/


/**
* PTHREAD KING
* NOMBRE : 1
* Regarde si les chevaliers sont tous dispos.
* => Si oui, leur ordonne de partir en quete & appelle merlin
* => Sinon
*     => Si 3 paysans attende de se faire juger, les juge.
*/
void king(void * ptr)
{
  printf("[KING] - Welcome in my Kingdom. \n");

  //Prend la ressource d'accès à la salle de jugement (1 place)
  sem_wait(&semJugement);

  while(1)
  {
    //Prend du repos
    sleep(2);

    //Regarde la disponibilité des chevaliers
    int placeChevalier;
    sem_getvalue(&semChevaliersDispo,&placeChevalier);
    printf("[INFO/KING] - WAITING KNIGHTS -> %d \n",placeChevalier);

    //Tout les chevaliers sont disponibles
    if(placeChevalier <= 0)
    {
      //Défini un timer commun pour lui et le chevalier (= temps de recherche du Graal)
      timerGraal =    rand() % 25  ;

      //Ordonne au chevalier de partir
      tag = 'G';

      //Invoque Merlin
      invoqueMerlin();

      //Retour au royaume
      printf("[KING] - I'm back to my kingdom !\n");

      //Ne donne plus d'ordre
      tag = ' ';
    }

    //Chevaliers indisponibles
    else
    {
      //Récupère le nombre de paysans en attente d'etre juge
      int placePaysans;
      sem_getvalue(&semPaysansEnJugement,&placePaysans);
      printf("[WARNING] - PLACE SEM FARMERS -> %d \n",placePaysans);

      //Si 3 paysants attendent
      if(placePaysans <= 0 ){jugement();}

    }

    //Reboucle
  }

  //Fin Thread King
  pthread_exit(0);
}

/**
* LIE A PTHREAD KING
* Invoque Merlin pour l'aider dans sa quete de recherche du Graal
*/
void invoqueMerlin()
{
  //Appelle Merlin
  printf("[MERLIN] - Merlin ! I need some help !\n");
  //Attends la fin du timer partagé avec les chevalier
  sleep( timerGraal);
}

/**
* LIE A PTHREAD KING
* Juge trois paysants qui attendent.
*/
void jugement()
{
  printf("[KING] - AND THIS IS MY JUGEMENT !!!\n");

  //Libère la semaphore de un pour indiquer aux paysants qu'ils sont jugés.
  sem_post(&semJugement);
  usleep(200);
  sem_wait(&semJugement);

  //On libère les trois paysans aprés le jugement
  for(int i=0;i<3;i++){sem_post(&semPaysansEnJugement);}
}


/********************************************************/
/************************* ROI **************************/
/********************************************************/



/**
* PTHREAD KING
* NOMBRE : NB_PAYSANS
* A partir d'un certain temps se rend à la cours. Attends que 3 autres soit présents
* ainsi que la présence du roi avant de se faire juger.
*/
void paysans(void *ptr)
{
  //ID PAYSANTS
  int x;
  x = *((int *) ptr);

  //N'a pas de reqete à faire au roi avant la fin du timer
  sleep( rand() % MAX_TIMER_FARMER) + 1;
  printf("[PAYSANS %d] - Se rend à la cours.\n",x );

  //Demande à voir le roi -> Attends de pouvoir rentrer dans le palais
  sem_wait(&semPaysansEnJugement);

  //Est dans le palais et attends dans la salle d'attente
  printf("[PAYSANS %d] - Attends dans la salle.\n",x );

  //Attend que le roi l'accueil et le juge
  sem_wait(&semJugement);
  printf("[PAYSANS %d] - Jugé.\n",x);
  sem_post(&semJugement);
  //Laisse la place à un autre paysant


  pthread_exit(0);
}
