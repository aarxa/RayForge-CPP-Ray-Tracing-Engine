#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "spotLight.h"

using namespace Ray;
using namespace Util;

///////////////
// SpotLight //
///////////////

Point3D SpotLight::getAmbient(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const
{
    ////////////////////////////////////////////////////
    // Get the ambient contribution of the light here //
    ////////////////////////////////////////////////////
    
    Point3D L = _location - iInfo.position; // Vector from point to light
    double distance = L.length();
    Point3D L_dir = L.unit(); // Normalized direction to light
    double cosTheta = L_dir.dot(-_direction); // Cosine of angle between light direction and vector to point

    // Check if point is within the spotlight cone
    if (cosTheta < cos(_cutOffAngle)) {
        return Point3D(); // Outside cone, no contribution
    }

    Point3D K_A = iInfo.material->ambient;
    double spotFactor = pow(cosTheta, _dropOffRate); // Angular attenuation
    Point3D I_A = _ambient * spotFactor / (_constAtten + _linearAtten * distance + _quadAtten * pow(distance, 2.0));
    return K_A * I_A;
}

Point3D SpotLight::getDiffuse( Ray3D ray , const RayShapeIntersectionInfo& iInfo ) const
{
	////////////////////////////////////////////////////
	// Get the diffuse contribution of the light here //
	////////////////////////////////////////////////////
	
    Point3D L = _location - iInfo.position; // Vector from point to light
    double distance = L.length();
    Point3D L_dir = L.unit(); // Normalized direction to light
    double cosTheta = L_dir.dot(-_direction); // Cosine of angle between light direction and vector to point

    // Check if point is within the spotlight cone
    if (cosTheta < cos(_cutOffAngle)) {
        return Point3D(); // Outside cone, no contribution
    }

    Point3D N = iInfo.normal;
    double cosNL = N.dot(L_dir);
    if (cosNL <= 0.0) {
        return Point3D(); // Light behind surface
    }

    Point3D K_D = iInfo.material->diffuse;
    double spotFactor = pow(cosTheta, _dropOffRate); // Angular attenuation
    Point3D I_D = _diffuse * spotFactor / (_constAtten + _linearAtten * distance + _quadAtten * pow(distance, 2.0));
    return K_D * cosNL * I_D;
}

Point3D SpotLight::getSpecular( Ray3D ray , const RayShapeIntersectionInfo& iInfo ) const
{
	/////////////////////////////////////////////////////
	// Get the specular contribution of the light here //
	/////////////////////////////////////////////////////
	// Vector from light position to intersection point
    Point3D L = _location - iInfo.position; // Vector from point to light
    double distance = L.length();
    Point3D L_dir = L.unit(); // Normalized direction to light
    double cosTheta = L_dir.dot(-_direction); // Cosine of angle between light direction and vector to point

    // Check if point is within the spotlight cone
    if (cosTheta < cos(_cutOffAngle)) {
        return Point3D(); // Outside cone, no contribution
    }

    Point3D N = iInfo.normal;
    Point3D V = (ray.position - iInfo.position).unit(); // View direction
    Point3D R = Scene::Reflect(-L_dir, N); // Reflect light direction around normal
    double cosVR = V.dot(R);
    if (cosVR <= 0.0) {
        return Point3D(); // Reflection away from viewer
    }

    Point3D K_S = iInfo.material->specular;
    double spotFactor = pow(cosTheta, _dropOffRate); // Angular attenuation
    Point3D I_S = _specular * spotFactor / (_constAtten + _linearAtten * distance + _quadAtten * pow(distance, 2.0));
    double n = iInfo.material->specularFallOff;
    return K_S * pow(cosVR, n) * I_S;
}

bool SpotLight::isInShadow( const RayShapeIntersectionInfo& iInfo , const Shape* shape ) const
{
	//////////////////////////////////////////////
	// Determine if the light is in shadow here //
	//////////////////////////////////////////////

	// Vector from intersection point to light position
    Point3D L = _location - iInfo.position; // Vector to light
    Point3D L_dir = L.unit();
    double cosTheta = L_dir.dot(-_direction);

    // Check if point is within the spotlight cone
    if (cosTheta < cos(_cutOffAngle)) {
        return false; // Outside cone, no light contribution, so not in shadow
    }

    Point3D p0 = iInfo.position + L_dir * 1e-5; // Offset to avoid self-intersection
    Ray3D ray(p0, L_dir);
    BoundingBox1D range(Epsilon, L.length()); // Range up to light position
    RayShapeIntersectionInfo info;
    double t = shape->intersect(ray, info, range);
    if (t < Infinity) {
        return true; // Occluder found between point and light
    }
    return false;
}

Point3D SpotLight::transparency( const RayShapeIntersectionInfo &iInfo , const Shape &shape , Point3D cLimit ) const
{
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////
	Point3D L = _location - iInfo.position; // Vector to light
    Point3D L_dir = L.unit();
    double cosTheta = L_dir.dot(-_direction);

    // Check if point is within the spotlight cone
    if (cosTheta < cos(_cutOffAngle)) {
        return Point3D(1.0, 1.0, 1.0); // Outside cone, fully transparent (no light contribution)
    }

    Point3D p0 = iInfo.position + L_dir * 1e-5; // Offset to avoid self-intersection
    Ray3D ray(p0, L_dir);
    Point3D trans = Point3D(1.0, 1.0, 1.0);
    BoundingBox1D range(Epsilon, L.length()); // Range up to light position
    RayShapeIntersectionInfo dummy;
    double intersect = shape.intersect(ray, dummy, range);
    while (intersect < Infinity && trans[0] > cLimit[0] && trans[1] > cLimit[1] && trans[2] > cLimit[2]) {
        trans *= dummy.material->transparent;
        Ray3D new_ray = Ray3D(dummy.position + L_dir * 1e-5, L_dir);
        intersect = shape.intersect(new_ray, dummy, range);
    }
    return trans;
}

void SpotLight::drawOpenGL( int index , GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
