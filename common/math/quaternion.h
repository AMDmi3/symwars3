#ifndef QUATERNION_H
#define QUATERNION_H

class Quaternion
{
public:
	Quaternion()	{}
	Quaternion( const float *angles )
	{
		fromAngles( angles );
	}
	Quaternion( const Quaternion& q1, Quaternion& q2, float interp )
	{
		slerp( q1, q2, interp );
	}

	void fromAngles( const float *angles ); // (x,y,z) Euler rotation angles.

	/*
		Set values from an interpolation between two other quaternions.
		This will also modify the second quaternion if it is backwards.
			q1, q2	The quaternions to interpolate between
			interp	A value from 0.0-1.0 indicating the linear interpolation parameter.
	*/
	void slerp( const Quaternion& q1, Quaternion& q2, float interp );

	float operator[]( int index ) const
	{
		return m_quat[index];
	}
	void inverse();

private:
	float m_quat[4];
};

#endif
