#ifndef PANELIGHT_H
#define PANELIGHT_H

#include "Light.h"

class PaneLight : public Light {
public:
    Vector3 center; //center of the pane
    Vector3 norm;//cross product between up and right
    Vector3 up;//v1 of the plane
    Vector3 right;//v2 of the plane

    float width;
    float height;

    PaneLight(float potencia, Color color, Vector3 center, float width, float height, Vector3 up, Vector3 right, Vector3 norm);
    Photon createPhoton(float potencia) override;
};

#endif // PANELIGHT_H
