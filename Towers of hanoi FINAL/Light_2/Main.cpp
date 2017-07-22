// fragment shading of sphere model

#include "Angel.h"
#include "LoadShaders.h"
#include "mat.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <list>
#include <vector>

using namespace std;

const int NumTimesToSubdivide = 5;
const int NumTriangles = 4096;  // (4 faces)^(NumTimesToSubdivide + 1)
const int NumVertices = 3 * NumTriangles;

float animationH = -10;

bool isGameStarted = false;

int cameraNum = 0;
int numOfCameras = 4;

typedef vec4 point4;
typedef vec4 color4;
float disc1x = 0;
float disc1y = 0;
float disc1z = 0;

int defaultSlices = 10;

GLuint location;
GLuint location2;
GLuint location3;

int movementArray[] = { 0,0 };

point4 points[NumVertices];
vec3   normals[NumVertices];
vec4 colors[NumVertices];

int disc1indexNum, disc2indexNum;
list<int> discIndexList;


float discColors[7][3] = {
	{1.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 1.0f},

	{1.0f, 1.0f, 0.0f},
	{1.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 1.0f},

	{0.5f, 0.5f, 0.5f}
};

glm::vec3 pegPositions[4] =
{
	glm::vec3(-22.5,0,0),
	glm::vec3(-7.5,0,0),
	glm::vec3(7.5,0,0),
	glm::vec3(22.5,0,0)
};

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

//----------------------------------------------------------------------------

int Index = 0;



void
triangle(const point4& a, const point4& b, const point4& c, float r, float g, float blue)
{
	vec3  normal = normalize(cross(b - a, c - b));

	normals[Index] = normal;  points[Index] = a; colors[Index] = vec4(r, g, blue, 1.0f);  Index++;
	normals[Index] = normal;  points[Index] = b; colors[Index] = vec4(r, g, blue, 1.0f);  Index++;
	normals[Index] = normal;  points[Index] = c; colors[Index] = vec4(r, g, blue, 1.0f); Index++;
}

//----------------------------------------------------------------------------

point4
unit(const point4& p)
{
	float len = p.x*p.x + p.y*p.y + p.z*p.z;

	point4 t;
	if (len > DivideByZeroTolerance) {
		t = p / sqrt(len);
		t.w = 1.0;
	}

	return t;
}

//void
//divide_triangle(const point4& a, const point4& b,
//	const point4& c, int count)
//{
//	if (count > 0) {
//		point4 v1 = unit(a + b);
//		point4 v2 = unit(a + c);
//		point4 v3 = unit(b + c);
//		divide_triangle(a, v1, v2, count - 1);
//		divide_triangle(c, v2, v3, count - 1);
//		divide_triangle(b, v3, v1, count - 1);
//		divide_triangle(v1, v3, v2, count - 1);
//	}
//	else {
//		triangle(a, b, c);
//	}
//}

//void
//tetrahedron(int count)
//{
//	point4 v[4] = {
//		vec4(0.0, 0.0, 1.0, 1.0),
//		vec4(0.0, 0.942809, -0.333333, 1.0),
//		vec4(-0.816497, -0.471405, -0.333333, 1.0),
//		vec4(0.816497, -0.471405, -0.333333, 1.0)
//	};
//
//	divide_triangle(v[0], v[1], v[2], count);
//	divide_triangle(v[3], v[2], v[1], count);
//	divide_triangle(v[0], v[3], v[1], count);
//	divide_triangle(v[0], v[2], v[3], count);
//}

float pi = 3.1415926;
void cylinder(float centreX, float centreY, float radius, float height, int slices, float r, float g, float b) {
	int n = 0;

	while (n < (slices * 2) + 1) {


		float tempX = centreX + radius * cos(n * (pi / slices));
		float tempY = centreY + radius * sin(n * (pi / slices));

		point4 p1 = point4(tempX, tempY, height / 2, 1.0);

		n++;
		float tempX2 = centreX + radius * cos(n * (pi / slices));
		float tempY2 = centreY + radius * sin(n * (pi / slices));
		point4 p2 = point4(tempX2, tempY2, height / 2, 1.0);

		// centre top piece
		point4 pc = point4(centreX, centreY, height / 2, 1.0);

		triangle(p1, p2, pc,r,g,b);
		// First triangle done

		point4 p_c = point4(centreX, centreY, -height / 2, 1.0);
		point4 p_2 = point4(tempX2, tempY2, -height / 2, 1.0);
		point4 p_1 = point4(tempX, tempY, -height / 2, 1.0);
		triangle(p_1, p_2, p_c,r,g,b);
		// bottom triangle done

		triangle(p1, p2, p_1,r,g,b);
		triangle(p2, p_2, p_1,r,g,b);

	}
}
//----------------------------------------------------------------------------



