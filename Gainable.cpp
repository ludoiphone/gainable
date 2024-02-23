#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "gpioPin.hpp"
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <array>
#include "BB_DS18B20.hpp"
#include <vector>
#include <ctime>

using namespace std;

BB_DS18B20 * ds18b20;

std::vector < unsigned long long > ds_ID; //vector contenant l'ID des ds18b20
std::vector < double >
ds_temperature; //vector qui va contenir les températures des ds18b20

void loadDSConfig (string filename,
                   std::vector < unsigned long long > & array) {
  unsigned long long ds_ID;
  stringstream ss;
  ifstream file (filename);
  if (file.is_open() ) {
    string line;
    while (getline (file, line) ) {
      // enleve espace
      line.erase (remove (line.begin(), line.end(), ' '), line.end() );
      //  avons-nous au moins 17 caracteres
      if (line.size() == 17) {
        // ok avons-nous 28-
        if (line.substr (0, 3) == "28-") {
          stringstream ss (line.substr (3, -1) );
          ss >> hex >> ds_ID;
          ds_ID = (ds_ID << 8) | 0x28;
          array.push_back (
            ds_ID); // valide donc insère ce capteur dans la matrice vector ds_ID
        }
      }
    }
    file.close();
  }
}

void lireDS18B20() {
  // partir une  conversion

  ds18b20 -> GlobalStartConversion();

  // lire les capteurs
  // l'indetification des DS18B20  sont dans ds_ID

  for (uint loop = 0; loop < ds_ID.size(); loop++) {
    if (ds18b20 -> ReadSensor (ds_ID[loop]) ) {
      // valeur valide
      ds_temperature[loop] = ds18b20 -> temperature;
    } else
      ds_temperature[loop] = -9999.9;
  }
}

// pin (sortie)

const int relaiEteHiver = 26;
const int relaiComp = 5;
const int relaiV4V = 6;
const int relaiVentUnitExt = 13;
const int relaiVitesseVentExt = 16;
const int relaiVentUnitInt = 19;
const int relaiVitesseVentInt = 20;

// les compteurs

long compteurV4V = 0;
long compteurComp = 0;
long compteurDegFr = 0;
long compteurEgouttageFr = 0;
long compteurFinEgouttageFr = 0;
long compteurCompCh = 0;
long compteurDegElec = 0;
long compteurFinDegTemperature = 0;
long compteurFinDegTemps = 0;
long compteurDegNat = 0;
long compteurEgouttageNat = 0;
long compteurFinEgouttageNat = 0;
long compteurV4VDegElec = 0;
long compteurCompDegElec = 0;
long compteurEgouttageCh = 0;
long compteurFinEgouttageCh = 0;
long compteurV4VCa = 0;
long compteurCompCa = 0;
long compteurDegCa = 0;

// pin (entrée)

const int thermostats = 17;
const int capteurFiltre = 27;

// consignes

float consigneExt = 13.5;
float consigneDegFr = 4.0; // a modifier en temps reel -10°C
float consigneFinDegFr = 15.0;
float consigneVitExtFr = 20.0;
float consigneVitIntFr = 23.0;
float consigneBlocChauf = 11.0;
float consigneDepartVentCh = 25.0; // 35.0°C
float consigneModeDegCh = 5.0;
float consigneFinDegCh = 12.5;
float consigneVitIntCh = 23.0;
float consigneVitExtCh = 5.0;
float consigneCanicule = 30.0;
float delta = 6;
float consigneDelta;
float consigneVentIntCa;

float hysteresis = 0.5;
float hysteresisVentExtFr = 3.0;

bool tempIntCa = false;
bool tempVitIntCa = false;
bool tempVitExtFr = false;
bool tempVitIntFr = false;
bool tempVentUnitIntCh = false;
bool tempVitIntCh = false;
bool tempVitExtCh = false;

// les temporisations

