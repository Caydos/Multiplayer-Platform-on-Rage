#ifndef PED_DATA_H
#define PED_DATA_H
#include "Vectors.h"
#include "Natives.h"






class Pedestrian
{
public:
    Ped Identifier;
    Vector3 Position;
    Vector3 Rotation;
    int ModelHash;
};



#endif // !PED_DATA_H
