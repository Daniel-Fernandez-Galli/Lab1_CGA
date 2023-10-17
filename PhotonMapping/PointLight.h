#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light {
public:
    Vector3 center;
    PointLight(Vector3 center, float potencia, Color color);
    Photon createPhoton() override;
};

#endif // POINTLIGHT_H
