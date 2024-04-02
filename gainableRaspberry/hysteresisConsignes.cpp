#include "hysteresisConsignes.hpp"

using namespace std;

bool tempFonctionCa = false;
bool tempIntCa = false;
bool tempVitIntCa = false;
bool tempFonctionRef = false;
bool tempRefroidissementUnitExt = false;
bool tempVitExtFr = false;
bool tempVitIntFr = false;
bool tempFonctionCh = false;
bool tempVitIntCh = false;
bool tempVitExtCh = false;

float tempExtLue;
float tempUnitIntLue;
float tempUnitExtLue;

float consigneDelta;
float consigneVentIntCa;

float consigneCanicule = 30.0;
float consigneRefroidissement = 14.0;
float consigneRefroidissementUnitExt = 14.0;
float consigneVitExtFr = 20.0;
float consigneVitIntFr = 23.0;
float consigneChauffage = 11.0;
float consigneVitIntCh = 23.0;
float consigneVitExtCh = 5.0;

float hysteresis = 0.5;
float hysteresis1 = 1.0;
float hysteresisVentExtFr = 3;

void hysteresisFonctionCa() {
  if (tempFonctionCa) {
    tempFonctionCa = tempExtLue <= (consigneCanicule + hysteresis);
  } else {
    tempFonctionCa = tempExtLue <= (consigneCanicule - hysteresis);
  }
}

void hysteresisTempIntCa() {
  if (tempIntCa) {
    tempIntCa = tempUnitIntLue >= (consigneDelta - hysteresis);
  } else {
    tempIntCa = tempUnitIntLue >= (consigneDelta + hysteresis);
  }
}

void hysteresisTempVitesseIntCa() {
  if (tempVitIntCa) {
    tempVitIntCa = tempUnitIntLue >= (consigneVentIntCa - hysteresis);
  } else {
    tempVitIntCa = tempUnitIntLue >= (consigneVentIntCa + hysteresis);
  }
}

void hysteresisFonctionRef() {
  if (tempFonctionRef) {
    tempFonctionRef = tempExtLue <= (consigneRefroidissement + hysteresis1);
  } else {
    tempFonctionRef = tempExtLue <= (consigneRefroidissement - hysteresis1);
  }
}

void hysteresisRefroidissementUnitExt() {
  if (tempRefroidissementUnitExt) {
    tempRefroidissementUnitExt = tempUnitExtLue >= (consigneRefroidissementUnitExt - hysteresis);
  } else {
    tempRefroidissementUnitExt = tempUnitExtLue >= (consigneRefroidissementUnitExt + hysteresis);
  }
}

void hysteresisTempVitesseExtFroid() {
  if (tempVitExtFr) {
    tempVitExtFr = tempUnitExtLue >= (consigneVitExtFr - hysteresisVentExtFr);
  } else {
    tempVitExtFr = tempUnitExtLue >= (consigneVitExtFr); // si la temperature unite Ext est supperieur 20°C
  }
}

void hysteresisTempVitesseIntFroid() {
  if (tempVitIntFr) {
    tempVitIntFr = tempUnitIntLue >= (consigneVitIntFr - hysteresis);
  } else {
    tempVitIntFr = tempUnitIntLue >= (consigneVitIntFr + hysteresis);
  }
}

void hysteresisFonctionCh() {
  if (tempFonctionCh) {
    tempFonctionCh = tempExtLue <= (consigneChauffage + hysteresis);
  } else {
    tempFonctionCh = tempExtLue <= (consigneChauffage - hysteresis);
  }
}

void hysteresisTempVitesseIntChauf() {
  if (tempVitIntCh) {
    tempVitIntCh = tempUnitIntLue >= (consigneVitIntCh - hysteresis);
  } else {
    tempVitIntCh = tempUnitIntLue >= (consigneVitIntCh + hysteresis);
  }
}

void hysteresisTempVitesseExtChauf() {
  if (tempVitExtCh) {
    tempVitExtCh = tempUnitExtLue >= (consigneVitExtCh - hysteresis);
  } else {
    tempVitExtCh = tempUnitExtLue >= (consigneVitExtCh + hysteresis);
  }
}
