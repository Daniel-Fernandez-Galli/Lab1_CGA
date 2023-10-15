#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light {
public:
    void emitPhoton(float& x, float& y, float& z) override;
};

#endif // POINTLIGHT_H