class disc
{
public:
	disc(float rad, float h, glm::vec3 pos, float r, float g, float b);

	int indexBegin;
	int indexEnd;
	glm::vec3 position;
	glm::vec3 oldPosition;
	glm::vec3 targetPosition;
	int animationStage = 0;
	float animationPerc = 0;
	bool isAnimating = false;
	float radius;

private:
	
	
	float height;
	float m_r, m_b, m_g;
	
};

disc::disc(float rad, float h, glm::vec3 pos, float r, float g, float b)
{
	position = pos;
	radius = rad;
	height = h;
	m_r = r;
	m_b = b;
	m_g = g;
	indexBegin = Index;
	printf("%i    ", indexBegin);
	cylinder(0, 0, radius, height, defaultSlices, m_r, m_g, m_b);
	indexEnd = Index;
	printf("%i \n", indexEnd);
	//discIndexList.push_back(Index);
}

class peg
{
public:
	peg(float rad, float h, glm::vec3 pos, float r, float g, float b);
	list<disc> discs;
	glm::vec3 position;
	int indexBegin;
	int indexEnd;
	float height;

private:
	float radius;
	float m_r, m_b, m_g;
};

peg::peg(float rad, float h, glm::vec3 pos, float r, float g, float b)
{
	position = pos;
	radius = rad;
	height = h;
	m_r = r;
	m_b = b;
	m_g = g;
	indexBegin = Index;
	printf("%i    ", indexBegin);
	cylinder(0, 0, radius, height, defaultSlices, m_r, m_g, m_b);
	indexEnd = Index;
	printf("%i \n", indexEnd);
}


class pegGame
{
public:
	pegGame(int pegs, int discs);

	//list<peg> myPegs;
	vector<peg> myPegs;
private:
	int numOfPegs;
	int numOfDiscs;
	
};

pegGame::pegGame(int pegs, int discs)
{
	numOfPegs = pegs;
	numOfDiscs = discs;

	for (int i = 0; i < numOfPegs; i++)
	{
		peg temp = peg(0.5, 5, pegPositions[i], 0, 0, 0);
		//myPegs.push_back(peg(0.5, 5, pegPositions[i], 0, 0, 0));

		if (i == 0) {
			for (int j = 0; j < numOfDiscs; j++)
			{
				//glm::vec3 tempPos = temp.position + glm::vec3(0.0f, -5, 0.0f) + glm::vec3(0.0f, 2*j, 0.0f);
				glm::vec3 tempPos = glm:: vec3(temp.position.x, temp.position.y, temp.position.z + (temp.height/2) - j);
				disc tempDisc = disc(numOfDiscs-j, 1, tempPos, discColors[j][0], discColors[j][1], discColors[j][2]);
				temp.discs.push_back(tempDisc);
			}
		}
		myPegs.push_back(temp);
	}

}

pegGame *myGame;

