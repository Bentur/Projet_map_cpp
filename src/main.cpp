#include <cstdlib>
#include <iostream>
#include <math.h>
#include <unistd.h> // pour usleep

#include <vector>

#include "Cell.h"
#include "vibes.h"
#include "Path.h"
#include <fstream>

using namespace std;

//structure maze
struct Maze {

	Cell *cell_start;
	Cell *cell_finish;

};

void save_cell(Cell *cell, ofstream *f){
	*f << *cell;
	*f << (cell->m_nb_neighb) << " ";
	cell->m_serialised = true;

	for(int j = 0; j< cell->m_nb_neighb; j++){
		*f << *(cell->m_neighb[j]);

	}
	*f << endl; 

	for(int j = 0; j < cell->m_nb_neighb; j++){
		if(!cell->m_neighb[j]->m_serialised)
			save_cell(cell->m_neighb[j], f);
	}
}
void save_maze(Maze& maze, const string& file_name){
	ofstream ofst(file_name); // ouverture en écriture

	ofst << "# start: " << endl;
	ofst << *(maze.cell_start) << endl;
	ofst << "# End: " << endl;
	ofst << *(maze.cell_finish) << endl;
	ofst << "# Cells: " << endl;

	save_cell(maze.cell_start, &ofst);

	ofst.close();
}

pair<int,int> convert_coords(string coords)
{
	coords = coords.substr(1, coords.size()-2);
	int x = stoi(coords.substr(0, coords.find(',')));
	int y = stoi(coords.substr(coords.find(',')+1, coords.size()));
	return make_pair(x, y);
}


Maze load_maze(const string& file_name){
	cout << "Loading Maze" << endl;

	Maze maze;

	map<pair<int,int>,Cell*> m_cells;

	ifstream ifst(file_name); // ouverture en écriture

	if(!ifst.is_open()){
		cout << "Erreur d'ouverture du fichier : " << file_name << endl;
		return maze;
	}
	cout << "File found : " << file_name << endl;

	string s1, s2, s3, s4;
	getline(ifst, s1);
	getline(ifst, s2);
	getline(ifst, s3);
	getline(ifst, s4);
	getline(ifst, s1);

	pair<int,int> startCellCoords = convert_coords(s2);
	pair<int,int> stopCellCoords = convert_coords(s4);

	cout << "Maze initialisation OK" << endl;

	while(!ifst.eof()) {

		Cell *c1 = new Cell(0, 0);
		int i; //nombre de voisins
		ifst >> *c1; //lecture de la cellule courante

		if(ifst.eof())
		  break;

		ifst >> i; //lecture du nombre de voisin(s)

		int x = c1->m_x;
		int y = c1->m_y;

		cout << "Compute new Cell : " << *c1 << endl;

		//on vérifie si cette cell a déja été créee
		if(m_cells.find(make_pair(x, y)) == m_cells.end()){
			m_cells[make_pair(x, y)] = c1;
			cout << "create cell " << *c1 << endl;
		} else{
			delete c1;
			c1 = m_cells.find(make_pair(x, y))->second;
		}

		for(int j = 0; j<i; j++){
			//lecture du voisin
			Cell *c2 = new Cell(0, 0);
			ifst >> *c2;
			int x2 = c2->m_x;
			int y2 = c2->m_y;

			//si le voisin n'est pas dans le dictionnaire
			if(m_cells.find(make_pair(x2, y2)) == m_cells.end()){
				m_cells[make_pair(x2, y2)] = c2;
				cout << "create cell " << *c2 << endl;

				cout << "add neighbor : " << endl;
				cout << *c2 <<endl;
				c1->add_neighb(c2);

			} else {
				delete c2;
				cout << "add neighbor : " << endl;
				Cell *c = m_cells.find(make_pair(x2, y2))->second;
				cout << *m_cells.find(make_pair(x2, y2))->second << endl;
				c1->add_neighb(c);
			}

			cout << "fin add neighbor" << endl;
		}

		cout << endl;

	}
	cout << "fin de création des cells " << endl;
	cout << "ajout des cell_start et cell_finish " << endl;
	maze.cell_start = m_cells.find(startCellCoords)->second;
	maze.cell_finish = m_cells.find(stopCellCoords)->second;
	
	ifst.close();

	return maze;
}


void draw_gate(const Cell *n1, const Cell *n2){
	vibes::drawBox(min(n1->m_x, n2->m_x) + 0.1, 
		max(n1->m_x, n2->m_x) + 0.9,
		min(n1->m_y, n2->m_y) + 0.1, 
		max(n1->m_y, n2->m_y) + 0.9,"lightGray[lightGray]");
}

void display_cell(Cell *cell){
	cout << "displaying cell " << *cell <<endl;
	vibes::drawBox(cell->m_x, cell->m_x + 1, cell->m_y, cell->m_y + 1, "[lightGray]");
	cell->m_displayed = true;
	for(int i = 0 ; i < cell->m_nb_neighb ; i++){

		cout << "displaying neighbor cell : " << endl;
		cout << *cell->m_neighb[i] <<endl;

		if(!cell->m_neighb[i]->m_displayed) 
			display_cell(cell->m_neighb[i]);
		draw_gate(cell, cell->m_neighb[i]);
	}
}

