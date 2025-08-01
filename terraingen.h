#pragma once
#include <mgdl.h>

#ifdef __cplusplus
extern "C" {
#endif

	static Mesh* GenerateFromPNG(float heightMultiplier, float uvRange, PNGFile* png, V3f forward, V3f right );
	static Mesh* CreateGridMesh(float width, float depth, float uvRange, V3f forward, V3f right);
	static Mesh* CreateNormals(Mesh* mesh, float width, float depth);
	static Mesh* Mesh_CreateUVs(Mesh* mesh, float range, float width, float depth);
	static Mesh* Mesh_SetHeightsFromPNG(Mesh* mesh, PNGFile* png, float heightMultiplier, float width, float depth);

	/*
	static TerrainMesh* CalculateMatcapFromNormals(TerrainMesh* mesh);
	static TerrainMesh* CalculateMatcapFromCamera(TerrainMesh* mesh, vec3 cameraDirection);
	*/
#ifdef __cplusplus
}
#endif