// OpenGL initialization
void
init()
{
	glEnable(GL_DEPTH_TEST);
	// Subdivide a tetrahedron into a sphere
	//tetrahedron(NumTimesToSubdivide);
	/*cylinder(0, 0, 1.5, 1, 20,0,0,0);
	disc1indexNum = Index;
	cylinder(0, 0, 1, 1, 20, 1, 0, 0);
	disc2indexNum = Index;*/

	//myGame = new pegGame(3, 5);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points),
		sizeof(normals), normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "vshader53.glsl" },
		{ GL_FRAGMENT_SHADER, "fshader53.glsl" },
		{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders(shaders);
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)+sizeof(normals)));

	// Initialize shader lighting parameters
	point4 light_position(10, 30, 10, 1.0);
	color4 light_ambient(0.3, 0.3, 0.3, 1.0);
	color4 light_diffuse(0.0, 1.0, 0.0, 0.0);
	color4 light_specular(1.0, 0.0, 0.0, 0.0);

	color4 material_ambient(1.0, 0.1, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.1, 1.0);
	color4 material_specular(1.0, 0.1, 1.0, 1.0);
	float  material_shininess = 5.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
		1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
		1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
		1, specular_product);

	glUniform4fv(glGetUniformLocation(program, "LightPosition"),
		1, light_position);

	glUniform1f(glGetUniformLocation(program, "Shininess"),
		material_shininess);

	// Retrieve transformation uniform variable locations
	//ModelView = glGetUniformLocation(program, "ModelView");
	//Projection = glGetUniformLocation(program, "Projection");

	location = glGetUniformLocation(program, "model_matrix");
	location2 = glGetUniformLocation(program, "camera_matrix");
	location3 = glGetUniformLocation(program, "projection_matrix");

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.5,0.5,0.5, 1.0); /* white background */
}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//point4 at(0.0, 0.0, 0.0, 1.0);
	//point4 eye(0.0, 0.0, 2.0, 1.0);
	//vec4   up(0.0, 1.0, 0.0, 0.0);

	//mat4 model_view = LookAt(eye, at, up);
	//glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);

	if (isGameStarted) {


		glm::mat4 model_view = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));

		//model_view = glm::rotate(model_view, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);

		if (cameraNum == 0) {
			glm::mat4 camera_matrix = glm::lookAt(glm::vec3(0, 56, 0.1), glm::vec3(0.0, 0.1, 0.0), glm::vec3(0.0, 1.0, 0.0));
			glUniformMatrix4fv(location2, 1, GL_FALSE, &camera_matrix[0][0]);
		}
		else if (cameraNum == 1) {
			glm::mat4 camera_matrix = glm::lookAt(glm::vec3(0, 56, -40), glm::vec3(0.0, 0.1, 0.0), glm::vec3(0.0, -1.0, 0.0));
			glUniformMatrix4fv(location2, 1, GL_FALSE, &camera_matrix[0][0]);
		}
		else if (cameraNum == 2) {
			glm::mat4 camera_matrix = glm::lookAt(glm::vec3(0, 21, -55), glm::vec3(0.0, 0.1, 0.0), glm::vec3(0.0, -1.0, 0.0));
			glUniformMatrix4fv(location2, 1, GL_FALSE, &camera_matrix[0][0]);
		}
		else if (cameraNum == 3) {
			glm::mat4 camera_matrix = glm::lookAt(glm::vec3(-7.5, 40, 0.0), glm::vec3(-7.5, 0.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
			glUniformMatrix4fv(location2, 1, GL_FALSE, &camera_matrix[0][0]);
		}
		

		glm::mat4 projection_matrix = glm::perspective(45.0f, 1024.0f / 1024.0f, 0.1f, 100.0f);

		glUniformMatrix4fv(location3, 1, GL_FALSE, &projection_matrix[0][0]);

		//glDrawArrays(GL_QUADS, 0, NumVertices);

		if (myGame != nullptr) {
			for each (peg this_peg in myGame->myPegs)
			{
				model_view = glm::translate(model_view, this_peg.position);
				//printf("%f \n", this_peg.position.x);
				glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);
				glDrawArrays(GL_TRIANGLES, this_peg.indexBegin, this_peg.indexEnd - this_peg.indexBegin);
				model_view = glm::translate(model_view, -this_peg.position);

				for each (disc this_disc in this_peg.discs)
				{
					//printf("%i", i); i++;
					model_view = glm::translate(model_view, this_disc.position);
					glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);
					//if(i == 4)
					glDrawArrays(GL_TRIANGLES, this_disc.indexBegin, this_disc.indexEnd - this_disc.indexBegin);
					model_view = glm::translate(model_view, -this_disc.position);
					//printf("%i ------%i \n ", this_disc.indexBegin, this_disc.indexEnd);

				}
			}

			//printf("\n");
		}

		/*glm::mat4 model_view_2 = glm::translate(model_view, glm::vec3(disc1x, disc1y, disc1z));
		glUniformMatrix4fv(location, 1, GL_FALSE, &model_view_2[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, disc1indexNum);

		glm::mat4 model_view_3 = glm::translate(model_view, glm::vec3(0, 0, 0));
		glUniformMatrix4fv(location, 1, GL_FALSE, &model_view_3[0][0]);
		glDrawArrays(GL_TRIANGLES, disc1indexNum, disc2indexNum);*/

		//glFlush();
		glutSwapBuffers();
	}
}

