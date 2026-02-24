#include <cmath>
#include <Util/exceptions.h>
#include "pointLight.h"
#include "scene.h"

using namespace Ray;
using namespace Util;

////////////////
// PointLight //
////////////////

Point3D PointLight::getAmbient( Ray3D ray , const RayShapeIntersectionInfo& iInfo ) const
{
	////////////////////////////////////////////////////
	// Get the ambient contribution of the light here //
	////////////////////////////////////////////////////
	// THROW( "method undefined" );
	// return Point3D();
	Point3D L = _location - iInfo.position;
	double distance = L.length();
	Point3D N = iInfo.normal;
	Point3D K_A = iInfo.material->ambient;
	Point3D I_A = _ambient/(_constAtten + _linearAtten * distance + _quadAtten*pow(distance,2.0));
	return K_A * I_A;
}

Point3D PointLight::getDiffuse( Ray3D ray , const RayShapeIntersectionInfo& iInfo ) const
{
	////////////////////////////////////////////////////
	// Get the diffuse contribution of the light here //
	////////////////////////////////////////////////////
	// THROW( "method undefined" );
	// return Point3D();
	Point3D L = _location - iInfo.position;
	double distance = L.length();
	Point3D N = iInfo.normal;
	double cosNL = N.dot(L.unit());
	if (cosNL <= 0.0){
		return Point3D();
	}
	Point3D K_D = iInfo.material->diffuse;
	Point3D I_D = _diffuse/(_constAtten + _linearAtten * distance + _quadAtten*pow(distance,2.0));
	return K_D * N.dot(L.unit())* I_D;
}

Point3D PointLight::getSpecular( Ray3D ray , const RayShapeIntersectionInfo& iInfo ) const
{
	/////////////////////////////////////////////////////
	// Get the specular contribution of the light here //
	/////////////////////////////////////////////////////
	// THROW( "method undefined" );
	// return Point3D();
	Point3D L = _location - iInfo.position;
	double distance = L.length();
	Point3D N = iInfo.normal;
	Point3D V = (ray.position - iInfo.position).unit();
	Point3D R = Scene::Reflect(-L.unit(), iInfo.normal);
	double cosVR = V.dot(R);
	if (cosVR <= 0.0){
		return Point3D();
	}
	Point3D K_S = iInfo.material->specular;
	Point3D I_S = _specular/(_constAtten + _linearAtten * distance + _quadAtten*pow(distance,2.0));
	double n = iInfo.material->specularFallOff;
	return K_S * pow(V.unit().dot(R),n) * I_S;

}

bool PointLight::isInShadow( const RayShapeIntersectionInfo& iInfo , const Shape* shape ) const
{
	//////////////////////////////////////////////
	// Determine if the light is in shadow here //
	//////////////////////////////////////////////
	// THROW( "method undefined" );
	// return false;
	Point3D v =  (_location - iInfo.position).unit();
	Point3D p0 = iInfo.position  + v * 1e-5;
	Ray3D ray(p0,v);
	BoundingBox1D range( Epsilon , Infinity );
	RayShapeIntersectionInfo info = RayShapeIntersectionInfo();
	double t = shape->intersect(ray, info, range); 
	if (t < Infinity){
		return true;
	}
	return false;
}

Point3D PointLight::transparency( const RayShapeIntersectionInfo &iInfo , const Shape &shape , Point3D cLimit ) const
{
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////
	// THROW( "method undefined" );
	// return Point3D( 1. , 1. , 1. );
	RayShapeIntersectionInfo dummy = RayShapeIntersectionInfo();
	Point3D v = (_location - iInfo.position).unit();
	Point3D p0 = iInfo.position + v * 1e-5;
	Ray3D ray(p0,v);
	Point3D trans = Point3D(1., 1., 1.);
	BoundingBox1D range( Epsilon , Infinity); 
	double intersect = shape.intersect(ray, dummy, range);
	while(intersect < Infinity && trans[0] > cLimit[0] && trans[1] > cLimit[1] && trans[2] > cLimit[2]){
		trans *= dummy.material->transparent;
		Ray3D new_ray = Ray3D(dummy.position + ray.direction * 1e-5, ray.direction);
		intersect = shape.intersect(new_ray, dummy);
	}
	return trans;
}

void PointLight::drawOpenGL( int index , GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

