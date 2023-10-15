#ifndef LIGHT_H
#define LIGHT_H

class Light {
public:
    virtual void emitPhoton(float& x, float& y, float& z) = 0;  // Pure virtual function
};

#endif // LIGHT_H
