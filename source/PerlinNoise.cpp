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

enum eDEBUG{
	PERLIN = 0,
	PERMUTATION,
	SAMPLING,
	NONE
};

eDEBUG DEBUG = NONE;

int w = 256;
int h = 256;
char* img = NULL;

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
		void printPermutation(){
			for(int i = 0; i < p.size(); i++){
				if(i == 0){std::cout << p[i];}
				else {std::cout << ", " << p[i];}
			}
			std::cout << std::endl;
		}
		
		void shuffle(){
			srand (time(NULL));
			if (DEBUG == PERMUTATION) {std::cout << "Shuffling..." << std::endl;}
			int shuffle_times = 1;
			for(int s = 0; s < shuffle_times; s++){
				if (DEBUG == PERMUTATION) {std::cout << "Shuffling..." << std::endl;}
				for(int i = p.size()-1; i > 0; i--){
					int index = round(rand() * i-1);
					if(index < 0){std::cout << "ERROR OUT OF BOUNDS" << std::endl;}
					index = index % p.size();
					double temp = p[i];
					
					p[index] = p[i];
					p[i] = temp;
				}
			}
			if (DEBUG == PERMUTATION) {printPermutation();}
		}
		
		void makePermutation(){
			if (DEBUG == PERMUTATION) {std::cout << "Generating initial permutation table..." << std::endl;}
			for(int i = 0; i < p_table_size; i++){
				p.push_back((double)i);
			}
			if (DEBUG == PERMUTATION) {printPermutation();}
			shuffle();
			
			if (DEBUG == PERMUTATION) {std::cout << "Generating shuffled permutation table..." << std::endl;}
			for(int i = 0; i < p_table_size; i++){
				p.push_back(p[i]);
			}
			
			if (DEBUG == PERMUTATION) {printPermutation();}
		}
		
		Vector2d toConstantVector(double v){
			int h = (int)fmod(v, 4.0);
			if (h == 0) 		{return Vector2d(1.0, 1.0);}	
			else if (h == 1) 	{return Vector2d(-1.0, 1.0);}
			else if (h == 2) 	{return Vector2d(-1.0, -1.0);}
			else 		     	{return Vector2d(1.0, -1.0);}
		}
	
		double SampleNoise(double x, double y){
			if(DEBUG == PERLIN){
				std::cout << "Sampling noise at (" << x << ", " << y << ")..." << std::endl;
			}
			
			x = x * (1.1);
			y = y * (1.1);
			
			int X = (int)x % 255;
			int Y = (int)y % 255;
			
			double xf = x - floor(x);
			double yf = y - floor(y);
			
			if(DEBUG == PERLIN){
				std::cout << "Floored vector: (" << xf << ", " << yf << ")..." << std::endl;
			}
			
			Vector2d tr_v = Vector2d(xf - 1.0f, yf - 1.0f);
			Vector2d tl_v = Vector2d(xf, yf - 1.0f);
			Vector2d br_v = Vector2d(xf - 1.0f, yf);
			Vector2d bl_v = Vector2d(xf, yf);
			
			double val_tr = p[p[X+1]+Y+1];
			double val_tl = p[p[X]+Y+1];
			double val_br = p[p[X+1]+Y];
			double val_bl = p[p[X]+Y];
	
			
			double dot_tr = tr_v.dot(toConstantVector(val_tr));
			double dot_tl = tl_v.dot(toConstantVector(val_tl));
			double dot_br = br_v.dot(toConstantVector(val_br));
			double dot_bl = bl_v.dot(toConstantVector(val_bl));
		
			double u = xf;//fade(xf);
			double v = yf;//fade(yf);
			
			if(DEBUG == PERLIN){
				std::cout << "Values: " << val_tr << ", " << val_tl << ", " << val_br << ", " << val_bl << std::endl;
				std::cout << "Dots: " << dot_tr << ", " << dot_tl << ", " << dot_br << ", " << dot_bl << std::endl;
				std::cout << "Lerps: " << lerp(v, dot_bl, dot_tl) << ", "  << lerp(v, dot_br, dot_tr) << std::endl;
			}
			
			double result = lerp(u,
			lerp(v, dot_bl, dot_tl),lerp(v, dot_br, dot_tr));
			
			if(DEBUG == PERLIN){
				std::cout << "Result: " << result << std::endl;
				std::cout << "Done sampling noise!" << std::endl;	
			}
			
			//result = p[x] + p[y];
			
			return result;
		}
		
		PerlinNoise2d(){makePermutation();}
};

/*
	ALL BMP-WRITING CODE WAS YOINKED FROM:
	https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries
	by users Morgoth and deusmacabre
*/

void make_BMP_Header(FILE* f, int filesize) {
	unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
	unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);

	bmpinfoheader[4] = (unsigned char)(w);
	bmpinfoheader[5] = (unsigned char)(w >> 8);
	bmpinfoheader[6] = (unsigned char)(w >> 16);
	bmpinfoheader[7] = (unsigned char)(w >> 24);
	bmpinfoheader[8] = (unsigned char)(h);
	bmpinfoheader[9] = (unsigned char)(h >> 8);
	bmpinfoheader[10] = (unsigned char)(h >> 16);
	bmpinfoheader[11] = (unsigned char)(h >> 24);
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);
}

void writeBMP(char* img) {
	FILE* f;
	fopen_s(&f, "img.bmp", "wb");

	unsigned char bmppad[3] = { 0,0,0 };
	int filesize = 54 + 3 * w * h;
	make_BMP_Header(f, filesize);
	for (int i = 0; i < h; i++)
	{
		fwrite(img + (w * (h - i - 1) * 3), 3, w, f);
		fwrite(bmppad, 1, (4 - (w * 3) % 4) % 4, f);
	}

	free(img);
	fclose(f);
}


int main(){
	
	srand (time(NULL));
	
	std::cout << "Hello World!" << std::endl;
	PerlinNoise2d perlin = PerlinNoise2d();
	
	img = (char *)malloc(3 * w * h);
	memset(img, 0, 3 * w * h);
	
	for(int x = 0; x < w; x++){
		for(int y = 0; y < h; y++){
			double freq = 0.01;
			double amp = 200.0;
			int octaves = 2;
			double sample = 0;
			for(int o = 0; o < octaves; o++){
				sample += amp * (perlin.SampleNoise(x * freq, y * freq));
				amp *= 0.5;
				freq *= 2.0;
			}

			int color = sample;
			if(color > 255) {color = 255;}
			else if(color < 0) {color = 0;}
			int pos = (x + (y * w)) * 3;
			img[pos + 2] 	= (unsigned char)color;
			img[pos + 1] 	= (unsigned char)color;
			img[pos] 		= (unsigned char)color;			
			if(DEBUG == SAMPLING){std::printf("%0.3f \t", amp * (sample + 1.0));}
		}
		if(DEBUG == SAMPLING){std::cout << std::endl;}
	}
	std::cout << "Done!" << std::endl;

	
	writeBMP(img);
	
	return 0;
};