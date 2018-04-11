#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/HeightMap.h"
#include ".././nclgl/ParticleEmitter.h"

#define SHADOWSIZE 2048 // New !

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected:
	void DrawMesh(); 
	void DrawMesh2();
	void DrawMesh3();
	void DrawMesh4();// New !
	void DrawFloor(); // New !
	void DrawShadowScene(); // New !
	void DrawCombinedScene(); // New !
	void DrawHeightMap();
	void DrawSkybox();
	void DrawBox();
	void DrawParticle();

	Shader* particleShader;
	Shader* sceneShader;
	Shader* shadowShader;
	Shader* skyboxShader;
	Mesh* quad;

	GLuint shadowTex;
	GLuint shadowFBO;

	MD5FileData* hellData;
	MD5Node* hellNode;

	MD5FileData* hellData2;
	MD5Node* hellNode2;

	Mesh* floor;
	Camera* camera;
	Light* light;
	HeightMap* heightMap;	GLuint cubeMap;	ParticleEmitter*	emitter;};