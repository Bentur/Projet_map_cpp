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

	float x;
	float y;
};

vector<Point> points;


pair<float, float> Interpreteur_GPS(float lx, float ly){
	float ro = 6371000.0;
	float x = ro*cos(ly)*(lx-lxref);
    float y = ro*(ly-lyref);
    cout << x << y << endl;

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

		ifst>>lat>>lon>>z;
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

	//calcul moyenne pour avoir la ref #swag
	double sum_lat, sum_lon;
	sum_lat, sum_lon = 0, 0;
	float x, y, lo, la;

	for (auto i = points.begin(); i != points.end(); ++i){
		la = i->lat;
		lo = i->lon;

		sum_lat += la;
		sum_lon += lo;

		//cout << la << " " << sum_lat << " " << lo << " " << sum_lon << " " << endl;

	}
	lxref = sum_lat/(float)points.size(); 
	lyref = sum_lon/(float)points.size();

	cout << points.size() << endl;
	cout<<lxref<<endl;
	cout<<lyref<<endl;

	pair<float, float> q;
	for (auto i = points.begin(); i != points.end(); ++i){
		la = i->lat;
		lo = i->lon;

		q = Interpreteur_GPS(la, lo);
		x = q.first; 
		y = q.second;

		//cout << la << " " << x << " " << lo << " " << y << " " << endl;
	}
	return EXIT_SUCCESS;
}


