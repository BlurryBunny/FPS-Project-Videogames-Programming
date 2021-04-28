#ifndef FILES_H
#define FILES_H
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

class File
{
private:
	string name;
	ifstream arch; //The file variable to open the file

	vector<glm::vec3> buffer_vertex; // My buffer contains 
	vector<glm::vec3>buffer_normals_vertex;

	vector<int>buffer_id_vertex;
	vector<int>buffer_id_normal_vertex;
	
public:
	File() {//an empty constructor

	}

	void setfileName(string name) {//This method get the file name, in a later part it will contain the obj files for the proyect
		this->name = name;
	}

	bool open(void) {
		bool resp = true;
		this->arch.open(name, ios::in);//Open the file
		if (arch.fail()) { //Checks if the file opened correctly
			resp = false;
		}
		return resp;
	}

	//A method to separe the string in substrings
	//The resultant strings save in a vector of strings
	vector<string> split(std::string line, std::string subString) {

		std::string auxline;
		std::vector <string> values;
		char char_separation;

		//Se elimina la subcadena de la cadena principal
		while (subString.size() > 1) {
			while (line.find(subString) < line.size())
			{
				line.erase(line.begin() + line.find(subString));
			}
			subString.erase(subString.begin() + (subString.size() - 1));
		}

		char_separation = subString[0];
		stringstream lineStream(line);

		while (getline(lineStream, auxline, char_separation)) {
			values.push_back(auxline);
		}

		return values;

	}

	//This method reads from the file and save it into the proper objects
	bool saveInfo() {

		bool res = true;
		vector<string> val;//We have this so we can read the file with out spaces
		string text;//String to copy all the text
		
		bool resp = true/*We have this bool so we can be certain that the file is being read*/, object_found = false;//If there is more than one object in a on obj file we have this boolean to serve as flag
		while (!arch.eof() && resp)//We do the cycle while the file has something in it
		{
			getline(arch, text); //Save one line of the text
			val = split(text, " ");//We use this method to have a vector of every string in the file per line without the blank spaces
			if (val.size())//If the vector has members
			{
				/*
				Our objs files were modify to identify with the first element of the split.
					v -> Vertex
					n -> Normals
					f -> Faces

				Also our faces were traingulate so each face have the format: 

				f v/vt/vn v/vt/vn v/vt/vn

				where:
						v -> Id Vertex
						vt -> texture.
						vn -> Id normal vertex.

				 For now we only use the values of v and vn.
				*/
				if (val[0] == "o" && object_found == false) {//When we encounter the object name for the first time
					
					object_found = true;
				}
				
				else if (val[0] == "v") {
					buffer_vertex.push_back(glm::vec3(stof(val[1]), stof(val[2]), stof(val[3])));
				}
				else if (val[0] == "f")//When we encounter a face
				{
					for (size_t i = 1; i < val.size(); i++) {
						vector<string> aux_val = split(val[i], "//");

						if (!aux_val.empty()) {
							try {
								buffer_id_vertex.push_back(stoi(aux_val[0]));
								buffer_id_normal_vertex.push_back(stoi(aux_val[1]));
							}
							catch (exception e) {
								cout << "Error" << endl;
							}
						}
					}

				}
				else if (val[0] == "n") {
					buffer_normals_vertex.push_back(glm::vec3(stof(val[1]), stof(val[2]), stof(val[3])));
				}
			}
			else {
				res = false;
			}
		}

		arch.close();//Close the archive
		return res;
	}

	//Getters
	string getName(void) {
		return this->name;
	}

	int get_size_bufferIdVer() {
		return buffer_id_vertex.size();
	}

	glm::vec3 getVertex(int id) {
		return buffer_vertex[id];
	}

	glm::vec3 getNormalVertex(int id) {
		return buffer_normals_vertex[id];
	}

	int getIdVertex(int index) {
		int res = 1;
		try
		{
			res = buffer_id_vertex[index];
		}
		catch (const std::exception&)
		{
			cout << "Error try to get id Vertex" << endl;
		}
		return res;
	}

	int getIdNormalVertex(int index) {
		
		int res = 1;

		try
		{
			res = buffer_id_normal_vertex[index];
		}
		catch (const std::exception&)
		{
			cout << "Error try to get id normal Vertex" << endl;
		}
		return res;
	}

	int total_elements_Vertex() {
		//Vertex	Normal	Color	NormalFace		Light Pos
		//x,y,z		x,y,z	r,g,b	x,y,z			  x,y,z
		return (3+3+3+3+3);
	}

	//ToString
	void toString() {
		cout << "------------- Vertices -------------" << endl;
		for (size_t i = 0; i < buffer_vertex.size(); i++)
		{
			cout <<"V #"<<i+1<< "	x: " << buffer_vertex[i][0] << "	y: " << buffer_vertex[i][1] << "	z: " << buffer_vertex[i][2] << endl;
		}

		int counter = 0;
		cout << "\nF: ";
		for (size_t i = 0; i < buffer_id_vertex.size(); i++)
		{
			if (counter >= 3) {
				cout << "\nF: ";
				counter = 1;
			}
			else {
				counter++;
			}

			cout << " " << buffer_id_vertex[i];
		}
		/*
		cout << "------------- Normals -------------" << endl;
		for (size_t i = 0; i < buffer_id_normal_vertex.size(); i++)
		{
			cout << "VN #" << i + 1 << "	V1: " << buffer_normals_vertex[buffer_id_normal_vertex[i]-1][0] << "		V2: " << buffer_normals_vertex[buffer_id_normal_vertex[i]-1][1] << "		V3: " << buffer_normals_vertex[buffer_id_normal_vertex[i]-1][2] << endl;
		}

		cout << "------------- Faces -------------" << endl;
		for (size_t i = 0; i < buffer_id_vertex.size(); i +=3)
		{
			cout << "F #" << i + 1 << "		V1: " << buffer_id_vertex[i] << "	V2: " << buffer_id_vertex[i+1] << "	V3: " << buffer_id_vertex[i+2] << endl;
		}
		*/
	}
};

#endif // FILE_H#pragma once
