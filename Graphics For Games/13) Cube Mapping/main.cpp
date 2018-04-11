#pragma comment(lib, "nclgl.lib")

#include "../../nclgl/window.h"
#include "Parent.h"





int main() {




	int selection = 1;
	bool paused = false;
	float f = 0;



	Window w("Daniel Crinnion - Coursework", 1600, 900, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Parent parent(w);

	if (!parent.HasInitialised()) {

		return -1;
	}


	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);




	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {



		if ((parent.totalMsec / 1000.0f) >= 25.0f) {

			selection++;
			parent.initialised = false;
		}


		if (selection == 4) {
			selection = 1;
			parent.initialised = false;
		}



		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) {


			parent.togglePaused();

		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {

			selection = 1;
			parent.initialised = false;

		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {

			selection = 2;
			parent.initialised = false;




		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {

			selection = 3;
			parent.initialised = false;
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT)) {

			if (selection <= 3) {

				selection++;
				parent.initialised = false;
			}



			if (selection == 4) {
				selection = 1;
				parent.initialised = false;
			}



		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT)) {

			if (selection >= 1) {

				selection--;
				parent.initialised = false;
			}



			if (selection == 0) {
				selection = 3;
				parent.initialised = false;
			}

		}


		f = w.GetTimer()->GetTimedMS();
		parent.UpdateScene(f);
		parent.RenderScene(selection);
		parent.totalMsec += (f);


	}



	return 0;
}


