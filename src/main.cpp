#include <cstdlib>
#include <iostream>
#include <math.h>
#include <unistd.h> // pour usleep

#include <vector>
#include <fstream>
#include "colormap/palettes.hpp"
#include "colormap/itadpt/map_iterator_adapter.hpp"

using namespace std;
using namespace colormap;

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
    //cout << x << y << endl;

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

bool write_PPM(const string& filename){
	// construct val vector
    vector<float> val;
    double max;
    double min;
    for (auto i = points.begin(); i != points.end(); ++i){
    	val.push_back(i->h);
    	// find the maximum value et min aussi
    	if(i->h < min) min = i->h;
    	else if(i->h > max) max = i->h;

    }

    // get a colormap and rescale it
    auto pal = palettes.at("plasma").rescale(min, max);
    // and use it to map the values to colors
    auto pix = itadpt::map(val, pal);

    //writing the fucking fuck
    ofstream file(filename,ios::out | ios::binary);
    //file.open(filename,ios::out | ios::binary);

    if (!file) {
        cerr << "Cannot open file" << endl;
        return false;
    }
    //header du cul
    bool binary = false;

    int size = points.size()/1000;

    file << "P";
    short h = binary ? 6 : 3;
    file << h << "\n";
    file << size << " ";
    file << size << "\n";
    file << "255" << "\n";

    cout << "salut" << endl;

    decltype(pix.begin()) it(pix.begin());
    cout << "salut salut" << endl;

    //write the bordel
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j, ++it) {
            file << *it;
            file << " ";
        }
        file << '\n';
    }

	file.close();
	cout << "fin writing" << endl;
	return true;
}

void maillage_de_ses_morts(){
	float maille_x = 1; //il n'y a que maille qui m'aie
	float maille_y = 1; //il n'y a que m'ail qui male

	//on recherche le max et le min sur x et sur y
	float x_max, x_min, y_max, y_min;

	for (auto i = points.begin(); i != points.end(); ++i){
		if(i->x < min) x_min = i->x;
    	else if(i->x > max) x_max = i->x;
    	else if(i->y < min) y_min = i->y;
    	else if(i->y > max) y_max = i->y;
	}
	int nb_element_x = (int)(x_max-x_min)/maille_x;
	list<int> nb_x (nb_element_x, 0); //nombre de points (pour la moyenne)
	list<int> hauteur (nb_element_x, 0); //coordonnée en x

	//on réaligne les points en position x
	int x_new;
	for (auto i = points.begin(); i != points.end(); ++i){
		x_new = int((i->x-x_min)/maille_x);
		hauteur[x_new] = (hauteur[x_new]*nb_x[x_new] + i->h)/(nb_x[x_new]+1);
		nb_x[x_new] +=1;
	}
	//de même sur y
}


int main(){
	load_map("map_rascas.txt");

	//calcul moyenne pour avoir la ref #swag
	double sum_lat, sum_lon;
	sum_lat = 0;
	sum_lon = 0;
	float lo, la;

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
		i->x = q.first; 
		i->y = q.second;

		//cout << la << " " << x << " " << lo << " " << y << " " << endl;
	}

	write_PPM("map_rascas_ASCII.ppm");

	return EXIT_SUCCESS;
}


