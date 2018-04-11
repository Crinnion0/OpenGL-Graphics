#pragma once


#include "Parent.h"
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/ParticleEmitter.h"	
#include "../../nclgl/TextMesh.h"



class Renderer {
public:
	Renderer(Parent * parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void initialise();

	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);


	void createText();

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawParticle();


	Font*	basicFont;
	Shader* textShader;


	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* particleShader;


	HeightMap* heightMap;
	Mesh* quad;

	Light* light;
	Camera* camera;

	GLuint cubeMap1;

	float waterRotate;
	void SetShaderParticleSize(float f);	ParticleEmitter*	emitter;	float framerate;	Parent* rend;};