unsigned long departAutoMode;
unsigned long autoMode = 2000000; // 10 secondes
unsigned long departTempoV4V;
unsigned long tempoV4V = 9000000; // 45 secondes
unsigned long departTempoComp;
unsigned long tempoComp = 18000000; // 1 minute 30 secondes
unsigned long departTempoDegFr;
unsigned long tempoDegFr = 240000000; // 20 minutes
unsigned long departTempoTempDegFr;
unsigned long tempoTempDegFr = 12000000; // 1 minutes
unsigned long departTempoCompDegElec;
unsigned long tempoCompDegElec = 12000000; // 1 minutes
unsigned long departTempoV4VDegElec;
unsigned long tempoV4VDegElec = 12000000; // 1 minutes
unsigned long departTempoTempDegElec;
unsigned long tempoTempDegElec = 60000000; // 5 minutes
unsigned long departTempoDegCh;
unsigned long tempoDegCh = 540000000; // 45 minutes
unsigned long departTempoDegNat;
unsigned long tempoDegNat = 120000000; // 10 minutes
unsigned long departTempoEgouttage;
unsigned long tempoEgouttage = 60000000; // 5 minutes
unsigned long departTempoFinEgouttage;
unsigned long tempoFinEgouttage = 36000000; // 3 minutes

unsigned long departChronoFiltre;
unsigned long finChronoFiltre;
unsigned long nettoyageFiltre;

bool fonctionFr = false;
bool fonctionCh = false;
bool fonctionCa = false;
bool canicule = false;
bool forceVentIntDegFr = false;

// les hysteresis de consignes

void hysteresisTempIntCa() {
  if (tempIntCa) {
    tempIntCa = ds_temperature[3] >= (consigneDelta - hysteresis);
  } else {
    tempIntCa = ds_temperature[3] >= (consigneDelta + hysteresis);
  }
}

void hysteresisTempVitesseIntCa() {
  if (tempVitIntCa) {
    tempVitIntCa = ds_temperature[3] >= (consigneVentIntCa - hysteresis);
  } else {
    tempVitIntCa = ds_temperature[3] >= (consigneVentIntCa + hysteresis);
  }
}

void hysteresisTempVitesseExtFroid() {
  if (tempVitExtFr) {
    tempVitExtFr = ds_temperature[1] >= (consigneVitExtFr - hysteresisVentExtFr);
  } else {
    tempVitExtFr = ds_temperature[1] >= (consigneVitExtFr); // si la temperature unite Ext est supperieur 20°C
  }
}

void hysteresisTempVitesseIntFroid() {
  if (tempVitIntFr) {
    tempVitIntFr = ds_temperature[3] >= (consigneVitIntFr - hysteresis);
  } else {
    tempVitIntFr = ds_temperature[3] >= (consigneVitIntFr + hysteresis);
  }
}

void hysteresisTempVitesseIntChauf() {
  if (tempVitIntCh) {
    tempVitIntCh = ds_temperature[3] >= (consigneVitIntCh - hysteresis);
  } else {
    tempVitIntCh = ds_temperature[3] >= (consigneVitIntCh + hysteresis);
  }
}

void hysteresisTempVitesseExtChauf() {
  if (tempVitExtCh) {
    tempVitExtCh = ds_temperature[1] >= (consigneVitExtCh - hysteresis);
  } else {
    tempVitExtCh = ds_temperature[1] >= (consigneVitExtCh + hysteresis);
  }
}

// les commandes des fonctions

void commandeFroid() {
  if (digitalRead (thermostats) == 0) {
    fonctionFr = true;
  } else {
    fonctionFr = false;
  }
}

void commandeChauffage() {
  if (ds_temperature[0] <= consigneBlocChauf &&
      digitalRead (thermostats) ==
      0) { // si temperature exterieur au NORD est inferieur a 11°C et que un des 5 thermostat est NC (normalement closed (fermer))
    fonctionCh = true;
  } else {
    fonctionCh = false;
  }
}

void commandeCanicule() {
  if (digitalRead (thermostats) == 0 && tempIntCa) {
    fonctionCa = true;
  } else {
    fonctionCa = false;
  }
}

// les relais des ventilateurs interieur et exterieur

void activeRelaisVentFroid() { 
  digitalWrite (relaiVentUnitExt, LOW);
  digitalWrite (relaiVentUnitInt, LOW);
  if (tempVitExtFr) { // si la temperature unité exterieur est supperieur a 23°C
    digitalWrite (relaiVitesseVentExt, LOW); // grande vitesse
  } else {
    digitalWrite (relaiVitesseVentExt, HIGH); // petite vitesse
  }
  if (forceVentIntDegFr == true) {
    digitalWrite(relaiVitesseVentInt, LOW);
  } else if (tempVitIntFr) { // si la temperature unité interieur est supperieur a 23°C
    digitalWrite (relaiVitesseVentInt, LOW); // grande vitesse
  } else {
    digitalWrite (relaiVitesseVentInt, HIGH); // petite vitesse
  }
}

