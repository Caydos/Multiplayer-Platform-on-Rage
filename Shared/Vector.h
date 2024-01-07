#ifndef VECTORS_H
#define VECTORS_H

struct Vector2
{
	float x;
	float y;

	Vector2();
	Vector2(float, float);
};


struct Vector3
{
	float x;
	float y;
	float z;
	Vector3();
	Vector3(float _x, float _y, float _z);
};

struct Vector4 
{
	float x;
	float y;
	float z;
	float w;
};

typedef struct
{
	float x;
	char pad0[4];
	float y;
	char pad1[4];
} NativeVector2;


typedef struct
{
	float x;
	char pad0[4];
	float y;
	char pad1[4];
	float z;
	char pad2[4];
} NativeVector3;

typedef struct
{
	float x;
	char pad0[4];
	float y;
	char pad1[4];
	float z;
	char pad2[4];
	float w;
	char pad3[4];
} NativeVector4;

float Vdist(Vector3 _origin, Vector3 _destination);

#endif // !VECTORS_H
