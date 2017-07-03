#pragma once
#include <opencv2\opencv.hpp>
#include <opencvInit.h>

#include <gl/glut.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include <stdio.h>

#include "Vector.h"

namespace iMiner {
	class Cus_PointCloud {
		char fileNmae[50];
		char textureNmae[50];
		int sSize;
		float hRatio;
		cv::Mat img, texture;
		int LoadCVTextures(char *FileName);
		void LoadImgFile(char *strName);
	public:
		void set(char *fileName, float hR = 1, char *textureNmae = NULL, int sS = 16);
		float Height(int X, int Y);
		void RenderHeighMap(bool bRender = 1, int pSize = 0);
	};


	inline void Cus_PointCloud::LoadImgFile(char *strName) {
		printf("file name : %s\n", strName);
		img = cv::imread(strName);
		printf("depth channels : %d\n", img.channels());
	}
	inline int Cus_PointCloud::LoadCVTextures(char *fn) {
		printf("file name : %s\n", fn);
		texture = cv::imread(fn);
		printf("texture channels : %d\n", img.channels());
		return 1;
	}
	inline void Cus_PointCloud::set(char *fileName, float hR, char *textureNmae, int sS) {
		strcpy(this->fileNmae, fileName);
		strcpy(this->textureNmae, textureNmae);
		hRatio = hR;
		sSize = sS;
		LoadImgFile(fileName);
		LoadCVTextures(textureNmae);
	}
	inline float Cus_PointCloud::Height(int X, int Y) {
		float bv = img.data[Y*(img.cols*3) + (X*3) +0 ];
		float gv = img.data[Y*(img.cols*3) + (X*3) +1 ];
		float rv = img.data[Y*(img.cols*3) + (X*3) +2 ];

		return 255 - (vec3(bv, gv, rv).Length() *hRatio);
		//return 0;
	}

	inline void Cus_PointCloud::RenderHeighMap(bool bRender, int pSize) {
		int X = 0, Y = 0;
		float x, y, z;

		glPushMatrix();
		//glColor3i(1, 1, 1);
		if (bRender) {
			glBegin(GL_TRIANGLES);
			//sSize = 3;
		} else {
			glPointSize(pSize);
			glBegin(GL_POINTS);
		}
		float tmpC_A[4] = { 0.5,0.5,0.5,1 };
		float tmpC_D[4] = { 1,1,1, 1.0 };
		float tmpSpec[4] = { 1.0f,1.0f,1.0f, 1.0 };
				
		if (bRender) {
			for (X = 0; X < img.cols - sSize; X += sSize) {
				for (Y = 0; Y < img.rows - sSize; Y += sSize) {
					x = X + sSize;
					y = Height(X + sSize, Y + sSize);
					z = Y + sSize;

					tmpC_D[0] = y / hRatio / 255.0 / 1.6;
					tmpC_D[1] = y / hRatio / 255.0 / 1.3;
					tmpC_D[2] = y / hRatio / 255.0 / 2.2;
					// Set Color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tmpC_D);
					glMaterialfv(GL_FRONT, GL_SPECULAR, tmpSpec);
					glVertex3f(x, y, z);

					x = X;
					y = Height(X, Y + sSize);
					z = Y + sSize;

					tmpC_D[0] = y / hRatio / 255.0 / 1.6;
					tmpC_D[1] = y / hRatio / 255.0 / 1.3;
					tmpC_D[2] = y / hRatio / 255.0 / 1.2;
					// Set Color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tmpC_D);
					glMaterialfv(GL_FRONT, GL_SPECULAR, tmpSpec);
					glVertex3f(x, y, z);

					x = X;
					y = Height(X, Y);
					z = Y;

					tmpC_D[0] = y / hRatio / 255.0 / 1.6;
					tmpC_D[1] = y / hRatio / 255.0 / 1.3;
					tmpC_D[2] = y / hRatio / 255.0 / 1.2;
					// Set Color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tmpC_D);
					glMaterialfv(GL_FRONT, GL_SPECULAR, tmpSpec);
					glVertex3f(x, y, z);


					x = X + sSize;
					y = Height(X + sSize, Y);
					z = Y;

					tmpC_D[0] = y / hRatio / 255.0 / 1.6;
					tmpC_D[1] = y / hRatio / 255.0 / 1.3;
					tmpC_D[2] = y / hRatio / 255.0 / 1.2;
					// Set Color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tmpC_D);
					glMaterialfv(GL_FRONT, GL_SPECULAR, tmpSpec);
					glVertex3f(x, y, z);

					x = X + sSize;
					y = Height(X + sSize, Y + sSize);
					z = Y + sSize;

					tmpC_D[0] = y / hRatio / 255.0 / 1.6;
					tmpC_D[1] = y / hRatio / 255.0 / 1.3;
					tmpC_D[2] = y / hRatio / 255.0 / 1.2;
					// Set Color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tmpC_D);
					glMaterialfv(GL_FRONT, GL_SPECULAR, tmpSpec);
					glVertex3f(x, y, z);

					x = X;
					y = Height(X, Y);
					z = Y;

					tmpC_D[0] = y / hRatio / 255.0 / 1.6;
					tmpC_D[1] = y / hRatio / 255.0 / 1.3;
					tmpC_D[2] = y / hRatio / 255.0 / 1.2;
					// Set Color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tmpC_D);
					glMaterialfv(GL_FRONT, GL_SPECULAR, tmpSpec);
					glVertex3f(x, y, z);
				}
			}
		} 
		else {
			for (X = 0; X < img.cols; X += sSize) {
				for (Y = 0; Y < img.rows; Y += sSize) {
					//x = X;
					//y = Height(X, Y);
					//z = Y;

					//if (y >= 230 || y<=10)
					//	continue;

					x = img.data[Y*(img.cols * 3) + (X * 3) + 0];
					y = img.data[Y*(img.cols * 3) + (X * 3) + 1];
					z = img.data[Y*(img.cols * 3) + (X * 3) + 2];

					//tmpC_D[0] = (255-(x+y+z)/3) / hRatio / 255.0 / 1.6;
					//tmpC_D[1] = (255-(x+y+z)/3) / hRatio / 255.0 / 1.3;
					//tmpC_D[2] = (255-(x+y+z)/3) / hRatio / 255.0 / 1.2;

					tmpC_D[0] = texture.data[Y*(img.cols * 3) + (X * 3) + 2] / hRatio / 255.0 / 1.6;
					tmpC_D[1] = texture.data[Y*(img.cols * 3) + (X * 3) + 1] / hRatio / 255.0 / 1.3;
					tmpC_D[2] = texture.data[Y*(img.cols * 3) + (X * 3) + 0] / hRatio / 255.0 / 1.2;

					// Set Color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tmpC_D);
					glMaterialfv(GL_FRONT, GL_SPECULAR, tmpSpec);
					//glVertex3f(X, y, Y);
					//glVertex3f(X, x, Y);
					//glVertex3f(x, y, z);
					glVertex3f(X, -2*(y-x+z), Y);

					
					//tmpC_D[0] = y / hRatio / 255.0 / 1.6;
					//tmpC_D[1] = y / hRatio / 255.0 / 1.3;
					//tmpC_D[2] = y / hRatio / 255.0 / 1.2;
					//// Set Color
					//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tmpC_D);
					//glMaterialfv(GL_FRONT, GL_SPECULAR, tmpSpec);
					////glVertex3f(X, y, Y);
					////glVertex3f(X, x, Y);
					//glVertex3f(X-600, 0, Y);
					
				}
			}
		}
		glEnd();
		glPopMatrix();
	}

}
