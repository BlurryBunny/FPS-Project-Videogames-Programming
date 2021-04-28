#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h> // put first GL libs
#include <glfw3.h>
#include <conio.h>
#include <iostream>
#include <Windows.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>	// glm::translate, glm::rotate, glm::scale, glm::persective
#include <glm/gtc/type_ptr.hpp>

#include "shaders.hpp"
#include "files.h"
#include "Object3D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



void Matrices();
int InitWindow();
void processInput();
GLuint LoadTexture(const char* path_texture);
glm::mat4 viewFPS(glm::vec3 pos, float pitch, float yaw);
/*void push_back_buffer(float value);
bool give_memory_buffer();
glm::vec3 calculate_normalFace(int stride_toNormal, int stride_toNextVer);
bool replace_Normal(glm::vec3 normal, int index, int stride, int n_vertex);
glm::vec3 vertexRes(glm::vec3 vf, glm::vec3	vi);
void printBuffer(int pos);
void printBufferFull();*/
//bool change_light(char sense, int stride);
//void attributeLoader(const GLfloat* g_vertex_buffer_data, int stride);

GLFWwindow* window;

glm::mat4x4 model, view, projection;
vector<string>myObjsNameFile;
vector<GLuint>texturesObjs;
vector<Object3D>myObjs;		// we need a collection to save each object

int width = 1200;
int height = 900;

float pitch = 0.0f, yaw = 0.0f;
float speed_camera = 0.25f;
float speed = 0.05f;
float mousePos_x = width/2;
float mousePos_y = height/2;

glm::vec3 mov(0.0f, -1.5f, -8.0f);