void activeRelaisVitesseVentIntChauffage() {
  if (tempVitIntCh) { // si la temperature unité interieur a l'aspiration est supperieur a 23°C
    digitalWrite (relaiVitesseVentInt, LOW); // grande vitesse
  } else {
    digitalWrite (relaiVitesseVentInt, HIGH); // petite vitesse
  }
}

void activeRelaisVentExtChauffage() {
  digitalWrite (relaiVentUnitExt, LOW);
  if (tempVitExtCh) { // si la temperature unité exterieur est supperieur a 5°c
    digitalWrite (relaiVitesseVentExt, HIGH); // petite vitesse
  } else {
    digitalWrite (relaiVitesseVentExt, LOW); // grande vitesse
  }
}

void activeRelaisVentExtCanicule() {
  digitalWrite (relaiVentUnitExt, LOW);
  digitalWrite (relaiVitesseVentExt, LOW);
}

void activeRelaisVentIntCanicule() {
  digitalWrite (relaiVentUnitInt, LOW);
  if (forceVentIntDegFr == true) { // si la temperature de l'echangeur interieur est pendant 1 minute a -5°C force grande vitesse
    digitalWrite (relaiVitesseVentInt, LOW); // grande vitesse
  } else if (tempVitIntCa) { // si la temperature de unite interieur est supperieur as consigne int canicule + 2°C
    digitalWrite (relaiVitesseVentInt, LOW); // grande vitesse
  } else {
    digitalWrite (relaiVitesseVentInt, HIGH); // petite vitesse
  }
}

void activeRelaisVentEgouttageFr() {
  digitalWrite (relaiVentUnitInt, LOW);
  digitalWrite (relaiVitesseVentInt, HIGH); // petite vitesse
}

void activeRelaisVentEgouttageCh() {
  digitalWrite (relaiVentUnitExt, LOW);
  digitalWrite (relaiVitesseVentExt, LOW); // grande vitesse
}

// l'arret des relais ( HIGH 0 )

void desactiveTousRelais() {
  digitalWrite (relaiVentUnitExt, HIGH);
  digitalWrite (relaiVentUnitInt, HIGH);
  digitalWrite (relaiVitesseVentExt, HIGH);
  digitalWrite (relaiVitesseVentInt, HIGH);
  digitalWrite (relaiComp, HIGH);
  digitalWrite (relaiV4V, HIGH);
}

enum {
  DEPART,
  COMMANDE_FROID,
  TEMPO_V4V,
  TEMPO_COMPRESSEUR_FROID,
  TEMPO_DEGIVRAGE_FROID,
  TEMP_DEGIVRAGE_FROID,
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
  TEMP_DEGIVRAGE_CANICULE,
  FILTRE
}
etatsGainable;

void autoModeEteHiver() {
  if (clock() - departAutoMode >= autoMode) {
    etatsGainable = DEPART;
  }
}

