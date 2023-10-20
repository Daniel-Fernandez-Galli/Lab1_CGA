#include "PhotonMapper.h"
using namespace raytracing;

PhotonMapper::PhotonMapper(Renderer* renderer) : renderer(renderer) {}


KDTree PhotonMapper::createGlobalIluminationMap(int numberOfPhotons, std::vector<Light*> lights) {
	vector<Photon> photonMap;
	float potenciaTotal = 0;
	for (Light* light : lights) {
		potenciaTotal += light->potencia;
	}
	for (Light* light : lights) {
		int lightNumberOfPhotons = numberOfPhotons * light->potencia / potenciaTotal;
		for (int i = 0; i < lightNumberOfPhotons; i++) {
			Photon photon = light->createPhoton(lightNumberOfPhotons/ numberOfPhotons);
			this->emitPhoton(photon, photonMap);
		}
	}
	return KDTree(photonMap);
}

//Se debe hacer que el Fotón recorra la escena hasta ser absorbido
void PhotonMapper::emitPhoton(Photon photon, vector<Photon>& photonMap) {
	//this->scene.
	Ray ray = {photon.position, photon.direction};
	Hit hit = {};
	hit = this->renderer->cast_ray(ray);

	////Delete
	//photon.position = photon.position + photon.direction * 5;
	//photonMap.push_back(photon);

	if (hit.hasHit) {

		float p = getRandomP();

		//Hacer mas eficiente
		float d_r = hit.material.basecolor.x;
		float d_g = hit.material.basecolor.y;
		float d_b = hit.material.basecolor.z;

		float s_r = hit.material.metallic;
		float s_g = hit.material.metallic;
		float s_b = hit.material.metallic;


		float Pd = math::max(d_r * photon.power.r, d_g * photon.power.g, d_b * photon.power.b) / math::max(photon.power.r, photon.power.g, photon.power.b);
		float Ps = math::max(s_r * photon.power.r, s_g * photon.power.g, s_b * photon.power.b) /math::max(photon.power.r, photon.power.g, photon.power.b);

		if (p < Pd) { // reflexión difusa
			photon.position = hit.intersection;

			if (hit.material.metallic < 1 && !photon.isFirstHit) {
				photon.direction = -1 * photon.direction;
				photonMap.push_back(photon);
			}

			photon.power = Color(photon.power.r * (d_r / Pd), photon.power.g * (d_g / Pd), photon.power.b * (d_b / Pd));
			photon.direction = chooseAPointCosineDistribution(hit.normal);
			photon.isFirstHit = false;

			emitPhoton(photon, photonMap);
		}
		else if (p < Pd + Ps) { // reflexión especular
			bool objectIsTransparent = (hit.material.transmission == 1.0f);
			photon.position = hit.intersection;	
			if (objectIsTransparent) {
				float materialRefractionIndex = 1.450f;
				if(dot_product(-1 * photon.direction, hit.normal) > 0){//Estoy afuera
				

					if (hasTotalInternalRefraction(-1 * photon.direction, hit.normal, 1, materialRefractionIndex)) {//Refracción Total Interna
						photon.direction = math::reflectRay(-1 * ray.dir, hit.normal);
					}
					else {
						photon.direction = calculateT(-1 * photon.direction,hit.normal, 1, materialRefractionIndex);
					}

				}
				else {//Estoy adentro

					if (hasTotalInternalRefraction(-1 * photon.direction, -1 * hit.normal, materialRefractionIndex, 1)) {//Refracción Total Interna
						photon.direction = math::reflectRay(-1 * ray.dir, hit.normal);
					}
					else {
						photon.direction = calculateT(-1 * photon.direction, -1 * hit.normal, materialRefractionIndex, 1);
					}

				}

			}
			else {
				photon.direction = math::reflectRay(-1 * ray.dir, hit.normal);
			}

			photon.power = Color(photon.power.r * (s_r / Ps), photon.power.g * (s_g / Ps), photon.power.b * (s_b / Ps));
			emitPhoton(photon, photonMap);
		}else { //absorción
			if (!photon.isFirstHit) {

				photon.position = hit.intersection;
				photon.direction = -1 * photon.direction;

				photonMap.push_back(photon);
			}
		}
	}
}

