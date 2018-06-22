# Arthur King Simulation (A C project)
# *Develop by Hagbuck & Drainman*


# Sommaire

<!--ts-->
   * [Arthur King Simulation (A C project)](#arthur-king-simulation-a-c-project)
   * [Utilisation](#utilisation)
      * [Lancement du programme](#lancement-du-programme)
      * [Modifier les paramèters](#modifier-les-paramèters)
   * [Algorithme](#algorithme)
      * [PTHREADS](#pthreads)
      * [Actions descriptions](#actions-descriptions)
         * [Le Roi](#le-roi)
         * [Les Chevaliers](#les-chevaliers)
         * [Les Paysans](#les-paysans)
      * [Semaphores](#semaphores)
         * [semChevaliersDispo](#semchevaliersdispo)
         * [semPaysansEnJugement](#sempaysansenjugement)
         * [semJugement](#semjugement)
      * [Thread](#thread)
         * [Le Roi](#le-roi-1)
         * [Les Chevaliers](#les-chevaliers-1)
         * [Les Paysans](#les-paysans-1)
      * [Fonctions par acteurs](#fonctions-par-acteurs)
         * [jugement() (Le Roi)](#jugement-le-roi)
         * [invoqueMerlin() (Le Roi)](#invoquemerlin-le-roi)
         * [chercherGraal() (Les Chevaliers)](#cherchergraal-les-chevaliers)
         * [rendreComptee() (Chevaliers)](#rendrecomptee-chevaliers)

<!-- Added by: kurai, at: 2018-06-22T22:11+02:00 -->

<!--te-->

# Utilisation

## Lancement du programme
* OS Support : Linux
* Entrer "make" dans le répertoire du projet.
* Lancer "./arthurKing"

## Modifier les paramèters
Editez arthurKing.h :
* Définir le nombre de paysans : #define NB_PAYSANS [VOTRE NOMBRE]
* Définir le timer max des chevaliers (retour de quete) : #define MAX_TIMER_KNIGHTS [VALEUR TIMER]
* Définir le timer max des paysans : #define MAX_TIMER_FARMER [VALEUR TIMER]

# Algorithme

## PTHREADS
* [1] - Roi - king()
* [11] - Chevaliers - chevalier()
* [NB_PAYSANS] - Paysans - paysans()

## Acteurs

### Le Roi
Boucle de manière infini. A chaque itération :
* Regarde si les chevaliers sont tous dispos.
* Si oui =>  leur ordonne de partir en quete & appelle merlin.
* Sinon => Si 3 paysans attende de se faire juger, les juge.

### Les Chevaliers
Boucle de manière infini. A chaque itération :
* Part en quete pendant un timer alétoire
* A la fin prévient le roi de sa disponibilité.
* Attends que ce dernier et tous les autres soient disponibles avant de partir à la recherche Graal avec tout le monde.
* Repart en quete.

### Les Paysans
Une seul itération :
* Fait sa vie jusqu'a avoir une requete à faire au Roi (timer aléatoire).
* Se rend à la cours.
* Tente de rentrer dans le palais.
* Attends que 3 autres paysans soient présents + la présence du roi
* Se fait juger.

## Semaphores

### semChevaliersDispo
* Description : Indique le nombre de chevaliers disponibles pour partir à la recherche du Graal.
* Place : 11
* Consomateur : Les Chevaliers.
* Lecteur : Le Roi.

### semPaysansEnJugement
* Description : Indique le nombre de paysans prets a etre jugé.
* Place : 3
* Consomateur : Les Paysans.
* Lecteur : Le Roi.

### semJugement
* Description : Sert au jugement des paysans. Le Roi prend la ressource au début du programme. Lorsque 3 paysans attendent de se faire juger et que le Roi n'est pas en quete, il libère la ressource alors consommé à tour de role par les paysans. Puis le Roi reprend la ressource pour empecher que d'autres paysans se fasse juger tant qu'il ne sont pas trois.
* Place : 1
* Consomateur : Le Roi et les Paysans.

## Thread

### Le Roi
```C
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

      //Si 3 paysans attendent
      if(placePaysans <= 0 ){jugement();}

    }

    //Reboucle
  }

  //Fin Thread King
  pthread_exit(0);
}
```

### Les Chevaliers
```C
void chevalier(void *ptr)
{
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
```

### Les Paysans
```C
void paysans(void *ptr)
{
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
  //Laisse la place à un autre paysan

  pthread_exit(0);
}
```

## Fonctions par acteurs

### jugement() (Le Roi)
```C
void jugement()
{
  printf("[KING] - AND THIS IS MY JUGEMENT !!!\n");

  //Libère la semaphore de un pour indiquer aux paysans qu'ils sont jugés.
  sem_post(&semJugement);
  usleep(200);
  sem_wait(&semJugement);

  //On libère les trois paysans aprés le jugement
  for(int i=0;i<3;i++){sem_post(&semPaysansEnJugement);}
}
```

### invoqueMerlin() (Le Roi)
```C
void invoqueMerlin()
{
  //Appelle Merlin
  printf("[MERLIN] - Merlin ! I need some help !\n");
  //Attends la fin du timer partagé avec les chevalier
  sleep( timerGraal);
}
```

### chercherGraal() (Les Chevaliers)
```C
void chercherGraal(void * ptr)
{
  //Préviens de son départ en quete
  printf("[CHEVALIER %d] - ALRIGHT LET'S GO TO FIND THIS BOOK !\n",x);
  //Timer de quete commun avec le Roi
  sleep( timerGraal);
}
```

### rendreComptee() (Chevaliers)
```C
void rendreCompte(void *ptr)
{
  //Se rend disponible pour le roi --> Consomme le sémaphore
  sem_wait(&semChevaliersDispo);

  printf("[CHEVALIER %d] - My king I've complete my quest.\n",x );
}
```
