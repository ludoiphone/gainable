#ifndef LECTURETEMPS_HPP
#define LECTURETEMPS_HPP

#include <vector>
#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//extern float tempExtLue;
//extern float tempUnitExtLue;
extern float tempEcExtLue;
//extern float tempUnitIntLue;
extern float tempEcIntLue;

extern vector < unsigned long long > ds_ID;
extern vector < double > ds_temperature;

void loadDSConfig (string filename, vector < unsigned long long > & array);
void lireDS18B20 ();

#endif
