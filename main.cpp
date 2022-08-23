/*

Graphics 2 Reassessment James Burrell 100263300

Controls

W      = Increase speed
S      = Decrease speed

Up     = Pitch up
Down   = Pitch down
Left   = Yaw left
Right  = Yaw right
Home   = Roll anticlockwise (left)
End    = Roll clockwise (right)

PageUp = Change camera angle

*/

#include <iostream>
using namespace std;
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"
#include "GL\freeglut.h"
#include "Images\FreeImage.h"
#include "shaders\Shader.h"
#include "GL/stb_image.h";

CShader* myShader;
CShader* myBasicShader;
CShader* myTextureShader;

#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

int view = 2;
float speed = 1.75f;
float maxSpeed = 4.5f;
float minSpeed = 1.0f;

CThreeDModel balloon;
CThreeDModel plane;
CThreeDModel planeProp;
CThreeDModel island;

COBJLoader objLoader;	

glm::mat4 ProjectionMatrix;
glm::mat4 ProjectionMatrix2D;
glm::mat4 ModelViewMatrix;
glm::mat4 objectRotation;

glm::vec3 posPlane = glm::vec3(150.0f, 0.0f, 250.0f);
glm::vec3 seaPos = glm::vec3(0.0f, 0.0f, 140.0f);
glm::vec3 islandPos = glm::vec3(0.0f, 0.0f, 500.0f);
glm::vec3 balloonPos;

glm::vec3 pilotView;
glm::vec3 balloonViewer;

float balloonY = 10.0f;
float height = 1.0f;
float sea = 1000000000;

