// mise à jour du 23/03/2024
// ajout grande vitesse dans les degivrages froid et canicule
// mise à jour du 02/04/2024
// Decoupage du code 
// mise a jour du 23/04/2024
// ajout de ventilation pour l'economie de production froid et chauffage
// mise a jour du 25/04/2024
// ajout d'une tempo de demarage ventilation
// mise a jour du 1/05/2024
/* modification relais de ventilation exterieur et interieur pour la future mise a jour 
  des vitesses 1,2,3,4 interieur .
*/
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <array>
#include <vector>
#include <ctime>
//Decoupage
#include "BB_DS18B20.hpp"
#include "gpioPin.hpp"
#include "lectureTemps.hpp"
#include "ecritTemps.hpp"
#include "vitesseVentilateurs.hpp"
#include "relais.hpp"
#include "commandes.hpp"
#include "affichageTerminal.hpp"

using namespace std;

time_t secondes = time(NULL);

BB_DS18B20 * ds18b20;

// consignes
float consigneExt = 13.5;
float consigneFr = 24.0;
float consigneDegFr = -3.0;
float consigneFinDegFr = 15.0;
float consigneCh = 22.0;
float consigneBlocChauf = 11.0;
float consigneDepartVentCh = 35.0;
float consigneModeDegCh = 5.0;
float consigneFinDegCh = 12.5;
float consigneCanicule = 30.0;
float delta = 6;
float consigneDelta;

// les temporisations
unsigned long long departAutoMode;
unsigned short autoMode = 1; // 10 secondes
unsigned long long departTempoVentilation;
unsigned short tempoVentilation = 5; // 5 secondes
unsigned long long departTempoFroid;
unsigned short tempoFroid = 18; // 3 minutes
unsigned long long departTempoV4V;
unsigned short tempoV4V = 4.5; // 45 secondes
unsigned long long departTempoComp;
unsigned short tempoComp = 9; // 1 minute 30 secondes
unsigned long long departTempoDegFr;
unsigned short tempoDegFr = 120; // 20 minutes
unsigned long long departTempoTempDegFr;
unsigned short tempoTempDegFr = 12; // 2 minutes
unsigned long long departTempoChauffage;
unsigned short tempoChauffage = 18; // 3 minutes
unsigned long long departTempoCompDegElec;
unsigned short tempoCompDegElec = 6.0; // 1 minutes
unsigned long long departTempoV4VDegElec;
unsigned short tempoV4VDegElec = 6.0; // 1 minutes
unsigned long long departTempoTempDegElec;
unsigned short tempoTempDegElec = 30; // 5 minutes
unsigned long long departTempoDegCh;
unsigned short tempoDegCh = 270; // 45 minutes
unsigned long long departTempoDegNat;
unsigned short tempoDegNat = 60; // 10 minutes
unsigned long long departTempoEgouttage;
unsigned short tempoEgouttage = 30; // 5 minutes
unsigned long long departTempoFinEgouttage;
unsigned short tempoFinEgouttage = 18; // 3 minutes
unsigned long long departTempoCanicule;
unsigned short tempoCanicule = 18; // 3 minutes

unsigned long long departChronoFiltre;
unsigned long long finChronoFiltre;
unsigned long long tempsNettoyageFiltre = 1820000; // 21 jours
unsigned long long tempsArretProgramme = 2679000; // 1 mois arret programme par manque d'entretien filtre

// la machine a etats
enum {
  DEPART,
  COMMANDE_FROID,
  TEMPO_V4V,
  TEMPO_COMPRESSEUR_FROID,
  TEMPO_DEGIVRAGE_FROID,
  DEGIVRAGE_FROID,
  EGOUTTAGE_FROID,
  FIN_EGOUTTAGE_FROID,
  COMMANDE_CHAUFFAGE,
  TEMPO_COMPRESSEUR_CHAUFFAGE,
  TEMPO_DEGIVRAGE,
  MODE_DEGIVRAGE,
  DEGIVRAGE_NATUREL,
  EGOUTTAGE_NATUREL,
  FIN_EGOUTTAGE_NATUREL,
  TEMPO_DEG_V4V,
  TEMPO_DEG_COMPRESSEUR,
  DEGIVRAGE_ELECTRIC,
  EGOUTTAGE_CHAUFFAGE,
  FIN_EGOUTTAGE_CHAUFFAGE,
  COMMANDE_CANICULE,
  TEMPO_V4V_CANICULE,
  TEMPO_COMPRESSEUR_CANICULE,
  TEMPO_DEGIVRAGE_CANICULE,
  FILTRE
}
etatsGainable;

