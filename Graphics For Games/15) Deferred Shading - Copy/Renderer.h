#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/heightmap.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/ParticleEmitter.h"


#define LIGHTNUM 16 // We ’ll generate LIGHTNUM squared lights ...

#define SHADOWSIZE 2048

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	

protected:
	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass
	void DrawParticle();
	// Make a new texture ...
	void GenerateScreenTexture(GLuint & into, bool depth = false);		void DrawMesh(); // New !
	void DrawFloor(); // New !
	void DrawShadowScene(); // New !
	void DrawCombinedScene();	Shader* particleShader;	Shader* sceneShader; // Shader to fill our GBuffers
	Shader* pointlightShader; // Shader to calculate lighting
	Shader* combineShader; // shader to stick it all together
	Shader* shadowShader;

	Light* pointLights; // Array of lighting data
	HeightMap* heightMap; // Terrain !
	OBJMesh* sphere; // Light volume
	Mesh* quad; // To draw a full - screen quad
	Camera* camera; // Our usual camera

	float rotation; // How much to increase rotation by

	GLuint bufferFBO; // FBO for our G- Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here

	GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex; // Store specular lighting
	SceneNode* root;

	ParticleEmitter* emitter;

	MD5FileData* hellData;
	MD5Node* hellNode;
};

