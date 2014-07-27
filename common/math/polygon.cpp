#include "polygon.h"
#include "3dmath.h"
#include "../utils.h"

Polyg::Polyg()
{
    m_drawoutva = NULL;
}

Polyg::~Polyg()
{
    freeva();
}

Polyg::Polyg(const Polyg& original)
{
    *this = original;
}

Polyg& Polyg::operator=(const Polyg& original)
{
    m_drawoutva = NULL;
    m_edv = original.m_edv;
    makeva();

    return *this;
}

void Polyg::makeva()
{
    freeva();
    m_drawoutva = new Vec3f[m_edv.size()];
    int i=0;
    for(auto j=m_edv.begin(); j!=m_edv.end(); i++, j++)
        m_drawoutva[i] = *j;
}

void Polyg::freeva()
{
    if(m_drawoutva)
    {
        delete [] m_drawoutva;
        m_drawoutva = NULL;
    }
}

bool InsidePoly(Vec3f vIntersection, Vec3f Poly[], long verticeCount)
{
    const double MATCH_FACTOR = 0.9999;		// Used to cover up the error in floating point
    double Angle = 0.0;						// Initialize the angle
    Vec3f vA, vB;						// Create temp vectors

    // Just because we intersected the plane, doesn't mean we were anywhere near the polygon.
    // This functions checks our intersection point to make sure it is inside of the polygon.
    // This is another tough function to grasp at first, but let me try and explain.
    // It's a brilliant method really, what it does is create triangles within the polygon
    // from the intersection point.  It then adds up the inner angle of each of those triangles.
    // If the angles together add up to 360 degrees (or 2 * PI in radians) then we are inside!
    // If the angle is under that value, we must be outside of polygon.  To further
    // understand why this works, take a pencil and draw a perfect triangle.  Draw a dot in
    // the middle of the triangle.  Now, from that dot, draw a line to each of the vertices.
    // Now, we have 3 triangles within that triangle right?  Now, we know that if we add up
    // all of the angles in a triangle we get 180° right?  Well, that is kinda what we are doing,
    // but the inverse of that.  Say your triangle is an equilateral triangle, so add up the angles
    // and you will get 180° degree angles.  60 + 60 + 60 is 360°.

    for (int i = 0; i < verticeCount; i++)		// Go in a circle to each vertex and get the angle between
    {
        vA = Vector(Poly[i], vIntersection);	// Subtract the intersection point from the current vertex
        // Subtract the point from the next vertex
        vB = Vector(Poly[(i + 1) % verticeCount], vIntersection);

        Angle += AngleBetweenVectors(vA, vB);	// Find the angle between the 2 vectors and add them all up as we go along
    }

    // Now that we have the total angles added up, we need to check if they add up to 360 degrees.
    // Since we are using the dot product, we are working in radians, so we check if the angles
    // equals 2*PI.  We defined PI in 3DMath.h.  You will notice that we use a MATCH_FACTOR
    // in conjunction with our desired degree.  This is because of the inaccuracy when working
    // with floating point numbers.  It usually won't always be perfectly 2 * PI, so we need
    // to use a little twiddling.  I use .9999, but you can change this to fit your own desired accuracy.

    if(Angle >= (MATCH_FACTOR * (2.0 * M_PI)) )	// If the angle is greater than 2 PI, (360 degrees)
        return true;							// The point is inside of the polygon

    return false;								// If you get here, it obviously wasn't inside the polygon, so Return FALSE
}

bool InterPoly(Vec3f vPoly[], Vec3f vLine[], int verticeCount, Vec3f* vIntersection)
{
    Vec3f vNormal;// = {0};
    float originDistance = 0;

    // First we check to see if our line intersected the plane.  If this isn't true
    // there is no need to go on, so return false immediately.
    // We pass in address of vNormal and originDistance so we only calculate it once

    // Reference   // Reference
    if(!InterPlane(vPoly, vLine,   vNormal,   originDistance))
        return false;

    // Now that we have our normal and distance passed back from InterPlane(),
    // we can use it to calculate the intersection point.  The intersection point
    // is the point that actually is ON the plane.  It is between the line.  We need
    // this point test next, if we are inside the polygon.  To get the I-Point, we
    // give our function the normal of the plan, the points of the line, and the originDistance.

    Vec3f vTemp = IntersectionPoint(vNormal, vLine, originDistance);

    // Now that we have the intersection point, we need to test if it's inside the polygon.
    // To do this, we pass in :
    // (our intersection point, the polygon, and the number of vertices our polygon has)

    if(InsidePoly(vTemp, vPoly, verticeCount))
    {
        if(vIntersection != NULL)
            (*vIntersection) = vTemp;

        return true;
    }

    return false;
}
