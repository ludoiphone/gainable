#include "vitesseVentilateurs.hpp"
#include "lectureTemps.hpp"

using namespace std;

bool tempVitIntCa = false;
bool tempVitExtFr = false;
bool tempVitIntFr = false;
bool tempVitIntCh = false;
bool tempVitExtCh = false;

float consigneVentIntCa;

float consigneVitExtFr = 20.0;
float consigneVitIntFr = 23.0;
float consigneVitIntCh = 23.0;
float consigneVitExtCh = 5.0;

float hysteresis = 0.5;
float hysteresis1 = 1.0;
float hysteresisVentExtFr = 3;

void hysteresisTempVitesseIntCa() {
  if (tempVitIntCa) {
    tempVitIntCa = tempUnitIntLue >= (consigneVentIntCa - hysteresis);
  } else {
    tempVitIntCa = tempUnitIntLue >= (consigneVentIntCa + hysteresis);
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

void hysteresisTempVitesseIntChauf() {
  if (tempVitIntCh) {
    tempVitIntCh = tempUnitIntLue <= (consigneVitIntCh + hysteresis);
  } else {
    tempVitIntCh = tempUnitIntLue <= (consigneVitIntCh - hysteresis);
  }
}

void hysteresisTempVitesseExtChauf() {
  if (tempVitExtCh) {
    tempVitExtCh = tempUnitExtLue >= (consigneVitExtCh - hysteresis);
  } else {
    tempVitExtCh = tempUnitExtLue >= (consigneVitExtCh + hysteresis);
  }
}
