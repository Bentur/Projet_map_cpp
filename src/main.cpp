#include <cstdlib>
#include <iostream>
#include <math.h>
#include <unistd.h> // pour usleep

#include <vector>
#include <list>

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
    auto pal = palettes.at("haxby").rescale(min, max);
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

    unsigned int size = points.size()/1000;

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

bool maillage_de_ses_morts(const string& filename, int largeur, bool binary){


	//on recherche le max et le min sur x et sur y
	float x_max = 0, x_min = 0, y_max = 0, y_min = 1000000;
	float h_max = 0, h_min = 100000;

	for (auto i = points.begin(); i != points.end(); ++i){
		if(i->x < x_min) x_min = i->x;
    	else if(i->x > x_max) x_max = i->x;
    	else if(i->y < y_min) y_min = i->y;
    	else if(i->y > y_max) y_max = i->y;
    	else if(i->h > h_max) h_max = i->h;
    	else if(i->h < h_min) h_min = i->h;
	}

	cout << x_max << "|" << x_min << "|" << y_max << "|" << y_min << "|" << endl; 

	//on en déduit le nombre d'éléments selon x et selon y
	const int nb_element_x = largeur;
	//const int nb_element_y = (int)largeur*(y_max-y_min)/(x_max-x_min);
	const int nb_element_y = largeur*1.1;

	float maille_x = (x_max-x_min)/nb_element_x; //il n'y a que maille qui m'aie
	float maille_y = (y_max-y_min)/nb_element_y; //il n'y a que m'ail qui male

	
	cout << (y_max-y_min) << " " << (x_max-x_min) << endl;
	cout << nb_element_x << "|"  << nb_element_y << "|" << endl;
	cout << maille_x << " " << maille_y << endl;

	//tableau contenant toutes les pairs de chaque ligne (rangé dans un vecteur)
	//on fait un pré-tri pour éviter de parcourir toutes les valeurs à chaque fois
	vector<pair<float, float>> points_y[nb_element_y+1];
	int y_new;

	//on rempli notre vecteur avec les pairs correspondantes
	for (auto i = points.begin(); i != points.end(); ++i){
		y_new = int((i->y-y_min)/maille_y);
		//cout  << y_new << "|" << nb_element_y << endl;
		points_y[y_new].push_back(make_pair(i->x-x_min, i->h));
		//cout << "salut, chat va ? " << endl;
	}

	//ouverture du fichier pour l'écriture
	// get a colormap and rescale it
    auto pal = palettes.at("moreland").rescale(h_min, h_max);
  
    ofstream file(filename,ios::out | ios::binary);
    cout << "écriture dans le fichier : " << filename << endl;

    if (!file) {
        cerr << "Cannot open file" << endl;
        return false;
    }

    //header	
    file << "P";
    short h = binary ? 6 : 3; //si écriture binaire, c'est 6, sinon 3
    file << h << "\n";
    file << nb_element_x << " ";
    file << nb_element_y << "\n";
    file << "255" << "\n";
    
    using color_type = typename std::iterator_traits<ForwardIterator>::value_type; //utile pour l'écriture binaire


	//on parcours notre tableau et on va écrire dans le fichier au fur et à mesure
	//les valeurs du pixmap
	unsigned int nb_total_points = 0;
	for (int i =0; i<nb_element_y+1; i++){

		vector<pair<float, float>> vec = points_y[i];
		//cout << "vec size : " << vec.size() << endl;
		nb_total_points += vec.size();

		int k = 0;
		int x_min_local = 0, x_max_local = maille_x;

		while(x_max_local < x_max-x_min){
			x_min_local = k*maille_x, x_max_local = (k+1)*maille_x;
			int nb_element = 0;
			float hauteur = h_min;
			for (unsigned int j = 0; j < vec.size(); j++){
				
				pair<float, float> p = vec[j];
				
				if(p.first >= x_min_local && p.first < x_max_local){
					hauteur = (p.second + nb_element*hauteur)/(nb_element+1);
					nb_element +=1;
					//cout << "on a trouvé un truc" << endl;
					vec.erase(vec.begin() + j);

				}
			}

			k+=1;
			  // and use it to map the values to colors
			vector<float> vec_h;
			vec_h.push_back(hauteur); 
    		auto pix = itadpt::map(vec_h, pal);
    		decltype(pix.begin()) it(pix.begin());
    		/**
    		unsigned char r, g, b;
    		string s = *it;
    		r = s[0], g = s[1], b = s[2];
			file << r << g << b;
			**/
			if(binary){ //écriture en binaire
				color_type pix = *it;
                pix.write(file);
			}else {
				file << *it;
            	file << " ";
			}
			
			//cout << "_____hauteur calculée : " << i << " " << x_min_local << " " << hauteur << endl;

		}
		file << "\n"; //cette ligne ne semble rien influencer, c'est bizarre, elle me parait pourtant critique ...

	}
	cout << "nombres total de points : " << nb_total_points << endl;
	cout << x_max << "|" << x_min << "|" << y_max << "|" << x_max << "|" << endl; 
	file.close();
	cout << "fin writing" << endl;

	return true;
}


int main(int argc, char* argv[]){
	//récupération des arguments:
	string path;
	int largeur;
	if(argc != 3){
		cout << "problème avec les arguments" << endl;
		cout << "usage : path largeur" << endl;
		exit(0);
	}else{
		path = argv[1];
		largeur = atoi(argv[2]);
	}
	cout << "path : " << path << " | largeur : " << largeur << endl; 

	int b;
	b = load_map(path);
	if(b == -1){
		exit(0);
	}

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

		//cout << la << " " << q.first << " " << lo << " " << q.second << " " << endl;
	}
	maillage_de_ses_morts(path.substr(0, path.size()-4) + "_ASCII.ppm", largeur, true);

	//maillage_de_ses_morts("map_guerledan_ASCII.ppm", 500);

	//write_PPM("map_rascas_ASCII.ppm");

	return EXIT_SUCCESS;
}


