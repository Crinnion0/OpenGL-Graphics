#include "Renderer.h"


Renderer::Renderer(Window & parent) : OGLRenderer(parent) {

	camera = new Camera(0, 90.0f, Vector3(-180, 60, 0));
	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);

	//GLuint tmpDiffuseTex = SOIL_load_OGL_texture(TEXTUREDIR"hellknight.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	//hellNode->GetMesh()->SetTexture(tmpDiffuseTex);

	if (!currentShader->LinkProgram()) {
		return;
	}

	hellData->AddAnim(MESHDIR"attack2.md5anim");
	hellNode->PlayAnim(MESHDIR"attack2.md5anim");

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUseProgram(0);

	init = true;
}Renderer ::~Renderer(void) {
	delete camera;
	delete hellData;
	delete hellNode;
}void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	hellNode->Update(msec);

}


void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader -> GetProgram());

	/*glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),
		"diffuseTex"), 0);*/

	UpdateShaderMatrices();

	hellNode -> Draw(*this);
	//hellNode->DebugDrawSkeleton();


	glUseProgram(0);
	SwapBuffers();
}