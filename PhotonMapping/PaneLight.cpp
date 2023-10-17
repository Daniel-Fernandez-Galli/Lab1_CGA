#include "PaneLight.h"
#include "utils.h"
using namespace std;


PaneLight::PaneLight(Vector3 center, float width, float height, Vector3 up, Vector3 right) : center(center), width(width), height(height), up(up), right(right){}

Photon PaneLight::createPhoton() {
	float width = getRandomFloat(- this->width / 2, this->width / 2);
	float heigh = getRandomFloat(- this->height / 2, this->height / 2);
	Vector3 center = this->up * heigh + this->right * width;
	Vector3 dir = math::chooseAPointCosineDistribution(this->norm);
	return Photon(center, dir, this->color, false);
}