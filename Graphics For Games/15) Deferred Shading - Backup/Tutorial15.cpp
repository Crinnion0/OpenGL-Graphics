#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "../../NCLGL/SceneNode.h"
#include "Renderer.h"


int main() {


	SceneNode* root;
	root = new SceneNode();
	root->AddChild(new ());


	Window w("Deferred Rendering!", 1600, 900, false); //This is all boring win32 window creation stuff!
	if (!w.HasInitialised()) {
		return -1;
	}

	srand((unsigned int)w.GetTimer()->GetMS() * 1000.0f);

	Renderer renderer(w);
	//This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if (!renderer.HasInitialised()) {
		return -1;
	}
	
	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {

		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();


		
	}

	return 0;
}