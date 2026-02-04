#include <iostream>
using namespace std;


//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "time.h"


#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "GL\freeglut.h"

#include "Images\FreeImage.h"

#include "shaders\Shader.h"

CShader* myShader;  ///shader object 
CShader* myBasicShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Box\Box.h"
#include "Obj\OBJLoader.h"

#include "Camera\Camera.h"

//Frame and frame count variables
const int FPS = 120;
int init_time = time(NULL), final_time, frame_count;

//Camera variables
Camera camera;
const glm::vec3 globalUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec2 oldMousePos;

//enum class CameraType {
//	FREECAM,
//	GROUNDCAM,
//	RIDECAM
//};
Camera::CamType startCamType = Camera::CamType::FREECAM;

glm::vec3 rideCamPos = glm::vec3(0.0f, 2.0f, -2.0f); //Used for setting the ride cam
const glm::vec3 originalPos = glm::vec3(10.0f, 0.5f, 0.0f); //starting pos for free cam and ground cam

float amount = 0;
float temp = 0.002f;
	

const int NumOfObjects = 3;

CThreeDModel myModels[NumOfObjects];
CThreeDModel ship;
CThreeDModel ground;

CBox testBox;

//angle variables
float maxAngle = 50.0f; //in degrees
float angle = 0.0f; //angle of ship
float angularSpeed = 1.0f; //Speed of ship rotation


//CThreeDModel boxLeft, boxRight, boxFront;
CThreeDModel model; //A threeDModel object is needed for each model loaded
COBJLoader objLoader;	//this object is used to load the 3d models.

//string myModels[] = { "axes.obj", "boxleft.obj", "boxRight.obj" };
///END MODEL LOADING

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

glm::mat4 objectRotation;
glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f); //vector for the position of the object.

//Material properties
float Material_Ambient[4] = {0.6f, 0.6f, 0.6f, 1.0f};
float Material_Diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float Material_Specular[4] = {0.9f,0.9f,0.8f,1.0f};
float Material_Shininess = 50;

//Light Properties
float Light_Ambient[4] = {0.49f, 0.43f, 0.56f, 0.0f};
//float Light_Ambient[4] = { 0.13f, 0.15f, 0.41f, 0.4f };
float Light_Diffuse[4] = { 0.71f, 0.63f, 0.95f, 1.0f };
//float Light_Diffuse[4] = { 0.631f, 0.776f, 0.773f, 0.34f };
//float Light_Ambient_And_Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float Light_Specular[4] = {1.0f,1.0f,1.0f,1.0f};
float LightPos[4] = {0.0f, 0.7f, 6.0f, 0.0f};

//Light2 Properties
float Light_Diffuse2[4] = { 0.8f, 0.07f, 0.0f, 1.0f };
float Light_Specular2[4] = { 1.0f, 0.27f, 0.0f, 1.0f };
float LightPos2[4] = { 0.0f, 1.0f, 5.0f, 0.0f };

//Spot light properties
float spotlight_direction[3] = {0.0f, 0.7f, 6.0f};
float spotlight_cutoff = glm::cos(glm::radians(12.5f));
float spotlight_outercutoff = glm::cos(glm::radians(17.5f));

//Attenuation properties
float Constant_Att = 0.7f;
float Linear_Att = 0.07f;
float Quadratic_Att = 0.03f;

//
int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
int screenWidth=600, screenHeight=600;

//booleans to handle when the arrow keys are pressed or released.
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;
bool Home = false;
bool End = false;
bool zero = false;
bool one = false;
bool two = false;
bool three = false;
bool o = false;
bool p = false;
bool a = false;
bool d = false; 
bool w = false;
bool s = false;
bool space = false;
bool ctrl = false;

float spin=180;
float speed=0;

