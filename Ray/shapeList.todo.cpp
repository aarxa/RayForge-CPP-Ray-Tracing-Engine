#include <Util/exceptions.h>
#include "shapeList.h"
#include "triangle.h"

using namespace Ray;
using namespace Util;

////////////////
// Difference //
////////////////
void Difference::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	THROW( "method undefined" );
}

double Difference::intersect( Util::Ray3D ray , class RayShapeIntersectionInfo &iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	//////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the ray here //
	//////////////////////////////////////////////////////////////////
	THROW( "method undefined" );
	return Infinity;
}

bool Difference::isInside( Util::Point3D p ) const
{
	//////////////////////////////////////////////////////////
	// Determine if the point is inside the difference here //
	//////////////////////////////////////////////////////////
	THROW( "method undefined" );
	return false;
}

///////////////
// ShapeList //
///////////////
double ShapeList::intersect( Ray3D ray , RayShapeIntersectionInfo &iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	//////////////////////////////////////////////////////////////////
	// Compute the intersection of the shape list with the ray here //
	//////////////////////////////////////////////////////////////////
	// THROW( "method undefined" );
	// return Infinity;
	
	
	////////////////////////////// version before acceleration /////////////////////////////////////
	// double inter = Infinity;
	// for(int i = 0; i < shapes.size();i++){
	// 	RayShapeIntersectionInfo newInfo;
	// 	double t = shapes[i]->intersect(ray, newInfo, range, validityLambda);
	// 	if (t < inter){
	// 		inter = t;
	// 		iInfo = newInfo;
	// 	}
	// }
	// return inter;
	///////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////// version after acceleration //////////////////////////////////////
	std::vector< ShapeBoundingBoxHit > all_hits;
	double t, bbox_t, inter;
	double ray_length = ray.direction.length();
	ray.direction = ray.direction.unit();
	int hit_counter = 0;
	inter = Infinity;

	if (_bBox.intersect(ray).isEmpty()) {
		return Infinity;
	}

	// hit all the bounding boxes and  store the hits
	for(int i = 0; i < shapes.size();i++){
		if(shapes[i]->boundingBox().isEmpty()){
			continue;
		}

		bbox_t = shapes[i]->boundingBox().intersect(ray)[0][0];
		if (!shapes[i]->boundingBox().intersect(ray).isEmpty()){
			ShapeBoundingBoxHit hit;
			hit.t = bbox_t;
			hit.shape = shapes[i];	
			all_hits.push_back(hit);
			hit_counter++;		
		}
	}


	if(hit_counter == 0){
		return Infinity;
	}

	// sort hits of bboxes
	std::sort(all_hits.begin(),all_hits.end(),ShapeBoundingBoxHit::Compare);

	// hit the shapes
	for(int i = 0; i < hit_counter; i++){
		double inter = all_hits[i].shape->intersect(ray,iInfo,range,validityLambda);
		if(inter < Infinity){
			return inter;
		}
	}
	return Infinity;
	///////////////////////////////////////////////////////////////////////////////////////////////
}

bool ShapeList::isInside( Point3D p ) const
{
	//////////////////////////////////////////////////////////
	// Determine if the point is inside the shape list here //
	//////////////////////////////////////////////////////////
	THROW( "method undefined" );
	return false;
}

void ShapeList::init( const LocalSceneData &data )
{
	// Initialize the children
	for( int i=0 ; i<shapes.size() ; i++ ) shapes[i]->init( data );

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void ShapeList::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	// THROW( "method undefined" );
	for (int i = 0; i < shapes.size(); i++) {
		shapes[i]->updateBoundingBox();
		// std::cout<<"ShapeList::shapes[i]: "<<shapes[i]->name() << "," << shapes[i]->boundingBox().isEmpty()<<std::endl;
	}
	// std::cout<<"ShapeList::updateBoundingBox"<<std::endl;
	BoundingBox3D temp  = shapes[0]->boundingBox();
	for (int i = 0; i < shapes.size(); i++) {
		temp += shapes[i]->boundingBox();
	}
	_bBox = temp;
}

void ShapeList::initOpenGL( void )
{
	// Initialize the children
	for( int i=0 ; i<shapes.size() ; i++ ) shapes[i]->initOpenGL();

	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE( "remainder of method undefined" );
}

void ShapeList::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

