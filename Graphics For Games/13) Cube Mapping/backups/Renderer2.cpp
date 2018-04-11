#include "Renderer2.h"

Renderer2::Renderer2(Parent * parent) {


	rend = parent;

	camera = new Camera(-5.17000437, 214.610184f, Vector3(-6350.60205f, 180.706055f, -5037.33252f));

	/*light = new Light(Vector3(3000.0f, 0.0f, 2000.0f),
		Vector4(1, 1, 1, 1), 15500.0f);*/

	emitter = new ParticleEmitter(); 

	light = new Light(Vector3(-6200.0f, 50, -8000.0f),
		Vector4(1, 1, 1, 1), 12000);

	heightMap = new HeightMap(TEXTUREDIR"NEWHEIGHTMAP.raw");
	
	quad = Mesh::GenerateQuad();

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);
	/*hellData2 = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode2 = new MD5Node(*hellData2);*/

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	//hellData2->AddAnim(MESHDIR"idle2.md5anim");
	//hellNode2->PlayAnim(MESHDIR"idle2.md5anim");

	particleShader = new Shader(SHADERDIR"vertex.glsl", SHADERDIR"fragment.glsl", SHADERDIR"geometry.glsl");

	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl",
		SHADERDIR"shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl ", SHADERDIR"skyboxFragment.glsl");

	if (!sceneShader->LinkProgram() || !shadowShader->LinkProgram() || !skyboxShader->LinkProgram() || !particleShader->LinkProgram()) {
		return;
	}



	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"alien_14.PNG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	

	//heightMap->SetBumpMap(SOIL_load_OGL_texture(
	//	TEXTUREDIR"alienBUMP.PNG", SOIL_LOAD_AUTO,
	//	SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	floor = Mesh::GenerateQuad();
	floor->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"ALIENFLOOR.PNG"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	/*floor->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"alienBUMP.JPG"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));*/

	glEnable(GL_DEPTH_TEST);

	rend->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)rend->width / (float)rend->height, 45.0f);


	


	
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_CULL_FACE);

	rend->SetTextureRepeating(heightMap->GetTexture(), true);
	rend->SetTextureRepeating(heightMap->GetBumpMap(), true);
	rend->SetTextureRepeating(floor->GetTexture(), true);
	rend->init = true;
}Renderer2::~Renderer2(void) {

	delete camera;
	delete light;
	delete hellData;
	delete hellNode;

	delete floor;

	delete emitter;

	delete quad;
	delete sceneShader;
	delete shadowShader;
	rend->currentShader = NULL;
	delete particleShader;
	delete skyboxShader;

	delete heightMap;
}

void Renderer2::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	emitter->Update(msec);
	hellNode->Update(msec);



}

void Renderer2::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	rend->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)rend->width / (float)rend->height, 45.0f);


	DrawSkybox();

	DrawShadowScene(); // First render pass ...
	
	DrawCombinedScene(); // Second render pass ...

	DrawParticle();


	rend->SwapBuffers();
}void Renderer2::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	rend->SetCurrentShader(shadowShader);	rend->viewMatrix = Matrix4::BuildViewMatrix(
		light->GetPosition(), Vector3(0,0,0));
	
	rend->textureMatrix = biasMatrix *(rend->projMatrix * rend->viewMatrix);

	rend->UpdateShaderMatrices();


	floor->Draw();
	
	DrawMesh();

	DrawHeightMap();
	


	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, rend->width, rend->height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);	rend->textureMatrix.ToIdentity();}void Renderer2::DrawCombinedScene() {
	rend->SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(rend->currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	rend->SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	rend->viewMatrix = camera->BuildViewMatrix();
	rend->UpdateShaderMatrices();

	floor->Draw();
	

	DrawMesh();

	DrawHeightMap();
	

	glUseProgram(0);
}

void Renderer2::DrawHeightMap()
{


	

	rend->modelMatrix = Matrix4::Translation(Vector3(-6600, -450, -10000));
	


	Matrix4 tempMatrix = rend->textureMatrix * rend->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& rend->modelMatrix.values);

	heightMap->Draw();

	//rend->modelMatrix.ToIdentity();
	//rend->textureMatrix.ToIdentity();



}void Renderer2::DrawMesh() {
	
	
	rend->modelMatrix = Matrix4::Translation(Vector3(-6000.0f, -68, -8000.0f));


	Matrix4 tempMatrix = rend->textureMatrix * rend->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& rend->modelMatrix.values);

	hellNode->Draw(*rend);
	rend->modelMatrix.ToIdentity();
}

void Renderer2::DrawFloor() {


	rend->modelMatrix = Matrix4::Rotation(90, Vector3(0, -4000, 0)) *
		Matrix4::Scale(Vector3(1800, 1800, 1));
	Matrix4 tempMatrix = rend->textureMatrix * rend->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& rend->modelMatrix.values);

	floor->Draw();
	rend->modelMatrix.ToIdentity();
}

void Renderer2::DrawSkybox() {

	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);
	glGetUniformLocation(skyboxShader->GetProgram(), "cubeMap2"), 0;
	rend->SetCurrentShader(skyboxShader);

	rend->UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);

	glEnable(GL_CULL_FACE);

}





void Renderer2::DrawParticle()
{
	glDepthMask(GL_FALSE);
	rend->SetCurrentShader(particleShader);

	rend->modelMatrix = Matrix4::Translation(Vector3(-6600, -450, -10000));
	


	emitter->SetDirection(Vector3(0, -30, 0));
	emitter->SetParticleSize(28.0f);
	emitter->SetParticleVariance(10.0f);
	emitter->SetLaunchParticles(1.0f);
	emitter->SetParticleLifetime(5000.0f);
	emitter->SetParticleSpeed(0.5f);
	emitter->SetParticleRate(0.75f);
	emitter->SetPosition(Vector3(10000.0f, 1000.0f, 10000.0f));
	


	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1f(glGetUniformLocation(rend->currentShader->GetProgram(), "particleSize"), emitter->GetParticleSize());

	rend->UpdateShaderMatrices();
	emitter->Draw();
	glUseProgram(0);
	glDepthMask(GL_TRUE);
	rend->modelMatrix.ToIdentity();

}

void Renderer2::initialise() {

	cubeMap2 = SOIL_load_OGL_cubemap(TEXTUREDIR"nuke_ft.tga", TEXTUREDIR"nuke_bk.tga", TEXTUREDIR"nuke_up.tga", TEXTUREDIR"nuke_dn.tga", TEXTUREDIR"nuke_rt.tga", TEXTUREDIR"nuke_lf.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);
	rend->textureMatrix.ToIdentity();
	rend->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)rend->width / (float)rend->height, 45.0f);
	rend->modelMatrix.ToIdentity();
	rend->viewMatrix = camera->BuildViewMatrix();
	rend->UpdateShaderMatrices();
}