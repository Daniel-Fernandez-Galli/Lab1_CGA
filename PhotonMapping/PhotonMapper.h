#ifndef PHOTONMAPPER_H
#define PHOTONMAPPER_H

#include "Photon.h"
#include "Light.h"
#include "KDTree.h"
#include "Renderer.h"

using namespace std;

class PhotonMapper {
public:
    Renderer* renderer;
    PhotonMapper(Renderer* renderer);
    void emitPhoton(Photon photon, vector<Photon>& photonMap);
    void emitCausticPhoton(Photon photon, vector<Photon>& photonMap);
    KDTree createGlobalIluminationMap(int numberOfPhotons, std::vector<Light*> lights);
    KDTree createCausticMap(int numberOfPhotons, std::vector<Light*> lights);
};

#endif // PHOTONMAPPER_H