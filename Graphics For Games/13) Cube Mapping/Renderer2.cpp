#include "Renderer2.h"


Renderer2::Renderer2(Parent * parent) {

	rend = parent;

	camera = new Camera(4.06999874f, 205.090378f, Vector3(-169.784439f, 48.9667969f, -243.906021f));


	light = new Light(Vector3(-2068.57349f, 1343.00977f, -1854.89990f),
		Vector4(1, 1, 1, 1), 5500);

	heightMap = new HeightMap(TEXTUREDIR"NEWHEIGHTMAP.raw");
	emitter = new ParticleEmitter();
	quad = Mesh::GenerateQuad();

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);


	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");


	textShader = new Shader(SHADERDIR"TexturedVertexFont.glsl", SHADERDIR"TexturedFragment.glsl");
	particleShader = new Shader(SHADERDIR"vertex.glsl", SHADERDIR"fragment.glsl", SHADERDIR"geometry.glsl");

	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl",
		SHADERDIR"shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl ", SHADERDIR"skyboxFragment.glsl");

	if (!sceneShader->LinkProgram() || !shadowShader->LinkProgram() || !skyboxShader->LinkProgram() || !particleShader->LinkProgram() || !textShader->LinkProgram()) {
		return;
	}

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"crate.PNG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"crate.PNG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"alien_14.PNG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

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

	rend->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)rend->width / (float)rend->height, 45.0f);






	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	rend->SetTextureRepeating(heightMap->GetTexture(), true);
	rend->SetTextureRepeating(heightMap->GetBumpMap(), true);
	rend->SetTextureRepeating(floor->GetTexture(), true);
	rend->init = true;
}Renderer2 ::~Renderer2(void) {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
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
	delete basicFont;
}

void Renderer2::UpdateScene(float msec) {


	if (rend->paused == false) {


		camera->SetPosition(camera->GetPosition() + Vector3(0.02 * msec, 0.01*msec, -0.04*msec));
	}
	camera->UpdateCamera(msec);

	rend->viewMatrix = camera->BuildViewMatrix();
	emitter->Update(msec);
	hellNode->Update(msec);
	framerate = (1000 / msec);


}

void Renderer2::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	rend->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)rend->width / (float)rend->height, 45.0f);


	DrawSkybox();

	DrawShadowScene(); // First pass rendering shadows 

	DrawCombinedScene(); // Second pass combining 

	DrawParticle();



	rend->SwapBuffers();
}void Renderer2::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	rend->SetCurrentShader(shadowShader);	rend->viewMatrix = Matrix4::BuildViewMatrix(
		light->GetPosition(), Vector3(0, 0, 0));

	rend->textureMatrix = biasMatrix *(rend->projMatrix * rend->viewMatrix);

	rend->UpdateShaderMatrices();

	DrawFloor();

	DrawMesh();


	DrawHeightMap();



	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, rend->width, rend->height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);}void Renderer2::DrawCombinedScene() {
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




	rend->modelMatrix = Matrix4::Translation(Vector3(-2000, -450, -3000));



	Matrix4 tempMatrix = rend->textureMatrix * rend->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& rend->modelMatrix.values);

	heightMap->Draw();




}void Renderer2::DrawMesh() {

	rend->modelMatrix.ToIdentity();
	rend->modelMatrix = Matrix4::Translation(Vector3(0, -68, 0));


	Matrix4 tempMatrix = rend->textureMatrix * rend->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& rend->modelMatrix.values);

	hellNode->Draw(*rend);

}

void Renderer2::DrawFloor() {


	rend->modelMatrix = Matrix4::Rotation(90, Vector3(0, 0, 0)) *
		Matrix4::Scale(Vector3(1800, 1800, 1));
	Matrix4 tempMatrix = rend->textureMatrix * rend->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& rend->modelMatrix.values);

	floor->Draw();

}

void Renderer2::DrawSkybox() {




	glDisable(GL_CULL_FACE);


	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);
	glGetUniformLocation(skyboxShader->GetProgram(), "cubeMap2"), 0;

	glDepthMask(GL_FALSE);
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

	rend->modelMatrix = Matrix4::Translation(Vector3(-1935.22876f, 1000.0f, -2955.21582f));



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

}

void Renderer2::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {

	TextMesh* mesh = new TextMesh(text, *basicFont);


	if (perspective) {
		rend->modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		rend->viewMatrix = camera->BuildViewMatrix();
		rend->projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)rend->width / (float)rend->height, 45.0f);
	}
	else {

		rend->modelMatrix = Matrix4::Translation(Vector3(position.x, rend->height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		rend->viewMatrix.ToIdentity();
		rend->projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)rend->width, 0.0f, (float)rend->height, 0.0f);
	}

	rend->UpdateShaderMatrices();
	mesh->Draw();

	delete mesh;
}

void Renderer2::createText()
{

	rend->modelMatrix.ToIdentity();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);


	rend->SetCurrentShader(textShader);
	glUniform1i(glGetUniformLocation(textShader->GetProgram(), "diffuseTex"), 0);


	DrawText(to_string(framerate), Vector3(0, 0, 1), 16.0f);


	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	rend->modelMatrix.ToIdentity();

}
void Renderer2::initialise() {

	camera = new Camera(4.06999874f, 205.090378f, Vector3(-169.784439f, 48.9667969f, -243.906021f));
	cubeMap2 = SOIL_load_OGL_cubemap(TEXTUREDIR"nuke_ft.tga", TEXTUREDIR"nuke_bk.tga", TEXTUREDIR"nuke_up.tga", TEXTUREDIR"nuke_dn.tga", TEXTUREDIR"nuke_rt.tga", TEXTUREDIR"nuke_lf.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);
	rend->textureMatrix.ToIdentity();
	rend->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)rend->width / (float)rend->height, 45.0f);
	rend->modelMatrix.ToIdentity();
	rend->viewMatrix = camera->BuildViewMatrix();
	rend->UpdateShaderMatrices();

}