KDTree PhotonMapper::createCausticMap(int numberOfPhotons, std::vector<Light*> lights) {
	vector<Photon> photonMap;
	float potenciaTotal = 0;
	for (Light* light : lights) {
		potenciaTotal += light->potencia;
	}
	for (Light* light : lights) {
		potenciaTotal += light->potencia;
		int lightNumberOfPhotons = numberOfPhotons * light->potencia / potenciaTotal;
		for (int i = 0; i < lightNumberOfPhotons; i++) {
			Photon photon = light->createPhoton(lightNumberOfPhotons / numberOfPhotons);
			photon.power = { 255,255,255,255 };
			this->emitCausticPhoton(photon, photonMap);
		}
	}
	return KDTree(photonMap);
}


//Se debe hacer que el Fotón recorra la escena hasta ser absorbido
void PhotonMapper::emitCausticPhoton(Photon photon, vector<Photon>& photonMap) {
	//this->scene.
	Ray ray = { photon.position, photon.direction };
	Hit hit = {};
	hit = this->renderer->cast_ray(ray);

	if (hit.hasHit) {

		float p = getRandomP();

		//Hacer mas eficiente
		float d_r = hit.material.basecolor.x;
		float d_g = hit.material.basecolor.y;
		float d_b = hit.material.basecolor.z;

		float s_r = hit.material.metallic;
		float s_g = hit.material.metallic;
		float s_b = hit.material.metallic;


		float Pd = math::max(d_r * photon.power.r, d_g * photon.power.g, d_b * photon.power.b) / math::max(photon.power.r, photon.power.g, photon.power.b);
		float Ps = math::max(s_r * photon.power.r, s_g * photon.power.g, s_b * photon.power.b) / math::max(photon.power.r, photon.power.g, photon.power.b);
		Pd = 1 - hit.material.transmission;
		Ps = hit.material.transmission;

		if (p < Pd) { // reflexión difusa
			photon.position = hit.intersection;

			if (hit.material.roughness > 0 && photon.hasSpecularReflection && !photon.isFirstHit) {
				photon.direction = -1 * photon.direction;
				photonMap.push_back(photon);
			}

		}
		else if (p < Pd + Ps) { // reflexión especular

			bool objectIsTransparent = (hit.material.transmission == 1.0f);
			photon.position = hit.intersection;
			if (objectIsTransparent) {
				float materialRefractionIndex = 1.450f;
				if (dot_product(-1 * photon.direction, hit.normal) > 0) {//Estoy afuera


					if (hasTotalInternalRefraction(-1 * photon.direction, hit.normal, 1, materialRefractionIndex)) {//Refracción Total Interna
						photon.direction = math::reflectRay(-1 * ray.dir, hit.normal);
					}
					else {
						photon.direction = calculateT(-1 * photon.direction, hit.normal, 1, materialRefractionIndex);
					}

				}
				else {//Estoy adentro

					if (hasTotalInternalRefraction(-1 * photon.direction, -1 * hit.normal, materialRefractionIndex, 1)) {//Refracción Total Interna
						photon.direction = math::reflectRay(-1 * ray.dir, hit.normal);
					}
					else {
						photon.direction = calculateT(-1 * photon.direction, -1 * hit.normal, materialRefractionIndex, 1);
					}

				}

			}
			else {
				photon.direction = math::reflectRay(-1 * ray.dir, hit.normal);
			}
			photon.hasSpecularReflection = true;
			photon.isFirstHit = false;
			//photon.power = Color(photon.power.r * (s_r / Ps), photon.power.g * (s_g / Ps), photon.power.b * (s_b / Ps));
			emitCausticPhoton(photon, photonMap);
		}
	}
}