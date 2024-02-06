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

using namespace std;

BB_DS18B20 * ds18b20;

std::vector<unsigned long long > ds_ID;  //vector contenant l'ID des ds18b20
std::vector<double> ds_temperature;      //vector qui va contenir les températures des ds18b20

void loadDSConfig(string filename, std::vector<unsigned long long> &array)
{
    unsigned long long ds_ID;
    stringstream ss;
    ifstream file(filename);
    if (file.is_open()) {
       string line;
       while (getline(file, line)) {
          // enleve espace
          line.erase(remove(line.begin(), line.end(), ' '), line.end());
          //  avons-nous au moins 17 caracteres
          if(line.size() ==17)
          {
             // ok avons-nous 28-
             if(line.substr(0,3) == "28-")
              {
                 stringstream ss(line.substr(3,-1));
                 ss >> hex >>  ds_ID;
                 ds_ID = (ds_ID << 8) | 0x28;
                 array.push_back(ds_ID);  // valide donc insère ce capteur dans la matrice vector ds_ID
               }
          }
       }
    file.close();
   }
}

void lireDS18B20()
{
    // partir une  conversion
    ds18b20->GlobalStartConversion();

    // lire les capteurs
    // l'indetification des DS18B20  sont dans ds_ID
    for(uint loop=0;loop < ds_ID.size();loop++)
    {
     if(ds18b20->ReadSensor(ds_ID[loop]))
      {
        // valeur valide
        ds_temperature[loop]=ds18b20->temperature;
      }
      else
        ds_temperature[loop]=-9999.9;
    }
}

// pin (sortie)
const int relaiEteHiver=26;
const int relaiComp=5;
const int relaiV4V=6;
const int relaiVentUnitExt=13;
const int relaiVitesseVentExt=16;
const int relaiVentUnitInt=19;
const int relaiVitesseVentInt=20;

// pin (entrée)
const int thermostats=17;
const int capteurFiltre=27;

// consignes
float consigneExt=13.5;
float consigneDegFr=4.0; // a modifier en temps reel -10°C
float consigneFinDegFr=15.0;
float consigneBlocChauf = 11.0;
float consigneDepartVentCh=35.0;
float consigneModeDegCh=5;
float consigneFinDegCh=12.5;
float consigneCanicule=30.0;
float delta = 6;
float consigneDelta;

// les temporisations

unsigned long departAutoMode;
unsigned long autoMode = 200000;
unsigned long departTempoV4V;
unsigned long tempoV4V = 1000000;
unsigned long departTempoComp;
unsigned long tempoComp = 2000000;
unsigned long departTempoCompDegElec;
unsigned long tempoCompDegElec = 1500000;
unsigned long departTempoV4VDegElec;
unsigned long tempoV4VDegElec = 1500000;
unsigned long departTempoTempDegElec;
unsigned long tempoTempDegElec = 5000000;
unsigned long departTempoDegCh;
unsigned long tempoDegCh = 20000000;
unsigned long departTempoDegNat;
unsigned long tempoDegNat = 10000000;
unsigned long departTempoEgouttage;
unsigned long tempoEgouttage = 5000000;
unsigned long departTempoFinEgouttage;
unsigned long tempoFinEgouttage = 3000000;

unsigned long departChronoFiltre;
unsigned long finChronoFiltre;
unsigned long nettoyageFiltre; 

bool fonctionFr = false;
bool fonctionCh = false;
bool fonctionCa = false;

// les commandes des fonctions

void commandeFroid()
{
  if (digitalRead(thermostats)==0)
  {
    fonctionFr = true;
  }
  else
  {
    fonctionFr = false;
  }
}

void commandeChauffage()
{
  if (ds_temperature[0] <= consigneBlocChauf && digitalRead(thermostats)==0) // si temperature exterieur au NORD est inferieur a 11°C et que un des 5 thermostat est NC (normalement closed (fermer))
  {
    fonctionCh = true;
  }
  else
  {
    fonctionCh = false;
  }
}

void commandeCanicule()
{
  if (ds_temperature[3] > consigneDelta)
  {
    fonctionCa = true;
  }
  else
  {
    fonctionCa = false;
  }
}