int main()
{
	srand(time(NULL));
	if (InitWindow() < 0) return -1;

	//we use intial time when we need to calcule when the aplication use 1 second in real life.
	//double initialTime = glfwGetTime(); // not presition timer
	bool res = false;
	int frameCounter = 0;
	double totalTime = 0.0;
	int unifLoc;

	// Id for program
	GLuint programID;

	//Inicializate all the objects that goint to be part of the world
	myObjsNameFile.push_back("Models\\fishbowl_CristalTexture.obj");
	myObjsNameFile.push_back("Models\\weapon_classic.obj");
	myObjsNameFile.push_back("Models\\plano_WoodTexture.obj");
	
	
	//Inicializate the textures that each object will use.
	texturesObjs.push_back(LoadTexture("C:\\Users\\ulise\\source\\repos\\Programacion_videojuegos\\Ejercicio_Texturas\\Objs_Shaders\\Objs_Shaders\\TexturesModels\\cristal_t.jpg"));
	texturesObjs.push_back(LoadTexture("C:\\Users\\ulise\\source\\repos\\Programacion_videojuegos\\Ejercicio_Texturas\\Objs_Shaders\\Objs_Shaders\\TexturesModels\\Material_gun_classic.png"));
	texturesObjs.push_back(LoadTexture("C:\\Users\\ulise\\source\\repos\\Programacion_videojuegos\\Ejercicio_Texturas\\Objs_Shaders\\Objs_Shaders\\TexturesModels\\wood_t.jpg"));
	
	for (size_t i = 0; i < texturesObjs.size(); i++)
	{
		if (!texturesObjs[i]) {
			cout << "No se cargo textura" << endl;
		}
	}
	/* ------------------------------------------- LOAD AND SAVE THE OBJS FILES -------------------------------------------*/
	for (int i = 0; i < myObjsNameFile.size(); i++) {
		//Declarate an object type FILE.h
		File objInfo = File();
		//Give values and inicilizate the FILE objs
		objInfo.setfileName(myObjsNameFile[i]);
		if (!objInfo.open()) {
			cout << "Error open the file" << endl;
			exit(0);
		}
		else {
			objInfo.saveInfo();

			if (!objInfo.fillBuffer()){
				cout << "Error on 3D Obj" << endl;
				exit(0);
			}else {
				Object3D obj3D = objInfo.getObject3D();
				myObjs.push_back(obj3D);
			}
			//obj1.toString();
		}
	}

	//move an object only to confirm that two objects draw
	myObjs.front().setModelMatrix(glm::scale(myObjs.front().getModelMatrix(), glm::vec3(0.5f, 0.5f, 0.5f)));
	myObjs.front().setModelMatrix(glm::translate(myObjs.front().getModelMatrix(), glm::vec3(0.0f, 0.4f, 0.0f)));

	//myObjs[1].setModelMatrix(glm::scale(myObjs[1].getModelMatrix(), glm::vec3(0.9f, 0.9f, 0.9f)));
	//myObjs[1].setModelMatrix(glm::translate(myObjs[1].getModelMatrix(), glm::vec3(1.0f, 0.1f, 1.0f)));
	myObjs.back().setModelMatrix(glm::scale(myObjs.back().getModelMatrix(), glm::vec3(5.5f, 5.5f, 5.5f)));
	myObjs.back().setModelMatrix(glm::rotate(myObjs.back().getModelMatrix(),glm::radians(0.0f), glm::vec3(1.0f,0.0f,0.0f)));
	myObjs.back().setModelMatrix(glm::translate(myObjs.back().getModelMatrix(),glm::vec3(0.0f,0.0f,0.0f)));

	//Load the shaders using the .glsl file
	programID = LoadShaders("Shaders\\vs_basico_proyect.glsl","Shaders\\fs_basico_proyect.glsl");

	//Inicializate cursor
	POINT p;
	if (GetCursorPos(&p)) {
		SetCursorPos(width / 2, height / 2);
		//ShowCursor(FALSE);
	}

	//main loop
	do
	{
		char sense;
		
		double initialTime = glfwGetTime(); // not a presition timer

		// a presition timer use the inside clock of the computer with ticks and frecuency
		// the problem of that timers it depends complety of the operative system.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		//check if a key was press

		/*Change shaders*/
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			programID = LoadShaders("Shaders\\vs_ilum_flat.glsl", "Shaders\\fs_ilum_flat.glsl");
		}

		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			programID = LoadShaders("Shaders\\vs_ilum_phong.glsl", "Shaders\\fs_ilum_phong.glsl");
		}

		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			programID = LoadShaders("Shaders\\vs_ilum_gourand.glsl", "Shaders\\fs_ilum_gourand.glsl");
		}
		
		for (size_t i = 0; i < myObjs.size(); i++)
		{
			Object3D obj3D = myObjs[i];
			
			processInput();
			glUseProgram(programID); //The ID use the fragment and vertex shader


			//Use Matrix to view, transform and proyect the objects.
			Matrices();	//Generate each frame the point of view.

			unifLoc = glGetUniformLocation(programID, "model");
			glUniformMatrix4fv(unifLoc, 1, GL_FALSE, glm::value_ptr(obj3D.getModelMatrix()));

			unifLoc = glGetUniformLocation(programID, "view");
			glUniformMatrix4fv(unifLoc, 1, GL_FALSE, glm::value_ptr(view));

			unifLoc = glGetUniformLocation(programID, "projection");
			glUniformMatrix4fv(unifLoc, 1, GL_FALSE, glm::value_ptr(projection));


			//get location of variables
			int location = glGetUniformLocation(programID, "color");
			glUniform3f(location, 1.0f, 0.0f, 1.0f); // write in the position the values on the function.

			//Set the texture

			glBindTexture(GL_TEXTURE_2D, texturesObjs[i]);
		

			//draw the shaders
			glBindVertexArray(obj3D.getVertexArrayID());
			glDrawArrays(GL_TRIANGLES, 0, obj3D.getNumVertex()); //Termina la configuracion del shader y esta listo para dibujar.
		}

		

		glfwSwapBuffers(window);
		glfwPollEvents();

		//GAME

		//Paints

		//Frame Rate
		double ts = 1.0f / 60.0f;
		double diff;
		double finalTime = glfwGetTime(); // not presition timer

		diff = finalTime - initialTime;
		while (true) {
			if (diff >= ts){break;}
			finalTime = glfwGetTime();
			diff = finalTime - initialTime;
		}

		//update total frames and the timer acoording to the limit time
		frameCounter++;
		totalTime += diff;

		//show results
		//printf("%f\n", diff); 
		if (totalTime >= 1.0) {
			//printf("Frame rate per second limit to: %d\n",frameCounter);
			frameCounter = 0;
			totalTime = 0.0;
			res = false;	//res return false if a second has passed.
		}
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwTerminate();

	return 1;
}

