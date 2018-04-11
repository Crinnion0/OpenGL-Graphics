#pragma once


#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/ParticleEmitter.h"	//A new class!
#include "../../nclgl/TextMesh.h"



class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);



protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawParticle();
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);
	

	void createText();

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

	GLuint cubeMap;

	float waterRotate;
	void SetShaderParticleSize(float f);	ParticleEmitter*	emitter;		float framerate;};