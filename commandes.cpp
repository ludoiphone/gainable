
#include "gpioPin.hpp"
#include "commandes.hpp"

const char thermostats = 17;
const char capteurFiltre = 27;

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
  if (digitalRead (thermostats) == 0) { // si un des 5 thermostat est NC (normalement closed (fermer))
    fonctionCh = true;
  } else {
    fonctionCh = false;
  }
}

void commandeCanicule() {
  if (digitalRead (thermostats) == 0) {
    fonctionCa = true;
  } else {
    fonctionCa = false;
  }
}