bool find_path(Cell *c, Cell *cf, Path *path){
	//cas de base
	if(c->m_x == cf->m_x && c->m_y == cf->m_y){
		return true;
	}
	cout << *c <<endl;
	bool trouve = false;
	c->m_checked = true;

	//on parcours tous les voisins
	for(int i =0; i<c->m_nb_neighb && !trouve; i++){
		

		//on regarde si la cellule voisine a déjà été visitée, dans quel cas on bouclerait :
		if(!c->m_neighb[i]->m_checked){
			path->add_to_path(c->m_neighb[i]);
			trouve = find_path(c->m_neighb[i], cf, path->m_next);
		}
		
	}

	return trouve;
}


Maze create_maze(){
	Maze maze;

	
	//Création des cellules
	Cell *c1 = new Cell(0,0);
	Cell *c2 = new Cell(1,0);
	Cell *c3 = new Cell(2,0);
	Cell *c4 = new Cell(3,0);
	Cell *c5 = new Cell(0,1);
	Cell *c6 = new Cell(1,1);
	Cell *c7 = new Cell(2,1);
	Cell *c8 = new Cell(3,1);
	Cell *c9 = new Cell(0,2);
	Cell *c10 = new Cell(1,2);
	Cell *c11 = new Cell(2,2);
	Cell *c12 = new Cell(3,2);
	Cell *c13 = new Cell(0,3);
	Cell *c14 = new Cell(1,3);
	Cell *c15 = new Cell(2,3);
	Cell *c16 = new Cell(3,3);

	//Définition des voisins de chaque cellule
	c1->add_neighb(c2);
	c2->add_neighb(c3);
	c3->add_neighb(c4);
	c4->add_neighb(c8);
	c8->add_neighb(c12);
	c12->add_neighb(c16);
	c16->add_neighb(c15);
	c15->add_neighb(c11);
	c11->add_neighb(c10);
	c10->add_neighb(c6, c14);
	c6->add_neighb(c7);
	c14->add_neighb(c13);
	c13->add_neighb(c9);
	c9->add_neighb(c5);

	maze.cell_start = c1;
	maze.cell_finish = c5;
	
	/**
	cells[0]->add_neighb(cells[1]);
	cells[1]->add_neighb(cells[2]);
	cells[2]->add_neighb(cells[3]);
	cells[3]->add_neighb(cells[7]);
	cells[7]->add_neighb(cells[11]);
	cells[11]->add_neighb(cells[15]);
	cells[15]->add_neighb(cells[14]);
	cells[14]->add_neighb(cells[10]);
	cells[10]->add_neighb(cells[9]);
	cells[9]->add_neighb(cells[5]);
	cells[5]->add_neighb(cells[6]);

	cells[9]->add_neighb(cells[13]);
	cells[13]->add_neighb(cells[12]);
	cells[12]->add_neighb(cells[8]);
	cells[8]->add_neighb(cells[4]);
	/**
	cells[0].add_neighb(cells[1]);
	cells[1].add_neighb(cells[2]);
	cells[2].add_neighb(cells[3]);
	cells[3].add_neighb(cells[7]);
	cells[7].add_neighb(cells[11]);
	cells[11].add_neighb(cells[15]);
	cells[15].add_neighb(cells[14]);
	cells[14].add_neighb(cells[10]);
	cells[10].add_neighb(cells[9]);
	cells[9].add_neighb(cells[5]);
	cells[5].add_neighb(cells[6]);

	cells[9].add_neighb(cells[13]);
	cells[13].add_neighb(cells[12]);
	cells[12].add_neighb(cells[8]);
	cells[8].add_neighb(cells[4]);
	
	maze.cell_start = cells[0];
	maze.cell_finish = cells[4];
	**/


	return maze;
}

int main(){
	
	const int line = 4; //nb ligne
	const int column = 4; //nb colonne

	//création du maze
	//Maze maze = create_maze();
	Maze maze = load_maze("salut.txt");
	cout << "fin de loading du maze" << endl;

	//initialisation de vibes
	vibes::beginDrawing(); // initialisation de VIBes
	vibes::newFigure("Maze"); // cr ́eation d’une figure
	vibes::axisLimits(0-0.5, line+0.5, 0-0.5, column+0.5);
	
	//affichage du labyrinthe
	display_cell(maze.cell_start);
	cout << "fin affichage des cells" << endl;
		//affichage depart et fin:
	vibes::drawCircle(maze.cell_start->m_x + 0.5, maze.cell_start->m_y +0.5 , 0.3, "[red]");
	vibes::drawCircle(maze.cell_finish->m_x + 0.5, maze.cell_finish->m_y +0.5 , 0.3, "[blue]");

	cout << "enregistrement du maze" << endl;
	save_maze(maze, "salut.txt");
	cout << "fin d'enregistrement du maze" << endl;

	cout <<  "recherche du cheming" << endl;
	Path* path = new Path(maze.cell_start);
	find_path(maze.cell_start, maze.cell_finish, path);
	path->display_path();

	//delete path;
	
	return EXIT_SUCCESS;
}


