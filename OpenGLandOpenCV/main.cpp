#define _CRT_SECURE_NO_WARNINGS
// because in few files of OpenCV using unsafe function like 'fopen'
// more information you can see in your Error List in Visual Studio if you delete this 'define'

// graphics
#include <GL\glut.h>
#include <cmath>
// screen capture
#include <opencv\cv.h>
#include <opencv\cvaux.h>
#include <opencv\highgui.h>

using namespace cv;

CvVideoWriter* writer = 0;

int LENGTH = 600;// size of screen
float meshSize = 600;// size of grid

float cRe;
float cIm;
float R;

float xMax;
float xMin;
float yMax;
float yMin;

void Initilize();
void display();
void Julia(int maxIteration);
void timer(int value);
//
void screenCapture();


int main(int argc, char* argv[])
{
	/*cRe = -0.74543;
	cIm = 0.11301;*/
	cRe = 0.285;
	cIm = 0.001;
	R = (1 + sqrtf(1 + 4 * sqrtf(cRe*cRe + cIm*cIm))) / 2.0;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(LENGTH, LENGTH);
	glutCreateWindow("Demo");

	Initilize();
	glutDisplayFunc(display);
	glutTimerFunc(100, timer, 0);
	glutMainLoop();

	cvReleaseVideoWriter(&writer);
	delete writer;

	return 0;
}

void screenCapture()
{
	unsigned char* pixels = (unsigned char*)calloc(LENGTH*LENGTH * 3, sizeof(char));
	glReadPixels(-R, -R, LENGTH, LENGTH, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	// Attention, please. OpenCV write frame in BGR
	// opencv
	IplImage* image = cvCreateImage(cvSize(LENGTH, LENGTH), IPL_DEPTH_8U, 3);
	image->imageData = (char*)pixels;
	cvWriteFrame(writer, image);
	cvReleaseImage(&image);
}

void Initilize()
{
	writer = cvCreateVideoWriter("animation.avi", CV_FOURCC('j', 'p', 'e', 'g'), 15, cvSize(LENGTH, LENGTH), 1);

	glClearColor(0, 0.7, 0.7, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	xMin = -R;
	xMax = R;
	yMin = -R;
	yMax = R;

	gluOrtho2D(xMin, xMax, yMin, yMax);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

void Julia(int maxIteration)
{
	int k;
	float size = LENGTH / meshSize;
	float dx = (xMax - xMin) / meshSize;
	float dy = (yMax - yMin) / meshSize;
	float x, y, Re, Im;
	float currentRe, currentIm;

	for (int j = 0; j < meshSize; j++)
	{
		for (int i = 0; i < meshSize; i++)
		{
			Re = xMin + i * dx;
			Im = yMin + j * dy;
			x = Re;
			y = Im;
			for (k = 0; k < maxIteration; k++)
			{
				currentRe = Re;
				currentIm = Im;
				// z^2 + c
				Re = currentRe*currentRe - currentIm*currentIm + cRe;
				Im = 2 * currentRe*currentIm + cIm;

				if (Re*Re + Im*Im > R*R)
				{
					float colorRed = (float)k / maxIteration;
					float colorGreen = (k + 0.3 * maxIteration) / maxIteration;
					float colorBlue = (k + 0.5 * maxIteration) / maxIteration;
					glColor3f(colorRed, colorGreen, colorBlue);
					glPointSize(size);
					glBegin(GL_POINTS);
					glVertex2f(x, y);
					glEnd();
					break;
				}
			}
		}
	}
}

void timer(int value)
{
	if (value < 100)
	{
		value++;
		Julia(value);
		glutSwapBuffers();
		screenCapture();
		glutTimerFunc(20, timer, value);
	}
}