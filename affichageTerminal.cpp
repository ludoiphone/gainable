#include "gpioPin.hpp"
#include "lectureTemps.hpp"
#include "relais.hpp"
#include "vitesseVentilateurs.hpp"
#include "affichageTerminal.hpp"

using namespace std;

bool canicule = false;
bool entretienFiltre = false;

extern float consigneDelta;

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
  
      cout << "chrono = " << time(NULL) << " Secondes" << endl;
      cout << "chronoNettoyageFiltre : " << nettoyageFiltre << " Secondes" << endl << endl;
  
      cout << "compteurDemarageV4V : " << "Froid = " << compteurV4V << " : Canicule = " << compteurV4VCa << endl;
      cout << "compteurDemarageCompresseur : " << "Froid = " << compteurComp << " : Chauffage = " << compteurCompCh << " : Canicule = " << compteurCompCa << endl;
      cout << "compteurDegivrage : " << "Froid = " << compteurDegFr << " : Chauffage : " << " Naturel = " << compteurDegNat << " Electric = " << compteurDegElec << " : Canicule = " << compteurDegCa << endl;
      cout << "compteurEgouttage : " << "FRoid = " << compteurEgouttageFr << " : Chauffage : " << " Naturel = " << compteurEgouttageNat << " Electric = " << compteurEgouttageCh << " : Canicule = " << compteurEgouttageCa << endl;
      cout << "compteurFinEgouttage : " << "Froid = " << compteurFinEgouttageFr << " : Chauffage : " << " Naturel = " << compteurFinEgouttageNat << " Electric = " << compteurFinEgouttageCh << " : Canicule = " << compteurFinEgouttageCa << endl;
      cout << "compteurFinDegivrageTemperature = " << compteurFinDegTemperature << endl;
      cout << "compteurFinDegivrageTemps = " << compteurFinDegTemps << endl;
      cout << "compteurV4VDegivrageElectric = " << compteurV4VDegElec << endl << endl;
  
      cout << "compteurArretProgrammeNettoyageFiltreOk = " << compteurArretProgramme << endl << endl;
  
      if ((digitalRead (relaiEteHiver) == 1) && canicule == true) {
        cout << "Relai EteHiver **Canicule**" << endl << endl;
      } else if (digitalRead (relaiEteHiver) == 1) {
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
      if (digitalRead (relaiVitesseVentExt) == 0) {
        cout << "Relai Ventilateur Exterieur Grande Vitesse oN" << endl;
      } else {
        cout << "Relai Ventilateur Exterieur Petite Vitesse oN" << endl;
      }
      if (digitalRead (relaiPetiteVitesseInt) == 0) {
        cout << "Relai Ventilateur Interieur Petite Vitesse oN" << endl;
       } else {
         cout << "Relai Ventilateur Interieur Petite Vitesse oFF" << endl;
       }
       if (digitalRead (relaiGrandeVitesseInt) == 0) {
         cout << "Relai Ventilateur Interieur Grande Vitesse oN " << endl << endl;
       } else {
         cout << "Relai Ventilateur Interieur Grande Vitesse oFF" << endl << endl;
       }
    }
}
