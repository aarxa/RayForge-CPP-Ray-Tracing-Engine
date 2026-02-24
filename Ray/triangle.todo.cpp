#include <cmath>
#include <Util/exceptions.h>
#include "triangle.h"
#include "scene.h"

using namespace Ray;
using namespace Util;

//////////////
// Triangle //
//////////////

void Triangle::init( const LocalSceneData &data )
{
	// Set the vertex pointers
	for( int i=0 ; i<3 ; i++ )
	{
		if( _vIndices[i]==-1 ) THROW( "negative vertex index: %d" , _vIndices[i] );
		else if( _vIndices[i]>=data.vertices.size() ) THROW( "vertex index out of bounds: %d <= %d" , _vIndices[i] , (int)data.vertices.size() );
		else _v[i] = &data.vertices[ _vIndices[i] ];
	}

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////

	// Set the material pointer (inherited from Shape)
    if (_materialIndex < 0) THROW("negative material index: %d", _materialIndex);
    else if (_materialIndex >= data.materials.size()) THROW("material index out of bounds: %d <= %d", _materialIndex, (int)data.materials.size());
    else _material = &data.materials[_materialIndex];

	//WARN_ONCE( "method undefined" );

	// Matrix3D P;
	// P[0][0] = _v[0]->position[0]; P[0][1] = _v[1]->position[0]; P[0][2] = _v[2]->position[0];
	// P[1][0] = _v[0]->position[1]; P[1][1] = _v[1]->position[1]; P[1][2] = _v[2]->position[1];
	// P[2][0] = _v[0]->position[2]; P[2][1] = _v[1]->position[2]; P[2][2] = _v[2]->position[2];
	// Matrix3D P_inv = P.inverse();
}
void Triangle::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	// THROW( "method undefined" );
	// std::cout<<"_bBox triangle"<<_bBox<<std::endl;

	Point3D p1 = _v[0]->position;
	Point3D p2 = _v[1]->position;
	Point3D p3 = _v[2]->position;

	float x_min = fmin(fmin(p1[0], p2[0]), p3[0]);
	float x_max = fmax(fmax(p1[0], p2[0]), p3[0]);
	float y_min = fmin(fmin(p1[1], p2[1]), p3[1]);
	float y_max = fmax(fmax(p1[1], p2[1]), p3[1]);
	float z_min = fmin(fmin(p1[2], p2[2]), p3[2]);
	float z_max = fmax(fmax(p1[2], p2[2]), p3[2]);


	Point3D p_min = Point3D(x_min, y_min, z_min);
	Point3D p_max = Point3D(x_max, y_max, z_max);

	for( int d=0 ; d<3 ; d++ ) if (p_min[d] == p_max[d]) p_max[d] += Epsilon;

    _bBox = BoundingBox3D(p_min, p_max);
	
	// std::cout<<"_bBox triangle end"<<_bBox<<std::endl;
	
}
void Triangle::initOpenGL( void )
{
	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

Point3D CrossProduct(Point3D p1, Point3D p2){
		Point3D result;
		result[0] = p1[1] * p2[2] - p1[2] * p2[1];
		result[1] = p1[2] * p2[0] - p1[0] * p2[2];
		result[2] = p1[0] * p2[1] - p1[1] * p2[0];
		return result;
}

double Triangle::intersect( Ray3D ray , RayShapeIntersectionInfo& iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	// std::cout<<"I'm inside triangle"<<std::endl;
	/////////////////////////////////////////////////////////////
	// Compute the intersection of the shape with the ray here //
	/////////////////////////////////////////////////////////////
	
    Util::Point3D v1 = _v[0]->position;
    Util::Point3D v2 = _v[1]->position;
    Util::Point3D v3 = _v[2]->position;

    Util::Point3D e1 = v2 - v1;
    Util::Point3D e2 = v3 - v1;
    Util::Point3D normal = CrossProduct(e1, e2).unit();
    double d = normal.dot(v1);

    Util::Point3D p0 = ray.position;
    Util::Point3D v = ray.direction;

    double denominator = normal.dot(v);
    if (fabs(denominator) < Util::Epsilon) return Util::Infinity;

    double t = (d - normal.dot(p0)) / denominator;

    if (t < range[0][0] || t > range[1][0] || !validityLambda(t)) {
        return Infinity;
    }

    Util::Point3D P = p0 + v * t;

    Util::Point3D c1 = CrossProduct(v2 - v1, P - v1);
    Util::Point3D c2 = CrossProduct(v3 - v2, P - v2);
    Util::Point3D c3 = CrossProduct(v1 - v3, P - v3);

    double area = normal.dot(CrossProduct(e1, e2));
    if (fabs(area) < Util::Epsilon) return Util::Infinity;

    double beta = c1.dot(normal) / area;
    double gamma = c2.dot(normal) / area;
    double alpha = 1.0 - beta - gamma;

    if (alpha < 0 || beta < 0 || gamma < 0) return Util::Infinity;

    iInfo.position = P;
    iInfo.normal = normal;
    iInfo.texture = alpha * _v[1]->texCoordinate + beta * _v[2]->texCoordinate + gamma * _v[0]->texCoordinate;
    iInfo.material = _material; // Using the inherited _material

    return t;
	
}

void Triangle::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
