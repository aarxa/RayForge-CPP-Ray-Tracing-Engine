#include <cmath>
#include <Util/exceptions.h>
#include "directionalLight.h"
#include "scene.h"

using namespace Ray;
using namespace Util;

//////////////////////
// DirectionalLight //
//////////////////////

Point3D DirectionalLight::getAmbient( Ray3D ray , const RayShapeIntersectionInfo& iInfo ) const
{
	////////////////////////////////////////////////////
	// Get the ambient contribution of the light here //
	////////////////////////////////////////////////////
	// Unlike point lights, directional lights don't attenuate with distance
    Point3D K_A = iInfo.material->ambient;
    Point3D I_A = _ambient;
    return K_A * I_A;
    //eturn Point3D();
}

Point3D DirectionalLight::getDiffuse( Ray3D ray , const RayShapeIntersectionInfo& iInfo ) const
{
	////////////////////////////////////////////////////
	// Get the diffuse contribution of the light here //
	////////////////////////////////////////////////////
    // Directional light direction (normalized, opposite to source direction)
    Point3D L = -_direction.unit();  // Light direction (normalized, pointing toward the surface)

    // Normal at intersection point
    Point3D N = iInfo.normal;

    // Diffuse component using rendering equation (Page 8): K_D * <N, L>
    double cosNL = N.dot(L);
    if (cosNL <= 0.0) {
        return Point3D();  // No diffuse if light is behind the surface
    }

    Point3D K_D = iInfo.material->diffuse;
    Point3D I_L = _diffuse;  // Constant intensity (no attenuation, Page 15)

    return K_D * cosNL * I_L;
}

Point3D DirectionalLight::getSpecular( Ray3D ray , const RayShapeIntersectionInfo& iInfo ) const
{
	/////////////////////////////////////////////////////
	// Get the specular contribution of the light here //
	/////////////////////////////////////////////////////
	Point3D L = -_direction.unit();
    Point3D N = iInfo.normal;
    Point3D V = (ray.position - iInfo.position).unit();
    Point3D R = Scene::Reflect(-L, N);
    double cosVR = V.dot(R);
    
    if (cosVR <= 0.0) {
        return Point3D(); // No specular contribution if reflection is away from viewer
    }
    
    Point3D K_S = iInfo.material->specular;
    Point3D I_S = _specular;
    double n = iInfo.material->specularFallOff;
    return K_S * pow(cosVR, n) * I_S;
}

bool DirectionalLight::isInShadow( const RayShapeIntersectionInfo& iInfo , const Shape* shape ) const
{
	//////////////////////////////////////////////
	// Determine if the light is in shadow here //
	//////////////////////////////////////////////
	// For directional light, cast a ray in the opposite direction of the light
    Point3D v = -_direction.unit();
    Point3D p0 = iInfo.position + v * 1e-5; // Offset slightly to avoid self-intersection
    Ray3D ray(p0, v);
    BoundingBox1D range(Epsilon, Infinity);
    RayShapeIntersectionInfo info;
    double t = shape->intersect(ray, info, range);
    
    if (t < Infinity) {
        return true; // Shadow if there's an intersection
    }
    return false;
}

Point3D DirectionalLight::transparency( const RayShapeIntersectionInfo &iInfo , const Shape &shape , Point3D cLimit ) const
{
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////
	Point3D L = -_direction; // Direction to light
    Point3D p0 = iInfo.position + L * 1e-5; // Offset to avoid self-intersection
    Ray3D ray(p0, L);
    Point3D trans = Point3D(1.0, 1.0, 1.0);
    BoundingBox1D range(Epsilon, Infinity);
    RayShapeIntersectionInfo dummy;
    double intersect = shape.intersect(ray, dummy, range);
    while (intersect < Infinity && trans[0] > cLimit[0] && trans[1] > cLimit[1] && trans[2] > cLimit[2]) {
        trans *= dummy.material->transparent;
        Ray3D new_ray = Ray3D(dummy.position + L * 1e-5, L);
        intersect = shape.intersect(new_ray, dummy, range);
    }
    return trans;
}

void DirectionalLight::drawOpenGL( int index , GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
