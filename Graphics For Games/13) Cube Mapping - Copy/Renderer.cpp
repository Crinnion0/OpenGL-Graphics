#pragma once

#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera(4.61996269, 308.899353, Vector3(2143.83594f, 1033.02734f, 5283.81543f));
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	quad = Mesh::GenerateQuad();

	

	light = new Light(Vector3(5010.90625f, 1135.53125f, 3966.91162f), Vector4(1.9f, 1.9f, 1.9f, 1), 3500.0f);

	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl ", SHADERDIR"skyboxFragment.glsl");
	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"PerPixelFragment.glsl");


	//Particle Shader
	particleShader = new Shader(SHADERDIR"vertex.glsl", SHADERDIR"fragment.glsl", SHADERDIR"geometry.glsl");

	textShader = new Shader(SHADERDIR"TexturedVertexFont.glsl", SHADERDIR"TexturedFragment.glsl");

	if (!reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram() || !particleShader->LinkProgram() || !textShader->LinkProgram() ){
		return;
	}

	
	//Particles
	emitter = new ParticleEmitter();


	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Water 3.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"waterbump.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"LUNAROCK.PNG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->setTexture2(SOIL_load_OGL_texture(
		TEXTUREDIR"GRASS.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR"alienBUMP.PNG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"bkg1_right.png",TEXTUREDIR"bkg1_left.png",TEXTUREDIR"bkg1_top.png",TEXTUREDIR"bkg1_bot.png",TEXTUREDIR"bkg1_front.png",TEXTUREDIR"bkg1_back.png",      SOIL_LOAD_RGB,SOIL_CREATE_NEW_ID, 1);
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	

	//cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"left.png", TEXTUREDIR"right.png", TEXTUREDIR"bot.png", TEXTUREDIR"top.png", TEXTUREDIR"back.png", TEXTUREDIR"front.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 1);

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	SetTextureRepeating(heightMap->getTexture2(), true);

	init = true;
	waterRotate = 10.0f;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;
	currentShader = 0;
	delete basicFont;	delete emitter;}void Renderer::UpdateScene(float msec) {
	
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;
	emitter->Update(msec);
	framerate = (1000 / msec);
}

void Renderer::RenderScene() {
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	
	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawParticle();
	createText();

	glUseProgram(0);
	SwapBuffers();

}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);

}

void Renderer::DrawParticle()
{
	glDepthMask(GL_FALSE);
	SetCurrentShader(particleShader);

	modelMatrix = Matrix4::Translation(Vector3(5000.0f, 1000.0f, 4000.0f));

	
	emitter->SetParticleSize(32.0f);
	emitter->SetParticleVariance(10.0f);
	emitter->SetLaunchParticles(1.0f);
	emitter->SetParticleLifetime(10000.0f);
	emitter->SetParticleSpeed(0.75f);
	emitter->SetParticleRate(1.0f);
	

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"),emitter->GetParticleSize());

	UpdateShaderMatrices();
	emitter->Draw();
	glUseProgram(0);
	glDepthMask(GL_TRUE);

}

void Renderer::DrawHeightmap() {
	SetCurrentShader(lightShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex2"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);

}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader -> GetProgram(),
		"cameraPos"), 1, (float *)& camera -> GetPosition());

	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),
		"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 256 * HEIGHTMAP_Y / 3.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));


	UpdateShaderMatrices();

	quad -> Draw();

	glUseProgram(0);

}


void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
}

void Renderer::createText()
{

	modelMatrix.ToIdentity();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer

	SetCurrentShader(textShader);	//Enable the shader...
												//And turn on texture unit 0
	glUniform1i(glGetUniformLocation(textShader->GetProgram(), "diffuseTex"), 0);

	//Render function to encapsulate our font rendering!
	DrawText(to_string(framerate), Vector3(0, 0, 1), 16.0f);
	//DrawText("This is perspective text!!!!", Vector3(0, 0, -1000), 64.0f, true);

	glUseProgram(0);	//That's everything!

	//SwapBuffers();

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

}
