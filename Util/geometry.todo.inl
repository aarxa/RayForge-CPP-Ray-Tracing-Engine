/*
Copyright (c) 2019, Michael Kazhdan
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

namespace Util
{
	////////////
	// Matrix //
	////////////
	template< unsigned int Dim >
	Matrix< Dim > Matrix< Dim >::Exp( const Matrix &m , int terms )
	{
		//////////////////////////////////////
		// Compute the matrix exponent here //
		//////////////////////////////////////
		THROW( "method undefined" );
		return Matrix();
	}

	template< unsigned int Dim >
	Matrix< Dim > Matrix< Dim >::closestRotation( void ) const
	{
		///////////////////////////////////////
		// Compute the closest rotation here //
		///////////////////////////////////////
		THROW( "method undefined" );
		return Matrix();
	}

	/////////////////
	// BoundingBox //
	/////////////////
	template< unsigned int Dim >
	BoundingBox< 1 > BoundingBox< Dim >::intersect( const Ray< Dim > &ray ) const
	{
		///////////////////////////////////////////////////////////////
		// Compute the intersection of a BoundingBox with a Ray here //
		///////////////////////////////////////////////////////////////
		// THROW( "method undefined" );
		// return BoundingBox<1>();
		Point<Dim> v0 = _p[0];
		Point<Dim> v1 = _p[1];
		Point<Dim> dir = ray.direction;
		Point<Dim> pos = ray.position;
		BoundingBox< 1 > result;
		Point<Dim> all_ones;
		for (int k = 0; k < Dim; k++){
			all_ones[k] = 1.0;
		}
		Point<Dim> dirInv = all_ones/dir;
		double tmin[Dim], tmax[Dim];
		for (int i = 0; i < Dim; i++) {
			tmin[i] = (dirInv[i] >= 0) ? (v0[i]- pos[i])*dirInv[i] : (v1[i]- pos[i])*dirInv[i];
			tmax[i] = (dirInv[i] >= 0) ? (v1[i]- pos[i])*dirInv[i] : (v0[i]- pos[i])*dirInv[i];
		}
		for (int i = 0; i < Dim; i++){
			for (int j =0; j < Dim; j++){
				if (tmin[i] > tmax[j]) {
					result[0] = Infinity;
					result[1] = Infinity;
					return result; // not completely sure  this can create an empty bbox
				}
			}
		}
		result[0][0] = tmin[0];
		for (int i = 1; i < Dim; i++) {
			if (tmin[i] > result[0][0]) {
				result[0][0] = tmin[i];
			}
		}
		result[1][0] = tmax[0];
		for (int i = 1; i < Dim; i++) {
			if (tmax[i] > result[1][0]) {
				result[1][0] = tmax[i];
			}
		}
		return result;
	}	
}