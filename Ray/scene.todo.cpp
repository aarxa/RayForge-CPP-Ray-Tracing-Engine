#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"

using namespace Ray;
using namespace Util;

///////////
// Scene //
///////////
Point3D Scene::Reflect( Point3D v , Point3D n )
{
	//////////////////
	// Reflect here //
	//////////////////
	// THROW( "method undefined" );
	// return Point3D();
	return v - n * v.dot(n) * 2;
}

bool Scene::Refract( Point3D v , Point3D n , double ir , Point3D& refract )
{
	//////////////////
	// Refract here //
	//////////////////
	// THROW( "method undefined" );
	// return false;
	double cos_in = -v.dot(n);
	double sin_in = sqrt(1 - cos_in * cos_in);
	double refract_ratio; // yita_i  / yita_r
	double cos_r, sin_r;
	// Point3D refra_T;
	if (cos_in < 0){ 
		// from material to air
		refract_ratio = ir;
		sin_r = sin_in * refract_ratio;
		if (sin_r > 1){ return false;}
		cos_r = sqrt(1 - pow(sin_r, 2.0));
		refract = v * refract_ratio + (-n) * (-refract_ratio * cos_in - cos_r);
		if(!isnan(refract[0]) && !isnan(refract[1] && !isnan(refract[2]))){
			return true;
		}
	} 
	else{
		// from air to material
		refract_ratio =  1 / ir;
		sin_r = sin_in * refract_ratio;
		if (sin_r > 1){ return false;}
		cos_r = sqrt(1 - pow(sin_r, 2.0));
		refract = v * refract_ratio + n * (refract_ratio * cos_in - cos_r);
		if(!isnan(refract[0]) && !isnan(refract[1] && !isnan(refract[2]))){
			return true;
		}
	}
	return false;
}

bool false_func(double d)
{
	return true;
}
int max_recursion = 0;

Point3D Scene::getColor( Ray3D ray , int rDepth , Point3D cLimit )
{
	////////////////////////////////////////////////
	// Get the color associated with the ray here //
	////////////////////////////////////////////////
	// std::cout<<"inside getcolor"<<std::endl;
	// THROW( "method undefined" );
	// return Point3D(0,0,0);
	// std::cout<<"rDepth: "<<rDepth<<std::endl;
	if (max_recursion < rDepth) { max_recursion = rDepth;} 
	BoundingBox1D range( Epsilon , Infinity );
	RayShapeIntersectionInfo iInfo = RayShapeIntersectionInfo();
	Point3D color(0,0,0);
	Point3D transparent;
	rDepth--;
	double t = intersect(ray,iInfo,range,false_func);
	
	if ( (rDepth == max_recursion-1) && t == Infinity) {return Point3D(0, 0, 0);}

	if(t < Infinity){
		color += iInfo.material->emissive;
		for (int i = 0; i < _globalData.lights.size(); i++){
			color += _globalData.lights[i]->getAmbient(ray, iInfo); 
			// if(_globalData.lights[i]->isInShadow(iInfo, this) == false){
			transparent = _globalData.lights[i]->transparency(iInfo, *this, cLimit);
			color += _globalData.lights[i]->getDiffuse(ray, iInfo) * transparent;
			color += _globalData.lights[i]->getSpecular(ray, iInfo) * transparent;
			// }
			
			if(iInfo.material->tex){
				double w = (double) (iInfo.material->tex->_image.width());
				double h = (double) (iInfo.material->tex->_image.height());
				// texture coordinates
				Point2D p_tex = iInfo.texture;
				p_tex[0] *= (w - 1);
				p_tex[1] *= (h - 1);
				if(p_tex[0] - floor(p_tex[0]) < 1e-9 && p_tex[1] - floor(p_tex[1]) < 1e-9 ){
					// no need for bilinear interpolation
					double r = iInfo.material->tex->_image(p_tex[0], p_tex[1]).r / 256.0;
					double g = iInfo.material->tex->_image(p_tex[0], p_tex[1]).g / 256.0;
					double b = iInfo.material->tex->_image(p_tex[0], p_tex[1]).b / 256.0;
					Point3D T(r,g,b);
					color *= T;
				}
				else{
					// need for bilinear interpolation
					double u = p_tex[0], v = p_tex[1];
					int u1 = floor(u), v1 = floor(v), u2 = u1 + 1, v2 = v1 + 1;
					double du = u - u1, dv = v - v1;
					Image::Pixel32 u1v1, u1v2, u2v1, u2v2;
					
					u1v1 = iInfo.material->tex->_image(u1, v1);
					u1v2 = iInfo.material->tex->_image(u1, v2);
					u2v1 = iInfo.material->tex->_image(u2, v1);
					u2v2= iInfo.material->tex->_image(u2, v2);
					double a_r = u1v1.r * (1 - du) + u2v1.r * du;
					double a_g = u1v1.g * (1 - du) + u2v1.g * du;
					double a_b = u1v1.b * (1 - du) + u2v1.b * du;

					double b_r = u1v2.r * (1 - du) + u2v2.r * du;
					double b_g = u1v2.g * (1 - du) + u2v2.g * du;
					double b_b = u1v2.b * (1 - du) + u2v2.b * du;

					double r = a_r * (1 - dv) + b_r * dv;
					double g = a_g * (1 - dv) + b_g * dv;
					double b = a_b * (1 - dv) + b_b * dv;
					Point3D T(r,g,b);
					T /= 256;
					color *= T;
				}
				
			}
				
		}


		// reflect
		Point3D v = ray.direction, n = iInfo.normal;
		Point3D reflect_dir = Reflect(v,n);
		Point3D reflect_pos = iInfo.position + reflect_dir * 1e-5;
		Ray3D reflect_ray(reflect_pos, reflect_dir);
		Point3D K_S = iInfo.material->specular;
		// std::cout<<"iInfo.material->specular"<< K_S <<std::endl;
		if (rDepth > 0 && K_S[0] > cLimit[0] && K_S[1] > cLimit[1] && K_S[2] > cLimit[2] ){
			color += getColor(reflect_ray, rDepth-1, cLimit/K_S) * K_S;
		}

		// refract
		Point3D refract_dir(0,0,0);
		Point3D K_T = iInfo.material->transparent;
		double ir = iInfo.material->ir;
		if(Refract(v,n,ir,refract_dir) == true){
			// refract_dir = ray.direction;
			Point3D refract_pos = iInfo.position + refract_dir * 1e-5;
			Ray3D refract_ray(refract_pos, refract_dir);
			if(rDepth > 0 && K_T[0] > cLimit[0] && K_T[1] > cLimit[1] && K_T[2] > cLimit[2]){
				color += getColor(refract_ray, rDepth-1, cLimit/K_T) * K_T;
			}
		}

		for (int c = 0; c < 3; c++) {
			if (color[c] > 1) {color[c] = 1; }
			if (color[c] < 0) {color[c] = 0; }
		}	
 	}
		
	return color;
}

//////////////
// Material //
//////////////
void Material::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

/////////////
// Texture //
/////////////
void Texture::initOpenGL( void )
{
	///////////////////////////////////
	// Do OpenGL texture set-up here //
	///////////////////////////////////
	THROW( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
