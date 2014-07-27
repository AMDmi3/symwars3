#ifndef MATRIX_H
#define MATRIX_H

#include "../platform.h"

class Quaternion;
class Vec4f;

class Matrix
{
public:
	//	Matrix data, stored in column-major order
	float m_matrix[16];

	Matrix();
	Matrix(Vec4f a, Vec4f b, Vec4f c, Vec4f d);
	~Matrix();

	inline void set( const float *matrix )
	{
		memcpy( m_matrix, matrix, sizeof( float )*16 );
	}

	inline void reset()
	{
		memset( m_matrix, 0, sizeof( float )*16 );
		m_matrix[0] = m_matrix[5] = m_matrix[10] = m_matrix[15] = 1;
	}

	void postmult( const Matrix& matrix );
	void postmult2( const Matrix& matrix );

	//	Set the translation of the current matrix. Will erase any previous values.
	void translation( const float *translation );

	//	Set the inverse translation of the current matrix. Will erase any previous values.
	void invtrans( const float *translation );

	void scale( const float *scale );

	//	Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
	void rotrad( const float *angles );

	//	Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
	void rotdeg( const float *angles );

	//	Make a rotation matrix from a quaternion. The 4th row and column are unmodified.
	void rotquat( const Quaternion& quat );

	//	Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.
	void invrotrad( const float *angles );

	//	Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.
	void invrotdeg( const float *angles );

	inline float* get(int row, int col)
	{
		return &m_matrix[ row + col*4 ];
	}

	//	Translate a vector by the inverse of the translation part of this matrix.
	void inverseTranslateVect( float *pVect );

	//	Rotate a vector by the inverse of the rotation part of this matrix.
	void inverseRotateVect( float *pVect );
};

#endif
