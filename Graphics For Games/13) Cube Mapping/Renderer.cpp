#pragma once

#include "Renderer.h"


//All renderers are similar

Renderer::Renderer(Parent * parent) {

	rend = parent;


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

	if (!reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram() || !particleShader->LinkProgram() || !textShader->LinkProgram()) {
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
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);




	rend->SetTextureRepeating(quad->GetTexture(), true);
	rend->SetTextureRepeating(heightMap->GetTexture(), true);
	rend->SetTextureRepeating(heightMap->GetBumpMap(), true);
	rend->SetTextureRepeating(heightMap->getTexture2(), true);

	rend->init = true;
	waterRotate = 10.0f;

	rend->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)rend->width / (float)rend->height, 45.0f);

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
	rend->currentShader = NULL;
	delete particleShader;	delete textShader;	delete basicFont;	delete emitter;}void Renderer::UpdateScene(float msec) {

	//Pause camera movement functionality 
	if (rend->paused == false) {


		camera->SetPosition(camera->GetPosition() + Vector3(0.02 * msec, 0, -0.02*msec));
	}
	camera->UpdateCamera(msec);


	rend->viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;
	emitter->Update(msec);
	framerate = (1000 / msec);



}

void Renderer::RenderScene() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	rend->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)rend->width / (float)rend->height, 45.0f);



	DrawSkybox();
	DrawHeightmap();

	DrawWater();
	DrawParticle();
	createText();

	glUseProgram(0);
	rend->SwapBuffers();

}

void Renderer::DrawSkybox() {

	glDisable(GL_CULL_FACE);


	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap1);
	glGetUniformLocation(skyboxShader->GetProgram(), "cubeMap1"), 0;

	glDepthMask(GL_FALSE);
	rend->SetCurrentShader(skyboxShader);

	rend->UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);

	glEnable(GL_CULL_FACE);
}

void Renderer::DrawParticle()
{
	glDepthMask(GL_FALSE);
	rend->SetCurrentShader(particleShader);

	rend->modelMatrix = Matrix4::Translation(Vector3(5000.0f, 1000.0f, 4000.0f));


	emitter->SetParticleSize(32.0f);
	emitter->SetParticleVariance(10.0f);
	emitter->SetLaunchParticles(1.0f);
	emitter->SetParticleLifetime(10000.0f);
	emitter->SetParticleSpeed(0.75f);
	emitter->SetParticleRate(1.0f);


	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1f(glGetUniformLocation(rend->currentShader->GetProgram(), "particleSize"), emitter->GetParticleSize());

	rend->UpdateShaderMatrices();
	emitter->Draw();
	glUseProgram(0);
	glDepthMask(GL_TRUE);
	rend->modelMatrix.ToIdentity();

}

void Renderer::DrawHeightmap() {
	rend->SetCurrentShader(lightShader);
	rend->SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(rend->currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"diffuseTex2"), 2);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"bumpTex"), 1);


	rend->modelMatrix.ToIdentity();
	rend->textureMatrix.ToIdentity();

	rend->UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);


}

void Renderer::DrawWater() {
	rend->SetCurrentShader(reflectShader);
	rend->SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(rend->currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap1);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 256 * HEIGHTMAP_Y / 3.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	rend->modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	rend->textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));


	rend->UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);
	rend->modelMatrix.ToIdentity();

	rend->textureMatrix.ToIdentity();
}


void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	//Create a temporary TextMesh,
	TextMesh* mesh = new TextMesh(text, *basicFont);

	// Setup to render in either perspective or
	//orthographic mode
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
	//update the matrices
	rend->UpdateShaderMatrices();
	mesh->Draw();

	delete mesh;
}

void Renderer::createText()
{

	rend->modelMatrix.ToIdentity();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);


	rend->SetCurrentShader(textShader);	//Enable 
												//Turn on texture unit 
	glUniform1i(glGetUniformLocation(textShader->GetProgram(), "diffuseTex"), 0);

	//Render function
	DrawText(to_string(framerate), Vector3(0, 0, 1), 16.0f);


	glUseProgram(0);



	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	rend->modelMatrix.ToIdentity();

}

void Renderer::initialise() {

	camera->SetPosition((4.61996269, 308.899353, Vector3(2143.83594f, 1033.02734f, 5283.81543f)));
	cubeMap1 = SOIL_load_OGL_cubemap(TEXTUREDIR"bkg1_right.png", TEXTUREDIR"bkg1_left.png", TEXTUREDIR"bkg1_top.png", TEXTUREDIR"bkg1_bot.png", TEXTUREDIR"bkg1_front.png", TEXTUREDIR"bkg1_back.png", SOIL_LOAD_RGB, cubeMap1, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap1);
	rend->textureMatrix.ToIdentity();
	rend->projMatrix.ToIdentity();
	rend->modelMatrix.ToIdentity();
	rend->viewMatrix = camera->BuildViewMatrix();
	rend->UpdateShaderMatrices();
}
