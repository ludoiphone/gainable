#ifndef VITESSEVENTILATEURS_HPP
#define VITESSEVENTILATEURS_HPP

#include <iostream>

extern bool tempVitIntCa;
extern bool tempVitExtFr;
extern bool tempVitIntFr;
extern bool tempVitExtCh;
extern bool tempVitIntCh;

extern float consigneVentIntCa;

void hysteresisTempVitesseIntCa();
void hysteresisTempVitesseExtFroid();
void hysteresisTempVitesseIntFroid();
void hysteresisTempVitesseIntChauf();
void hysteresisTempVitesseExtChauf();

#endif
