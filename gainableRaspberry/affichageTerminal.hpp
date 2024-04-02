#ifndef AFFICHAGETERMINAL_HPP
#define AFFICHAGETERMINAL_HPP

#include <iostream>

extern bool canicule;
extern bool entretienFiltre;

extern unsigned long long nettoyageFiltre;

extern unsigned short compteurV4V; 
extern unsigned short compteurComp;
extern unsigned short compteurDegFr;
extern unsigned short compteurEgouttageFr;
extern unsigned short compteurFinEgouttageFr;
extern unsigned short compteurCompCh;
extern unsigned short compteurDegElec;
extern unsigned short compteurFinDegTemperature;
extern unsigned short compteurFinDegTemps;
extern unsigned short compteurDegNat;
extern unsigned short compteurEgouttageNat;
extern unsigned short compteurFinEgouttageNat;
extern unsigned short compteurV4VDegElec;
extern unsigned short compteurCompDegElec;
extern unsigned short compteurEgouttageCh;
extern unsigned short compteurFinEgouttageCh;
extern unsigned short compteurV4VCa;
extern unsigned short compteurCompCa;
extern unsigned short compteurDegCa;
extern unsigned short compteurEgouttageCa;
extern unsigned short compteurFinEgouttageCa;
extern unsigned short compteurArretProgramme;

void affichageTerminal();

#endif
