#include "../platform.h"

// compute the area of a triangle using Heron's formula
double triarea(double a, double b, double c)
{
	double s = (a + b + c)/2.0;
	double area=sqrt(fabs(s*(s-a)*(s-b)*(s-c)));
	return area;
}

// compute the distance between two 2d points
double dist(double x0, double y0, double z0, double x1, double y1, double z1)
{
	double a = x1 - x0;
	double b = y1 - y0;
	double c = z1 - z0;
	return sqrt(a*a + b*b + c*c);
}

// calculate barycentric coordinates
// triangle 1st vertex: x0,y0,z0
// triangle 2nd vertex: x1,y1,z1
//  triangle 3rd vertex: x2,y2,z2
// point inside triangle: vx, vy,vz
// *u,*v,*w are the coordinates returned
void barycent(double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2,
			  double vx, double vy, double vz,
			  double *u, double *v, double *w)
{
	// compute the area of the big triangle
	double a = dist(x0, y0, z0, x1, y1, z1);
	double b = dist(x1, y1, z1, x2, y2, z2);
	double c = dist(x2, y2, z2, x0, y0, z0);
	double totalarea = triarea(a, b, c);

	// compute the distances from the outer vertices to the inner vertex
	double length0 = dist(x0, y0, z0, vx, vy, vz);
	double length1 = dist(x1, y1, z1, vx, vy, vz);
	double length2 = dist(x2, y2, z2, vx, vy, vz);

	// divide the area of each small triangle by the area of the big triangle
	*u = triarea(b, length1, length2)/totalarea;
	*v = triarea(c, length0, length2)/totalarea;
	*w = triarea(a, length0, length1)/totalarea;
}

#if 0
void main()
{
	double r1,r2,r3; // barycentric coordinates
	barycent(t1.x, t1.y, t1.z, t2.x, t2.y, t2.z, t3.x, t3.y, t3.z, firstPoint.x, firstPoint.y, firstPoint.z, &r1, &r2, &r3);
	Point3d newPoint(T1.x*r1 + T2.x*r2 + T3.x*r3, T1.y*r1 + T2.y*r2 + T3.y*r3, T1.z*r1 + T2.z*r2 + T3.z*r3); // T1,T2,T3 are the coordinates of another triangle
}
#endif
