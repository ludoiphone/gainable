#include "gpioPin.hpp"
#include "relais.hpp"
#include "hysteresisConsignes.hpp"

using namespace std;

// pins relais
const char relaiComp = 5;
const char relaiV4V = 6;
const char relaiVentUnitExt = 13;
const char relaiVitesseVentExt = 16;
const char relaiVentUnitInt = 19;
const char relaiVitesseVentInt = 20;
const char relaiEteHiver = 26;

bool forceVentIntDegFr = false;

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

