#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {


	camera = new Camera(-5.17000437, 214.610184f, Vector3(-1029.46399f, 180.706055f, -1102.29346f));

	/*light = new Light(Vector3(3000.0f, 0.0f, 2000.0f),
		Vector4(1, 1, 1, 1), 15500.0f);*/

	emitter = new ParticleEmitter();

	light = new Light(Vector3(-2068.57349f, 1343.00977f, -1854.89990f),
		Vector4(1, 1, 1, 1), 10500);

	heightMap = new HeightMap(TEXTUREDIR"NEWHEIGHTMAP.raw");
	
	quad = Mesh::GenerateQuad();

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);
	hellData2 = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode2 = new MD5Node(*hellData2);

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	hellData2->AddAnim(MESHDIR"idle2.md5anim");
	hellNode2->PlayAnim(MESHDIR"idle2.md5anim");

	particleShader = new Shader(SHADERDIR"vertex.glsl", SHADERDIR"fragment.glsl", SHADERDIR"geometry.glsl");

	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl",
		SHADERDIR"shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl ", SHADERDIR"skyboxFragment.glsl");

	if (!sceneShader->LinkProgram() || !shadowShader->LinkProgram() || !skyboxShader->LinkProgram() || !particleShader->LinkProgram()) {
		return;
	}

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"crate.PNG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"crate.PNG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"alien_14.PNG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	

	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR"alienBUMP.PNG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

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
	floor->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"alienBUMP.JPG"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	glEnable(GL_DEPTH_TEST);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);


	

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"nuke_ft.tga", TEXTUREDIR"nuke_bk.tga", TEXTUREDIR"nuke_up.tga", TEXTUREDIR"nuke_dn.tga",  TEXTUREDIR"nuke_rt.tga", TEXTUREDIR"nuke_lf.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 1);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	SetTextureRepeating(floor->GetTexture(), true);
	init = true;
}Renderer ::~Renderer(void) {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	delete camera;
	delete light;
	delete hellData;
	delete hellNode;
	delete hellData2;
	delete hellNode2;
	delete floor;

	delete emitter;

	delete quad;
	delete sceneShader;
	delete shadowShader;
	currentShader = NULL;

	delete heightMap;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	//light->SetPosition(camera->GetPosition());
	emitter->Update(msec);
	hellNode->Update(msec);
	hellNode2->Update(msec);


}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//DrawHeightMap();
	
	//light->SetPosition(camera->GetPosition());

	DrawSkybox();

	DrawShadowScene(); // First render pass ...
	
	DrawCombinedScene(); // Second render pass ...

	DrawParticle();


	SwapBuffers();
}void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);	viewMatrix = Matrix4::BuildViewMatrix(
		light->GetPosition(), Vector3(0, 0, 0));
	
	textureMatrix = biasMatrix *(projMatrix * viewMatrix);

	UpdateShaderMatrices();

	DrawFloor();

	DrawMesh();
	DrawMesh2();
	DrawMesh3();
	DrawMesh4();
	//DrawHeightMap();
	DrawHeightMap();
	


	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);}void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	floor->Draw();
	
	DrawMesh();
	DrawMesh2();
	DrawMesh3();
	DrawMesh4();
	
	DrawHeightMap();
	

	glUseProgram(0);
}

void Renderer::DrawHeightMap()
{
	//SetCurrentShader(sceneShader);
	//SetShaderLight(*light);
	//Matrix4 tempMatrix = textureMatrix * modelMatrix;
	/*glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());*/

	

	modelMatrix = Matrix4::Translation(Vector3(-2000, -450, -3000));
	


	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

	heightMap->Draw();




}void Renderer::DrawMesh() {
	
	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3(0, -68, 0));


	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

	hellNode->Draw(*this);

}

void Renderer::DrawFloor() {


	modelMatrix = Matrix4::Rotation(90, Vector3(0, 0, 0)) *
		Matrix4::Scale(Vector3(1800, 1800, 1));
	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

	floor->Draw();

}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);

}
void Renderer::DrawMesh2() {
	modelMatrix = Matrix4::Translation(Vector3(0, -70, 100));


	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

	hellNode2->Draw(*this);

}

void Renderer::DrawMesh3() {
	modelMatrix = Matrix4::Translation(Vector3(65, -60, 250));


	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

	hellNode2->Draw(*this);

}

void Renderer::DrawMesh4() {
	modelMatrix = Matrix4::Translation(Vector3(-60, -20, -300));


	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

	hellNode->Draw(*this);

}


void Renderer::DrawBox() {
	

}

void Renderer::DrawParticle()
{
	glDepthMask(GL_FALSE);
	SetCurrentShader(particleShader);

	modelMatrix = Matrix4::Translation(Vector3(-1935.22876f, 1000.0f, -2955.21582f));
	


	emitter->SetDirection(Vector3(0, -30, 0));
	emitter->SetParticleSize(28.0f);
	emitter->SetParticleVariance(10.0f);
	emitter->SetLaunchParticles(1.0f);
	emitter->SetParticleLifetime(5000.0f);
	emitter->SetParticleSpeed(0.5f);
	emitter->SetParticleRate(0.75f);
	emitter->SetPosition(Vector3(10000.0f, 1000.0f, 10000.0f));
	


	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), emitter->GetParticleSize());

	UpdateShaderMatrices();
	emitter->Draw();
	glUseProgram(0);
	glDepthMask(GL_TRUE);

}

