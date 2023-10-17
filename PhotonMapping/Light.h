#ifndef LIGHT_H
#define LIGHT_H

#include "Photon.h"

class Light {
public:
    float potencia; // En Watts
    Color color;
    Light(float potencia, Color color);
    virtual Photon createPhoton() = 0;  // Pure virtual function
};

#endif // LIGHT_H
