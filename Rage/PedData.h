#ifndef PED_DATA_H
#define PED_DATA_H
#include "Vectors.h"
#include "Natives.h"






class SyncPed
{
public:
    std::int64_t player;
    Ped Identifier;
    Vector3 Position;
    Vector3 Rotation;
    int ModelHash;
};



#endif // !PED_DATA_H