// les relais des ventilateurs interieur et exterieur
void activeRelaisVentFroid()
{
  digitalWrite(relaiVentUnitExt,LOW);
  
  if (ds_temperature[1] > 20.0) // si temperature unité exterieur est supperieur a 20°C
  {
    digitalWrite(relaiVitesseVentExt,LOW); // grande vitesse 
  }
  else
  {
    digitalWrite(relaiVitesseVentExt,HIGH); // petite vitesse
  }
  
  digitalWrite(relaiVentUnitInt,LOW);
  
  if (ds_temperature[3] < 23.0) // si la temperature unité interieur est inferieur a 23°C
  {
    digitalWrite(relaiVitesseVentInt,HIGH);  // ventilateur interieur petite vitesse
  }
  else
  {
    digitalWrite(relaiVitesseVentInt,LOW);  // ventilateur interieur grande vitesse
  }
}

void activeRelaisVentIntChauffage()
{  
  if (ds_temperature[4] >= consigneDepartVentCh)
  {
    digitalWrite(relaiVentUnitInt,LOW); // active la ventilation
  }
  
  if (ds_temperature[3] > 23.0) // temperature a l'aspiration  
  {
    digitalWrite(relaiVitesseVentInt,HIGH); // petite vitesse
  }
  else
  {
    digitalWrite(relaiVitesseVentInt,LOW); // grande vitesse
  }
}

void activeRelaisVentExtChauffage()
{
  digitalWrite(relaiVentUnitExt,LOW);
  
  if (ds_temperature[1] < 5.0)
  {
    digitalWrite(relaiVitesseVentExt,LOW); // grande vitesse
  }
  else
  {
    digitalWrite(relaiVitesseVentExt,HIGH); // petite vitesse
  }
}

void activeRelaisVentsCanicule()
{
  digitalWrite(relaiVentUnitInt,LOW);
  digitalWrite(relaiVitesseVentInt,LOW);
  digitalWrite(relaiVentUnitExt,LOW);
  digitalWrite(relaiVitesseVentExt,LOW);
}

void activeRelaisVentEgouttageFr()
{
  digitalWrite(relaiVentUnitInt,LOW);
  digitalWrite(relaiVitesseVentInt,HIGH); // petite vitesse
}

void activeRelaisVentEgouttageCh()
{
  digitalWrite(relaiVentUnitExt,LOW);
  digitalWrite(relaiVitesseVentExt,LOW); // grande vitesse
}

// l'arret des relais ( HIGH 0 )
void desactiveTousRelais()
{
  digitalWrite(relaiVentUnitExt,HIGH);
  digitalWrite(relaiVentUnitInt,HIGH);
  digitalWrite(relaiVitesseVentExt,HIGH);
  digitalWrite(relaiVitesseVentInt,HIGH);
  digitalWrite(relaiComp,HIGH);
  digitalWrite(relaiV4V,HIGH);
}

enum
{
  DEPART,
  COMMANDE_FROID,
  TEMPO_V4V,
  TEMPO_COMPRESSEUR_FROID,
  COMPRESSEUR_FROID,
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
  COMPRESSEUR_CANICULE,
  FILTRE
} etatsGainable;

void autoModeEteHiver()
{
  if (clock() - departAutoMode >= autoMode)
  {
    etatsGainable = DEPART;
  }
}
     
