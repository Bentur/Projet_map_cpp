#include <cstdlib>
#include <iostream>
#include <math.h>
#include <unistd.h> // pour usleep

#include <vector>
#include <fstream>

using namespace std;

float lxref, lyref;

struct Point{
	float lon;
	float lat;
	float h;
};

vector<Point> points;


pair<float, float> Mesure_GPS(float lo, float la){
    float dd = lo;
    float mm = lo + dd * 100;
    float lat = (dd + mm/60) * M_PI/180;

    dd = la; // 100
    mm = la - dd * 100;
    float lon = (dd + mm/60) * M_PI/180;

    return make_pair(lon, lat);
}
    

pair<float, float> Interpreteur_GPS(float lx, float ly){
	float ro = 6400000;
	float x = ro*cos(ly)*(lx-lxref);
    float y = ro*(ly-lyref);
    return make_pair(x, y);
}
    
int load_map(const string& file_name){
	ifstream ifst(file_name);
	float lon,lat,z;

	if(!ifst.is_open()){
		cout << "Erreur d'ouverture du fichier : " << file_name << endl;
		return -1;
	}

	while (!ifst.eof()){
		if(ifst.eof()) break;

		ifst>>lon>>lat>>z;
		Point p;
		p.lon = lon;
		p.lat = lat; 
		p.h = z;

		points.push_back(p);

	}
	ifst.close();
	return 1;
}


int main(){
	load_map("map_rascas.txt");
	lxref,lyref=points.front().lon, points.front().lat;

	cout<<lxref<<endl;
	cout<<lyref<<endl;
	return EXIT_SUCCESS;
}