void autoModeEteHiver() {
  if (time(NULL) - departAutoMode >= autoMode) {
    etatsGainable = DEPART;
  }
}

void filtre() {
  if (nettoyageFiltre >= tempsNettoyageFiltre) {
    cout << "*** !!!Faire Nettoyage du Filtre!!! ***" << endl << endl;
    if (nettoyageFiltre >= tempsArretProgramme) {
      etatsGainable = FILTRE;
    }
  }
}

void gainable() {
  
  switch (etatsGainable) {
    
  case DEPART:
    cout << "DEPART" << endl << endl;

    departAutoMode = time(NULL);
    departTempoVentilation = time(NULL);
    filtre();
    if (tempExtLue < consigneExt) { // si la temperature ext(nord moyenne regionnale) est inferieur as 13.5°C
      departTempoChauffage = time(NULL);
      digitalWrite (relaiEteHiver, LOW); // relai étéHiver
      etatsGainable = COMMANDE_CHAUFFAGE; // mode chauffage
    } else if (tempExtLue < consigneCanicule) { // si la temperature ext(nord) est inferieur as 30°C
      canicule = false;
      departTempoFroid = time(NULL);
      digitalWrite (relaiEteHiver, HIGH);
      etatsGainable = COMMANDE_FROID; // mode froid
    } else { // si non 
      canicule = true;
      departTempoCanicule = time(NULL);
      etatsGainable = COMMANDE_CANICULE; // mode canicule
    }

    break;

  case COMMANDE_FROID:
    cout << "COMMANDE_FROID" << endl << endl;
    
    if (fonctionFr == true) { // si un des thermostats est a 1
      if (time(NULL) - departTempoVentilation >= tempoVentilation) {
        if (time(NULL) - departTempoFroid >= tempoFroid) { // tempo de 3 minutes
          if (tempUnitIntLue >= consigneFr) { // si la temperature unite interieur est supperieur as 24°C
            departTempoV4V = time(NULL); // enregistre temps pour la tempo de la vanne 4 voies
            etatsGainable = TEMPO_V4V; // passe en prodution de froid 
          } else if (tempUnitIntLue <= consigneCh) { // si non si la temperature unite interieur est inferieur as 22°C
            departTempoChauffage = time(NULL); // enregistre temps pour la tempo chauffage
            digitalWrite (relaiEteHiver, LOW); // relai étéHiver (mode chauffage)
            etatsGainable = COMMANDE_CHAUFFAGE; // passe en mode chauffage
          } else { // si non 
            departTempoFroid = time(NULL); // enregistre temps pour relancé la tempo froid
          }
        } else { // si non
          activeRelaisVentInt(); // active les relais ventilateur interieur en grande vitesse
          cout << "VENTILATION" << endl << endl;
        }
      } else {
        departTempoFroid = time(NULL);
      }
    } else { // si non 
      departTempoVentilation = time(NULL);
      autoModeEteHiver(); // utilise la fonction auto hiver
      digitalWrite (relaiEteHiver, HIGH); // relai étéHiver 1 allumé
      desactiveTousRelais(); // desactive tous les relais
    }

    break;

  case TEMPO_V4V:
    cout << "TEMPO_V4V" << endl << endl;

    if (fonctionFr == false) {
      etatsGainable = DEPART;
    } else if (time(NULL) - departTempoV4V >= tempoV4V) {
      departTempoComp = time(NULL);
      digitalWrite (relaiV4V, LOW);
      compteurV4V++;
      etatsGainable = TEMPO_COMPRESSEUR_FROID;
    } else {
      activeRelaisVentFroid();
    }

    break;

  case TEMPO_COMPRESSEUR_FROID:
    cout << "TEMPO_COMPRESSEUR_FROID" << endl << endl;

    if (fonctionFr == false) {
      etatsGainable = DEPART;
    } else if (time(NULL) - departTempoComp >= tempoComp) {
      digitalWrite (relaiComp, LOW);
      departTempoDegFr = time(NULL);
      departChronoFiltre = time(NULL);
      compteurComp++;
      etatsGainable = TEMPO_DEGIVRAGE_FROID;
    } else {
      activeRelaisVentFroid();
    }

    break;

  case TEMPO_DEGIVRAGE_FROID:
    cout << "TEMPO_DEGIVRAGE_FROID" << endl << endl;
    
    if (fonctionFr == false) {
      finChronoFiltre = time(NULL);
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      desactiveTousRelais();
      etatsGainable = DEPART;
    } else if (time(NULL) - departTempoDegFr >= tempoDegFr) { // tempo 20 minutes
      if (tempEcIntLue < consigneDegFr) { // si temperature echangeur interieur est inferieur as -3°C 
        forceVentIntDegFr = true; // ventilation interieur grande vitesse 
        if (time(NULL) - departTempoTempDegFr >= tempoTempDegFr) { // tempo 2 minutes
          if (tempEcIntLue < consigneDegFr) { // si la temperature echangeur est toujours a -3°C 
            finChronoFiltre = time(NULL);
            nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
            forceVentIntDegFr = false;
            if (canicule == true) {
              compteurDegCa++;
            } else {
              compteurDegFr++;
            }
            etatsGainable = DEGIVRAGE_FROID; // passe en degivrage
          }
        } else {
          activeRelaisVentFroid();
        }
      } else {
        departTempoTempDegFr = time(NULL);
        forceVentIntDegFr = false;
        activeRelaisVentFroid();
      }
    } else {
      departTempoTempDegFr = time(NULL);
      activeRelaisVentFroid();
    }
    
    break;

  case DEGIVRAGE_FROID:
    cout << "DEGIVRAGE_FROID" << endl << endl;

    if (tempEcIntLue >= consigneFinDegFr) {
      departTempoEgouttage = time(NULL);
      if (canicule == true) {
        compteurEgouttageCa++;
      } else {
        compteurEgouttageFr++;
      }
      etatsGainable = EGOUTTAGE_FROID;
    } else {
      desactiveTousRelais();
    }

    break;

  case EGOUTTAGE_FROID:
    cout << "EGOUTTAGE_FROID" << endl << endl;

    if (time(NULL) - departTempoEgouttage >= tempoEgouttage) {
      departTempoFinEgouttage = time(NULL);
      if (canicule == true) {
        compteurFinEgouttageCa++;
      } else {
        compteurFinEgouttageFr++;
      }
      etatsGainable = FIN_EGOUTTAGE_FROID;
    } else {
      activeRelaisVentEgouttageFr();
    }

    break;

  case FIN_EGOUTTAGE_FROID:
    cout << "FIN_EGOUTTAGE_FROID" << endl << endl;

    if (time(NULL) - departTempoFinEgouttage >= tempoFinEgouttage) {
      etatsGainable = DEPART;
    } else {
      desactiveTousRelais();
    }

    break;

  case COMMANDE_CHAUFFAGE:
    cout << "COMMANDE_CHAUFFAGE" << endl << endl;

    if (fonctionCh == true) { // si un des thermostat est as 1
      if (time(NULL) - departTempoVentilation >= tempoVentilation) {
        if (time(NULL) - departTempoChauffage >= tempoChauffage) { // tempo de 3 minutes
          if (tempUnitIntLue <= consigneCh) { // si la temperature unite interieur est inferieur a 22°C
            if (tempUnitExtLue <= consigneBlocChauf) { // si la temperature unite exterieur est inferieur a 11°C
              departTempoComp = time(NULL);
              desactiveRelaisVentIntCh();
              etatsGainable = TEMPO_COMPRESSEUR_CHAUFFAGE; // production de chauffage
            } else {
              departTempoChauffage = time(NULL);
            }
          } else if (tempUnitIntLue >= consigneFr) { // si non si la temperature unite interieur est superieur a 24°C
            departTempoFroid = time(NULL); // enregistre temps pour la tempo froid
            digitalWrite (relaiEteHiver, HIGH); // relai étéHiver mode froid
            etatsGainable = COMMANDE_FROID; // passe en froid
          } else {
            departTempoChauffage = time(NULL);
          }
        } else {
          activeRelaisVentInt();
          cout << "VENTILATION" << endl << endl;
        }
      } else {
        departTempoChauffage = time(NULL);
      }
    } else {
      departTempoVentilation = time(NULL);
      autoModeEteHiver();
      digitalWrite (relaiEteHiver, HIGH); // relai étéHiver 0 eteint
      desactiveTousRelais();
    }

    break;

  case TEMPO_COMPRESSEUR_CHAUFFAGE:
    cout << "TEMPO_COMPRESSEUR_CHAUFFAGE" << endl << endl;

    if (fonctionCh == false) {
      etatsGainable = DEPART;
    } else if (time(NULL) - departTempoComp >= tempoComp) {
      departChronoFiltre = time(NULL);
      departTempoDegCh = time(NULL);
      compteurCompCh++;
      etatsGainable = TEMPO_DEGIVRAGE;
    } else {
      activeRelaisVentExtChauffage();
    }

    break;

  case TEMPO_DEGIVRAGE:
    cout << "TEMPO_DEGIVRAGE" << endl << endl;
    
    if (fonctionCh == false) {
      finChronoFiltre = time(NULL);
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      etatsGainable = DEPART;
    } else if (time(NULL) - departTempoDegCh >= tempoDegCh) {
      etatsGainable = MODE_DEGIVRAGE;
    } else {
      digitalWrite (relaiComp, LOW);
      activeRelaisVentExtChauffage();
      if (tempEcIntLue >= consigneDepartVentCh) {
        activeRelaisVitesseVentIntChauffage();
      }
    }
    break;

  case MODE_DEGIVRAGE:
    cout << "MODE_DEGIVRAGE" << endl << endl;
    
    if (tempUnitExtLue <= consigneModeDegCh) {
      finChronoFiltre = time(NULL);
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      departTempoV4VDegElec = time(NULL);
      compteurDegElec++;
      etatsGainable = TEMPO_DEG_V4V;
    } else {
      finChronoFiltre = time(NULL);
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      departTempoDegNat = time(NULL);
      compteurDegNat++;
      etatsGainable = DEGIVRAGE_NATUREL;
    }
    break;

  case DEGIVRAGE_NATUREL:
    cout << "DEGIVRAGE_NATUREL" << endl << endl;
    
    if (time(NULL) - departTempoDegNat >= tempoDegNat) {
      departTempoEgouttage = time(NULL);
      compteurEgouttageNat++;
      etatsGainable = EGOUTTAGE_NATUREL;
    } else {
      desactiveTousRelais();
    }

    break;

  case EGOUTTAGE_NATUREL:
    cout << "EGOUTTAGE_NATUREL" << endl << endl;

    if (time(NULL) - departTempoEgouttage >= tempoEgouttage) {
      departTempoFinEgouttage = time(NULL);
      compteurFinEgouttageNat++;
      etatsGainable = FIN_EGOUTTAGE_NATUREL;
    } else {
      activeRelaisVentEgouttageCh();
    }

    break;

  case FIN_EGOUTTAGE_NATUREL:
    cout << "FIN_EGOUTTAGE_NATUREL" << endl << endl;
    
    if (time(NULL) - departTempoFinEgouttage >= tempoFinEgouttage) {
      etatsGainable = DEPART;
    } else {
      desactiveTousRelais();
    }

    break;

  case TEMPO_DEG_V4V:
    cout << "TEMPO_DEG_V4V" << endl << endl;
    
    if (time(NULL) - departTempoV4VDegElec >= tempoV4VDegElec) {
      departTempoCompDegElec = time(NULL);
      compteurV4VDegElec++;
      etatsGainable = TEMPO_DEG_COMPRESSEUR;
    } else {
      desactiveTousRelais();
    }

    break;

  case TEMPO_DEG_COMPRESSEUR:
    cout << "TEMPO_DEG_COMPRESSEUR" << endl << endl;
    
    if (time(NULL) - departTempoCompDegElec >= tempoCompDegElec) {
      departTempoTempDegElec = time(NULL);
      compteurCompDegElec++;
      etatsGainable = DEGIVRAGE_ELECTRIC;
    } else {
      digitalWrite (relaiV4V, LOW);
    }

    break;

  case DEGIVRAGE_ELECTRIC:
    cout << "DEGIVRAGE_ELECTRIC" << endl << endl;
    
    if (tempEcExtLue >= consigneFinDegCh) {
      departTempoEgouttage = time(NULL);
      compteurEgouttageCh++;
      compteurFinDegTemperature++;
      etatsGainable = EGOUTTAGE_CHAUFFAGE;
    } else if (time(NULL) - departTempoTempDegElec >= tempoTempDegElec) {
      departTempoEgouttage = time(NULL);
      compteurEgouttageCh++;
      compteurFinDegTemps++;
      etatsGainable = EGOUTTAGE_CHAUFFAGE;
    } else {
      digitalWrite (relaiComp, LOW);
    }

    break;

  case EGOUTTAGE_CHAUFFAGE:
    cout << "EGOUTTAGE_CHAUFFAGE" << endl << endl;
    
    if (time(NULL) - departTempoEgouttage >= tempoEgouttage) {
      departTempoFinEgouttage = time(NULL);
      compteurFinEgouttageCh++;
      etatsGainable = FIN_EGOUTTAGE_CHAUFFAGE;
    } else {
      desactiveTousRelais();
    }
    break;

  case FIN_EGOUTTAGE_CHAUFFAGE:
    cout << "FIN_EGOUTTAGE_CHAUFFAGE" << endl << endl;
    
    if (time(NULL) - departTempoFinEgouttage >= tempoFinEgouttage) {
      desactiveTousRelais();
      etatsGainable = DEPART;
    } else {
      activeRelaisVentEgouttageCh();
    }

    break;

  case COMMANDE_CANICULE:
    cout << "COMMANDE_CANICULE" << endl << endl;
    
    if (fonctionCa == true) {
      if (time(NULL) - departTempoVentilation >= tempoVentilation) {
        if (time(NULL) - departTempoFroid >= tempoFroid) { // tempo de 3 minutes
          if (tempUnitIntLue >= consigneDelta) {
            departTempoV4V = time(NULL);
            etatsGainable = TEMPO_V4V_CANICULE;
          } else {
            departTempoFroid = time(NULL);
          }
        } else {
          activeRelaisVentInt();
        }
      } else {
        departTempoCanicule = time(NULL);
      }
    } else {
      autoModeEteHiver();
      departTempoV4V = time(NULL);
      desactiveTousRelais();
    } 

    break;

  case TEMPO_V4V_CANICULE:
    cout << "TEMPO_V4V_CANICULE" << endl << endl;
    
    if (fonctionCa == false) {
      etatsGainable = COMMANDE_CANICULE;
    } else if (time(NULL) - departTempoV4V >= tempoV4V) {
      departTempoComp = time(NULL);
      digitalWrite (relaiV4V, LOW);
      compteurV4VCa++;
      etatsGainable = TEMPO_COMPRESSEUR_CANICULE;
    } else {
      activeRelaisVentIntCanicule(); 
    }

    break;

  case TEMPO_COMPRESSEUR_CANICULE:
    cout << "TEMPO_COMPRESSEUR_CANICULE" << endl << endl;
    
    if (fonctionCa == false) {
      etatsGainable = COMMANDE_CANICULE;
    } else if (time(NULL) - departTempoComp >= tempoComp) {
      departChronoFiltre = time(NULL);
      departTempoDegFr = time(NULL);
      compteurCompCa++;
      digitalWrite (relaiComp, LOW);
      etatsGainable = TEMPO_DEGIVRAGE_CANICULE;
    } else {
      activeRelaisVentExtCanicule();
      activeRelaisVentIntCanicule();
    }

    break;
    
  case TEMPO_DEGIVRAGE_CANICULE:
    cout << "TEMPO_DEGIVRAGE_CANICULE" << endl << endl;

    if (fonctionFr == false) {
      finChronoFiltre = time(NULL);
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      desactiveTousRelais();
      etatsGainable = COMMANDE_CANICULE;
    } else if (time(NULL) - departTempoDegFr >= tempoDegFr) { // tempo 20 minutes
        if (tempEcIntLue < consigneDegFr) { // si temperature est inferieur a -3°C
          forceVentIntDegFr = true; // force grande vitesse
          if (time(NULL) - departTempoTempDegFr >= tempoTempDegFr) { // tempo de 1 minute (a regler)
            if (tempEcIntLue < consigneDegFr) { // si temperature est toujours inferieur a -3°C on degivre
              finChronoFiltre = time(NULL);
              nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
              forceVentIntDegFr = false;
              etatsGainable = DEGIVRAGE_FROID;
            }
          } else {
            activeRelaisVentExtCanicule();
            activeRelaisVentIntCanicule();
          }
        } else {
          departTempoTempDegFr = time(NULL);
          forceVentIntDegFr = false;
          activeRelaisVentExtCanicule();
          activeRelaisVentIntCanicule();
        }
      } else {
        departTempoTempDegFr = time(NULL);
        activeRelaisVentExtCanicule();
        activeRelaisVentIntCanicule();
      }
    
    break;

  case FILTRE:
    cout << "FILTRE" << endl << endl;
    
    desactiveTousRelais();
    if (digitalRead (capteurFiltre) == 0) {
      nettoyageFiltre = 0;
      compteurArretProgramme++,
      entretienFiltre = false;
      etatsGainable = DEPART;
    } else {
      entretienFiltre = true;
    }
    
    break;
  }
}

