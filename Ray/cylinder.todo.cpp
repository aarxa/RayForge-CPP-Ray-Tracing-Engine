#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "cylinder.h"

using namespace Ray;
using namespace Util;

//////////////
// Cylinder //
//////////////

void Cylinder::init( const LocalSceneData &data )
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

void Cylinder::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	// Cylinder axis is along y, centered at 'center'
    // Bottom at center[1] - height/2, top at center[1] + height/2, radius 'radius'
    Point3D bottomCenter(center[0], center[1] - height / 2, center[2]);
    Point3D topCenter(center[0], center[1] + height / 2, center[2]);

    // Bounding box encloses the entire cylinder (circular base and top)
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

void Cylinder::initOpenGL( void )
{
	/////////////////////////////////////////
	// Do any necessary OpenGL set-up here //
	/////////////////////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

double Cylinder::intersect( Ray3D ray , RayShapeIntersectionInfo& iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	// From the header, the cylinder's central axis is parallel to the y-axis
    // center is the center point, height is the total height, radius is the radius
    
    // Get ray origin and direction
    Point3D p0 = ray.position;
    Point3D v = ray.direction;
    
    // Calculate top and bottom cap centers
    Point3D topCenter(center[0], center[1] + height/2, center[2]);
    Point3D bottomCenter(center[0], center[1] - height/2, center[2]);
    
    // We need to solve for three possible intersections:
    // 1. The cylinder's curved surface
    // 2. The top cap
    // 3. The bottom cap
    
    double t_surface = Infinity;
    double t_top = Infinity;
    double t_bottom = Infinity;
    
    // 1. Intersection with the cylinder's curved surface
    // For a cylinder along the y-axis, the equation is (x - cx)² + (z - cz)² = r²
    // We ignore the y component for the curved surface calculation
    
    // Quadratic equation coefficients for the curved surface
    // We're essentially intersecting a ray with an infinite cylinder
    double a = v[0]*v[0] + v[2]*v[2];
    
    // If ray is parallel to the y-axis, no intersection with the curved surface
    if (a > Epsilon) {
        double b = 2 * ((p0[0] - center[0]) * v[0] + (p0[2] - center[2]) * v[2]);
        double c = pow(p0[0] - center[0], 2) + pow(p0[2] - center[2], 2) - radius*radius;
        
        double delta = b*b - 4*a*c;
        
        if (delta >= 0) {
            double t1 = (-b - sqrt(delta)) / (2*a);
            double t2 = (-b + sqrt(delta)) / (2*a);
            
            // Check if intersections are within the height range
            if (t1 > 0) {
                Point3D p1 = p0 + v * t1;
                if (p1[1] >= bottomCenter[1] && p1[1] <= topCenter[1]) {
                    t_surface = t1;
                } else if (t2 > 0) {
                    Point3D p2 = p0 + v * t2;
                    if (p2[1] >= bottomCenter[1] && p2[1] <= topCenter[1]) {
                        t_surface = t2;
                    }
                }
            } else if (t2 > 0) {
                Point3D p2 = p0 + v * t2;
                if (p2[1] >= bottomCenter[1] && p2[1] <= topCenter[1]) {
                    t_surface = t2;
                }
            }
        }
    }
    
    // 2. Intersection with the top cap
    if (fabs(v[1]) > Epsilon) {
        double t = (topCenter[1] - p0[1]) / v[1];
        if (t > 0) {
            Point3D p = p0 + v * t;
            double dx = p[0] - topCenter[0];
            double dz = p[2] - topCenter[2];
            if (dx*dx + dz*dz <= radius*radius) {
                t_top = t;
            }
        }
    }
    
    // 3. Intersection with the bottom cap
    if (fabs(v[1]) > Epsilon) {
        double t = (bottomCenter[1] - p0[1]) / v[1];
        if (t > 0) {
            Point3D p = p0 + v * t;
            double dx = p[0] - bottomCenter[0];
            double dz = p[2] - bottomCenter[2];
            if (dx*dx + dz*dz <= radius*radius) {
                t_bottom = t;
            }
        }
    }
    
    // Find the closest valid intersection
    double t = std::min({t_surface, t_top, t_bottom});
    
    // Check if the intersection is within bounds and satisfies the validity function
    if (t != Infinity && t >= range[0][0] && t <= range[1][0] && validityLambda(t)) {
        // Calculate the intersection point
        Point3D p = p0 + v * t;
        
        // Set intersection info
        iInfo.material = _material;
        iInfo.position = p;
        
        // Calculate the normal at the intersection point
        if (t == t_top) {
            // Top cap normal points up
            iInfo.normal = Point3D(0, 1, 0);
        } else if (t == t_bottom) {
            // Bottom cap normal points down
            iInfo.normal = Point3D(0, -1, 0);
        } else {
            // Curved surface normal points outward from the cylinder axis
            Point3D axisPoint(center[0], p[1], center[2]);
            iInfo.normal = (p - axisPoint).unit();
        }
        
        return t;
    }
    
    return Infinity;
}

bool Cylinder::isInside( Point3D p ) const
{
	////////////////////////////////////////////////////////
	// Determine if the point is inside the cylinder here //
	////////////////////////////////////////////////////////
	// Cylinder axis along y, from bottom (center[1] - height/2) to top (center[1] + height/2)
    Point3D bottomCenter(center[0], center[1] - height / 2, center[2]);
    Point3D topCenter(center[0], center[1] + height / 2, center[2]);

    // Check y-bounds first
    double y = p[1];
    if (y < bottomCenter[1] || y > topCenter[1]) return false;

    // Distance from point to cylinder axis in xz-plane
    double dx = p[0] - center[0];
    double dz = p[2] - center[2];
    double dist = sqrt(dx * dx + dz * dz);

    // Point is inside if its distance from the axis is less than the radius
    return dist < radius - Epsilon; // Subtract Epsilon to exclude surface
}

void Cylinder::drawOpenGL( GLSLProgram *glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
