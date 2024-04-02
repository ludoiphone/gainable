// mise à jour du 23/03/2024
// ajout grande vitesse dans les degivrages froid et canicule
// mise à jour du 02/04/2024
// Decoupage du code 
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
#include "hysteresisConsignes.hpp"
#include "relais.hpp"
#include "commandes.hpp"
#include "affichageTerminal.hpp"

using namespace std;

BB_DS18B20 * ds18b20;

// consignes
float consigneExt = 13.5;
float consigneDegFr = 4.0; // a modifier en temps reel -3°C
float consigneFinDegFr = 15.0;
float consigneDepartVentCh = 25.0; // 35.0°C
float consigneModeDegCh = 5.0;
float consigneFinDegCh = 12.5;
float delta = 6;

bool tempVentUnitIntCh = false;

// les temporisations
unsigned long long departAutoMode;
unsigned long autoMode = 2000000; // 10 secondes
unsigned long long departTempoV4V;
unsigned long tempoV4V = 9000000; // 45 secondes
unsigned long long departTempoComp;
unsigned long tempoComp = 18000000; // 1 minute 30 secondes
unsigned long long departTempoDegFr;
unsigned long tempoDegFr = 240000000; // 20 minutes
unsigned long long departTempoTempDegFr;
unsigned long tempoTempDegFr = 12000000; // 1 minutes
unsigned long long departTempoCompDegElec;
unsigned long tempoCompDegElec = 12000000; // 1 minutes
unsigned long long departTempoV4VDegElec;
unsigned long tempoV4VDegElec = 12000000; // 1 minutes
unsigned long long departTempoTempDegElec;
unsigned long tempoTempDegElec = 60000000; // 5 minutes
unsigned long long departTempoDegCh;
unsigned long tempoDegCh = 540000000; // 45 minutes
unsigned long long departTempoDegNat;
unsigned long tempoDegNat = 120000000; // 10 minutes
unsigned long long departTempoEgouttage;
unsigned long tempoEgouttage = 60000000; // 5 minutes
unsigned long long departTempoFinEgouttage;
unsigned long tempoFinEgouttage = 36000000; // 3 minutes

