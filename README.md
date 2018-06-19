# Arthur King Simulation (A C project)
# */Develop by Hagbuck & Drainman*

# Algorythm

## Process
Processus Père : TheKing
11 Processus fils pour chaque chevalier
N Processus fils pour les paysans (N valeur aléatoire ou donné en paramètre)

## Actions descriptions

### TheKing
If chevaliers non dispo && nb_paysans == 3 --> rend_jugement() --> = kill process paysans ?
Else If chevaliers non dispo && nb_paysans > 3 --> paysans convié au prochain jugement (file d'attente)
Else If chevaliers non dispo && nb_paysans < 3 --> Wait 3 paysans
Else appelMerlin()

### Chevaliers
Timer random pendant lequel il font leur quete --> MainProcess
A la fin du timer se rend disponible pour le roi --> rendreCompte()
Si tout les chevaliers sont dispo et que le roi l'ordonne --> chercheGraal()

### Paysans
Timer random pour attendre qu'il se présente à la cours
Si quand se rend à la cours, il y a déja 3 paysans --> Fil d'attente

## Fonction

### rend_jugement() (TheKing)
--> Kill process paysans + 'And my jugement is .... [actionRandom] ! Justice done for [PID] '

### appelMerlin() (TheKing)
--> 'Merlin, please. Help Me.' + timer commun au chevalier

### chercheGraal() (Chevaliers)
--> 'But.. Where is this fuc**** book ! ' + timer commun avec le roi

### rendreComptee() (Chevaliers)
--> 'My king I've complete my quest, I'm Waiting for you now' + timer random perso avant

### mainProcess des paysans
--> Timer random avant de se rendre à la cours
