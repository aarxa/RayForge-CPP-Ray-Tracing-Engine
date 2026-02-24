#include <cmath>
#include <Util/exceptions.h>
#include <Util/polynomial.h>
#include "scene.h"
#include "cone.h"

using namespace Ray;
using namespace Util;

//////////
// Cone //
//////////

void Cone::init( const LocalSceneData &data )
{
	// Set the material pointer
	if( _materialIndex<0 ) THROW( "negative material index: %d" , _materialIndex );
	else if( _materialIndex>=data.materials.size() ) THROW( "material index out of bounds: %d <= %d" , _materialIndex , (int)data.materials.size() );
	else _material = &data.materials[ _materialIndex ];

	//////////////////////////////////
	// Do any necessary set-up here //
	//////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void Cone::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	
    // Cone axis is along y, centered at 'center'
    // Base at center[1] - height/2, radius 'radius'
    // Tip at center[1] + height/2
    Point3D baseCenter(center[0], center[1] - height / 2, center[2]);
    Point3D tip(center[0], center[1] + height / 2, center[2]);

    // Bounding box encloses the base (circle of radius) and tip
    Point3D pMin(
        center[0] - radius,
        center[1] - height / 2,
        center[2] - radius
    );
    Point3D pMax(
        center[0] + radius,
        center[1] + height / 2,
        center[2] + radius
    );

    // Ensure non-zero volume (add Epsilon if any dimension is zero)
    for (int d = 0; d < 3; d++) {
        if (pMin[d] == pMax[d]) pMax[d] += Epsilon;
    }

    _bBox = BoundingBox3D(pMin, pMax);
}

void Cone::initOpenGL( void )
{
	/////////////////////////////////////////
	// Do any necessary OpenGL set-up here //
	/////////////////////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

double Cone::intersect( Ray3D ray , RayShapeIntersectionInfo &iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	// From the header, the cone's central axis is parallel to the y-axis
    // center is the center of the cone, height is from tip to base, radius is at the base
    
    // Get ray origin and direction
    Point3D p0 = ray.position;
    Point3D v = ray.direction;
    
    // Calculate the tip of the cone (highest point)
    Point3D tip(center[0], center[1] + height/2, center[2]);
    // Calculate the center of the base
    Point3D baseCenter(center[0], center[1] - height/2, center[2]);
    
    // Calculate the ratio of radius to height (tangent of the cone angle)
    double tanTheta = radius / height;
    double tanTheta2 = tanTheta * tanTheta;
    
    // Translate the ray origin relative to the cone tip
    Point3D p = p0 - tip;
    
    // Coefficients for the quadratic equation
    // For a cone along the y-axis, we have:
    // (x^2 + z^2) = (y * tan(theta))^2 for points on the cone surface
    
    double a = v[0]*v[0] + v[2]*v[2] - tanTheta2 * v[1]*v[1];
    double b = 2 * (p[0]*v[0] + p[2]*v[2] - tanTheta2 * p[1]*v[1]);
    double c = p[0]*p[0] + p[2]*p[2] - tanTheta2 * p[1]*p[1];
    
    // Calculate discriminant
    double delta = b*b - 4*a*c;
    
    // No intersection if discriminant is negative
    if (delta < 0) {
        return Infinity;
    }
    
    // Calculate intersection times
    double t1, t2, t;
    
    if (delta == 0) {
        // One solution (tangent)
        t1 = -b / (2*a);
        if (t1 <= 0) {
            return Infinity;
        }
        t = t1;
    } else {
        // Two solutions
        t1 = (-b - sqrt(delta)) / (2*a);
        t2 = (-b + sqrt(delta)) / (2*a);
        
        // Find the first positive intersection
        if (t1 > 0) {
            t = t1;
        } else if (t2 > 0) {
            t = t2;
        } else {
            return Infinity;
        }
    }
    
    // Check if the intersection is within the bounds
    if (t >= range[0][0] && t <= range[1][0] && validityLambda(t)) {
        // Calculate the intersection point
        Point3D intersection = p0 + v * t;
        
        // Check if the intersection is within the height bounds of the cone
        double y = intersection[1];
        if (y < baseCenter[1] || y > tip[1]) {
            // Try the other intersection if available
            if (delta > 0 && t == t1 && t2 > 0) {
                t = t2;
                intersection = p0 + v * t;
                y = intersection[1];
                
                if (y < baseCenter[1] || y > tip[1]) {
                    return Infinity;
                }
            } else {
                return Infinity;
            }
        }
        
        // Also check for intersection with the base of the cone
        if (fabs(y - baseCenter[1]) < Epsilon) {
            double dx = intersection[0] - baseCenter[0];
            double dz = intersection[2] - baseCenter[2];
            double distSquared = dx * dx + dz * dz;
            if (distSquared > radius * radius) return Infinity;
        }
        
        // Set the intersection info
        iInfo.material = _material;
        iInfo.position = intersection;
        
        // Calculate the normal at the intersection point
        // For a point on the cone surface, the normal is perpendicular to both
        // the cone surface and the line from the tip to the point
        
        // Check if we hit the base
        if (fabs(intersection[1] - baseCenter[1]) < Epsilon) {
            // Normal is pointing down for the base
            iInfo.normal = Point3D(0, -1, 0);
        } else {
            // Normal for the cone surface
            // The normal is perpendicular to the cone surface
            // We can compute it as the gradient of the cone equation
            double h = tip[1] - intersection[1];
            double r = sqrt(pow(intersection[0] - tip[0], 2) + pow(intersection[2] - tip[2], 2));
            
            // Vector from tip to intersection projected onto xz-plane
            Point3D projVector(intersection[0] - tip[0], 0, intersection[2] - tip[2]);
            if (r > 0) {
                projVector = projVector / r;
            }
            
            // Compute the normal (need to consider the cone angle)
            iInfo.normal = Point3D(
                projVector[0] * height,
                radius,
                projVector[2] * height
            ).unit();
        }
        
        return t;
    }
    
    return Infinity;
}

bool Cone::isInside( Point3D p ) const
{
	///////////////////////////////////////////////////
	// Determine if the point is inside the box here //
	///////////////////////////////////////////////////
	THROW( "method undefined" );
	return false;
}

void Cone::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
