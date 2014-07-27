#ifndef BARYCENTRIC_H
#define BARYCENTRIC_H


// compute the area of a triangle using Heron's formula
double triarea(double a, double b, double c);

// compute the distance between two 2d points
double dist(double x0, double y0, double z0, double x1, double y1, double z1);

// calculate barycentric coordinates
// triangle 1st vertex: x0,y0,z0
// triangle 2nd vertex: x1,y1,z1
//  triangle 3rd vertex: x2,y2,z2
// point inside triangle: vx, vy,vz
// *u,*v,*w are the coordinates returned
void barycent(double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2,
			  double vx, double vy, double vz,
			  double *u, double *v, double *w);

#endif