void gainable() {
  
  switch (etatsGainable) {
    
  case DEPART:
    cout << "DEPART" << endl << endl;

    departAutoMode = clock();
    if (ds_temperature[0] < consigneExt) {
      digitalWrite (relaiEteHiver, HIGH);
      etatsGainable = COMMANDE_CHAUFFAGE;
    } else if (ds_temperature[0] <= consigneCanicule) {
      digitalWrite (relaiEteHiver, LOW); // relais ete hiver a l'etat 1
      etatsGainable = COMMANDE_FROID;
    } else {
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
      if (ds_temperature[4] < consigneDegFr) {
        forceVentIntDegFr = true;
        departTempoTempDegFr = clock();
        etatsGainable = TEMP_DEGIVRAGE_FROID;
      } else {
        activeRelaisVentFroid();
      }
    } else {
        activeRelaisVentFroid();
    }
    
    break;
          
  case TEMP_DEGIVRAGE_FROID:
    cout << "TEMP_DEGIVRAGE_FROID" << endl << endl;
    
    if (fonctionFr == false) {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      desactiveTousRelais();
      etatsGainable = COMMANDE_FROID;
    } else if (clock() - departTempoTempDegFr >= tempoTempDegFr) {
      if (ds_temperature[4] < consigneDegFr) {
        etatsGainable = DEGIVRAGE_FROID;
      } else {
        forceVentIntDegFr = false;
        etatsGainable = TEMPO_DEGIVRAGE_FROID;
      }
    } else {
      activeRelaisVentFroid();
    }
    
   break;

  case DEGIVRAGE_FROID:
    cout << "DEGIVRAGE_FROID" << endl << endl;

    if (ds_temperature[4] >= consigneFinDegFr) {
      departTempoEgouttage = clock();
      compteurEgouttageFr++;
      etatsGainable = EGOUTTAGE_FROID;
    } else {
      desactiveTousRelais();
    }

    break;

  case EGOUTTAGE_FROID:
    cout << "EGOUTTAGE_FROID" << endl << endl;

    if (clock() - departTempoEgouttage >= tempoEgouttage) {
      departTempoFinEgouttage = clock();
      compteurFinEgouttageFr++;
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
      if (ds_temperature[4] >= consigneDepartVentCh) {
        digitalWrite (relaiVentUnitInt, LOW);
      }
    }
    break;

  case MODE_DEGIVRAGE:
    cout << "MODE_DEGIVRAGE" << endl << endl;

    if (ds_temperature[1] <= consigneModeDegCh) {
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

    if (ds_temperature[2] >= consigneFinDegCh) {
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
      if (ds_temperature[4] < consigneDegFr) {
        forceVentIntDegFr = true;
        departTempoTempDegFr = clock();
        etatsGainable = TEMP_DEGIVRAGE_CANICULE;
      } else {
        activeRelaisVentIntCanicule();
      }
    } else {
        activeRelaisVentIntCanicule();
    }
    
    break;
          
  case TEMP_DEGIVRAGE_CANICULE:
    cout << "TEMP_DEGIVRAGE_CANICULE" << endl << endl;
    
    if (fonctionFr == false) {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      desactiveTousRelais();
      etatsGainable = COMMANDE_CANICULE;
    } else if (clock() - departTempoTempDegFr >= tempoTempDegFr) {
      if (ds_temperature[4] < consigneDegFr) {
        compteurDegCa++;
        etatsGainable = DEGIVRAGE_FROID;
      } else {
        forceVentIntDegFr = false;
        etatsGainable = TEMPO_DEGIVRAGE_CANICULE;
      }
    } else {
      activeRelaisVentIntCanicule();
    }
    
   break;

  case FILTRE:

    if (digitalRead (capteurFiltre) == 0) {
      nettoyageFiltre = 0;
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

void affichageRelais() {
  if (digitalRead (relaiEteHiver) == 0) {
    cout << "Relai EteHiver Froid, Canicule" << endl;
  } else {
    cout << "Relai EteHiver Chauffage" << endl;
  }
  if (digitalRead (relaiV4V) == 0) {
    cout << "Relai V4V oN" << endl;
  } else {
    cout << "Relai V4V oFF" << endl;
  }
  if (digitalRead (relaiComp) == 0) {
    cout << "Relai Compresseur oN" << endl;
  } else {
    cout << "Relai Compresseur oFF" << endl;
  }
  if (digitalRead (relaiVentUnitExt) == 0) {
    cout << "Relai Ventilateur Unite Exterieur oN" << endl;
  } else {
    cout << "Relai Ventilateur Unite Exterieur oFF" << endl;
  }
  if (digitalRead (relaiVitesseVentExt) == 0) {
    cout << "Relai Vitesse Ventilateur Exterieur Grande Vitesse" << endl;
  } else {
    cout << "Relai Vitesse Ventilateur Exterieur Petite Vitesse" << endl;
  }
  if (digitalRead (relaiVentUnitInt) == 0) {
    cout << "Relai Ventilateur Unite Interieur oN" << endl;
  } else {
    cout << "Relai Ventilateur Unite Interieur oFF" << endl;
  }
  if (digitalRead (relaiVitesseVentInt) == 0) {
    cout << "Relai Vitesse Ventilateur Interieur Grande Vitesse" << endl << endl;
  } else {
    cout << "Relai Vitesse Ventilateur Interieur Petite Vitesse" << endl << endl;
  }
}

void fonctionsDivers() {
  consigneDelta = ds_temperature[0] - delta;
  consigneVentIntCa = consigneDelta + 2;
  commandeFroid();
  commandeChauffage();
  commandeCanicule();
  hysteresisTempIntCa();
  hysteresisTempVitesseExtChauf();
  hysteresisTempVitesseIntChauf();
  hysteresisTempVitesseExtFroid();
  hysteresisTempVitesseIntFroid();
  hysteresisTempVitesseIntCa();
  affichageRelais();
  signal (SIGINT, my_ctrl_c_handler);
}

int main (void) {

  /******  ceci est le setup  ******/

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

  int DS_PIN = 4;
  pinMode (DS_PIN, OPENDRAIN_PULLUP);
  ds18b20 = new BB_DS18B20 (gpioline[DS_PIN]);
  // charge info sur les ds18b20
  loadDSConfig ("DS18B20.conf", ds_ID);
  std::cout << "config DS\n";
  // charge info sur les ds18b20
  char  ficConf[654] = "DS18B20.conf";
  FILE * confHan = fopen(ficConf, "r");
  if (NULL == confHan) {
    std::cout <<
        "Le fichier de configuration \n" << ficConf << "\n doit exister\n";
    return (111);
  }
  fclose(confHan);
  loadDSConfig("DS18B20.conf", ds_ID); // protégé contre le cas où il est absent 
  std::cout << "\n configuré\n";
  if (ds_ID.size() < 2) {
    std::cout << "\nVous devez avoir au moins un thermomètre\n";
    return(112);
  }

  // créer  le vecteur contenant la température des DS18b20
  for (uint loop = 0; loop < ds_ID.size(); loop++)
    ds_temperature.push_back (
      -9999.9); //  enregistre une information invalide pour commencer

  /****** ceci est la loop  *****/

  while (1) {
    cout << "ds_temperature[0] // temperatureExt = " << (int (ds_temperature[0] * 2) ) / 2.0 << " C " << endl; // sonde NORD
    cout << "ds_temperature[1] // temperatureUniteExt = " << (int (ds_temperature[1] * 2) ) / 2.0 << " C " << endl;
    cout << "ds_temperature[2] // temperatureEchangeurExt = " << (int (ds_temperature[2] * 2) ) / 2.0 << " C " << endl;
    cout << "ds_temperature[3] // temperatureUniteInt = " << (int (ds_temperature[3] * 2) ) / 2.0 << " C " << endl;
    cout << "ds_temperature[4] // temperatureEchangeurInt = " << (int (ds_temperature[4] * 2) ) / 2.0 << " C " << endl;
    cout << "consigne Canicule Interieur =  " << (int (consigneDelta * 2) ) / 2.0 << " C " << endl;
    cout << "consigne Ventilateur interieur Canicule = " << (int (consigneVentIntCa * 2) ) / 2.0 << " C " << endl << endl;
    time_t rawtime;
    time ( & rawtime);
    cout << "date, heure, annee -> " << ctime ( & rawtime) << endl;
    cout << "chrono = " << clock() << " tick/microsecondes" << endl;
    cout << "departChronoFiltre : " << departChronoFiltre << endl;
    cout << "finChronoFiltre : " << finChronoFiltre << endl;
    cout << "chronoNettoyageFiltre : " << nettoyageFiltre << endl << endl;
    cout << "compteurDemarageV4V = " << compteurV4V << endl;
    cout << "compteurDemarageCompresseurFroid = " << compteurComp << endl;
    //cout << "compteurDemarageCompresseurChauffage = " << compteurCompCh << endl;
    //cout << "compteurDemarageCompresseurCanicule = " << compteurCompCa << endl;
    cout << "compteurDegivrageFroid = " << compteurDegFr << endl;
    cout << "compteurEgouttageFroid = " << compteurEgouttageFr << endl;
    cout << "compteurFinEgouttageFroid = " << compteurFinEgouttageFr << endl << endl;
    //cout << "compteurDegivrageNaturelChauffage = " << compteurDegNat << endl;
    //cout << "compteurEgouttageNaturel = " << compteurEgouttageNat << endl;
    //cout << "compteurFinEgouttageNaturel = " << compteurFinEgouttageNat << endl;
    //cout << "compteurDegivrageElectric = " << compteurCompDegElec << endl;
    //cout << "compteurFinDegivrageTemperature = " << compteurFinDegTemperature << endl;
    //cout << "compteurFinDegivrageTemps = " << compteurFinDegTemps << endl;
    //cout << "compteurV4VDegivrageElectric = " << compteurV4VDegElec << endl;
    //cout << "compteurCompresseurDegivrageElectric = " << compteurCompDegElec << endl;
    //cout << "compteurEgouttageChauffage = " << compteurEgouttageCh << endl;
    //cout << "compteurFinEgouttageChauffage = " << compteurFinEgouttageCh << endl;
    //cout << "compteurV4VCanicule = " << compteurV4VCa << endl;
    //cout << "compteurDegivrageCanicule = " << compteurDegCa << endl << endl;

    lireDS18B20();
    gainable();
    fonctionsDivers();

  }
  delete ds18b20;
  return 0;
}
