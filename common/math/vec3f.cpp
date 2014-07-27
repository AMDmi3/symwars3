#include "../platform.h"
#include "vec3f.h"
#include "matrix.h"
#include "physics.h"

void Vec3f::transform( const Matrix& m )
{
	double vector[4];
	const float *matrix = m.m_matrix;

	vector[0] = x*matrix[0]+y*matrix[4]+z*matrix[8]+matrix[12];
	vector[1] = x*matrix[1]+y*matrix[5]+z*matrix[9]+matrix[13];
	vector[2] = x*matrix[2]+y*matrix[6]+z*matrix[10]+matrix[14];
	vector[3] = x*matrix[3]+y*matrix[7]+z*matrix[11]+matrix[15];

	x = vector[0];
	y = vector[1];
	z = vector[2];
}

void Vec3f::transform3( const Matrix& m )
{
	double vector[3];
	const float *matrix = m.m_matrix;

	vector[0] = x*matrix[0]+y*matrix[4]+z*matrix[8];
	vector[1] = x*matrix[1]+y*matrix[5]+z*matrix[9];
	vector[2] = x*matrix[2]+y*matrix[6]+z*matrix[10];

	x = vector[0];
	y = vector[1];
	z = vector[2];
}

bool Close(Vec3f a, Vec3f b)
{
	if(fabs(a.x - b.x) <= CLOSE_EPSILON && fabs(a.y - b.y) <= CLOSE_EPSILON && fabs(a.z - b.z) <= CLOSE_EPSILON)
		return true;

	return false;
}
