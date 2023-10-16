#ifndef PHOTONMAPPER_H
#define PHOTONMAPPER_H

#include "Photon.h"
#include "Light.h"
using namespace std;

class PhotonMapper {
public:
    Photon emitPhoton(Photon photon);  // Pure virtual function
    std::vector<Photon> createGlobalIluminationMap(int numberOfPhotons, std::vector<Light*> lights);
    std::vector<Photon> createCausticMap(int numberOfPhotons, std::vector<Light*> lights);
};

#endif // PHOTONMAPPER_H