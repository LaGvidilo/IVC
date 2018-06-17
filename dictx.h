/*
DictX 1.4.1(One)
Invented By Rick Sanchez
In 2018
On a iMac G3 500Mhz with 256Mb of RAM
*/

/*
Release notes:
Version		Comment
  1.0			First version
  1.1			Load/Write a file (dictx)
  1.2			Terminal de requete DictX
  1.3			Delete from id & Drop table
  1.3.1			Get id
  1.4.0			Update

*/
#ifndef DICTX_H
#define DICTX_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>
#include <sqlite3.h>
#include <map>
#include <list>
#include <fstream>
using namespace std;
void LRstr(char input_dict[],int distanceL,char x_in[10000],int distanceR,char y_out[10000]);
void decoupe(char input_dict[], char input_dict2[],int lentmp);
int counting(string delim);
void affiche_tab(map<int, string> outtab);
const unsigned long hash(const char *str);
class DictX{
	public:
	map <int, string> search(const string table_name, string key);
	void aff_search(const string table_name, string key);
	void load_database(string namefile);
	void delete_id(const string table_name, int ID);
	void insert_from(string table_name, string key, string value);
	void insert_from_by_id(string table_name, string key, string value, int id_code);
	void insert_from_new(string table_name, string key, string value);
	void save_database(const string nom_fichier);
	void drop_table(const string table_name);
	void update(const int ID, const string table_name, string key,string value);
	void create_table(const string table_name);
};
#endif