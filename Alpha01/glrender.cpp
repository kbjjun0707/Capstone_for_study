#include "glrender.h"
#include "alpha01.h"
#include <opencv2\opencv.hpp>

#include <gl/glu.h>

float CameraModelVertices[] = {
	-0.5f, -0.5f, -0.5f, // triangle 1 : begin
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f, // triangle 1 : end
	 0.5f,  0.5f, -0.5f, // triangle 2 : begin
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f, // triangle 2 : end
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f
};

GlRender::GlRender(QWidget* parent) : QOpenGLWidget(parent){ 
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	//format.setVersion(3, 2);
	format.setProfile(QSurfaceFormat::CoreProfile);
	this->setFormat(format);
	
	startTimer(40);

	m_CamX = m_CamY = m_CamAxisX = m_CamAxisY = 0;

	m_FirstFar = -30.0f;
	m_CamFar = m_FirstFar;

	m_GLCam = new QMatrix4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

GlRender::~GlRender() {

	delete m_GLCam;
}

void GlRender::setQMain(Alpha01 * qMainWindow) {
	qMainWin = qMainWindow;
}

void GlRender::initializeGL() {
	initializeOpenGLFunctions();
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	m_CamModel = glGenLists(1);
	glNewList(m_CamModel, GL_COMPILE); {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, CameraModelVertices);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(CameraModelVertices) / 12);
		glDisableClientState(GL_VERTEX_ARRAY);
	}glEndList();

	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GlRender::cleanup);
}

void GlRender::resizeGL(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);
	gluPerspective(45, (float)w / h, 0.00001, 10000);

}
int a = 0;

void GlRender::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix(); {
		gluLookAt(0, 0, m_CamFar, 0, 0, m_FirstFar - m_CamFar, 0, 1, 0);

		glRotatef(dy, 1, 0, 0);
		glRotatef(dx, 0, 1, 0);
		iMiner::Scanner *tmpScnnr = qMainWin->m_Scanner;
		for (int i = 0; (i < tmpScnnr->m_RecoverDone); i++) {
			glPushMatrix(); {
				glMultMatrixf(tmpScnnr->getAffineCameraPose(i));
				glScalef(0.3, 0.3, 0.3);
				glColor3f(140, 0, 140);
				glCallList(m_CamModel);
			}glPopMatrix();

		}
		for (int j = 0; (tmpScnnr->m_RecoverDone > 0) && (j < tmpScnnr->testData.size()); j++) {
			cv::Mat point = tmpScnnr->testData[j];
			glPushMatrix(); {
				glColor3f(0, 180, 0);
				glBegin(GL_POINTS); {
					for (int j = 0; j < point.rows; j++) {
						glVertex3f(point.at<float>(j, 0), point.at<float>(j, 1), point.at<float>(j, 2));
					}
				}glEnd();

			}glPopMatrix();
		}
		
	}glPopMatrix();
}

void GlRender::timerEvent(QTimerEvent * e) {
	m_CamAxisY += 0.005;
	update();
}

void GlRender::wheelEvent(QWheelEvent * e) {
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;

	if (e->orientation() == Qt::Horizontal) {
		m_CamFar += numSteps;
	} else {
		m_CamFar -= numSteps;
	}
	e->accept();
}

void GlRender::mousePressEvent(QMouseEvent * e) {
	mouseClicked = true;

	xAtPress = e->x();
	yAtPress = e->y();
}

void GlRender::mouseMoveEvent(QMouseEvent * e) {
	if (mouseClicked) {
		int xAtRelease = e->x();
		int yAtRelease = e->y();

		// example code
		dx = (xAtRelease - xAtPress);
		dy = (yAtRelease - yAtPress);

		//xAtPress = xAtRelease;
		//yAtPress = yAtRelease;
	}
}

void GlRender::mouseReleaseEvent(QMouseEvent * e) {
	mouseClicked = false;
}

void GlRender::cleanup() {

}