void clearMovementArray() {
	movementArray[0] = 0;
	movementArray[1] = 0;
	printf("Cleared movement array!");
}

void doMovement() {
	//printf("whatever \n");
	/*list<disc>::iterator i = myGame->myPegs[movementArray[0]-1].discs.end();
	i--;
	myGame->myPegs[movementArray[1]-1].discs.splice(
		i,
		myGame->myPegs[movementArray[0]-1].discs
		);*/
	myGame->myPegs[movementArray[1] - 1].discs.push_back(
		myGame->myPegs[movementArray[0]-1].discs.back()
		);

	myGame->myPegs[movementArray[0] - 1].discs.pop_back();

	myGame->myPegs[movementArray[1] - 1].discs.back().oldPosition = myGame->myPegs[movementArray[1] - 1].discs.back().position;
	myGame->myPegs[movementArray[1] - 1].discs.back().targetPosition = glm::vec3(myGame->myPegs[movementArray[1] - 1].position.x, myGame->myPegs[movementArray[1] - 1].position.y, 
		myGame->myPegs[movementArray[1] - 1].position.z + (myGame->myPegs[movementArray[1] - 1].height / 2) - myGame->myPegs[movementArray[1] - 1].discs.size()+1);

	printf("moving! \n");

	myGame->myPegs[movementArray[1] - 1].discs.back().isAnimating = true;


}

void checkMovement() {
	if (movementArray[0] == movementArray[1]) {
		// Are we trying to move to the same place?
		clearMovementArray();
		return;
	}
		

	if (movementArray[0] > myGame->myPegs.size() || movementArray[1] > myGame->myPegs.size()) {
		//Are both numbers showing a peg?
		clearMovementArray();
		return;
	}

	if (myGame->myPegs[movementArray[0] - 1].discs.size() == 0) {
		// Are we trying to take from an empty peg?
		clearMovementArray();
		return;
	}

	if (myGame->myPegs[movementArray[1] - 1].discs.size() > 0 &&
		myGame->myPegs[movementArray[0]-1].discs.back().radius > myGame->myPegs[movementArray[1]-1].discs.back().radius) {
		clearMovementArray();
		return;
	}
		

	doMovement();
	clearMovementArray();
}


//----------------------------------------------------------------------------
void getInput(int input) {
	if (input > myGame->myPegs.size())
		return;

	if (movementArray[0] == 0) {
		printf("from %i \n", input);
		movementArray[0] = input;
	}
	else if (movementArray[1] == 0) {
		printf("to %i \n", input);
		movementArray[1] = input;
		checkMovement();
	}
	else {
		printf("Error in input!!!! \n");
	}
}

void startGame(int diff) {
	isGameStarted = true;

	switch (diff)
	{
	case 1:
		myGame = new pegGame(3, 3);
		break;
	case 2:
		myGame = new pegGame(4, 4);
		break;
	case 3:
		myGame = new pegGame(4, 6);
		break;
	default:
		break;
	}
}

void switchCamera() {
	cameraNum++;

	if (cameraNum >= numOfCameras) {
		cameraNum = 0;
	}
}

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':
		//printf("1 \n");
		if (isGameStarted) {
			getInput(1);
		}
		else {
			startGame(1);
		}
		break;
	case '2':
		if (isGameStarted) {
			getInput(2);
		}
		else {
			startGame(2);
		}
		break;
	case '3':
		if (isGameStarted) {
			getInput(3);
		}
		else {
			startGame(3);
		}		break;
	case '4':
		if(isGameStarted)
		getInput(4);
		break;
	case 'C':
	case 'c':
		switchCamera();
		break;
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}

	init();
}

