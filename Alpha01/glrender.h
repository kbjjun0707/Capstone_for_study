#pragma once

#include <QOpenGLWidget>
#include <QWheelEvent>
#include <QOpenGLFunctions>
#include <QMatrix4x4>

class Alpha01;

class GlRender : public QOpenGLWidget, protected QOpenGLFunctions {
	Q_OBJECT
public:
	explicit GlRender(QWidget* parent = nullptr);
	virtual ~GlRender();

	QSurfaceFormat format;
	float m_FirstFar;
	float m_CamX, m_CamY, m_CamFar, m_CamAxisX, m_CamAxisY;

	void setQMain(Alpha01 * qMainWindow);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	virtual void timerEvent(QTimerEvent *e);

	virtual void wheelEvent(QWheelEvent *e);

	int xAtPress, yAtPress;
	float dx, dy;
	bool mouseClicked = 0;

	virtual void mousePressEvent(QMouseEvent *e);

	virtual void mouseMoveEvent(QMouseEvent *e);

	virtual void mouseReleaseEvent(QMouseEvent *e);

private:
	Alpha01 *qMainWin;
	QMatrix4x4 *m_GLCam;

	int m_CamModel;

	///////////////////////////SLOTS//////////////////////////
	private slots :
	void cleanup();

};