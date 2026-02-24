#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "torus.h"

using namespace Ray;
using namespace Util;

///////////
// Torus //
///////////

void Torus::init( const LocalSceneData &data )
{
	// Set the material pointer
	if( _materialIndex<0 ) THROW( "negative material index: %d" , _materialIndex );
	else if( _materialIndex>=data.materials.size() ) THROW( "material index out of bounds: %d <= %d" , _materialIndex , (int)data.materials.size() );
	else _material = &data.materials[ _materialIndex ];

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );
}
void Torus::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	// Torus is in xy-plane, centered at 'center'
    // Major radius (center to tube center) is (iRadius + oRadius) / 2
    // Minor radius (tube thickness) is (oRadius - iRadius) / 2
    // Total extent in xy is oRadius (outer radius), in z is minorRadius on each side
    double majorRadius = (iRadius + oRadius) / 2.0;
    double minorRadius = (oRadius - iRadius) / 2.0;

    Point3D pMin(
        center[0] - oRadius,
        center[1] - oRadius,
        center[2] - minorRadius
    );
    Point3D pMax(
        center[0] + oRadius,
        center[1] + oRadius,
        center[2] + minorRadius
    );

    // Ensure non-zero volume
    for (int d = 0; d < 3; d++) {
        if (pMin[d] == pMax[d]) pMax[d] += Epsilon;
    }

    _bBox = BoundingBox3D(pMin, pMax);
}

void Torus::initOpenGL( void )
{
	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

double Torus::intersect( Ray3D ray , RayShapeIntersectionInfo &iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	/////////////////////////////////////////////////////////////
	// Compute the intersection of the shape with the ray here //
	/////////////////////////////////////////////////////////////
    
    // A torus is defined by an inner radius and outer radius
    // The inner radius is the distance from the center to the inner edge of the torus
    // The outer radius is the distance from the center to the outer edge
    
    // Transform ray to object space if needed
    Point3D p0 = ray.position;
    Point3D v = ray.direction;
    
    double closestT = Infinity;
    const int maxIterations = 100;
    const double epsilon = 1e-6;
    
    // The major radius is the distance from the center to the middle of the tube
    double majorRadius = (iRadius + oRadius) / 2.0;
    // The minor radius is the radius of the tube itself
    double minorRadius = (oRadius - iRadius) / 2.0;
    
    // Check several points along the ray to find the approximate intersection
    for (double t = range[0][0]; t <= range[1][0]; t += (range[1][0] - range[0][0]) / maxIterations) {
        if (!validityLambda(t)) continue;
        
        Point3D p = p0 + v * t;
        
        // Calculate the distance from the point to the torus
        // First, project the point onto the xy-plane
        double projDist = sqrt(p[0] * p[0] + p[1] * p[1]);
        
        // Calculate the distance from the projected point to the circle with radius majorRadius
        double distToCircle = projDist - majorRadius;
        
        // Calculate the total distance from the point to the torus surface
        double distToSurface = sqrt(distToCircle * distToCircle + p[2] * p[2]) - minorRadius;
        
        // If we're close enough to the surface, refine with binary search
        if (fabs(distToSurface) < epsilon) {
            double lowerT = t - (range[1][0] - range[0][0]) / maxIterations;
            double upperT = t;
            
            // Binary search to refine the intersection point
            for (int i = 0; i < 10; i++) {
                double midT = (lowerT + upperT) / 2.0;
                Point3D midP = p0 + v * midT;
                
                double midProjDist = sqrt(midP[0] * midP[0] + midP[1] * midP[1]);
                double midDistToCircle = midProjDist - majorRadius;
                double midDistToSurface = sqrt(midDistToCircle * midDistToCircle + midP[2] * midP[2]) - minorRadius;
                
                if (fabs(midDistToSurface) < epsilon) {
                    // Found a good intersection
                    if (midT < closestT && validityLambda(midT)) {
                        closestT = midT;
                        iInfo.material = _material;
                        iInfo.position = p0 + v * midT;
                        
                        // Calculate normal at intersection point
                        Point3D p = iInfo.position;
                        double projDist = sqrt(p[0] * p[0] + p[1] * p[1]);
                        
                        if (projDist > 0) {
                            // Calculate normalized direction to the center of the tube
                            double scale = majorRadius / projDist;
                            Point3D centerOfTube(p[0] * scale, p[1] * scale, 0);
                            
                            // The normal points from the center of the tube to the intersection point
                            iInfo.normal = (p - centerOfTube).unit();
                        } else {
                            // Handle the special case where the ray hits exactly on the z-axis
                            iInfo.normal = Point3D(0, 0, p[2] > 0 ? 1 : -1);
                        }
                    }
                }
                
                if (midDistToSurface > 0) {
                    upperT = midT;
                } else {
                    lowerT = midT;
                }
            }
        }
    }
    
    return closestT;
}

bool Torus::isInside( Point3D p ) const
{
	////////////////////////////////////////////////////////
	// Determine if the point is inside the cylinder here //
	////////////////////////////////////////////////////////
	p = p - center;

    double majorRadius = (iRadius + oRadius) / 2.0;
    double minorRadius = (oRadius - iRadius) / 2.0;

    // Distance from point to xy-plane circle of majorRadius
    double xyDist = sqrt(p[0] * p[0] + p[1] * p[1]);
    double distToCircle = xyDist - majorRadius;

    // Distance from point to torus surface
    double distToSurface = sqrt(distToCircle * distToCircle + p[2] * p[2]) - minorRadius;

    // Inside if distance to surface is negative and within minorRadius
    return distToSurface < -Epsilon;
}

void Torus::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
