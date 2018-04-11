#pragma comment(lib, "nclgl.lib")

#include "../../nclgl/window.h"
#include "Renderer.h"

int main() {

	int selection = 1;

	bool selection1;
	bool selection2;
	bool selection3;


	Window w("Daniel Crinnion - Coursework", 1920,1080,true);
	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			
			selection++;


		}
		
		
		if (selection == 1) {


		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();

		}

	}



	return 0;
}
