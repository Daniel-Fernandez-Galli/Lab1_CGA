#include "PhotonMapper.h"

//Se debe hacer que el Fotón recorra la escena hasta ser absorbido
Photon PhotonMapper::emitPhoton(Photon photon){
	return Photon({ 1.0f, 2.0f, 3.0f }, { 1.0f, 0.0f, 0.0f }, { 255, 0, 0, 255 });
}

std::vector<Photon> PhotonMapper::createGlobalIluminationMap(int numberOfPhotons, std::vector<Light*> lights) {
	vector<Photon> photonMap;
	float potenciaTotal = 0;
	for (Light* light : lights) {
		potenciaTotal += light->potencia;
	}
	for (Light* light : lights) {
		potenciaTotal += light->potencia;
		int lightNumberOfPhotons = numberOfPhotons * light->potencia / potenciaTotal;
		for (int i = 0; i < lightNumberOfPhotons; i++) {
			Photon photon = light->createPhoton();
			photon = this->emitPhoton(photon);
			photonMap.push_back(photon);
		}
	}
	return photonMap;
}