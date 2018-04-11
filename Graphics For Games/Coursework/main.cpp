#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "Renderer1.h"

int main() {
	Window w("Deferred Rendering!", 1280, 720, false); //This is all boring win32 window creation stuff!
	if (!w.HasInitialised()) {
		return -1;
	}

	srand((unsigned int)w.GetTimer()->GetMS() * 1000.0f);

	Renderer1 renderer1(w);
	/*Renderer2 renderer2(w);
	Renderer3 renderer3(w);*///This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if (!renderer1.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
			renderer1.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer1.RenderScene();
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {

		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {

		}
		
	}

	return 0;
}