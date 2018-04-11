//Stops circular includes
#include "Parent.h"
#include "Renderer.h"
#include "Renderer2.h"
#include "Renderer3.h"


Parent::Parent(Window &parent) : OGLRenderer(parent)
{

	Scene1 = new Renderer(this);
	Scene2 = new Renderer2(this);
	Scene3 = new Renderer3(this);



}


Parent::~Parent()
{
}

void Parent::UpdateScene(float msec)
{

	if (tempFlag == 1) {



		Scene1->UpdateScene(msec);

	}

	else if (tempFlag == 2) {



		Scene2->UpdateScene(msec);

	}
	else if (tempFlag == 3) {



		Scene3->UpdateScene(msec);

	}



}

void Parent::RenderScene()
{

	RenderScene(1);

}
void Parent::RenderScene(int flag)
{

	//Decide which scene to render
	tempFlag = flag;


	if (flag == 1) {

		if (!initialised) {
			Scene1->initialise();
			initialised = true;
			totalMsec = 0;
		}

		Scene1->RenderScene();

	}

	else if (flag == 2) {

		if (!initialised) {
			Scene2->initialise();
			initialised = true;
			totalMsec = 0;
		}



		Scene2->RenderScene();

	}
	else if (flag == 3) {


		if (!initialised) {
			Scene3->initialise();
			initialised = true;
			totalMsec = 0;
		}

		Scene3->RenderScene();

	}



}