/////////////////
// AffineShape //
/////////////////
double AffineShape::intersect( Ray3D ray , RayShapeIntersectionInfo &iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	//////////////////////////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the affinely deformed shape here //
	//////////////////////////////////////////////////////////////////////////////////////
	// THROW( "method undefined" );
//	 return _shape->intersect( ray , iInfo , range , validityLambda );
	Matrix4D Mi = getInverseMatrix();
	Matrix4D M = getMatrix();
	Matrix4D Mn = getNormalMatrix();
	Ray3D new_ray;
	Point3D position_p;
	Point3D n_p;
	double t;

	new_ray = Mi * ray;

	t = _shape->intersect(Mi * ray, iInfo, range, validityLambda);
	if(t < Infinity){
		// do affine transform on position, M times p
		position_p = M * iInfo.position;
		iInfo.position = M * iInfo.position;
		// do affine transformation on normal, transpose inverse of ML times n
		n_p = Mn * iInfo.normal;
		iInfo.normal = (Mn * iInfo.normal).unit();
	}
	return t;
}

bool AffineShape::isInside( Point3D p ) const
{
	///////////////////////////////////////////////////////////////////////
	// Determine if the point is inside the affinely deformed shape here //
	///////////////////////////////////////////////////////////////////////
	THROW( "method undefined" );
	return _shape->isInside( p );
}

void AffineShape::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	_shape->updateBoundingBox();
	// std::cout<<"AffineShape::updateBoundingBox _shape->boundingBox: "<< _shape->boundingBox() << std::endl;
	// std::cout<<"AffineShape::updateBoundingBox _shape.name: "<< _shape->name() << std::endl;
	_bBox = getMatrix() * (_shape->boundingBox());
	// std::cout<<"AffineShape::updateBoundingBox _bBox: "<< _bBox << std::endl;
}

void AffineShape::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	WARN_ONCE( "method undefined" );
	_shape->drawOpenGL( glslProgram );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

//////////////////
// TriangleList //
//////////////////
double TriangleList::intersect( Ray3D ray , RayShapeIntersectionInfo &iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	////////////////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the triangle list here //
	////////////////////////////////////////////////////////////////////////////
	// THROW( "method undefined" );
	// return Infinity;
	RayShapeIntersectionInfo dummyInfo;
	double inter = Infinity;
	for(int i = 0; i < _shapeList.shapes.size();i++){
		double dummy = _shapeList.shapes[i]->intersect(ray, dummyInfo, range, validityLambda);
		if (dummy < inter){
			inter = dummy;
			iInfo = dummyInfo;
			iInfo.material = _material;
		}
	}
	return inter;
}

void TriangleList::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

void TriangleList::init( const LocalSceneData &data )
{
	// Set the vertex and material pointers
	_vertices = &data.vertices[0];
	_vNum = (unsigned int)data.vertices.size();
	if( _materialIndex>=data.materials.size() ) THROW( "shape specifies a material that is out of bounds: %d <= %d" , _materialIndex , (int)data.materials.size() );
	else if( _materialIndex<0 ) THROW( "negative material index: %d" , _materialIndex );
	else _material = &data.materials[ _materialIndex ];

	_shapeList.init( data );

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void TriangleList::initOpenGL( void )
{
	_shapeList.initOpenGL();

	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

///////////
// Union //
///////////
double Union::intersect( Ray3D ray , RayShapeIntersectionInfo &iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	/////////////////////////////////////////////////////////////
	// Compute the intersection of the union with the ray here //
	/////////////////////////////////////////////////////////////
	THROW( "method undefined" );
	return Infinity;
}

void Union::init( const LocalSceneData &data )
{
	_shapeList.init( data );

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void Union::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	THROW( "method undefined" );
}

bool Union::isInside( Point3D p ) const
{
	/////////////////////////////////////////////////////
	// Determine if the point is inside the union here //
	/////////////////////////////////////////////////////
	THROW( "method undefined" );
	return false;
}

//////////////////
// Intersection //
//////////////////
double Intersection::intersect( Ray3D ray , RayShapeIntersectionInfo &iInfo , BoundingBox1D range , std::function< bool (double) > validityLambda ) const
{
	/////////////////////////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the intersection of shapes here //
	/////////////////////////////////////////////////////////////////////////////////////
	THROW( "method undefined" );
}

void Intersection::init( const LocalSceneData &data )
{
	_shapeList.init( data );

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void Intersection::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	THROW( "method undefined" );
}

bool Intersection::isInside( Point3D p ) const
{
	///////////////////////////////////////////////////////////////////////
	// Determine if the point is inside the instersection of shapes here //
	///////////////////////////////////////////////////////////////////////
	THROW( "method undefined" );
	return false;
}