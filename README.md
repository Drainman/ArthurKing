# Arthur King Simulation (A C project)
# *Develop by Hagbuck & Drainman*

# Algorythm

## PTHREADS
[1] - Roi - king()
[11] - Chevaliers - chevalier()
[NB_PAYSANS] - Paysans - paysans()

## Actions descriptions

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

## Fonctions par acteurs.

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
