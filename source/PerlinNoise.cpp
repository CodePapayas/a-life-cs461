/*
 *  Dillon Stickler - Oregon State University - 2026
 */

#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <cstring>
#include <time.h>

class Vector2d{
	public:
		double x;
		double y;
		Vector2d(){x = 0; y = 0;}
		Vector2d(double n_x, double n_y){x = n_x; y = n_y;}
		double dot(Vector2d other) {return (x * other.x + y * other.y);} 
};

/* 
   Perlin Noise function, 
   adapted from Ken Perlin's "JAVA REFERENCE IMPLEMENTATION OF IMPROVED NOISE",
   https://cs.nyu.edu/~perlin/noise/
   and Raouf Touti's adaptation of Ken Perlin's function,
   https://rtouti.github.io/graphics/perlin-noise-algorithm
*/
class PerlinNoise2d{
	private:
		std::vector<double> p; //permutation table
		int p_table_size = 256;
		
	public:		
		double lerp(double a1, double a2, double t) {return (a1 + t * (a2 - a1));}
		double fade(double t) 						{return ((6.0 * t - 15.0) * t + 10.0) * t * t * t ;}
		
		void shuffle(){
			int shuffle_times = 1;
			for(int s = 0; s < shuffle_times; s++){
				for(int i = p.size()-1; i > 0; i--){
					int index = round(rand() * i-1);
					if(index < 0){std::cout << "ERROR OUT OF BOUNDS" << std::endl;}
					index = index % p.size();
					double temp = p[i];
					
					p[index] = p[i];
					p[i] = temp;
				}
			}
		}
		
		void makePermutation(){
			for(int i = 0; i < p_table_size; i++){
				p.push_back((double)i);
			}
			shuffle();
			
			for(int i = 0; i < p_table_size; i++){
				p.push_back(p[i]);
			}
			
		}
		
		Vector2d toConstantVector(double v){
			int h = (int)fmod(v, 4.0);
			switch (h){
				case 0:
					return Vector2d(1.0, 1.0);
				case 1:
					return Vector2d(-1.0, 1.0);
				case 2:
					return Vector2d(-1.0, -1.0);
				case 3:
					return Vector2d(1.0, -1.0);
				default:
					return Vector2d(0.0, 0.0);
			}
		}
	
		double SampleNoise(double x, double y){
			
			x = x * (1.1);
			y = y * (1.1);

			int X = x;
			X = X % p_table_size;
			int Y = y;
			Y = Y % p_table_size;


			double xf = x - floor(x);
			double yf = y - floor(y);

			Vector2d tr_v = Vector2d(xf - 1.0, yf - 1.0);
			Vector2d tl_v = Vector2d(xf, yf - 1.0);
			Vector2d br_v = Vector2d(xf - 1.0, yf);
			Vector2d bl_v = Vector2d(xf, yf);

			double val_tr = p[p[X + 1] + Y + 1];
			double val_tl = p[p[X] + Y + 1];
			double val_br = p[p[X + 1] + Y];
			double val_bl = p[p[X] + Y];

			double dot_tr = tr_v.dot(toConstantVector(val_tr));
			double dot_tl = tl_v.dot(toConstantVector(val_tl));
			double dot_br = br_v.dot(toConstantVector(val_br));
			double dot_bl = bl_v.dot(toConstantVector(val_bl));

			double u = fade(xf);
			double v = fade(yf);

			double result = lerp(lerp(dot_bl, dot_tl, v),lerp( dot_br, dot_tr, v), u);

			return result;
		}
		
		PerlinNoise2d(){makePermutation();}
};