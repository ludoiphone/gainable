#include "gpioPin.hpp"
#include "relais.hpp"
#include "hysteresisConsignes.hpp"
#include "affichageTerminal.hpp"

using namespace std;

bool canicule = false;
bool entretienFiltre = false;

extern float tempExtLue;
extern float tempUnitExtLue;
extern float tempEcExtLue;
extern float tempUnitIntLue;
extern float tempEcIntLue;

unsigned long long nettoyageFiltre;

unsigned short compteurV4V = 0; 
unsigned short compteurComp = 0;
unsigned short compteurDegFr = 0;
unsigned short compteurEgouttageFr = 0;
unsigned short compteurFinEgouttageFr = 0;
unsigned short compteurCompCh = 0;
unsigned short compteurDegElec = 0;
unsigned short compteurFinDegTemperature = 0;
unsigned short compteurFinDegTemps = 0;
unsigned short compteurDegNat = 0;
unsigned short compteurEgouttageNat = 0;
unsigned short compteurFinEgouttageNat = 0;
unsigned short compteurV4VDegElec = 0;
unsigned short compteurCompDegElec = 0;
unsigned short compteurEgouttageCh = 0;
unsigned short compteurFinEgouttageCh = 0;
unsigned short compteurV4VCa = 0;
unsigned short compteurCompCa = 0;
unsigned short compteurDegCa = 0;
unsigned short compteurEgouttageCa = 0;
unsigned short compteurFinEgouttageCa = 0;
unsigned short compteurArretProgramme = 0;

void affichageTerminal() {
  
  if (entretienFiltre == true) {
      cout << "*** ARRET PROGRAMME !!! NETTOYAGE ABSOLU DU FILTRE !!! ***" << endl;
    } else {
      cout << "temperatureExt = " << (int (tempExtLue * 2) ) / 2.0 << " C " << endl; // sonde NORD
      cout << "temperatureUniteExt = " << (int (tempUnitExtLue * 2) ) / 2.0 << " C " << endl;
      cout << "temperatureEchangeurExt = " << (int (tempEcExtLue * 2) ) / 2.0 << " C " << endl;
      cout << "temperatureUniteInt = " << (int (tempUnitIntLue * 2) ) / 2.0 << " C " << endl;
      cout << "temperatureEchangeurInt = " << (int (tempEcIntLue * 2) ) / 2.0 << " C " << endl;
      cout << "consigne Canicule Interieur =  " << (int (consigneDelta * 2) ) / 2.0 << " C " << endl;
      cout << "consigne Ventilateur interieur Canicule = " << (int (consigneVentIntCa * 2) ) / 2.0 << " C " << endl << endl;
  
      cout << "chrono = " << clock() << " tick **/0000" << endl;
      cout << "chronoNettoyageFiltre : " << nettoyageFiltre << endl << endl;
  
      cout << "compteurDemarageV4V : " << "Froid = " << compteurV4V << " : Canicule = " << compteurV4VCa << endl;
      cout << "compteurDemarageCompresseur : " << "Froid = " << compteurComp << " : Chauffage = " << compteurCompCh << " : Canicule = " << compteurCompCa << endl;
      cout << "compteurDegivrage : " << "Froid = " << compteurDegFr << " : Chauffage : " << " Naturel = " << compteurDegNat << " Electric = " << compteurDegElec << " : Canicule = " << compteurDegCa << endl;
      cout << "compteurEgouttage : " << "FRoid = " << compteurEgouttageFr << " : Chauffage : " << " Naturel = " << compteurEgouttageNat << " Electric = " << compteurEgouttageCh << " : Canicule = " << compteurEgouttageCa << endl;
      cout << "compteurFinEgouttage : " << "Froid = " << compteurFinEgouttageFr << " : Chauffage : " << " Naturel = " << compteurFinEgouttageNat << " Electric = " << compteurFinEgouttageCh << " : Canicule = " << compteurFinEgouttageCa << endl;
      cout << "compteurFinDegivrageTemperature = " << compteurFinDegTemperature << endl;
      cout << "compteurFinDegivrageTemps = " << compteurFinDegTemps << endl;
      cout << "compteurV4VDegivrageElectric = " << compteurV4VDegElec << endl << endl;
  
      cout << "compteurArretProgrammeNettoyageFiltreOk = " << compteurArretProgramme << endl << endl;
  
      if ((digitalRead (relaiEteHiver) == 0) && canicule == true) {
        cout << "Relai EteHiver **Canicule**" << endl << endl;
      } else if (digitalRead (relaiEteHiver) == 0) {
        cout << "Relai EteHiver **Froid**" << endl << endl;
      } else {
        cout << "Relai EteHiver **Chauffage**" << endl << endl;
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
}
