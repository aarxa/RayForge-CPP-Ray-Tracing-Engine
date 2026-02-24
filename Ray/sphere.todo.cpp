#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "sphere.h"


using namespace Ray;
using namespace Util;

////////////
// Sphere //
////////////

void Sphere::init( const LocalSceneData &data )
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
void Sphere::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	// THROW( "method undefined" );
	Point3D p(radius, radius, radius), p1, p2;
	p1 = center - p;
	p2 = center + p;
	_bBox = BoundingBox3D(p1,p2);
	// std::cout<<"Bbox for Sphere"<<std::endl;
	// std::cout<<"Bbox for Sphere"<<_bBox<<std::endl;
}
void Sphere::initOpenGL( void )
{
	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

double Sphere::intersect( Ray3D ray , RayShapeIntersectionInfo &iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	//////////////////////////////////////////////////////////////
	// Compute the intersection of the sphere with the ray here //
	//////////////////////////////////////////////////////////////
	// THROW( "method undefined" );
	// return Infinity;
	// std::cout<<"I'm inside sphere intersect"<<std::endl;

	Point3D p0 = ray.position, v = ray.direction, p = p0 - center;
	double a = v.squareNorm(), b = v.dot(p)*2, c = p.squareNorm() - pow(radius,2.0);
	double delta = pow(b,2.0) - 4 * a * c;
	double t1,t2,t;
	if (delta < 0){
		return Infinity;
	}
	else if(delta == 0){
		t1 = -b/(2*a);
		if(t1 > 0){
			t = t1;
		}
		else{
			return Infinity;
		}
	}
	else{
		t1 = (-b - sqrt(delta))/(2*a);
		t2 = (-b + sqrt(delta))/(2*a);
	}
	if(t1 > 0){
		t = t1;
	}
	else if(t2 > 0){
		t = t2;
	}
	else{
		return Infinity;
	}

	double range_l = range[0][0], range_r = range[1][0];
	
	if (t >= range_l && t <= range_r && validityLambda(t)){
		iInfo.material = _material;
		iInfo.position = p0 + v*t;
		iInfo.normal = (iInfo.position - center).unit();
		return t;
	}

	return Infinity;

}

bool Sphere::isInside( Point3D p ) const
{
	//////////////////////////////////////////////////////
	// Determine if the point is inside the sphere here //
	//////////////////////////////////////////////////////
	THROW( "method undefined" );
	return false;
}

void Sphere::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}