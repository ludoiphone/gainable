#ifndef CONSIGNES_HPP
#define CONSIGNES_HPP

#include <iostream>

extern bool tempFonctionCa;
extern bool tempIntCa;
extern bool tempVitIntCa;
extern bool tempFonctionRef;
extern bool tempRefroidissementUnitExt;
extern bool tempVitExtFr;
extern bool tempVitIntFr;
extern bool tempFonctionCh;
extern bool tempVitExtCh;
extern bool tempVitIntCh;

extern float tempExtLue;
extern float tempUnitIntLue;
extern float tempUnitExtLue;

extern float consigneCanicule;
extern float consigneDelta;
extern float consigneVentIntCa;

void hysteresisFonctionCa();
void hysteresisTempIntCa();
void hysteresisTempVitesseIntCa();
void hysteresisFonctionRef();
void hysteresisTempVitesseExtFroid();
void hysteresisTempVitesseIntFroid();
void hysteresisFonctionCh();
void hysteresisTempVitesseIntChauf();
void hysteresisTempVitesseExtChauf();

#endif
