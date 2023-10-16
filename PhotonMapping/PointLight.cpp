#include "PointLight.h"
#include "utils.h"
using namespace std;

PointLight::PointLight(Vector3 center) : center(center) {}

Photon PointLight::createPhoton() {
	Vector3 dir = chooseARandomPointFromASphere();
	return Photon(this->center, dir, this->color);
}