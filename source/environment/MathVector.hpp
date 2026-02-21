/*
 *  Dillon Stickler - Oregon State University - 2026
 */

#pragma once

#ifndef MATHVECTOR_H
#define MATHVECTOR_H

// class for tracking positions, like a <float, float> tuple but with vector math.
// not to be confused with C++ dynamic arrays, aka Vectors<>
// WILL BE REPLACED WHEN OPENGL IS IMPLEMENTED
class Vector2d{
	public:
		double x;
		double y;
		Vector2d(){x = 0; y = 0;}
		Vector2d(double n_x, double n_y){x = n_x; y = n_y;}
		double dot(Vector2d other) {return (x * other.x + y * other.y);} 
};

#endif