void my_ctrl_c_handler (int s) {
  desactiveTousRelais();
  digitalWrite (relaiEteHiver,HIGH);
  delete ds18b20;
  release_gpiod();
  exit (0);
}

// les autres fonctions
void fonctionsDivers() {
  consigneDelta = tempExtLue - delta;
  consigneVentIntCa = consigneDelta + 2;
  commandeFroid();
  commandeChauffage();
  commandeCanicule();
  hysteresisTempVitesseIntCa();
  hysteresisTempVitesseExtFroid();
  hysteresisTempVitesseIntFroid();
  hysteresisTempVitesseExtChauf();
  hysteresisTempVitesseIntChauf();
  signal (SIGINT, my_ctrl_c_handler);
}

// le setup
int main (void) {

  /******  ceci est le setup  ******/
// les pins
  pinMode (relaiEteHiver, OUTPUT);
  digitalWrite (relaiEteHiver, HIGH);
  pinMode (relaiComp, OUTPUT);
  digitalWrite (relaiComp, HIGH);
  pinMode (relaiV4V, OUTPUT);
  digitalWrite (relaiV4V, HIGH);
  pinMode (relaiVitesseVentExt, OUTPUT);
  digitalWrite (relaiVitesseVentExt, HIGH);
  pinMode (relaiPetiteVitesseInt, OUTPUT);
  digitalWrite (relaiPetiteVitesseInt, HIGH);
  pinMode (relaiGrandeVitesseInt, OUTPUT);
  digitalWrite (relaiGrandeVitesseInt, HIGH);

  pinMode (thermostats, INPUT_PULLUP);
  pinMode (capteurFiltre, INPUT_PULLUP);

// pin Sondes ds18b20
  int DS_PIN = 4;
  pinMode (DS_PIN, OPENDRAIN_PULLUP);
  ds18b20 = new BB_DS18B20 (gpioline[DS_PIN]);
  
// le fichier ID sur les ds18b20
  loadDSConfig ("DS18B20.conf", ds_ID);
  // charge info sur les ds18b20
  char  ficConf[654] = "DS18B20.conf";
  FILE * confHan = fopen(ficConf, "r");
  if (NULL == confHan) {
    cout << "Le fichier de configuration \n" << ficConf << "\n doit exister\n";
    return (111);
  }
  fclose(confHan);
  loadDSConfig("DS18B20.conf", ds_ID); // protégé contre le cas où il est absent 
  cout << "\n configure\n";
  if (ds_ID.size() < 2) {
    cout << "\nVous devez avoir au moins un thermomètre\n";
    return(112);
  }

  // créer  le vecteur contenant la température des DS18b20
  for (uint loop = 0; loop < ds_ID.size(); loop++)
    ds_temperature.push_back (
      -9999.9); //  enregistre une information invalide pour commencer

  /****** ceci est la loop  *****/

// la loop
  while (1) {
    lireDS18B20();
    ecritTemps();
    gainable();
    fonctionsDivers();
    affichageTerminal();
    time_t rawtime;
    time ( & rawtime);
    cout << ctime ( & rawtime) << endl;
  }
  delete ds18b20;
  return 0;
}
