#pragma once

#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "../../NCLGL/SceneNode.h"
#include "Renderer.h"
#include "Scene1.h"


int main() {

	int sceneSelection = 1;



	Window w("Coursework - Daniel Crinnion!", 1600, 900, false); //This is all boring win32 window creation stuff!
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

		

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {

			Scene1 renderer2(w);
			renderer.~Renderer();

			sceneSelection = 1;
			

			renderer2.UpdateScene(w.GetTimer()->GetTimedMS());
			renderer2.RenderScene();
		}


		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();



	}

	return 0;
}