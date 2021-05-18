#pragma once
class Cube {
	f32 width;
	f32 height;
	f32 depth;

	f32* vertices;
	ui32* indices;
public:
	Cube(f32 width = 1.0f, f32 height = 1.0f, f32 depth = 1.0f)
		:width(width), height(height), depth(depth),
		vertices(new f32[16 * 8]), indices(new ui32[6 * 6]) {

		f32 wm = width / 2.0f;
		f32 hm = width / 2.0f;
		f32 dm = width / 2.0f;

		//temporal de vertices
		f32 temp[] = {
			//vertices(posiciones)  //colores         //coord. textura
			//cara de los lados
			-wm,  hm,  dm,    0.5f,0.5f,0.5f,     0.0f,1.0f,
			 wm,  hm,  dm,    0.5f,0.5f,0.5f,     1.0f,1.0f,
			-wm, -hm,  dm,    0.5f,0.5f,0.5f,     0.0f,0.0f,
			 wm, -hm,  dm,    0.5f,0.5f,0.5f,     1.0f,0.0f,
			-wm,  hm, -dm,    0.5f,0.5f,0.5f,     1.0f,1.0f,
			 wm,  hm, -dm,    0.5f,0.5f,0.5f,     0.0f,1.0f,
			-wm, -hm, -dm,    0.5f,0.5f,0.5f,     1.0f,0.0f,
			 wm, -hm, -dm,    0.5f,0.5f,0.5f,     0.0f,0.0f,
			 //cara de arriba
			-wm,  hm,  dm,     1.0f, 1.0f, 1.0f,    0.0f, 0.0f,
			 wm,  hm,  dm,     1.0f, 1.0f, 1.0f,    1.0f, 0.0f,
			-wm, -hm,  dm,     1.0f, 1.0f, 1.0f,    0.0f, 0.0f,
			 wm, -hm,  dm,     1.0f, 1.0f, 1.0f,    1.0f, 0.0f,
			-wm,  hm, -dm,     1.0f, 1.0f, 1.0f,    0.0f, 1.0f,
			 wm,  hm, -dm,     1.0f, 1.0f, 1.0f,    1.0f, 1.0f,
			-wm, -hm, -dm,     1.0f, 1.0f, 1.0f,    0.0f, 1.0f,
			 wm, -hm, -dm,     1.0f, 1.0f, 1.0f,    1.0f, 1.0f };

		for (ui32 i = 0; i < 16 * 8; ++i)
		{
			vertices[i] = temp[i];
		}
		//temporal de indices
		ui32 temp2[] = {
			0,  1,  2,		1,2,3,
			8,  9,  12,		9,12,13,
			1,  5,  3,		3,5,7,
			11, 14, 15,	    10,11,14,
			0,  4,  6,		0,2,6,
			4,  5,  6,		5,6,7 };

		for (ui32 i = 0; i < 6 * 6; ++i)
		{
			indices[i] = temp2[i];
		}
	}
	~Cube() {
		delete[] vertices;
		delete[] indices;
	}
	f32* getVertices() {
		return vertices;
	}
	ui32* getIndices() {
		return indices;
	}
	f32 getVsize() {
		return 16 * 8;
	}
	ui32 getIsize() {
		return 6 * 6;
	}
};