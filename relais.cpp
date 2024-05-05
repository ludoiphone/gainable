#include "gpioPin.hpp"
#include "relais.hpp"
#include "vitesseVentilateurs.hpp"

using namespace std;

// pins relais
const char relaiComp = 5;
const char relaiV4V = 6;
const char relaiVitesseVentExt = 13;
const char relaiPetiteVitesseInt = 16;
const char relaiGrandeVitesseInt = 19;
const char relaiEteHiver = 26;

bool forceVentIntDegFr = false;

void activeRelaisVentInt() {
  digitalWrite (relaiGrandeVitesseInt, LOW);
  digitalWrite (relaiPetiteVitesseInt, HIGH);
}

void activeRelaisVentFroid() { 
  if (tempVitExtFr) { // si la temperature unité exterieur est supperieur a 23°C
    digitalWrite (relaiVitesseVentExt, LOW); // grande vitesse
  } else {
    digitalWrite (relaiVitesseVentExt, HIGH); // petite vitesse
  }
  if (forceVentIntDegFr == true) {
    digitalWrite (relaiGrandeVitesseInt, LOW);
    digitalWrite (relaiPetiteVitesseInt, HIGH);
  } else if (tempVitIntFr) { // si la temperature unité interieur est supperieur a 23°C (aspiration)
    digitalWrite (relaiGrandeVitesseInt, LOW); // grande vitesse
    digitalWrite (relaiPetiteVitesseInt, HIGH);
  } else {
    digitalWrite (relaiPetiteVitesseInt, LOW); // petite vitesse
    digitalWrite (relaiGrandeVitesseInt, HIGH);
  }
}

void activeRelaisVitesseVentIntChauffage() {
  if (tempVitIntCh) { // si la temperature unité interieur est supperieur a 23°C (aspiration)
    digitalWrite (relaiGrandeVitesseInt, LOW); // grande vitesse
    digitalWrite (relaiPetiteVitesseInt, HIGH);
  } else {
    digitalWrite (relaiGrandeVitesseInt, HIGH); // petite vitesse
    digitalWrite (relaiPetiteVitesseInt, LOW);
  }
}

void activeRelaisVentExtChauffage() {
  if (tempVitExtCh) { // si la temperature unité exterieur est supperieur a 5°c
    digitalWrite (relaiVitesseVentExt, HIGH); // petite vitesse
  } else {
    digitalWrite (relaiVitesseVentExt, LOW); // grande vitesse
  }
}

void activeRelaisVentExtCanicule() {
  digitalWrite (relaiVitesseVentExt, LOW);
}

void activeRelaisVentIntCanicule() {
  if (forceVentIntDegFr == true) { // si la temperature de l'echangeur interieur est pendant 1 minute a -5°C force grande vitesse
    digitalWrite (relaiGrandeVitesseInt, LOW); // grande vitesse
    digitalWrite (relaiPetiteVitesseInt, HIGH);
  } else if (tempVitIntCa) { // si la temperature de unite interieur est supperieur a la consigne int canicule + 2°C
    digitalWrite (relaiGrandeVitesseInt, LOW); // grande vitesse
    digitalWrite (relaiPetiteVitesseInt, HIGH);
  } else {
    digitalWrite (relaiGrandeVitesseInt, HIGH); // petite vitesse
    digitalWrite (relaiPetiteVitesseInt, LOW);
  }
}

void activeRelaisVentEgouttageFr() {
  digitalWrite (relaiPetiteVitesseInt, LOW); // petite vitesse
}

void activeRelaisVentEgouttageCh() {
  digitalWrite (relaiVitesseVentExt, LOW); // grande vitesse
}

void desactiveRelaisVentIntCh() {
  digitalWrite (relaiPetiteVitesseInt, HIGH);
  digitalWrite (relaiGrandeVitesseInt, HIGH);
}
// l'arret des relais ( HIGH 0 )
void desactiveTousRelais() {
  digitalWrite (relaiVitesseVentExt, HIGH);
  digitalWrite (relaiPetiteVitesseInt, HIGH);
  digitalWrite (relaiGrandeVitesseInt, HIGH);
  digitalWrite (relaiComp, HIGH);
  digitalWrite (relaiV4V, HIGH);
}

