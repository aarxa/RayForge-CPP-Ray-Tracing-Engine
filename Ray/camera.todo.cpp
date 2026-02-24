#include <cmath>
#include <Util/exceptions.h>
#include "camera.h"
#include "shape.h"

using namespace Ray;
using namespace Util;

////////////
// Camera //
////////////

Ray3D Camera::getRay( int i , int j , int width , int height ) const
{
	/////////////////////////////////////////////////
	// Get the ray through the (i,j)-th pixel here //
	/////////////////////////////////////////////////
	
	// Use the camera position as the ray origin
    Point3D rayOrigin = position;
    
    // Calculate the vertical field of view in radians
    // Check the documentation to confirm if heightAngle is in degrees or radians
    float theta_v;
    if (heightAngle > M_PI) { // Likely in degrees if > π
        theta_v = heightAngle * M_PI / 180.0f;
    } else {
        theta_v = heightAngle; // Already in radians
    }
    
    float aspectRatio = static_cast<float>(width) / height;
    // Calculate horizontal field of view
    float theta_h = 2.0f * atan(aspectRatio * tan(theta_v / 2.0f));
    
    // Distance to view plane - try using a larger value to push the view plane further
    // from the camera (making objects appear smaller/further away)
    float d = 1.0f;
    
    // View plane half-size calculations
    float height_half = d * tan(theta_v / 2.0f);
    float width_half = d * tan(theta_h / 2.0f);
    
    // Calculate normalized device coordinates (NDC)
    // This maps pixels to [-1,1] range
    float ndc_x = (2.0f * ((i + 0.5f) / width) - 1.0f) * width_half;
    float ndc_y = (2.0f * ((j + 0.5f) / height) - 1.0f) * height_half;
    
    // Calculate the ray direction using camera basis vectors
    Point3D rayDirection = (d * forward) + (ndc_x * right) + (ndc_y * up);
    rayDirection /= rayDirection.length(); // Normalize
    
    return Ray3D(rayOrigin, rayDirection);
}

void Camera::drawOpenGL( void ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

void Camera::rotateUp( Point3D center , float angle )
{
	///////////////////////////////////////////////////
	// Rotate the camera about the up direction here //
	///////////////////////////////////////////////////
	THROW( "method undefined" );
}

void Camera::rotateRight( Point3D center , float angle )
{
	//////////////////////////////////////////////////////
	// Rotate the camera about the right direction here //
	//////////////////////////////////////////////////////
	THROW( "method undefined" );
}

void Camera::moveForward( float dist )
{
	//////////////////////////////////
	// Move the camera forward here //
	//////////////////////////////////
	THROW( "method undefined" );
}

void Camera::moveRight( float dist )
{
	///////////////////////////////////////
	// Move the camera to the right here //
	///////////////////////////////////////
	THROW( "method undefined" );
}

void Camera::moveUp( float dist )
{
	/////////////////////////////
	// Move the camera up here //
	/////////////////////////////
	THROW( "method undefined" );
}
