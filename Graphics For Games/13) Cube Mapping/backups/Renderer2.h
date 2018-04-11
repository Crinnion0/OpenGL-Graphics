#pragma once

#include "Parent.h"
//#include "../../nclgl/OGLRenderer.h"
//#include "../../nclgl/Camera.h"
//#include "../../nclgl/MD5Mesh.h"
//#include "../../nclgl/MD5Node.h"
//#include "../../nclgl/HeightMap.h"
//#include ".././nclgl/ParticleEmitter.h"

#define SHADOWSIZE 2048 // New !

class Renderer2  {
public:
	Renderer2(Parent * parent);
	virtual ~Renderer2(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

	void initialise();
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

	//MD5FileData* hellData2;
	//MD5Node* hellNode2;

	Mesh* floor;
	Camera* camera;
	Light* light;
	HeightMap* heightMap;	GLuint cubeMap2;	ParticleEmitter*	emitter;	Parent* rend;};