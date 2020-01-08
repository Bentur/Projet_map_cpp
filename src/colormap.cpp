#include "colormap.h"
#include <math.h>
#include <iostream>

using namespace std;

Colormap::Colormap(){

}

void Colormap::getColor(float h, int& r, int& g, int& b){
	/**
	entrée : hauteur h, entre 0 et 1
	sortie : couleur correspondante
	**/
	int c[11][3] = {{37, 57, 175}, {40, 127, 251}, {50, 190, 255}, {106, 235, 255}, {138, 236, 174}, {205, 255, 162}, 
				  {240, 236, 121}, {255, 189, 87}, {255, 161, 68}, {255, 186, 133}, {255, 255, 255}};


	if(h == 0){
		r = c[0][0], g = c[0][1], b = c[0][2];
	} else if(h==1){
		r = c[-1][0], g = c[-1][1], b = c[-1][2];
	}else {
		int i = 11*h;

		r = c[i][0] +  (c[i+1][0]-c[i][0])*h;
		g = c[i][1] +  (c[i+1][1]-c[i][1])*h;
		b = c[i][2] +  (c[i+1][2]-c[i][2])*h;
	
		//cout << h << " " << i << " " << r << " " << g << " " << b << " " << endl;

	}

	
}

void Colormap::getGreyColor(float h, int& r, int& g, int& b){
	/**
	entrée : hauteur h, entre 0 et 1
	sortie : couleur correspondante
	**/

	r = 255 - (int)255*h;
	g = 255 - (int)255*h;
	b = 255 - (int)255*h;

	//cout << h << " " << r << " " << g << " " << b << " " << endl;


}