float zSpeed = 0;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void keyfunc(unsigned char key, int x, int y);
void keyfuncUp(unsigned char key, int x, int y);
void special(int key, int x, int y);
void specialUp(int key, int x, int y);
void processKeys();         //called in winmain to process keyboard input
void idle(int);		//idle function
void updateTransform(float xinc, float yinc, float zinc);
void updateTransformTranslate(float xinc, float yinc, float zinc);
void passiveMouseMotion(int x, int y);

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()									
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(myShader->GetProgramObjID());  // use the shader

	//Part for displacement shader.
	amount += temp;
	if(amount > 1.0f || amount < -1.5f)
		temp = -temp;
	//amount = 0;
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "displacement"), amount);

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(myShader->GetProgramObjID(), "ProjectionMatrix");  
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewingMatrix = glm::mat4(1.0f);
	
	//translation and rotation for view
	
	//viewingMatrix = objectRotation * glm::lookAt(glm::vec3(10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
	viewingMatrix = glm::lookAt(glm::vec3(10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));


	//Camera strafe implementation
	viewingMatrix = glm::translate(viewingMatrix, pos);

	//apply a rotation to the view
	//static float angle = 0.0f;
	//angle += 0.01;
	//viewingMatrix = glm::rotate(viewingMatrix, angle, glm::vec3(1.0f, 0.0f, 0.0));




	//viewingMatrix = glm::lookAt(glm::vec3(-6.42f, 10.0f, 6.0f), glm::vec3(0, 20, 0) + pos, glm::vec3(0.0f, 1.0f, 0.0f));


	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_ambient"), 1, Light_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse"), 1, Light_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos2"), 1, LightPos2);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse2"), 1, Light_Diffuse2);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_specular2"), 1, Light_Specular2);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "material_shininess"), Material_Shininess);

	glUniform3fv(glGetUniformLocation(myShader->GetProgramObjID(), "SpotlightDirection"), 1, spotlight_direction);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "spotLight.cutOff"), spotlight_cutoff);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "spotLight.outerCutOff"), spotlight_outercutoff);


	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "constantAttenuation"), Constant_Att);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "linearAttenuation"), Linear_Att);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "quadrativAttenuation"), Quadratic_Att);


	pos.x += objectRotation[2][0]*zSpeed;
	pos.y += objectRotation[2][1]*zSpeed;
	pos.z += objectRotation[2][2]*zSpeed;
	
	//Rotation of ship around pivot point
	glm::mat4 modelmatrix = glm::translate(glm::mat4(1.0), glm::vec3(0, 3.8447, 0));

	modelmatrix = glm::rotate(modelmatrix, glm::radians(angle), glm::vec3(1, 0, 0));


	modelmatrix = glm::translate(modelmatrix, glm::vec3(0, -3.8447, 0));
	
	if (camera.getCamType() == Camera::CamType::RIDECAM)
	{
		//cout << "Switching to RIDECAM" << endl; 
		const glm::vec3 rideCamCurrentPos = glm::vec3(modelmatrix * glm::vec4(rideCamPos, 1.0f));

		camera.updateCameraPos(rideCamCurrentPos);

		//new up vector for camera
		//const glm::vec3 up = glm::vec3(modelmatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

	}
	//modelmatrix = glm::translate(modelmatrix, translation);
	// 
	//Working viewing matrix
	//ModelViewMatrix = viewingMatrix * modelmatrix;

	//Testing camera 
	ModelViewMatrix = camera.getViewMatrix() * modelmatrix;

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	 
	
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	
	ship.DrawElementsUsingVBO(myShader);
	

	//Switch to basic shader to draw the lines for the bounding boxes
	glUseProgram(myBasicShader->GetProgramObjID());
	projMatLocation = glGetUniformLocation(myBasicShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//ship.DrawAllBoxesForOctreeNodes(myBasicShader);
	//ship.DrawBoundingBox(myBasicShader);
	//ship.DrawOctreeLeaves(myBasicShader);

	//switch back to the shader for textures and lighting on the objects.
	glUseProgram(myShader->GetProgramObjID());  // use the shader

	//ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0));
	//Testing camera
	ModelViewMatrix = glm::translate(camera.getViewMatrix(), glm::vec3(0, 0, 0));
	
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
//	boxLeft.DrawElementsUsingVBO(myShader);
	//boxRight.DrawElementsUsingVBO(myShader);
	//boxFront.DrawElementsUsingVBO(myShader);
	
	for (CThreeDModel m : myModels)
	{
		m.DrawElementsUsingVBO(myShader);

		
	}

	glFlush();
	glutSwapBuffers();

	//Frame counter
	frame_count++;
	final_time = time(NULL);
	if (final_time - init_time > 0)
	{
		cout << "FPS: " << frame_count << endl;
		frame_count = 0;
		init_time = final_time;
	}
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(90.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 0.3f, 200.0f); //changing the near clip from 1.0f
}
void init()
{
	glClearColor(0.65,0.55,0.70,0.0);						//sets the clear colour to yellow
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);


	myShader = new CShader();
	//if(!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag"))
	if(!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load shader" << endl;
	}		

	myBasicShader = new CShader();
	if(!myBasicShader->CreateShaderProgram("Basic", "glslfiles/basic.vert", "glslfiles/basic.frag"))
	{
		cout << "failed to load shader" << endl;
	}		

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	glEnable(GL_TEXTURE_2D);

	//lets initialise our object's rotation transformation 
	//to the identity matrix
	objectRotation = glm::mat4(1.0f);

	//Render objects from array myModels
