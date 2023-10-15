#ifndef PANELIGHT_H
#define PANELIGHT_H

#include "Light.h"

class PaneLight : public Light {
public:
    void emitPhoton(float& x, float& y, float& z) override;
};

#endif // PANELIGHT_H