unsigned long long departChronoFiltre;
unsigned long long finChronoFiltre;
unsigned long long tempsNettoyageFiltre = 362880000000; //362880000000; // 21 jours
unsigned long long tempsArretProgramme = 518400000000; //518400000000; // 1 mois arret programme par manque d'entretien filtre

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
  if (clock() - departAutoMode >= autoMode) {
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

    departAutoMode = clock();
    filtre();
    if (tempExtLue < consigneExt) {
      digitalWrite (relaiEteHiver, HIGH);
      etatsGainable = COMMANDE_CHAUFFAGE;
    } else if (tempExtLue <= consigneCanicule) {
      canicule = false;
      digitalWrite (relaiEteHiver, LOW); // relais ete hiver a l'etat 1
      etatsGainable = COMMANDE_FROID;
    } else {
      canicule = true;
      digitalWrite (relaiEteHiver, LOW);
      etatsGainable = COMMANDE_CANICULE;
    }

    break;

  case COMMANDE_FROID:
    cout << "COMMANDE_FROID" << endl << endl;
    
    if (fonctionFr == true) {
      departTempoV4V = clock();
      etatsGainable = TEMPO_V4V;
    } else {
      autoModeEteHiver();
      departTempoV4V = clock();
      desactiveTousRelais();
    }

    break;

  case TEMPO_V4V:
    cout << "TEMPO_V4V" << endl << endl;

    if (fonctionFr == false) {
      etatsGainable = COMMANDE_FROID;
    } else if (clock() - departTempoV4V >= tempoV4V) {
      departTempoComp = clock();
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
      etatsGainable = COMMANDE_FROID;
    } else if (clock() - departTempoComp >= tempoComp) {
      digitalWrite (relaiComp, LOW);
      departTempoDegFr = clock();
      departChronoFiltre = clock();
      compteurComp++;
      etatsGainable = TEMPO_DEGIVRAGE_FROID;
    } else {
      activeRelaisVentFroid();
    }

    break;

  case TEMPO_DEGIVRAGE_FROID:
    cout << "TEMPO_DEGIVRAGE_FROID" << endl << endl;
    
    if (fonctionFr == false) {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      desactiveTousRelais();
      etatsGainable = COMMANDE_FROID;
    } else if (clock() - departTempoDegFr >= tempoDegFr) { // tempo 20 minutes
      if (tempEcIntLue < consigneDegFr) {
        forceVentIntDegFr = true;
        if (clock() - departTempoTempDegFr >= tempoTempDegFr) {
          if (tempEcIntLue < consigneDegFr) {
            finChronoFiltre = clock();
            nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
            forceVentIntDegFr = false;
            if (canicule == true) {
              compteurDegCa++;
            } else {
              compteurDegFr++;
            }
            etatsGainable = DEGIVRAGE_FROID;
          }
        } else {
          activeRelaisVentFroid();
        }
      } else {
        departTempoTempDegFr = clock();
        forceVentIntDegFr = false;
        activeRelaisVentFroid();
      }
    } else {
      departTempoTempDegFr = clock();
      activeRelaisVentFroid();
    }
    
    break;

  case DEGIVRAGE_FROID:
    cout << "DEGIVRAGE_FROID" << endl << endl;

    if (tempEcIntLue >= consigneFinDegFr) {
      departTempoEgouttage = clock();
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

    if (clock() - departTempoEgouttage >= tempoEgouttage) {
      departTempoFinEgouttage = clock();
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

    if (clock() - departTempoFinEgouttage >= tempoFinEgouttage) {
      etatsGainable = DEPART;
    } else {
      desactiveTousRelais();
    }

    break;

  case COMMANDE_CHAUFFAGE:
    cout << "COMMANDE_CHAUFFAGE" << endl << endl;

    if (fonctionCh == true) {
      departTempoComp = clock();
      etatsGainable = TEMPO_COMPRESSEUR_CHAUFFAGE;
    } else {
      autoModeEteHiver();
      departTempoComp = clock();
      desactiveTousRelais();
    }

    break;

  case TEMPO_COMPRESSEUR_CHAUFFAGE:
    cout << "TEMPO_COMPRESSEUR_CHAUFFAGE" << endl << endl;

    if (fonctionCh == false) {
      etatsGainable = COMMANDE_CHAUFFAGE;
    } else if (clock() - departTempoComp >= tempoComp) {
      departChronoFiltre = clock();
      departTempoDegCh = clock();
      compteurCompCh++;
      etatsGainable = TEMPO_DEGIVRAGE;
    } else {
      activeRelaisVentExtChauffage();
    }

    break;

  case TEMPO_DEGIVRAGE:
    cout << "TEMPO_DEGIVRAGE" << endl << endl;
    
    if (fonctionCh == false) {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      etatsGainable = COMMANDE_CHAUFFAGE;
    } else if (clock() - departTempoDegCh >= tempoDegCh) {
      etatsGainable = MODE_DEGIVRAGE;
    } else {
      digitalWrite (relaiComp, LOW);
      activeRelaisVitesseVentIntChauffage();
      activeRelaisVentExtChauffage();
      if (tempEcIntLue >= consigneDepartVentCh) {
        digitalWrite (relaiVentUnitInt, LOW);
      }
    }
    break;

  case MODE_DEGIVRAGE:
    cout << "MODE_DEGIVRAGE" << endl << endl;
    
    if (tempUnitExtLue <= consigneModeDegCh) {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      departTempoV4VDegElec = clock();
      compteurDegElec++;
      etatsGainable = TEMPO_DEG_V4V;
    } else {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      departTempoDegNat = clock();
      compteurDegNat++;
      etatsGainable = DEGIVRAGE_NATUREL;
    }
    break;

  case DEGIVRAGE_NATUREL:
    cout << "DEGIVRAGE_NATUREL" << endl << endl;
    
    if (clock() - departTempoDegNat >= tempoDegNat) {
      departTempoEgouttage = clock();
      compteurEgouttageNat++;
      etatsGainable = EGOUTTAGE_NATUREL;
    } else {
      desactiveTousRelais();
    }

    break;

  case EGOUTTAGE_NATUREL:
    cout << "EGOUTTAGE_NATUREL" << endl << endl;

    if (clock() - departTempoEgouttage >= tempoEgouttage) {
      departTempoFinEgouttage = clock();
      compteurFinEgouttageNat++;
      etatsGainable = FIN_EGOUTTAGE_NATUREL;
    } else {
      activeRelaisVentEgouttageCh();
    }

    break;

  case FIN_EGOUTTAGE_NATUREL:
    cout << "FIN_EGOUTTAGE_NATUREL" << endl << endl;
    
    if (clock() - departTempoFinEgouttage >= tempoFinEgouttage) {
      etatsGainable = DEPART;
    } else {
      desactiveTousRelais();
    }

    break;

  case TEMPO_DEG_V4V:
    cout << "TEMPO_DEG_V4V" << endl << endl;
    
    if (clock() - departTempoV4VDegElec >= tempoV4VDegElec) {
      departTempoCompDegElec = clock();
      compteurV4VDegElec++;
      etatsGainable = TEMPO_DEG_COMPRESSEUR;
    } else {
      desactiveTousRelais();
    }

    break;

  case TEMPO_DEG_COMPRESSEUR:
    cout << "TEMPO_DEG_COMPRESSEUR" << endl << endl;
    
    if (clock() - departTempoCompDegElec >= tempoCompDegElec) {
      departTempoTempDegElec = clock();
      compteurCompDegElec++;
      etatsGainable = DEGIVRAGE_ELECTRIC;
    } else {
      digitalWrite (relaiV4V, LOW);
    }

    break;

  case DEGIVRAGE_ELECTRIC:
    cout << "DEGIVRAGE_ELECTRIC" << endl << endl;
    
    if (tempEcExtLue >= consigneFinDegCh) {
      departTempoEgouttage = clock();
      compteurEgouttageCh++;
      compteurFinDegTemperature++;
      etatsGainable = EGOUTTAGE_CHAUFFAGE;
    } else if (clock() - departTempoTempDegElec >= tempoTempDegElec) {
      departTempoEgouttage = clock();
      compteurEgouttageCh++;
      compteurFinDegTemps++;
      etatsGainable = EGOUTTAGE_CHAUFFAGE;
    } else {
      digitalWrite (relaiComp, LOW);
    }

    break;

  case EGOUTTAGE_CHAUFFAGE:
    cout << "EGOUTTAGE_CHAUFFAGE" << endl << endl;
    
    if (clock() - departTempoEgouttage >= tempoEgouttage) {
      departTempoFinEgouttage = clock();
      compteurFinEgouttageCh++;
      etatsGainable = FIN_EGOUTTAGE_CHAUFFAGE;
    } else {
      desactiveTousRelais();
    }
    break;

  case FIN_EGOUTTAGE_CHAUFFAGE:
    cout << "FIN_EGOUTTAGE_CHAUFFAGE" << endl << endl;
    
    if (clock() - departTempoFinEgouttage >= tempoFinEgouttage) {
      desactiveTousRelais();
      etatsGainable = DEPART;
    } else {
      activeRelaisVentEgouttageCh();
    }

    break;

  case COMMANDE_CANICULE:
    cout << "COMMANDE_CANICULE" << endl << endl;
    
    if (fonctionCa == true) {
      departTempoV4V = clock();
      etatsGainable = TEMPO_V4V_CANICULE;
    } else {
      autoModeEteHiver();
      departTempoV4V = clock();
      desactiveTousRelais();
    } 

    break;

  case TEMPO_V4V_CANICULE:
    cout << "TEMPO_V4V_CANICULE" << endl << endl;
    
    if (fonctionCa == false) {
      etatsGainable = COMMANDE_CANICULE;
    } else if (clock() - departTempoV4V >= tempoV4V) {
      departTempoComp = clock();
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
    } else if (clock() - departTempoComp >= tempoComp) {
      departChronoFiltre = clock();
      departTempoDegFr = clock();
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
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      desactiveTousRelais();
      etatsGainable = COMMANDE_CANICULE;
    } else if (clock() - departTempoDegFr >= tempoDegFr) { // tempo 20 minutes
        if (tempEcIntLue < consigneDegFr) { // si temperature est inferieur a -3°C
          forceVentIntDegFr = true; // force grande vitesse
          if (clock() - departTempoTempDegFr >= tempoTempDegFr) { // tempo de 1 minute (a regler)
            if (tempEcIntLue < consigneDegFr) { // si temperature est toujours inferieur a -3°C on degivre
              finChronoFiltre = clock();
              nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
              forceVentIntDegFr = false;
              etatsGainable = DEGIVRAGE_FROID;
            }
          } else {
            activeRelaisVentExtCanicule();
            activeRelaisVentIntCanicule();
          }
        } else {
          departTempoTempDegFr = clock();
          forceVentIntDegFr = false;
          activeRelaisVentExtCanicule();
          activeRelaisVentIntCanicule();
        }
      } else {
        departTempoTempDegFr = clock();
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
  hysteresisFonctionCa();
  hysteresisTempIntCa();
  hysteresisTempVitesseIntCa();
  hysteresisFonctionRef();
  hysteresisTempVitesseExtFroid();
  hysteresisTempVitesseIntFroid();
  hysteresisFonctionCh();
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
  pinMode (relaiVentUnitExt, OUTPUT);
  digitalWrite (relaiVentUnitExt, HIGH);
  pinMode (relaiVitesseVentExt, OUTPUT);
  digitalWrite (relaiVitesseVentExt, HIGH);
  pinMode (relaiVentUnitInt, OUTPUT);
  digitalWrite (relaiVentUnitInt, HIGH);
  pinMode (relaiVitesseVentInt, OUTPUT);
  digitalWrite (relaiVitesseVentInt, HIGH);

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
    
    time_t rawtime;
    time ( & rawtime);
    cout << ctime ( & rawtime) << endl;
    lireDS18B20();
    gainable();
    fonctionsDivers();
    affichageTerminal();
  }
  delete ds18b20;
  return 0;
}
