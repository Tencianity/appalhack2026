#ifndef MATERIAL_H
#define MATERIAL_H

#include "scene/color.h"

typedef struct {
    RGBA color;
    float density;
    float elasticity;
    float ref_index;
    float attenuation;
} Mat;


#endif
