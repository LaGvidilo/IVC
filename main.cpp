/*
icv: informabox control version
By Rick Sanchez in 2018
Made with an iMac G3 500mhz and with 256Mb of RAM
*/

#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <math.h>
#include <sqlite3.h>
#include <map>

#include <list>
#include <glob.h>
#include <sys/stat.h>

#include "dictx.h"
#include "sqlite3.h"
#include "md5.h"
#include "base64.h"
//#include <curl/curl.h>

using namespace std;
DictX DX;

string get_current_path(){
	char nom_dossier[2048];
	if (getcwd(nom_dossier, sizeof(nom_dossier)) == NULL){
		cout << "ERROR FILE PATH" << endl;
	}
	else{
		strcat(nom_dossier,"/");
	}
	return nom_dossier;
}

bool is_file(const char* path){
	struct stat buf;
	stat(path, &buf);
	return S_ISREG(buf.st_mode);
}

bool is_dir(const char* path){
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}

list<string> globVector(const string& pattern){
	//cout << "Search for: " << pattern << endl;
	glob_t glob_result;
	glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
	list<string> files;
	for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
		files.push_back(string(glob_result.gl_pathv[i]));
	}
	globfree(&glob_result);
	return files;
}

list<string> get_files_names(string name){
	name = name + "*";
	list<string> files = globVector(name);
	return files;
}

string get_data_from_file(string namefile){
	ifstream fichier(namefile.c_str());
	string line;
	string input_dict;
	if (fichier.is_open()){
		while (getline(fichier,line)){
			input_dict = input_dict + line;				
		}
		fichier.close();
	}
	return input_dict;
}

static int callback(void *NotUsed, int argc, char **argv, char**azColName){
	return 0;
}

static int recup_sqlite(void *NotUsed, int argc, char **argv, char**azColName){
	int i;
	for(i=0; i<argc; i++)
	{
		cout << azColName[i]<<" = "<<(argv[i] ? argv[i] : "NULL") << "\n";
	}
	cout << endl;
	return 0;
}

void init(){
	/* INIT VARS BASE */
	string VERSION = "0.1.0a";
	string COMMENT = "Init for this project";
	string nom_dossier;
	nom_dossier = get_current_path();
	cout << "\nCurrent path: " << nom_dossier << endl;
	list<string> fichiers;
	fichiers = get_files_names(nom_dossier);
	string REQ;
	
	unsigned int Hip = hash(nom_dossier.c_str());
	stringstream Hop;
	Hop << Hip;
	string tmp_str = Hop.str();
	cout << "PROJECT:[" << tmp_str << "]" << endl;
	DX.load_database(tmp_str+".dictx");
	/* SQLITE3 INIT */
	/*sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	string dbname = tmp_str+".db";
	rc = sqlite3_open(dbname.c_str(), &db);
	if ( rc ) {
		cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
	}
	
	REQ = "CREATE TABLE FICHIERS(ID INT PRIMARY KEY NOT NULL, HASH TEXT NOT NULL, BASE BLOB NOT NULL)";
	rc = sqlite3_exec(db, REQ.c_str(), callback, 0, &zErrMsg);
	*/


	/* REST OF CODE - INIT DictX */
	DX.create_table(VERSION);
	for (list<string>::iterator it=fichiers.begin(); it!=fichiers.end(); ++it){
		string strval = *it;
		if (strval != nom_dossier + tmp_str + ".dictx"){
		if (is_file(strval.c_str())){
			cout << "...(+)File: " << strval.c_str() << endl;
			
			//DictX
			DX.insert_from_new(VERSION, "file", strval);
			string HASH = md5(get_data_from_file(strval.c_str()));
			DX.insert_from(VERSION, "md5", HASH);
			DX.insert_from(VERSION, "comment", COMMENT);
			
			string DATA = get_data_from_file(strval.c_str());
			string B64 = base64_encode((unsigned char*)DATA.c_str(),strlen(DATA.c_str()));
			DX.insert_from(VERSION, "data", B64);
	
			//SQLITE3
			//string DATA = get_data_from_file(strval.c_str());
			//string B64 = base64_encode((unsigned char*)DATA.c_str(),strlen(DATA.c_str()));
			//string REQ = "INSERT INTO FICHIERS (HASH,BASE) VALUES("+HASH+","+B64+")";
			//rc = sqlite3_exec(db, REQ.c_str(),callback, 0, &zErrMsg);
		}
		if (is_dir(strval.c_str())){
			cout << "Directory(igniored by default): " << strval.c_str() << endl;
		}
		}
	}

	//sqlite3_close(db);
	DX.save_database(tmp_str+".dictx");
}

