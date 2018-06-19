# Arthur King Simulation (A C project)
# *Develop by Hagbuck & Drainman*

# Algorythm

## Process
TheKing : Process boucle infini.
11 Threads représenatant les chevaliers.
N Threads représenatant les paysans. (N valeurs aléatoires ou donner en paramètre)

## Actions descriptions

### TheKing
Boucle infini (while 1).
* If(nb_chevaliersDispo == 11) --> appelMerlin()
* Else if(nb_paysans_en_attente_de_jugement == 3 ) --> rend_jugement() --> = kill process paysans.
* Else --> CONTINUE

### Chevaliers
Boucle infi (while 1).
*  If(nb_chevaliersDispo == 11) --> chercheGraal()
*  Else (en_quete) --> Timer random pendant lequel il font leur quete --> MainProcess
A la fin du timer se rend disponible pour le roi --> rendreCompte()

### Paysans
Timer random pour attendre qu'il se présente à la cours
* Si quand se rend à la cours, il y a déja 3 paysans --> bloqué par accés ressources.

## Variables protégées

### nb_chevaliersDispo
Description : Représente le nombre de chevalier disponible pour partir en quete.
* Contrainte : <= 11
* A chaque appel de rendreCompte() ++
* A l'appel de appelMerlin() = 0

### nb_paysansEnJugement
Description : Représente le nombre de paysans pret à etre juger.
* Contrainte : <= 3
* A chaque fin de timer des paysans et < 3 --> ++
* Reset à l'appel de rend_jugement() --> =0

### timerGraal
Desription : Le temps pendant lequel le roi et ses 11 chevlaiers partent. (random)

## Fonction

### rend_jugement() (TheKing)
* Appelé quand nb_paysansEnJugement==3 & si roi présent
* Kill process des 3 paysans
* PRINT : 'And my jugement is .... [actionRandom] ! Justice done for [PID] '
* RESET $nb_paysansEnJugement

### appelMerlin() (TheKing)
* Appelé par le Roi quand nb_chevaliersDispo == 11
* PRINT : 'Merlin, please. Help Me.'
* RESET $nb_chevaliersDispo
* timer commun aux chevaliers

### chercheGraal() (Chevaliers)
* Appelé par les chevaliers quand nb_chevaliersDispo == 11
* PRINT : 'But.. Where is this fuc**** book ! ' + timer commun avec le roi
* A la fin de cette fonction le chevalier doit repartir en quete. (semaphore de son etat MAJ par le roi et visible dans le boucle infini)

### rendreComptee() (Chevaliers)
* Appelé par les chevaliers quand leur timer de quete est fini.
* PRINT : 'My king I've complete my quest.'
* INCREMENT : $nb_chevaliersDispo

### mainProcess des paysans
* Timer random avant de se rendre à la cours
* PRINT (arrive à la cours) : "I have a request !"
* INCREMENT : $nb_paysansEnJugement
* WAIT : JUGEMENT
* WHEN JUGER -> PRINT : "King juge me !"
