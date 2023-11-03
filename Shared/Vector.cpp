#include "../Shared/Vector.h"
#include <math.h>



float Vdist(Vector3 _origin, Vector3 _destination)
{
	return sqrt(pow(_origin.x - _destination.x, 2) + pow(_origin.y - _destination.y, 2) + pow(_origin.z - _destination.z, 2));
}

Vector2::Vector2()
{
	x = 0;
	y = 0;
}

Vector2::Vector2(float _x, float _y)
{
	x = _x;
	y = _y;
}

Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3::Vector3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}