//----------------------------------------------------------------------------

//void
//reshape(int width, int height)
//{
//	glViewport(0, 0, width, height);
//
//	GLfloat left = -2.0, right = 2.0;
//	GLfloat top = 2.0, bottom = -2.0;
//	GLfloat zNear = -20.0, zFar = 20.0;
//
//	GLfloat aspect = GLfloat(width) / height;
//
//	if (aspect > 1.0) {
//		left *= aspect;
//		right *= aspect;
//	}
//	else {
//		top /= aspect;
//		bottom /= aspect;
//	}
//
//	mat4 projection = Ortho(left, right, bottom, top, zNear, zFar);
//	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
//}

//----------------------------------------------------------------------------

void checkForWin() {

	bool temp = true;

	for (int i = 0; i < myGame->myPegs.size(); i++) {
		if (i + 1 != myGame->myPegs.size()) {

			if (myGame->myPegs[i].discs.size() != 0) {
				temp = false;
			}
		}
	}


	if (temp == true) {
		exit(EXIT_SUCCESS);
	}

}


void moveDiscUp(disc *animatingDisc) {
	printf("%f \n", animatingDisc->animationPerc);
	animatingDisc->animationPerc = animatingDisc->animationPerc + 0.001f;

	animatingDisc->position = glm::vec3(animatingDisc->position.x, animatingDisc->position.y, 
		(animationH *(animatingDisc->animationPerc)) + (animatingDisc->oldPosition.z * (1- animatingDisc->animationPerc)));

	if (animatingDisc->animationPerc >= 1) {
		animatingDisc->oldPosition = animatingDisc->position;
		animatingDisc->animationPerc = 0;
		animatingDisc->animationStage++;
	}
}

void moveDiscHorizontal(disc* animatingDisc) {
	animatingDisc->animationPerc = animatingDisc->animationPerc + 0.001f;

	animatingDisc->position = glm::vec3(animatingDisc->targetPosition.x * (animatingDisc->animationPerc) + (animatingDisc->oldPosition.x * (1 - animatingDisc->animationPerc)),
		animatingDisc->position.y, animatingDisc->position.z);


	if (animatingDisc->animationPerc >= 1) {
		animatingDisc->oldPosition = animatingDisc->position;
		animatingDisc->animationPerc = 0;
		animatingDisc->animationStage++;
	}
}

void moveDiscDown(disc* animatingDisc) {
	animatingDisc->animationPerc = animatingDisc->animationPerc + 0.001f;

	animatingDisc->position = glm::vec3(animatingDisc->position.x, animatingDisc->position.y,
		(animatingDisc->targetPosition.z *(animatingDisc->animationPerc)) + (animatingDisc->oldPosition.z * (1 - animatingDisc->animationPerc)));

	if (animatingDisc->animationPerc >= 1) {
		animatingDisc->position = animatingDisc->targetPosition;
		animatingDisc->animationPerc = 0;
		animatingDisc->animationStage++;
		checkForWin();
	}
}

void handleAnimation(disc *animatingDisc) {

	switch (animatingDisc->animationStage)
	{
	case 0:
		moveDiscUp(animatingDisc);
		break;
	case 1:
		moveDiscHorizontal(animatingDisc);
		break;
	case 2:
		moveDiscDown(animatingDisc);
		break;
	case 3:
		animatingDisc->animationStage = 0;
		animatingDisc->isAnimating = false;
		break;
	}

	//animatingDisc->position = animatingDisc->targetPosition;
}


void idle()
{
	//disc1y += 0.001;

	if (isGameStarted) {


		for (int i = 0; i < myGame->myPegs.size(); i++) {
			//printf("1");
			if (myGame->myPegs[i].discs.size() > 0 && myGame->myPegs[i].discs.back().isAnimating == true) {

				handleAnimation(&myGame->myPegs[i].discs.back());


			}

		}
		//printf("\n");




		glutPostRedisplay();
	}
}




int
main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	//glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	//glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glEnable(GL_CULL_FACE);
	glutCreateWindow("Sphere");

	glewInit();

	init();

	glutDisplayFunc(display);
	//glutReshapeFunc(reshape);
	glutKeyboardUpFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}