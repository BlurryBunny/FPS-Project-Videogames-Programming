#ifndef FILES_H
#define FILES_H
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <glm/vec3.hpp>
#include "Object3D.h"


using namespace std;

class File
{
private:
	string name;
	ifstream arch; //The file variable to open the file

	vector<glm::vec3> buffer_vertex; // My buffer contains 
	vector<glm::vec3>buffer_normals_vertex;
	vector<glm::vec2> buffer_texturePos;

	vector<int>buffer_id_vertex;
	vector<int>buffer_id_normal_vertex;
	vector<int>buffer_id_texturePos;

	float* buffer_obj;
	int size_total;
	int index_buffer;
	int stride;
	int numVertex;

	GLuint v_buffer, v_ArrayID;
	const GLfloat* g_vertex_buffer_data;

	//Light pos (this will use to diffuse and specular light) INITIAL LIGHT
	glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, -7.0f);
	
	
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
					t -> texture 
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
						vector<string> aux_val = split(val[i], "/");

						if (!aux_val.empty()) {
							try {
								buffer_id_vertex.push_back(stoi(aux_val[0]));			//idVertex
								buffer_id_texturePos.push_back(stoi(aux_val[1]));		//idTexturePos
								buffer_id_normal_vertex.push_back(stoi(aux_val[2]));	//idNormalVertex
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
				else if (val[0] == "t") {
					buffer_texturePos.push_back(glm::vec2(stof(val[1]), stof(val[2])));
				}
			}
			else {
				res = false;
			}
		}

		arch.close();//Close the archive
		return res;
	}


	/*---------------------------------------- Functions to fill the buffer obj ---------------------------------------- */
	bool fillBuffer() {
		
		bool res = true;

		//Inicializate values 
		stride = total_elements_Vertex();
		size_total = get_size_bufferIdVer() * stride;
		index_buffer = 0;

		//cout << "Stride: " << stride << endl;
		//cout << "size total: " << size_total << endl;

		//Give memory to the array
		if (!give_memory_buffer()) {
			cout << "Error give memory to buffer" << endl;
			res = false;
		}

		int counter_faces = 0;
		for (int i = 0; i < get_size_bufferIdVer(); i++)
		{
			//Check if we have a new face to calculate the normal 
			if (counter_faces < 3) {
				counter_faces++;
			}
			else {
				//replace_Normal(vec3,index to the value 'z' of  last normal face saved)
				//calculate_normalFace(index to the 'x' of the last normal vertex saved, last position saved of the buffer)

				//cout << buffer_obj[((i + 1) * stride) - 6] << " " << buffer_obj[((i + 1) * stride) - 5] << " " << buffer_obj[((i + 1) * stride) - 4] << endl;
				//cout << endl;

				glm::vec3 normalFace = calculate_normalFace((i + 1) * stride - 14, stride);
				//cout << normalFace[0] << " " << normalFace[0] << " " << normalFace[0] << " " << endl;
				if (replace_Normal(normalFace, ((i + 1) * stride) - 6, stride, 3) != true) {
					cout << "Error calculate and replace normal" << endl;
					res = false;
				}

				counter_faces = 0;
			}

			glm::vec3 v =getVertex(getIdVertex(i) - 1); //Get the value of x,y,z of a vertex indicates on a face of the obj
			glm::vec3 vn = getNormalVertex(getIdNormalVertex(i) - 1);
			glm::vec2 tPos = getTexturePos(getIdTexturePos(i) - 1);

			//cout << "new" << endl;	cout << "value: " << v[o] << endl;
			for (int o = 0; o < 3; o++) { push_back_buffer(v[o]); }	//Vertex Pos
			for (int p = 0; p < 3; p++) { push_back_buffer(vn[p]); }	//Normal Pos
			for (int q = 0; q < 3; q++) {							//Color RGB
				//push_back_buffer(1.0f/float(rand() % 10 + 1));	//rand color
				//push_back_buffer(1.0f / float(q + 1));				//cream color
				push_back_buffer(1.0f);							//gray color.

			}
			for (int r = 0; r < 3; r++) { push_back_buffer(0.0f); }	//Normal Face
			for (int s = 0; s < 3; s++) { push_back_buffer(lightPos[s]); } //Light pos 
			for (int t = 0; t < 2; t++) { push_back_buffer(tPos[t]); }	//Texture Pos
		}

		/*buff_obj;
		Vertice		Normal		Color		NormalFace		Light Pos	TexturePos	
		[x,y,z]		[x,y,z]		[r,g,b]		  [x,y,z]	    [x,y,z]			[u,v]
		*/
		
		if (res) { attributeLoader(); }

		return res;
	}

	bool give_memory_buffer() {
		bool res = true;
		buffer_obj = (float*)malloc(sizeof(float) * size_total);
		if (!buffer_obj) { res = false; }
		return res;
	}

	void push_back_buffer(float value) {
		if (index_buffer <= size_total) { buffer_obj[index_buffer] = value; index_buffer++; }
	}

	/*----------------------------------------Calculate normal vector to each face ----------------------------------------*/
	glm::vec3 calculate_normalFace(int stride_toNormal, int stride_toNextVer) {

		glm::vec3 v3 = glm::vec3(buffer_obj[stride_toNormal], buffer_obj[stride_toNormal + 1], buffer_obj[stride_toNormal + 2]);
		stride_toNormal = stride_toNormal - stride_toNextVer;
		glm::vec3 v2 = glm::vec3(buffer_obj[stride_toNormal], buffer_obj[stride_toNormal + 1], buffer_obj[stride_toNormal + 2]);
		stride_toNormal = stride_toNormal - stride_toNextVer;
		glm::vec3 v1 = glm::vec3(buffer_obj[stride_toNormal], buffer_obj[stride_toNormal + 1], buffer_obj[stride_toNormal + 2]);

		glm::vec3 res = glm::cross(vertexRes(v2, v1), vertexRes(v3, v1));
		return res;
	}

	glm::vec3 vertexRes(glm::vec3 vf, glm::vec3	vi) {
		return glm::vec3(vf[0] - vi[0], vf[1] - vi[0], vf[2] - vi[2]);
	}

	bool replace_Normal(glm::vec3 normal, int index, int stride, int n_vertex) {
		bool res = true;
		for (int i = 0; i < n_vertex && res; i++) {
			if (index < size_total && index >= 0) {
				buffer_obj[index] = normal[2];
				buffer_obj[index - 1] = normal[1];
				buffer_obj[index - 2] = normal[0];
				index -= stride;
			}
			else {
				res = false;
			}
		}
		return res;
	}

	/* ---------------------------------------- Create the buffer and load the data of the array ----------------------------------------*/
	void attributeLoader() {
		//VAO
		glGenVertexArrays(1, &v_ArrayID);
		glBindVertexArray(v_ArrayID);

		//VBO 
		glGenBuffers(1, &v_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, v_buffer);

		//cast to const GLfloat to GL recognize
		g_vertex_buffer_data =  buffer_obj;
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size_total, g_vertex_buffer_data, GL_STATIC_DRAW);

		//glVertexAttribPointer(location shader, elements to save, kind of value , polarize , next begin since it count the strite ); 

		//Vertex
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)0);
		glEnableVertexAttribArray(0);
		//Normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)(sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);
		//Color
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)(sizeof(GLfloat) * 6));
		glEnableVertexAttribArray(2);
		//normal Face
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)(sizeof(GLfloat) * 9));
		glEnableVertexAttribArray(3);
		//Light pos
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)(sizeof(GLfloat) * 12));
		glEnableVertexAttribArray(4);
		//Texture Pos
		glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (void*)(sizeof(GLfloat) * 15));
		glEnableVertexAttribArray(5);

		//cout << "Se lleno buffer de  " << name << endl;
	}

	/*------------------------------------------- GETTERS  -------------------------------------------*/
	
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

	glm::vec2 getTexturePos(int id) {
		return buffer_texturePos[id];
	}

	/*------------------------------------------- Possible cracks on the main program -------------------------------------------*/
	int getIdTexturePos(int index) {
		int res = 1;
		try
		{
			res = buffer_id_texturePos[index];
		}
		catch (const std::exception&)
		{
			cout << "Error try to get id Vertex" << endl;
		}
		return res;
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
		//Vertex	Normal	Color	NormalFace	Light Pos	TexturePos
		//x,y,z		x,y,z	r,g,b	x,y,z		  x,y,z			u,v
		return (3 + 3 + 3 + 3 + 3 + 2);
	}


	/*------------------------------------------- Return the entire object -------------------------------------------*/
	Object3D getObject3D() {
		numVertex = get_size_bufferIdVer();
		return (Object3D(glm::mat4(1.0f), numVertex, v_buffer, v_ArrayID,name));
	}


	/*----------------------------------------To string ----------------------------------------*/
	void printBuffer(int pos) {
		for (int i = pos; i < index_buffer; i++)
		{
			cout << buffer_obj[i] << " ";
		}
		cout << endl;
	}

	void printBuffer() {
		int counter = 0;
		for (int i = 0; i < index_buffer; i++)
		{

			if (counter == 15) {
				cout << endl;
				counter = 0;
			}
			counter++;
			cout << buffer_obj[i] << " ";
		}
	}


	void toString() {
		cout << "------------- Vertices -------------" << endl;
		for (size_t i = 0; i < buffer_vertex.size(); i++)
		{
			cout << "V #" << i + 1 << "	x: " << buffer_vertex[i][0] << "	y: " << buffer_vertex[i][1] << "	z: " << buffer_vertex[i][2] << endl;
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
