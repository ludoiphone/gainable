#ifndef RELAIS_HPP
#define RELAIS_HPP

extern const char relaiComp;
extern const char relaiV4V;
extern const char relaiVentUnitExt;
extern const char relaiVitesseVentExt;
extern const char relaiVentUnitInt;
extern const char relaiVitesseVentInt;
extern const char relaiEteHiver;

extern bool forceVentIntDegFr;

void activeRelaisVentFroid();
void activeRelaisVitesseVentIntChauffage();
void activeRelaisVentExtChauffage();
void activeRelaisVentExtCanicule();
void activeRelaisVentIntCanicule();
void activeRelaisVentEgouttageFr();
void activeRelaisVentEgouttageCh();
void desactiveTousRelais();

#endif
