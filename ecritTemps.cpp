#include "lectureTemps.hpp"
#include "ecritTemps.hpp"


using namespace std;

void ecritTemps() {
  FILE * nomFich = NULL;
  nomFich = fopen ("temperaturesLue.txt", "wt");
  if (nomFich != NULL) {
    fprintf (nomFich, "%3.3f, %8.3f, %8.3f, %8.3f, %8.3f", tempExtLue, tempUnitExtLue, tempEcExtLue, tempUnitIntLue, tempEcIntLue);
    fclose (nomFich); // on ferme le plus vite possible, que l'on puisse lire à tout moment...
    cout << "Ecriture Ok !" << endl << endl;
  } else { 
    cout << "Impossible d'ouvrir le fichier en ecriture !" << endl;
    exit (77); // on s'arrete car il y a un gros problème
  }
}
