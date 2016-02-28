// Core Libraries
#include <iostream>
#include <string>
#include <math.h>
#include <winsock2.h>
#include <GLUT\glut.h>
#include <Thread>
#include <ctime>


#pragma comment(lib, "ws2_32.lib")
#define BUFLEN 512
#define PORT 8888
#define MILI_PER_SECOND 1000
//define network variables
clock_t Timer;
float intervalTime;
float lastPOSX, lastPOSY;
float xVel, yVel;
clock_t LastTimeRECV;
SOCKET s;
struct sockaddr_in sockAddr;
int slen = sizeof(sockAddr);
char buf[1024];
char msg[1024];
bool deadReckoning = false;

// Defines and Core variables
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Miliseconds per frame

int windowWidth = 800;
int windowHeight = 600;

int mousepositionX;
int mousepositionY;

float redTransX, redTransY = 0.0f;

float xTrans = 100.0;
float yTrans = 0.0;
int keyDown[256];

// A few conversions to know
float degToRad = 3.14159f / 180.0f;
float radToDeg = 180.0f / 3.14159f;

bool running = true;

bool connected = false;

bool recordPOS = false;



// Receive function running on separate thread.
void receive()
{
	char buffer[1024];
	float XY[2];
	while (running)
	{
		memset(buffer, 0, 1024);
		recvfrom(s, buffer, 1024, 0, (struct sockaddr *) &sockAddr, &slen);

		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK && error != 0)
		{
			std::cout << "Error code : " << error << "\n";

			shutdown(s, SD_SEND);
			closesocket(s);
			break;
		}
		else if (error == 0)
		{
			if (!connected)
			{
				connected = true;
				continue;
			}
			lastPOSX = XY[0];
			lastPOSY = XY[1];
			Timer = clock();
			intervalTime = ((Timer - LastTimeRECV)/ (double)CLOCKS_PER_SEC);
			memcpy(XY, buffer, sizeof(float) * 2);
				
			std::cout << "RECV: " << XY[0] << "," << XY[1] << std::endl;

			redTransX = XY[0];
			redTransY = XY[1];

			xVel = (redTransX - lastPOSX) / intervalTime;
			yVel = (redTransY - lastPOSY) / intervalTime;
			recordPOS = true;
			LastTimeRECV = Timer;
		}
	}
}

/* function DisplayCallbackFunction(void)
* Description:
*  - this is the openGL display routine
*  - this draws the sprites appropriately
*/
void DisplayCallbackFunction(void)
{
	/* clear the screen */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); // clear our the transform matrix

	/* This is where we draw thigns */

	if (deadReckoning)
	if (!recordPOS)
	{
		redTransX = redTransX + (xVel * (FRAME_DELAY / 1000.0f));
		redTransY = redTransY + (yVel * (FRAME_DELAY / 1000.0f));
	}
	else
	{
		recordPOS = false;
	}
	//Transformations
	glTranslatef(redTransX, redTransY, -200.f);


	glColor3f(0.8f, 0.0f, 0.0f);

	glBegin(GL_POLYGON);
		glVertex3f(-10.0f, -10.0f, 10.0f);
		glVertex3f(10.0f, -10.0f, 10.0f);
		glVertex3f(10.0f, 10.0f, 10.0f);
		glVertex3f(-10.0f, 10.0f, 10.0f);
	glEnd();

	glLoadIdentity();
	
	glTranslatef(xTrans, yTrans, -200.f);
	glColor3f(0.0f, 0.0f, 0.8f);

	glBegin(GL_POLYGON);
	glVertex3f(-10.0f, -10.0f, 10.0f);
	glVertex3f(10.0f, -10.0f, 10.0f);
	glVertex3f(10.0f, 10.0f, 10.0f);
	glVertex3f(-10.0f, 10.0f, 10.0f);
	glEnd();



	/* Swap Buffers to Make it show up on screen */
	glutSwapBuffers();
}

/* function void KeyboardCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is pressed
*/
void KeyboardCallbackFunction(unsigned char key, int x, int y)
{
	std::cout << "Key Down:" << (int)key << std::endl;
	keyDown[key] = 1;


	switch (key)
	{
	case 32: // the space bar
		break;
	case 27: // the escape key
	case 'q': // the 'q' key
		exit(0);
		break;
	} 
}