/*	cout << " loading model " << endl;
	for (string m : filename)
	{
		objLoader.LoadModel("TestModels/" + m);

		cout << " model loaded " << endl;

		model.ConstructModelFromOBJLoader(objLoader);


	}
	*/

	//cout << " loading model " << endl;
	//if(objLoader.LoadModel("TestModels/axes.obj"))//returns true if the model is loaded
	//{
	//	cout << " model loaded " << endl;		

	//	//copy data from the OBJLoader object to the threedmodel class
	//	model.ConstructModelFromOBJLoader(objLoader);

	//	//if you want to translate the object to the origin of the screen,
	//	//first calculate the centre of the object, then move all the vertices
	//	//back so that the centre is on the origin.
	//	//model.CalcCentrePoint();
	//	model.CentreOnZero();

	//
	//	model.InitVBO(myShader);
	//}
	//else
	//{
	//	cout << " model failed to load " << endl;
	//}
	
	//Initialising animated ship
	if (objLoader.LoadModel("TestModels/ship/ship.obj"))
	{
		ship.ConstructModelFromOBJLoader(objLoader);
		ship.InitVBO(myShader);
	}

	//Initialising static objects
	string filenames[NumOfObjects] = {"TestModels/frame/frame.obj", 
		"TestModels/ground/grass_plane4.obj"};

	for (int count = 0; count < NumOfObjects; count++)
	{
		if (objLoader.LoadModel(filenames[count]))//returns true if the model is loaded
		{
			myModels[count].ConstructModelFromOBJLoader(objLoader);
			myModels[count].InitVBO(myShader);
		}
	}


	//if (objLoader.LoadModel("TestModels/boxleft.obj"))//returns true if the model is loaded
	//{
	//	boxLeft.ConstructModelFromOBJLoader(objLoader);
	//	boxLeft.InitVBO(myShader);
	//}
	//if (objLoader.LoadModel("TestModels/boxRight.obj"))//returns true if the model is loaded
	//{
	//	boxRight.ConstructModelFromOBJLoader(objLoader);
	//	boxRight.InitVBO(myShader);
	//}

	//if (objLoader.LoadModel("TestModels/newBox.obj"))
	//{
	//	boxLeft.ConstructModelFromOBJLoader(objLoader);
	//	boxLeft.InitVBO(myShader);
	//}
}

void keyfunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'o':
		o = true;
		//cout << "o pressed" << endl;
		break;
	case 'p':
		p = true;
		//cout << "p pressed" << endl;
		break;
	case 'a':
		a = true;
		//cout << "a pressed" << endl;
		break;
	case 'd':
		d = true;
		//cout << "d pressed" << endl;
		break;
	case 'w':
		w = true;
		//cout << "w pressed" << endl;
		break;
	case 's':
		s = true;
		//cout << "s pressed" << endl;
		break;
	case '0':
		zero = true;
		break;
	case '1':
		one = true;
		break;
	case '2':
		two = true;
		break;
	case 32:
		space = true;
		//cout << "space pressed" << endl;
		break;
	}
}

void keyfuncUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'o':
		o = false;
		break;
	case 'p':
		p = false;
		break;
	case 'a':
		a = false;
		break;
	case 'd':
		d = false;
		break;
	case 'w':
		w = false;
		break;
	case 's':
		s = false;
		break;
	case '0':
		zero = false;
		break;
	case '1':
		one = false;

		if (camera.getCamType() != Camera::CamType::FREECAM) 
		{
			camera.setCamType(Camera::CamType::FREECAM);
			cout << "Free Cam activated" << endl;
			camera.updateCameraPos(originalPos);
		}

		break;
	case '2':
		two = false;

		if (camera.getCamType() != Camera::CamType::GROUNDCAM)
		{
			camera.setCamType(Camera::CamType::GROUNDCAM);
			cout << "Ground Cam activated" << endl;
			camera.updateCameraPos(originalPos);
		}

		break;
	case '3':
		three = false;

		if (camera.getCamType() != Camera::CamType::RIDECAM)
		{
			camera.setCamType(Camera::CamType::RIDECAM);
			cout << "Ride Cam activated" << endl;
		}

		break;
	case 32:
		space = false;
		break;
	case 27:
		cout << "ESC pressed" << endl;
		glutLeaveMainLoop();
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
	case GLUT_KEY_CTRL_L:
		ctrl = true;
		cout << "ctrl pressed" << endl;
		break;
	case 30:
		zero = true;
		break;
	case 31:
		one = true;
		break;
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
		
		//angularSpeed += 0.1f;

		break;
	case GLUT_KEY_DOWN:
		Down = false;

		//angularSpeed -= 0.1f;

		break;
	case GLUT_KEY_HOME:
		Home = false;
		break;
	case GLUT_KEY_END:
		End = false;
		break;		
	case GLUT_KEY_CTRL_L:
		ctrl = false;
		break;
	case 30:
		zero = false;
		break;
	case 31:
		one = false;
		break;
	}
}

void processKeys()
{
	float spinXinc = 0.0f, spinYinc = 0.0f, spinZinc = 0.0f;
	float translateXinc = 0.0f, translateYinc = 0.0f, translateZinc = 0.0f;
	if (Left)
	{
		spinYinc = -0.001f;
	}
	if (Right)
	{
		spinYinc = 0.001f;
	}
	if (Up)
	{
		spinXinc = -0.001f;
	}
	if (Down)
	{
		spinXinc = 0.001f;
	}
	if (zero)
	{
		spinZinc = 0.0005f;
	}
	if (one)
	{
		spinZinc = -0.0005f;
	}

	if (o)
	{
		maxAngle += 0.05f;

		if (maxAngle > 180.0f)
		{
			maxAngle -= 0.05f;
		}
	}
	if (p)
	{

		maxAngle -= 0.05f;

		if (maxAngle < 0.0f)
		{
			maxAngle += 0.05f;
		}
	}

	if (camera.getCamType() != (Camera::CamType::RIDECAM)) {
		if (a)
		{
			camera.moveLeft();
			//cout << "a" << endl;
		}
		if (d)
		{
			camera.moveRight();
			//cout << "d" << endl;
		}
		if (w)
		{
			camera.moveForward();
			//cout << "w" << endl;
		}
		if (s)
		{
			camera.moveBack();
			//cout << "s" << endl;
		}

		if (camera.getCamType() == (Camera::CamType::FREECAM))
		{
			if (space)
			{
				camera.moveUp();
				//cout << "space" << endl;
			}
			if (ctrl)
			{
				camera.moveDown();
				//cout << "ctrl" << endl;
			}
		}
	}
	updateTransform(spinXinc, spinYinc, spinZinc);
	updateTransformTranslate(translateXinc, translateYinc, translateZinc);

}


