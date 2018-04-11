#pragma once


#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/ParticleEmitter.h"	
#include "../../nclgl/TextMesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/OBJMesh.h"
#include "../../nclgl/HeightMap.h"


//All renderers load from this class. Saves time for loading between scenes. 

class Parent : public OGLRenderer {


	friend class Renderer;
	friend class Renderer2;
	friend class Renderer3;

public:
	Parent(Window & parent);
	virtual ~Parent(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();
	virtual void RenderScene(int flag);
	bool initialised = false;

	void togglePaused() { paused = !paused; }
	void resetMsec() { totalMsec = 0; }
	float totalMsec = 0;


protected:

	Renderer* Scene1;
	Renderer2* Scene2;
	Renderer3* Scene3;
	bool paused = false;




	int tempFlag;

};

