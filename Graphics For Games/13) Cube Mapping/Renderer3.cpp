#include "Renderer3.h"

//This renderer utilises defered rendering to create the lighting in the scene.

Renderer3::Renderer3(Parent * parent) {

	rend = parent;

	rotation = 0.0f;
	camera = new Camera(0.0f, 0.0f,
		Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 750, RAW_WIDTH * HEIGHTMAP_X));



	quad = Mesh::GenerateQuad();



	//MESH MODEL

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);
	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	textShader = new Shader(SHADERDIR"TexturedVertexFont.glsl", SHADERDIR"TexturedFragment.glsl");
	pointLights = new Light[(LIGHTNUM * LIGHTNUM)];

	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light & l = pointLights[(x * LIGHTNUM) + z];

			float xPos = (RAW_WIDTH * HEIGHTMAP_X / (LIGHTNUM - 1)) * x;
			float zPos = (RAW_HEIGHT * HEIGHTMAP_Z / (LIGHTNUM - 1)) * z;
			l.SetPosition(Vector3(xPos, 200.0f, zPos));

			float r = 0.5f + (float)(rand() % 129) / 128.0f;
			float g = 0.5f + (float)(rand() % 129) / 128.0f;
			float b = 0.5f + (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 0.5f));

			float radius = ((RAW_WIDTH * HEIGHTMAP_X / LIGHTNUM) * 10);
			l.SetRadius(radius);

		}

	}

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"ALIENFLOOR.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->setTexture2(SOIL_load_OGL_texture(
		TEXTUREDIR"alien_14.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));


	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR"alienBUMP.PNG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));




	rend->SetTextureRepeating(heightMap->GetTexture(), true);
	rend->SetTextureRepeating(heightMap->GetBumpMap(), true);
	rend->SetTextureRepeating(heightMap->getTexture2(), true);


	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR"ico.obj")) {
		return;

	}

	sceneShader = new Shader(SHADERDIR"BumpVertex.glsl",
		SHADERDIR"bufferFragment.glsl");
	if (!sceneShader->LinkProgram()) {
		return;

	}

	//SHADOWS
	if (!shadowShader->LinkProgram()) {

		return;
	}
	if (!textShader->LinkProgram()) {

		return;
	}


	combineShader = new Shader(SHADERDIR"combinevert.glsl", SHADERDIR"combinefrag.glsl");
	if (!combineShader->LinkProgram()) {
		return;

	}

	pointlightShader = new Shader(SHADERDIR"pointlightvertex.glsl",
		SHADERDIR"pointlightfragment.glsl");
	if (!pointlightShader->LinkProgram()) {
		return;

	}

	//PARTICLE EMITTER 
	emitter = new ParticleEmitter();



	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	// Generate our scene depth texture ...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;

	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	rend->init = true;

}


Renderer3 ::~Renderer3(void) {
	delete sceneShader;
	delete shadowShader;
	delete combineShader;
	delete pointlightShader;
	delete textShader;

	delete hellData;
	delete hellNode;

	delete basicFont;

	delete emitter;
	delete heightMap;
	delete camera;
	delete sphere;
	delete quad;
	delete[] pointLights;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	rend->currentShader = NULL;

}

void Renderer3::GenerateScreenTexture(GLuint & into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0,
		depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8,
		rend->width, rend->height, 0,
		depth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

}
void Renderer3::UpdateScene(float msec) {

	emitter->Update(msec);
	if (rend->paused == false) {


		camera->SetPosition(camera->GetPosition() + Vector3(0.02 * msec, 0.01*msec, -0.04*msec));
	}
	camera->UpdateCamera(msec);
	hellNode->Update(msec);
	rend->viewMatrix = camera->BuildViewMatrix();

	rotation = msec * 0.0f;
	framerate = (1000 / msec);
}

void Renderer3::RenderScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	rend->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)rend->width / (float)rend->height, 45.0f);

	FillBuffers();

	DrawPointLights();
	CombineBuffers();


	createText();


	glUseProgram(0);

	rend->SwapBuffers();

}

void Renderer3::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	rend->SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"diffuseTex2"), 2);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"bumpTex"), 1);









	rend->projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)rend->width / (float)rend->height, 45.0f);
	rend->modelMatrix.ToIdentity();
	rend->UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer3::DrawPointLights() {
	rend->SetCurrentShader(pointlightShader);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"depthTex"), 3);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"normTex"), 4);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(rend->currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform2f(glGetUniformLocation(rend->currentShader->GetProgram(),
		"pixelSize"), 1.0f / rend->width, 1.0f / rend->height);	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500,
		(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));

	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);

	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light & l = pointLights[(x * LIGHTNUM) + z];
			float radius = l.GetRadius();

			rend->modelMatrix =
				pushMatrix *
				Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
				popMatrix *
				Matrix4::Translation(l.GetPosition()) *
				Matrix4::Scale(Vector3(radius, radius, radius));

			l.SetPosition(rend->modelMatrix.GetPositionVector());

			rend->SetShaderLight(l);

			rend->UpdateShaderMatrices();

			float dist = (l.GetPosition() - camera->GetPosition()).Length();
			if (dist < radius) {
				glCullFace(GL_FRONT);
			}
			else {
				glCullFace(GL_BACK);
			}

			sphere->Draw();
		}
	}
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	rend->modelMatrix.ToIdentity();

}

void Renderer3::CombineBuffers() {
	rend->SetCurrentShader(combineShader);

	rend->projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	rend->UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"emissiveTex"), 3);
	glUniform1i(glGetUniformLocation(rend->currentShader->GetProgram(),
		"specularTex"), 4);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	quad->Draw();

	glUseProgram(0);
}void Renderer3::DrawMesh() {
	rend->modelMatrix.ToIdentity();

	Matrix4 tempMatrix = rend->textureMatrix * rend->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(rend->currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& rend->modelMatrix.values);

	hellNode->Draw(*rend);
	rend->modelMatrix.ToIdentity();

}

void Renderer3::DrawParticle()
{
	glDepthMask(GL_FALSE);
	rend->SetCurrentShader(particleShader);

	rend->modelMatrix = Matrix4::Translation(Vector3(5000.0f, 1000.0f, 4000.0f));


	emitter->SetParticleSize(48.0f);
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


void Renderer3::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {

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

void Renderer3::createText()
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

void Renderer3::initialise() {
	camera = new Camera(0.979999781f, 330.319855f,
		Vector3(4112.0f, 750, 8224.0f));

	rend->textureMatrix.ToIdentity();
	rend->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)rend->width / (float)rend->height, 45.0f);
	rend->modelMatrix.ToIdentity();
	rend->viewMatrix = camera->BuildViewMatrix();
	rend->UpdateShaderMatrices();
}