void gainable()
{
  switch (etatsGainable)
  {
    case DEPART:
    cout << "DEPART" << endl;
    
    if (ds_temperature[0] < consigneExt)
    {
      digitalWrite(relaiEteHiver,HIGH);
      etatsGainable = COMMANDE_CHAUFFAGE;
    }
    else if (ds_temperature[0] <= consigneCanicule)
    {
      digitalWrite(relaiEteHiver,LOW); // relais ete hiver a l'etat 1 
      etatsGainable = COMMANDE_FROID;
    }
    else 
    {
      etatsGainable = COMMANDE_CANICULE;
    }
    
    break;
    
    case COMMANDE_FROID:
    cout << "COMMANDE_FROID" << endl;
    
    if (fonctionFr == true)
    {
      departTempoV4V = clock();
      etatsGainable = TEMPO_V4V;
    }
    else
    {
      autoModeEteHiver();
      departTempoV4V = clock();
      desactiveTousRelais();
    }
    
    break;

    case TEMPO_V4V:
    cout << "TEMPO_V4V" << endl;
    
    if (fonctionFr == false)
    {
      etatsGainable = COMMANDE_FROID;
    }
    else if (clock() - departTempoV4V >= tempoV4V)
    {
      departTempoComp = clock();
      digitalWrite(relaiV4V, LOW);
      etatsGainable = TEMPO_COMPRESSEUR_FROID;
    }
    else
    {
      activeRelaisVentFroid();
    }
      
    break;

    case TEMPO_COMPRESSEUR_FROID:
    cout << "TEMPO_COMPRESSEUR_FROID" << endl;

    if (fonctionFr == false)
    {
      etatsGainable = COMMANDE_FROID;
    }
    else if (clock() - departTempoComp >= tempoComp)
    {
      departChronoFiltre = clock();
      etatsGainable = COMPRESSEUR_FROID;
    }
    else
    {
      activeRelaisVentFroid();
    }
      
    break;

    case COMPRESSEUR_FROID:
    cout << "COMPRESSEUR_FROID" << endl;

    if (fonctionFr == false)
    {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      desactiveTousRelais();
      etatsGainable = COMMANDE_FROID;
    }
    else if (ds_temperature[4] <= consigneDegFr)
    {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      etatsGainable = DEGIVRAGE_FROID;
    }
    else
    {
      activeRelaisVentFroid();
      digitalWrite(relaiComp,LOW);
    }
      
    break;

    case DEGIVRAGE_FROID:
    cout << "DEGIVRAGE_FROID" << endl;

    if (ds_temperature[4] >= consigneFinDegFr)
    {
      departTempoEgouttage = clock();
      etatsGainable = EGOUTTAGE_FROID;
    }
    else
    {
      desactiveTousRelais();
    }
      
    break;

    case EGOUTTAGE_FROID:
    cout << "EGOUTTAGE_FROID" << endl;

    if (clock() - departTempoEgouttage >= tempoEgouttage)
    {
      departTempoFinEgouttage = clock();
      etatsGainable = FIN_EGOUTTAGE_FROID;
    }
    else
    {
      activeRelaisVentEgouttageFr();
    }
      
    break;
    
    case FIN_EGOUTTAGE_FROID:
    cout << "FIN_EGOUTTAGE_FROID" << endl;

    if (clock() - departTempoFinEgouttage >= tempoFinEgouttage)
    {
      etatsGainable = DEPART;
    }
    else
    {
      desactiveTousRelais();
    }
    
    
    break;

    case COMMANDE_CHAUFFAGE:
    cout << "COMMANDE_CHAUFFAGE" << endl;

    if (fonctionCh == true)
    {
      departTempoComp = clock();
      etatsGainable = TEMPO_COMPRESSEUR_CHAUFFAGE;
    }
    else
    {
      autoModeEteHiver();
      departTempoComp = clock();
      desactiveTousRelais();
    }
      
    break;

    case TEMPO_COMPRESSEUR_CHAUFFAGE:
    cout << "TEMPO_COMPRESSEUR_CHAUFFAGE" << endl;

    if (fonctionCh == false)
    {
      etatsGainable = COMMANDE_CHAUFFAGE;
    }
    else if (clock() - departTempoComp >= tempoComp)
    {
      departChronoFiltre = clock();
      departTempoDegCh = clock();
      etatsGainable = TEMPO_DEGIVRAGE;
    }
    else
    {
      activeRelaisVentExtChauffage();
    }
      
    break;

    case TEMPO_DEGIVRAGE:
    cout << "TEMPO_DEGIVRAGE" << endl;

    if (fonctionCh == false)
    {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      etatsGainable = COMMANDE_CHAUFFAGE;
    }
    else if (clock() - departTempoDegCh >= tempoDegCh)
    {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      etatsGainable = MODE_DEGIVRAGE;
    }
    else
    {
      digitalWrite(relaiComp,LOW);
      activeRelaisVentIntChauffage();
      activeRelaisVentExtChauffage();
    }
    break;

    case MODE_DEGIVRAGE:
    cout << "MODE_DEGIVRAGE" << endl;

    if (ds_temperature[1] <= consigneModeDegCh)
    {
      departTempoV4VDegElec = clock();
      etatsGainable = TEMPO_DEG_V4V;
    }
    else
    {
      departTempoDegNat = clock();
      etatsGainable = DEGIVRAGE_NATUREL;
    }
    break;

    case DEGIVRAGE_NATUREL:
    cout << "DEGIVRAGE_NATUREL" << endl;

    if (clock() - departTempoDegNat >= tempoDegNat)
    {
      departTempoEgouttage = clock();
      etatsGainable = EGOUTTAGE_NATUREL;
    }
    else
    {
      desactiveTousRelais();
    }
      
    break;

    case EGOUTTAGE_NATUREL:
    cout << "EGOUTTAGE_NATUREL" << endl;

    if (clock() - departTempoEgouttage >= tempoEgouttage)
    {
      departTempoFinEgouttage = clock();
      etatsGainable = FIN_EGOUTTAGE_NATUREL;
    }
    else
    {
      activeRelaisVentEgouttageCh();
    }
      
    break;
    
    case FIN_EGOUTTAGE_NATUREL:
    cout << "FIN_EGOUTTAGE_NATUREL" << endl;
    
    if (clock() - departTempoFinEgouttage >= tempoFinEgouttage)
    {
      etatsGainable = DEPART;
    }
    else
    {
      desactiveTousRelais();
    }
    
    break;

    case TEMPO_DEG_V4V:
    cout << "TEMPO_DEG_V4V" << endl;

    if (clock() - departTempoV4VDegElec >= tempoV4VDegElec)
    {
      departTempoCompDegElec = clock();
      etatsGainable = TEMPO_DEG_COMPRESSEUR;
    }
    else
    {
      desactiveTousRelais();
    }
      
    break;

    case TEMPO_DEG_COMPRESSEUR:
    cout << "TEMPO_DEG_COMPRESSEUR" << endl;

    if (clock() - departTempoCompDegElec >= tempoCompDegElec)
    {
      departTempoTempDegElec = clock();
      etatsGainable = DEGIVRAGE_ELECTRIC;
    }
    else
    {
      digitalWrite(relaiV4V,LOW);
    }
      
    break;

    case DEGIVRAGE_ELECTRIC:
    cout << "DEGIVRAGE_ELECTRIC" << endl;

    if (ds_temperature[2] >= consigneFinDegCh || (clock() - departTempoTempDegElec >=tempoTempDegElec))
    {
      departTempoEgouttage = clock();
      etatsGainable = EGOUTTAGE_CHAUFFAGE;
    }
    else
    {
      digitalWrite(relaiComp,LOW);
    }
      
    break;

    case EGOUTTAGE_CHAUFFAGE:
    cout << "EGOUTTAGE_CHAUFFAGE" << endl;

    if (clock() - departTempoEgouttage >= tempoEgouttage)
    {
      departTempoFinEgouttage = clock();
      etatsGainable = FIN_EGOUTTAGE_CHAUFFAGE;
    }
    else
    {
      desactiveTousRelais();
    }
    break;

    case FIN_EGOUTTAGE_CHAUFFAGE:
    cout << "FIN_EGOUTTAGE_CHAUFFAGE" << endl;

    if (clock() - departTempoFinEgouttage >= tempoFinEgouttage)
    {
      desactiveTousRelais();
      etatsGainable = DEPART;
    }
    else
    {
      activeRelaisVentEgouttageCh();
    }
    
    break;

    case COMMANDE_CANICULE:
    cout << "COMMANDE_CANICULE" << endl;

    if (fonctionCa == true)
    {
      departTempoV4V = clock();
      etatsGainable = TEMPO_V4V_CANICULE;
    }
    else
    {
      autoModeEteHiver();
      departTempoV4V = clock();
      desactiveTousRelais();
    }
    
    break;

    case TEMPO_V4V_CANICULE:
    cout << "TEMPO_V4V_CANICULE" << endl;

    if (fonctionCa == false)
    {
      etatsGainable = COMMANDE_CANICULE;
    }
    else if (clock() - departTempoV4V >= tempoV4V)
    {
      departTempoComp = clock();
      digitalWrite(relaiV4V,LOW);
      etatsGainable = TEMPO_COMPRESSEUR_CANICULE;
    }
    else
    {
      activeRelaisVentsCanicule();
      digitalWrite(relaiVentUnitInt, HIGH);
    }
      
    break;

    case TEMPO_COMPRESSEUR_CANICULE:
    cout << "TEMPO_COMPRESSEUR_CANICULE" << endl;

    if (fonctionCa == false)
    {
      etatsGainable = COMMANDE_CANICULE;
    }
    else if (clock() - departTempoComp >= tempoComp)
    {
      departChronoFiltre = clock();
      etatsGainable = COMPRESSEUR_CANICULE;
    }
    else
    {
      activeRelaisVentsCanicule();
    }
      
    break;

    case COMPRESSEUR_CANICULE:
    cout << "COMPRESSEUR_CANICULE" << endl;

    if (fonctionCa == false)
    {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      etatsGainable = COMMANDE_CANICULE;
    }
    else if (ds_temperature[4] <= consigneDegFr)
    {
      finChronoFiltre = clock();
      nettoyageFiltre = (finChronoFiltre - departChronoFiltre) + nettoyageFiltre;
      etatsGainable = DEGIVRAGE_FROID;
    }
    else
    {
      digitalWrite(relaiComp,LOW);
    }
      
    break;

    case FILTRE:

    if (digitalRead(capteurFiltre)==0)
    {
      nettoyageFiltre = 0;
    }
      
    break;
  }
}