// open sources to show something in a new window.
int InitWindow(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Test", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	//Glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);

	//OpenGl
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glViewport(0, 0, 1000, 1000);
	glEnable(GL_DEPTH_TEST);

	return 1;
}

void Matrices() {
	view = viewFPS(mov,pitch,yaw);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

glm::mat4 viewFPS(glm::vec3 pos, float pitch, float yaw) {

	GLfloat pitchr = 0.0f;
	GLfloat yawr = 0.0f;

	if (pitch > 60) {
		pitch = 60;
	}

	if (pitch < -40) {
		pitch = -40;
	}

	yawr = glm::radians(yaw);
	pitchr = glm::radians(pitch);

	/*Matrix for all objects*/
	glm::mat4 matrix = glm::rotate(glm::mat4(1.0f), pitchr, glm::vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, yawr, glm::vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::translate(matrix, pos);

	/*Rotation weapon*/
	glm::mat4 rotation_yaw = glm::rotate(glm::mat4(1.0f), -yawr, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotation_pitch = glm::rotate(glm::mat4(1.0f), -pitchr, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotate_yp = rotation_yaw * rotation_pitch;
	glm::vec3 posWeapon(-pos[0]+0.55f,-pos[1]-0.45f,-pos[2]-3.0f);
	glm::mat4 trans_to_pivot= glm::translate(glm::mat4(1.0f),pos);
	glm::mat4 trans_from_pivot = glm::translate(glm::mat4(1.0f), -pos);

	glm::mat4 rotationWeapon = trans_from_pivot * rotate_yp * trans_to_pivot;

	myObjs[1].setModelMatrix(rotationWeapon * glm::translate(glm::mat4(1.0f), posWeapon) * glm::rotate(glm::mat4(1.0f),glm::radians(195.0f),glm::vec3(0.0f,1.0f,0.0f))); //rotation if weapon seems in other direction;
	myObjs[1].setModelMatrix(glm::scale(myObjs[1].getModelMatrix(), glm::vec3(0.25f, 0.25f, 0.25f)));
	return matrix;
}

void processInput() {

	//Close program
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}


	//glfwGetCursoPos(window, &xpos, &ypos);

	POINT p;
	//Change view position FPS
	if (GetCursorPos(&p))
	{
		//cursor position now in p.x and p.y

		/*Check current pos and last pos*/

		/*RIGHT*/
		if (p.x < mousePos_x && p.x < width ) {
			yaw -= ((mousePos_x - p.x)*speed_camera);
		}/*LEFT*/
		else if (p.x > mousePos_x && p.x > 0  ) {
			yaw += ((p.x -mousePos_x ) * speed_camera);
		}

		/*UP*/
		if (p.y < mousePos_y && p.y > 0 ) {
			pitch -= ((mousePos_y - p.y) * speed_camera);

		}/*DOWN*/
		else if (p.y > mousePos_y && p.y < height ) {
			pitch += ((p.y - mousePos_y ) * speed_camera);
		}

		if (p.y != mousePos_y && p.x != mousePos_x) {
			//cout << "yaw: " << yaw << endl;
			//cout << "pitch: " << pitch << endl;

			//cout << endl;
			
		}
		SetCursorPos(width / 2, height / 2);

	}

	//Translation
	GLfloat pitchr = 0.0f;
	GLfloat yawr = 0.0f;

	yawr = glm::radians(yaw);
	pitchr = glm::radians(pitch);

	glm::vec3 forwardVector(-sin(yawr) * cos(pitchr), sin(yawr) * sin(pitchr), cos(yawr));

	//Forward vector
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		mov += forwardVector * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		mov -= forwardVector * speed;
	}

	//movement to the side
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		mov[0] += speed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		mov[0] -= speed;
	}

}

GLuint LoadTexture(const char* path_texture) {
	GLuint texture;
	
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D,texture);
	/*define what kind of texture wrapping we need to use on our image */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int t_width, t_heigh, nrChannels;

	unsigned char* data = stbi_load(path_texture, &t_width, &t_heigh, &nrChannels, 0);

	if (!data) {
		cout << "Failed to load texture" << endl;
		exit(0);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_heigh, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	return (texture);
}



