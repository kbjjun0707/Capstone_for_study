#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#ifndef _WINDOWS_
#include <Windows.h>
#endif

#ifndef _VECTOR_
#include <vector>
#endif

#ifndef BMP_H
#include "bmp.h"
#endif // !BMP_H

class MapInfo {
public:
	char Nmae[24];
	int No;
	unsigned int TextureID;
	int texture_is;
};

class HeightMap : public MapInfo {
	int sSize;
	float hRatio;
	BYTE *heightMap;
	AUX_RGBImageRec *LoadBMP(char *Filename);
	int LoadGLTextures(char *FileName);
	void LoadRawFile(LPSTR strName, int nSize);
public:
	int xSize, ySize;
	void Mapset(char *fileName, int xS, int yS, float hR = 1, char *textureNmae = NULL, int sS = 16);
	float Height(int X, int Y);
	void RenderHeighMap(bool bRender = 1, int pSize = 0);
};


#endif