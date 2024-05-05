#ifndef RELAIS_HPP
#define RELAIS_HPP

extern const char relaiComp;
extern const char relaiV4V;
extern const char relaiVitesseVentExt;
extern const char relaiGrandeVitesseInt;
extern const char relaiPetiteVitesseInt;
extern const char relaiEteHiver;

extern bool forceVentIntDegFr;

void activeRelaisVentInt();
void activeRelaisVentFroid();
void activeRelaisVitesseVentIntChauffage();
void activeRelaisVentExtChauffage();
void activeRelaisVentExtCanicule();
void activeRelaisVentIntCanicule();
void activeRelaisVentEgouttageFr();
void activeRelaisVentEgouttageCh();
void desactiveRelaisVentIntCh();
void desactiveTousRelais();

#endif