float Material_Ambient[4] = { 0.45f, 0.45f, 0.45f, 1.0f };
float Material_Diffuse[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
float Material_Specular[4] = { 0.9f,0.9f,0.9f,1.0f };
float Material_Shininess = 400;

float Light_Ambient_And_Diffuse[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
float Light_Specular[4] = { 1.0f,1.0f,1.0f,1.0f };
float LightPos[4] = { 0.0f, 100.0f, 0.0f, 0.0f };

int screenWidth = 1800, screenHeight = 900;

bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;
bool Home = false;
bool End = false;
bool incSpeed = false;
bool decSpeed = false;

GLuint seaVBO;
GLuint seaVAO;
GLuint seaIBO;

GLuint insVBO[2];
GLuint insVAO;
GLuint insIBO;
GLuint texture;

GLfloat insVertices[] =
{
	 761.0f,   143.0f ,  0.0f,    // top right 0
	 761.0f,  -143.0f,  0.0f,  // bottom right 1
	-761.0f,  -143.0f,  0.0f,  // bottom left 2
	-761.0f,   143.0f,  0.0f   // top left 3
};

GLfloat insTexCoords[] =
{
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};

GLuint insIndices[] =
{
	   0, 1, 2, 
	   2, 3, 0
};

GLfloat seaVertices[] =
{
		 screenWidth * 100,  screenWidth * 100, 0.0f,  // top right
		 screenWidth * 100, -screenWidth * 100, 0.0f,  // bottom right
		-screenWidth * 100, -screenWidth * 100, 0.0f,  // bottom left
		-screenWidth * 100,  screenWidth * 100, 0.0f   // top left
};

GLuint seaIndices[] =
{
	   0, 1, 3,  // first Triangle
	   1, 2, 3   // second Triangle
};

void display();				
void reshape(int width, int height);	
void init();		
void processKeys();
void idle();
void updateTransform(float xinc, float yinc, float zinc);
void constructSea();
void renderSea();
void constructInstruments();
void renderInstruments();

void display()
{
	
	// float "animation" for hot air balloon
	balloonY += 0.03f;
	float v = sin(balloonY);
	balloonPos = glm::vec3(0.0, v, 500.0);
	balloonViewer = glm::vec3(0.0, v + 2.0, 500.0);

	glClearColor(0.8f, 0.9f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(myShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewingMatrix = glm::mat4(1.0f);

	// Select view to use
	if (view == 0)
	{
		// Island View
		viewingMatrix = glm::lookAt(glm::vec3(-300.0, 30, 350.0), posPlane, glm::vec3(0.0, 1.0, 0.0));
	}
	else if (view == 1)
	{
		// Balloon View
		viewingMatrix = glm::lookAt(balloonViewer, posPlane, glm::vec3(0.0, 1.0, 0.0));
	}
	else if (view == 2)
	{
		// Birdseye View
		viewingMatrix = glm::lookAt(glm::vec3(150.0, 200.0, 50.0), posPlane, glm::vec3(0.0, 1.0, 0.0));
	}
	else
	{

		// Pilot View
		pilotView.x = posPlane.x + objectRotation[2][0];
		pilotView.y = posPlane.y + objectRotation[2][1];
		pilotView.z = posPlane.z + objectRotation[2][2];

		glm::vec3 planeUp = glm::vec3(objectRotation[1][0], objectRotation[1][1], objectRotation[1][2]);
		viewingMatrix = glm::lookAt(posPlane, pilotView, planeUp);

	}

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "material_shininess"), Material_Shininess);

	//-------------------------------------------------------------------- Plane and prop ----------------------------------------------------------------
	
	glUseProgram(myShader->GetProgramObjID());

	glm::mat4 modelmatrix = glm::translate(glm::mat4(1.0f), posPlane);
	ModelViewMatrix = viewingMatrix * modelmatrix * objectRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	plane.DrawElementsUsingVBO(myShader);
	planeProp.DrawElementsUsingVBO(myShader);

	//-------------------------------------------------------------------- Balloon ----------------------------------------------------------------

	modelmatrix = glm::translate(glm::mat4(1.0f), balloonPos);
	ModelViewMatrix = viewingMatrix * modelmatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	balloon.DrawElementsUsingVBO(myShader);

	//-------------------------------------------------------------------- Island ----------------------------------------------------------------

	modelmatrix = glm::translate(glm::mat4(1.0f), islandPos);
	ModelViewMatrix = viewingMatrix * modelmatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	island.DrawElementsUsingVBO(myShader);

	//-------------------------------------------------------------------------  Sea  --------------------------------------------------------------------
	
	glUseProgram(myBasicShader->GetProgramObjID());

	GLuint projMatLocationBasic = glGetUniformLocation(myBasicShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocationBasic, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	
	modelmatrix = glm::rotate(modelmatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	modelmatrix = glm::translate(modelmatrix, seaPos);
	ModelViewMatrix = viewingMatrix * modelmatrix;
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	constructSea();
	renderSea();

	//---------------------------------------------------------------------  Instruments  ------------------------------------------------------------------

	if (view == 3) 
	{
		glUseProgram(myTextureShader->GetProgramObjID());

		// send 2dprojectionmatrix to shader
		glUniformMatrix4fv(glGetUniformLocation(myTextureShader->GetProgramObjID(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix2D[0][0]);

		// Set modelview matrix 
		glm::mat4 t = glm::translate(glm::mat4(1.0), glm::vec3((screenWidth / 2), (screenHeight / 2) - 397, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(myTextureShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &t[0][0]);

		constructInstruments();
		renderInstruments();
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------

	posPlane.x += objectRotation[2][0] * speed;
	posPlane.y += objectRotation[2][1] * speed;
	posPlane.z += objectRotation[2][2] * speed;



	glFlush();
	glutSwapBuffers();
}

void constructSea() 
{

	glGenVertexArrays(1, &seaVAO);
	glBindVertexArray(seaVAO);
	glGenBuffers(1, &seaVBO);
	glBindBuffer(GL_ARRAY_BUFFER, seaVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(seaVertices) * 3 * sizeof(GLfloat), seaVertices, GL_STATIC_DRAW);
	GLint vertexLocation = glGetAttribLocation(myBasicShader->GetProgramObjID(), "in_Position");
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLocation);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &seaIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, seaIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(seaIndices) * 3 * sizeof(GLuint), seaIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

}

void renderSea()
{

	glBindVertexArray(seaVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, seaIBO);
	glDisable(GL_CULL_FACE);
	glDrawElements(GL_TRIANGLES, sizeof(seaIndices), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); //unbind the vertex array object
}

void constructInstruments()
{

	int widthImg, heightImg, numColCh;
	unsigned char* texbytes = stbi_load("ViewFromCockpit2.png", &widthImg, &heightImg, &numColCh, 0);

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, texbytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(texbytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint tex0uni = glGetUniformLocation(myTextureShader->GetProgramObjID(), "tex0");
	glUniform1i(tex0uni, 0);

	glGenVertexArrays(1, &insVAO);
	glBindVertexArray(insVAO);
	glGenBuffers(2, insVBO);
	
	//Position
	glBindBuffer(GL_ARRAY_BUFFER, insVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(insVertices) * 3 * sizeof(GLfloat), insVertices, GL_STATIC_DRAW);
	GLint vertexLocation = glGetAttribLocation(myTextureShader->GetProgramObjID(), "in_Position");
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLocation);

	// Texture
	glBindBuffer(GL_ARRAY_BUFFER, insVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(insTexCoords) * 2 * sizeof(GLfloat), insTexCoords, GL_STATIC_DRAW);
	GLint texLocation = glGetAttribLocation(myTextureShader->GetProgramObjID(), "aTex");
	glVertexAttribPointer(texLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(texLocation);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &insIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, insIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(insIndices) * 3 * sizeof(GLuint), insIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void renderInstruments()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(insVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, insIBO);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDepthFunc(GL_ALWAYS);
	glDrawElements(GL_TRIANGLES, sizeof(insIndices), GL_UNSIGNED_INT, 0);
	glDepthFunc(GL_LESS);

	glDisable(GL_CULL_FACE);
};

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth = width; screenHeight = height;           // to ensure the mouse coordinates match 
													// we will use these values to set the coordinate system
	glViewport(0, 0, width, height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 1.3f, 4000.0f);

	//Set the projection matrix for 2d.
	ProjectionMatrix2D = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);

}

void init()
{
	glEnable(GL_DEPTH_TEST);
	
	myBasicShader = new CShader();
	if (!myBasicShader->CreateShaderProgram("Basic", "glslfiles/basic.vert", "glslfiles/basic.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	myTextureShader = new CShader();
	if (!myTextureShader->CreateShaderProgram("BasicTex", "glslfiles/basicTex.vert", "glslfiles/basicTex.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	myShader = new CShader();
	//if(!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag"))
	if (!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	glEnable(GL_TEXTURE_2D);

	//lets initialise our object's rotation transformation 
	//to the identity matrix
	objectRotation = glm::mat4(1.0f);

	cout << " loading model " << endl;
	if (objLoader.LoadModel("TestModels/planeNoProp.obj"))
	{
		cout << " model loaded " << endl;
		plane.ConstructModelFromOBJLoader(objLoader);
		plane.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/planeProp.obj"))//returns true if the model is loaded
	{

		planeProp.ConstructModelFromOBJLoader(objLoader);
		planeProp.InitVBO(myShader);
	}


	if (objLoader.LoadModel("TestModels/balloon5.obj"))//returns true if the model is loaded
	{
		balloon.ConstructModelFromOBJLoader(objLoader);
		balloon.InitVBO(myShader);
	}

	if (objLoader.LoadModel("TestModels/island3.obj"))//returns true if the model is loaded
	{
		island.ConstructModelFromOBJLoader(objLoader);
		island.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = true;
		break;
	case GLUT_KEY_RIGHT:
		Right = true;
		break;
	case GLUT_KEY_UP:
		Up = true;
		break;
	case GLUT_KEY_DOWN:
		Down = true;
		break;
	case GLUT_KEY_HOME:
		Home = true;
		break;
	case GLUT_KEY_END:
		End = true;
		break;
	case GLUT_KEY_PAGE_UP:
		if (view == 0) {
			view = 1;
		}
		else if (view == 1) {
			view = 2;
		}
		else if (view == 2) {
			view = 3;
		}
		else if (view == 3) {
			view = 0;
		}
	}

}

void specialUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = false;
		break;
	case GLUT_KEY_RIGHT:
		Right = false;
		break;
	case GLUT_KEY_UP:
		Up = false;
		break;
	case GLUT_KEY_DOWN:
		Down = false;
		break;
	case GLUT_KEY_HOME:
		Home = false;
		break;
	case GLUT_KEY_END:
		End = false;
		break;
	}
}

void processKeys()
{
	float spinXinc = 0.0f, spinYinc = 0.0f, spinZinc = 0.0f;
	if (Left)
	{
		spinYinc = 0.01f;
	}
	if (Right)
	{
		spinYinc = -0.01f;
	}
	if (Up)
	{
		spinXinc = -0.01f;
	}
	if (Down)
	{
		spinXinc = 0.01f;
	}
	if (Home)
	{
		spinZinc = -0.01f;

	}
	if (End)
	{
		spinZinc = 0.01f;
	}
	if (incSpeed)
	{
		if (speed >= maxSpeed) 
		{
			// Dont increase speed.
		}
		else 
		{
			speed += 0.025f;
		}
		
	}
	if (decSpeed)
	{
		if (speed <= minSpeed)
		{
			// Dont decrease speed
		}
		else 
		{
			speed -= 0.025f;
		}
		
	}
	updateTransform(spinXinc, spinYinc, spinZinc);
}

void updateTransform(float xinc, float yinc, float zinc)
{
	objectRotation = glm::rotate(objectRotation, xinc, glm::vec3(1, 0, 0));
	objectRotation = glm::rotate(objectRotation, yinc, glm::vec3(0, 1, 0));
	objectRotation = glm::rotate(objectRotation, zinc, glm::vec3(0, 0, 1));
}

void idle()
{
	processKeys();
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // Escape key
		exit(0);
		break;
	case 'w':
		incSpeed = true;
		break;
	case 's':
		decSpeed = true;
		break;
	}

}

void KeyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		incSpeed = false;
		break;
	case 's':
		decSpeed = false;
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	//glutInitWindowSize(screenWidth, screenHeight);
	//glutInitWindowPosition(0, 0);

	glutCreateWindow("Basic Flight Simulator");
	glutFullScreen();

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//Check the OpenGL version being used
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;

	//initialise the objects for rendering
	init();

	glutReshapeFunc(reshape);
	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);


	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);

	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}
