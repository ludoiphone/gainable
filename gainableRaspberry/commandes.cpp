
#include "gpioPin.hpp"
#include "commandes.hpp"
#include "hysteresisConsignes.hpp"

const char thermostats = 17;
const char capteurFiltre = 27;

float consigneBlocChauf = 11.0;

bool fonctionFr = false;
bool fonctionCh = false;
bool fonctionCa = false;

void commandeFroid() {
  if (digitalRead (thermostats) == 0) {
    fonctionFr = true;
  } else {
    fonctionFr = false;
  }
}

void commandeChauffage() {
  if (tempExtLue <= consigneBlocChauf && digitalRead (thermostats) == 0) { // si temperature exterieur au NORD est inferieur a 11°C et que un des 5 thermostat est NC (normalement closed (fermer))
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