/* function void KeyboardUpCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is lifted
*/
void KeyboardUpCallbackFunction(unsigned char key, int x, int y)
{
	keyDown[key] = 0;
}

void idleFunc()
{
	
}

/* function TimerCallbackFunction(int value)
* Description:
*  - this is called many times per second
*  - this enables you to animate things
*  - no drawing, just changing the state
*  - changes the frame number and calls for a redisplay
*  - FRAME_DELAY is the number of milliseconds to wait before calling the timer again
*/
void TimerCallbackFunction(int value)
{

	/* this call makes it actually show up on screen */
	glutPostRedisplay();

	bool sendPacket = (keyDown['a'] || keyDown['d'] || keyDown['w'] || keyDown['s']);

	if (keyDown['a'])
		xTrans -= 5.0;
	if (keyDown['d'])
		xTrans += 5.0;
	if (keyDown['w'])
		yTrans += 5.0;
	if (keyDown['s'])
		yTrans -= 5.0;
	if (keyDown['f'])
		deadReckoning = !deadReckoning;
	if (sendPacket)
	{
		//////// SOCKETS ////////////////////
		//Copy xTrans to msg, then send
		memcpy(msg, &xTrans, sizeof(float));
		memcpy(msg + sizeof(float), &yTrans, sizeof(float));

		if (sendto(s, msg, sizeof(float) * 2, 0, (struct sockaddr*) &sockAddr, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		memset(msg, '\0', BUFLEN);

		std::cout << "SENT: " << xTrans << "," << yTrans << std::endl;
	}
	/* this call gives it a proper frame delay to hit our target FPS */
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}

/* function WindowReshapeCallbackFunction()
* Description:
*  - this is called whenever the window is resized
*  - and sets up the projection matrix properly
*/
void WindowReshapeCallbackFunction(int w, int h)
{
	// switch to projection because we're changing projection
	float asp = (float)w / (float)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Setup a perspective projection
	gluPerspective(90.f, 1.f, 1.f, 10000.f); // (fov, aspect, near z, far z)	

	//gluOrtho2D(0, w, 0, h);

	windowWidth = w;
	windowHeight = h;
	glViewport(0, 0, windowWidth, windowHeight);

	//switch back to modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	// Handle mouse clicks
	if (state == GLUT_DOWN)
	{
		std::cout << "Mouse x:" << x << " y:" << y << std::endl;
	}
}


/* function MouseMotionCallbackFunction()
* Description:
*   - this is called when the mouse is clicked and moves
*/
void MouseMotionCallbackFunction(int x, int y)
{
}

/* function MousePassiveMotionCallbackFunction()
* Description:
*   - this is called when the mouse is moved in the window
*/
void MousePassiveMotionCallbackFunction(int x, int y)
{
	mousepositionX = x;
	mousepositionY = y;
}

void init()
{
	// NETWORK Initialization
	WSADATA wsa;

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_port = htons(PORT);

	if (bind(s, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	// If iMode!=0, non-blocking mode is enabled.
	u_long iMode = 1;
	ioctlsocket(s, FIONBIO, &iMode);
	///// INIT OpenGL /////
	// Set color clear value
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);

	// Enable Z-buffer read and write (for hidden surface removal)
	glEnable(GL_DEPTH_TEST);

}


/* function main()
* Description:
*  - this is the main function
*  - does initialization and then calls glutMainLoop() to start the event handler
*/

int main(int argc, char **argv)
{
	/* initialize the window and OpenGL properly */
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("INFR1350U - Example");

	/* set up our function callbacks */
	glutDisplayFunc(DisplayCallbackFunction);
	glutKeyboardFunc(KeyboardCallbackFunction);
	glutKeyboardUpFunc(KeyboardUpCallbackFunction);
	glutIdleFunc(idleFunc);
	glutReshapeFunc(WindowReshapeCallbackFunction);
	glutMouseFunc(MouseClickCallbackFunction);
	glutMotionFunc(MouseMotionCallbackFunction);
	glutPassiveMotionFunc(MousePassiveMotionCallbackFunction);
	glutTimerFunc(1, TimerCallbackFunction, 0);

	init(); //Setup OpenGL States

	std::thread t1(receive);
	/* start the event handler */
	glutMainLoop();
	running = false;
	t1.join();
	return 0;
}