void updateTransform(float xinc, float yinc, float zinc)
{
	objectRotation = glm::rotate(objectRotation, xinc, glm::vec3(1,0,0));
	objectRotation = glm::rotate(objectRotation, yinc, glm::vec3(0,1,0));
	objectRotation = glm::rotate(objectRotation, zinc, glm::vec3(0,0,1));
}

void updateTransformTranslate(float xinc, float yinc, float zinc)
{
	//translation = { xinc, yinc, zinc };


	pos.x += objectRotation[0][0] * xinc;
	pos.y += objectRotation[0][1] * xinc;
	pos.z += objectRotation[0][2] * xinc;
	
	//Translates local y axis
	
	pos.x += objectRotation[0][0] * yinc;
	pos.y += objectRotation[0][1] * yinc;
	pos.z += objectRotation[0][2] * yinc;
	

	//Translates local z axis
	pos.x += objectRotation[2][0] * zinc;
	pos.y += objectRotation[2][1] * zinc;
	pos.z += objectRotation[2][2] * zinc;
}

//Method for recording mouse movememnt
void passiveMouseMotion(int x, int y)
{
	// Ignore mouse movement when wraping pointer to other side
	static bool warping = false;
	if (warping) {
		return;
	}

	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	int centerX = windowWidth / 2;
	int centerY = windowHeight / 2;

	// Compute deltas from centre
	int dx = centerX - x;
	int dy = centerY - y;

	if (dx != 0 || dy != 0)
	{
		camera.updateMouseDelta(dx, dy);

		// Recenter mouse
		warping = true;
		glutWarpPointer(centerX, centerY);
		warping = false;
	}

	/*if (x < 0) {
		warping = true;
		x = windowWidth - 1;
		glutWarpPointer(windowWidth - 1, y);
	}
	else if (x >= windowWidth) {
		warping = true;
		x = 0;
		glutWarpPointer(0, y);
	}
	if (y < 0) {
		warping = true;
		y = windowHeight - 1;
		glutWarpPointer(x, windowHeight - 1);
	}
	else if (y >= windowHeight) {
		warping = true;
		y = 0;
		glutWarpPointer(x, 0);
	}*/

	/*warping = false;

	glm::vec2 newMousePos(x, y);

	

	camera.updateMousePos(newMousePos);

	oldMousePos = newMousePos;*/
}

void idle(int)
{
	spin += speed;
	if(spin > 360)
		spin = 0;

	//Changes value of angle over time
	angle = maxAngle * sin(angularSpeed * glutGet(GLUT_ELAPSED_TIME) / 1000.0f);

	processKeys();

	glutPostRedisplay();

	glutTimerFunc((1000 / FPS), idle, 0);
}
/**************** END OPENGL FUNCTIONS *************************/

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Fairground Swing Ride");

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

	//initialise cam type
	camera.setCamType(startCamType);

	glutReshapeFunc(reshape);
	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);

	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp); //Deals with when key is released

	glutKeyboardFunc(keyfunc);
	glutKeyboardUpFunc(keyfuncUp);

	glutSetCursor(GLUT_CURSOR_NONE);

	//Mouse moevement
	glutPassiveMotionFunc(passiveMouseMotion);


	//glutIdleFunc(idle);
	//Limiting FPS
	glutTimerFunc((1000 / FPS), idle, 0);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}
