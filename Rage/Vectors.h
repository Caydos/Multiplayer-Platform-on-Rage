#ifndef VECTORS_H
#define VECTORS_H

typedef struct
{
	float x;
	float y;
} Vector2;

typedef struct
{
	float x;
	float y;
	float z;
} Vector3;


typedef struct
{
	float x;
	float y;
	float z;
	float w;
} Vector4;

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

#endif // !VECTORS_H
