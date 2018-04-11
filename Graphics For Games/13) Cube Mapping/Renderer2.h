#pragma once

#include "Parent.h"


#define SHADOWSIZE 2048

class Renderer2 {
public:
	Renderer2(Parent * parent);
	virtual ~Renderer2(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);


	void createText();
	void initialise();
protected:
	void DrawMesh();
	void DrawFloor();
	void DrawShadowScene();
	void DrawCombinedScene();
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

	Mesh* floor;
	Camera* camera;
	Light* light;
	HeightMap* heightMap;	GLuint cubeMap2;	ParticleEmitter*	emitter;	Parent* rend;	Font*	basicFont;
	Shader* textShader;	float framerate;};