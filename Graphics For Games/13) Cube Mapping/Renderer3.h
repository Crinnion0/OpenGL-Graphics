#pragma once

#include "Parent.h"


//#include "../../nclgl/OGLRenderer.h"
//#include "../../nclgl/Camera.h"
//#include "../../nclgl/OBJmesh.h"
//#include "../../nclgl/heightmap.h"
//#include "../../nclgl/SceneNode.h"
//#include "../../nclgl/MD5Mesh.h"
//#include "../../nclgl/MD5Node.h"
//#include "../../nclgl/ParticleEmitter.h"


#define LIGHTNUM 16 // We ’ll generate LIGHTNUM squared lights ...

#define SHADOWSIZE 2048

class Renderer3 {
public:
	Renderer3(Parent * parent);
	virtual ~Renderer3(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void initialise();

	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);


	void createText();

protected:
	void FillBuffers(); //G- Buffer Fill  Pass
	void DrawPointLights(); // Lighting  Pass
	void CombineBuffers(); // Combination  Pass
	void DrawParticle();

	void GenerateScreenTexture(GLuint & into, bool depth = false);	void DrawMesh();
	Shader* particleShader;	Shader* sceneShader; // Fill Buffer
	Shader* pointlightShader; // Light point shader
	Shader* combineShader; // Combine Shader
	Shader* shadowShader;

	Light* pointLights; // Array of lights
	HeightMap* heightMap;
	OBJMesh* sphere; // Light spherical volume
	Mesh* quad;
	Camera* camera;

	float rotation;

	GLuint bufferFBO; // FBO for our G- Buffer
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;

	GLuint pointLightFBO; // FBO for our lighting 
	GLuint lightEmissiveTex; // Emissive lights
	GLuint lightSpecularTex; // Specular lights


	ParticleEmitter* emitter;

	MD5FileData* hellData;
	MD5Node* hellNode;

	Parent* rend;


	Font*	basicFont;
	Shader* textShader;	float framerate;
};

