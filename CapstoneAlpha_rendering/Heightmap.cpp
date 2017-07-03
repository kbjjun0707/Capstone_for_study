#include <iostream>

#include <gl/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>

#include "Heightmap.h"

void HeightMap::LoadRawFile(LPSTR strName, int nSize) {
	FILE *fp = NULL; fopen_s(&fp, strName, "rb");

	if (!fp) {
		MessageBox(NULL, "Can't FInd The Height Map!", "Error", MB_OK);
		return;
	}
	fread(heightMap, 1, nSize, fp);

	int result = ferror(fp);

	if (result) {
		MessageBox(NULL, "Failed To Get Data!", "Error", MB_OK);
	}

	fclose(fp);
}
float HeightMap::Height(int X, int Y) {
	int x = X%xSize;
	int y = Y%ySize;

	if (!heightMap) return 0;

	return (float)heightMap[x + (y*xSize)]*hRatio;
}
void HeightMap::RenderHeighMap(bool bRender, int pSize) {
	int X = 0, Y = 0;
	float x, y, z;
	if (!heightMap) return;

	glPushMatrix();
	//glColor3i(1, 1, 1);
	if (bRender) {
		glBegin(GL_TRIANGLES);
		sSize = 3;
	}
	else {
		glPointSize(pSize);
		glBegin(GL_POINTS);
	}
	float tmpC_A[4] = { 0.5,0.5,0.5,1 };
	float tmpC_D[4] = { 1,1,1, 1.0 };
	float tmpSpec[4] = { 1.0f,1.0f,1.0f, 1.0 };

	if (bRender) {
		for (X = 0; X < (xSize - sSize); X += sSize) {
			for (Y = 0; Y < (ySize - sSize); Y += sSize) {
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
		for (X = 0; X < (xSize - sSize); X += sSize) {
			for (Y = 0; Y < (ySize - sSize); Y += sSize) {
				x = X;
				y = 1.5*Height(X, Y);
				z = Y;
				
				if (y <= 30)
					continue;

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
	glEnd();
	glPopMatrix();
}
void HeightMap::Mapset(char *fileName, int xS, int yS, float hR, char *textureName, int sS) {
	TextureID = -1;
	sSize = sS;
	xSize = xS;	ySize = yS;
	hRatio = hR;
	heightMap = new BYTE[xSize*ySize];
	LoadRawFile(fileName, xSize*ySize);

}
AUX_RGBImageRec *HeightMap::LoadBMP(char *Filename) {
	FILE *fp = NULL;
	if (!Filename) return NULL;
	fopen_s(&fp, Filename, "r");
	if (fp) {
		fclose(fp);
		return auxDIBImageLoad(Filename);
	}
	return NULL;
}
int HeightMap::LoadGLTextures(char *FileName) {
	if (!FileName) return -1;

	int Status = TRUE;
	AUX_RGBImageRec *Textureimage;

	Textureimage = NULL;
	if (Textureimage = LoadBMP(FileName)) {
		glGenTextures(1, &this->TextureID);
		glBindTexture(GL_TEXTURE_2D, this->TextureID);
		
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Textureimage->sizeX, Textureimage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, Textureimage->data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);	// GL_LINEAR_MIPMAP_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);			// GL_SPHERE_MAP
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	}
	else {
		MessageBox(NULL, "Image file has a error !", FileName, MB_ICONERROR);
		Status = false;
	}
	if (Textureimage) {
		if (Textureimage->data)free(Textureimage->data);
		free(Textureimage);
	}

	return Status;
}
