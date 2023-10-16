#ifndef PHOTONMAPPER_H
#define PHOTONMAPPER_H

#include "Photon.h"
#include "Light.h"
#include "KDTree.h"

using namespace std;

class PhotonMapper {
public:
    Photon emitPhoton(Photon photon);  // Pure virtual function
    KDTree createGlobalIluminationMap(int numberOfPhotons, std::vector<Light*> lights);
    KDTree createCausticMap(int numberOfPhotons, std::vector<Light*> lights);
};

#endif // PHOTONMAPPER_H