int main(void)
{

  /******  ceci est le setup  ******/
    
  pinMode(relaiEteHiver,OUTPUT);
  digitalWrite(relaiEteHiver,HIGH);
  pinMode(relaiComp,OUTPUT);
  digitalWrite(relaiComp,HIGH);
  pinMode(relaiV4V,OUTPUT);
  digitalWrite(relaiV4V,HIGH);
  pinMode(relaiVentUnitExt,OUTPUT);
  digitalWrite(relaiVentUnitExt,HIGH);
  pinMode(relaiVitesseVentExt,OUTPUT);
  digitalWrite(relaiVitesseVentExt,HIGH);
  pinMode(relaiVentUnitInt,OUTPUT);
  digitalWrite(relaiVentUnitInt,HIGH);
  pinMode(relaiVitesseVentInt,OUTPUT);
  digitalWrite(relaiVitesseVentInt,HIGH);
  
  pinMode(thermostats,INPUT_PULLUP);
  pinMode(capteurFiltre,INPUT_PULLUP);
  
  int DS_PIN=4;
  pinMode(DS_PIN,OPENDRAIN_PULLUP);
  ds18b20 = new BB_DS18B20(gpioline[DS_PIN]);
  // charge info sur les ds18b20
  loadDSConfig("DS18B20.conf", ds_ID);

  // créer  le vecteur contenant la température des DS18b20
  for(uint loop=0;loop< ds_ID.size() ;loop++)
      ds_temperature.push_back(-9999.9); //  enregistre une information invalide pour commencer


  /****** ceci est la loop  *****/

  while(1)
  {
    lireDS18B20();
    //afficherDS18B20();
    // insérer deux lignes
    cout << endl;

    // Délai de 0.20 secondes environ
    usleep(200000);
    
    cout << "ds_temperature[0] // temperatureExt = " << ds_temperature[0] << endl; // sonde NORD
    cout << "ds_temperature[1] // temperatureUnitéExt = " << ds_temperature[1] << endl;
    cout << "ds_temperature[2] // temperatureEchangeurExt = " << ds_temperature[2] << endl;
    cout << "ds_temperature[3] // temperatureUnitéInt = " << ds_temperature[3] << endl;
    cout << "ds_temperature[4] // temperatureEchangeurInt = " << ds_temperature[4] << endl;
    consigneDelta = ds_temperature[0] - delta; 
    cout << "temperature Canicule Interieur =  " << consigneDelta << endl;
    cout << "chrono = " << clock() << endl;
    
    commandeFroid();
    commandeChauffage();
    commandeCanicule();
    
    gainable();
    cout << "tempo V4V : " << departTempoV4V << endl;
    cout << "tempo comp : " << departTempoComp << endl;
    cout << "departChronoFiltre : " << departChronoFiltre << endl;
    cout << "finChronoFiltre : " << finChronoFiltre << endl;
    cout << "chronoNettoyageFiltre : " << nettoyageFiltre << endl;
    
  }
 delete ds18b20;
 return 0;
}