void add_all(string const VERSION, string COMMENT){
	/* INIT VARS BASE */
	string nom_dossier;
	nom_dossier = get_current_path();
	cout << "\nCurrent path: " << nom_dossier << endl;
	list<string> fichiers;
	fichiers = get_files_names(nom_dossier);
	string REQ;
	
	unsigned int Hip = hash(nom_dossier.c_str());
	stringstream Hop;
	Hop << Hip;
	string tmp_str = Hop.str();
	cout << "PROJECT:[" << tmp_str << "]" << endl;
	DX.load_database(tmp_str+".dictx");
	DX.create_table(VERSION);
	map <int, string> SEA;
	bool ISNOTIN;
	/* REST OF CODE - INIT DictX */
	for (list<string>::iterator it=fichiers.begin(); it!=fichiers.end(); ++it){
		string strval = *it;
		if (strval != nom_dossier + tmp_str + ".dictx"){
			if (is_file(strval.c_str())){
			
				SEA = DX.search(VERSION,(string)"file");
				//cout << "VERIFY EXIST: "<< strval.c_str() << endl;
				ISNOTIN = true;
				for (map<int,string>::iterator it = SEA.begin(); it != SEA.end(); ++it){
					//cout << "#" << it->second.c_str() <<endl;
					if (it->second == strval){
						//cout << "IS EXIST" << endl;
						ISNOTIN = false;
					}
				}
			
				if (ISNOTIN==true){
					cout << "...(+)File: " << strval.c_str() << endl;
					//DictX
					DX.insert_from_new(VERSION, "file", strval);
					string HASH = md5(get_data_from_file(strval.c_str()));
					DX.insert_from(VERSION, "md5", HASH);
					DX.insert_from(VERSION, "comment", COMMENT);
					string DATA = get_data_from_file(strval.c_str());
					string B64 = base64_encode((unsigned char*)DATA.c_str(),strlen(DATA.c_str()));
					DX.insert_from(VERSION, "data", B64);
				}
			
	
			//SQLITE3
			//string DATA = get_data_from_file(strval.c_str());
			//string B64 = base64_encode((unsigned char*)DATA.c_str(),strlen(DATA.c_str()));
			//string REQ = "INSERT INTO FICHIERS (HASH,BASE) VALUES("+HASH+","+B64+")";
			//rc = sqlite3_exec(db, REQ.c_str(),callback, 0, &zErrMsg);
			}
			if (is_dir(strval.c_str())){
				cout << "Directory(igniored by default): " << strval.c_str() << endl;
			}
		}
	}

	//sqlite3_close(db);
	DX.save_database(tmp_str+".dictx");
}



int main (int argc, char * const argv[]) {
	///init();
	//sqlite3 *db;
	//int rc;
	//rc = sqlite3_open("test.db", &db);
	//init();
	//add_all("toncul","toutsimplement");
	if (argc == 1){
		cout << "Usage: icv <command>" << endl << "For see more, type: icv help" << endl;
		return 1;
	}
	if (argc == 2){
		if (string(argv[1]) == "help"){
			cout << "icv 1.0 (informabox control of version) - 2018 GPLv3" << endl;
			cout << "Usage: icv <command> <argument>" << endl << endl;
			cout << "Command:" << endl;
			cout << "init -- Init the project directory" << endl;
			cout << "igniore <filename> -- Igniore a file" << endl;
			cout << "add all <version> <comment> -- Save the version" << endl;
			cout << "reload <version> -- fetch previous version" << endl;
			cout << "revert -- cancel changes" << endl;
			return 1;
		}
		if (string(argv[1]) == "init"){
			init();
		}
	}
	//cout << argc << " : " << argv << endl;	
	if (argc == 5){
		if (string(argv[1]) == "add"){
			if (string(argv[2]) == "all"){
				add_all(argv[3],argv[4]);			
			}
		}
	}
	/*if (argc > 3){
		if (string(argv[1]) == "add"){
			add_precise();
		}
	}*/
	
	
    return 0;
}
