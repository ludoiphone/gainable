#include "lectureTemps.hpp"
#include "BB_DS18B20.hpp"

using namespace std;

extern BB_DS18B20 * ds18b20;

float tempExtLue;
float tempUnitExtLue;
float tempEcExtLue;
float tempUnitIntLue;
float tempEcIntLue;

vector < unsigned long long > ds_ID;
vector < double > ds_temperature;

void loadDSConfig (string filename, vector < unsigned long long > & array) {
  unsigned long long ds_ID;
  stringstream ss;
  ifstream file (filename);
  if (file.is_open() ) {
    string line;
    while (getline (file, line) ) {
      // enleve espace
      line.erase (remove (line.begin(), line.end(), ' '), line.end() );
      //  avons-nous au moins 17 caracteres
      if (line.size() == 17) {
        // ok avons-nous 28-
        if (line.substr (0, 3) == "28-") {
          stringstream ss (line.substr (3, -1) );
          ss >> hex >> ds_ID;
          ds_ID = (ds_ID << 8) | 0x28;
          array.push_back (
            ds_ID); // valide donc insère ce capteur dans la matrice vector ds_ID
        }
      }
    }
    file.close();
  }
}

void lireDS18B20 () {
  // partir une  conversion

  ds18b20 -> GlobalStartConversion();

  // lire les capteurs
  // l'indentification des DS18B20  sont dans ds_ID

  for (uint loop = 0; loop < ds_ID.size(); loop++) {
    if (ds18b20 -> ReadSensor (ds_ID[loop]) ) {
      // valeur valide
      ds_temperature[loop] = ds18b20 -> temperature;
      tempExtLue = ds_temperature[0];
      tempUnitExtLue = ds_temperature[1];
      tempEcExtLue = ds_temperature[2];
      tempUnitIntLue = ds_temperature[3];
      tempEcIntLue = ds_temperature[4];
    } else {
      ds_temperature[loop] = -9999.9;
    }
  }
}
