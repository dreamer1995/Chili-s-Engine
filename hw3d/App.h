#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "TestPlane.h"
#include "TestCube.h"
#include "TestSphere.h"
#include "Mesh.h"
#include <set>
#include "SkyBox.h"
#include "DirectionalLight.h"
#include "PreSkyBox.h"
#include "PrePlane.h"

class App
{
public:
	App();
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
	void ShowImguiDemoWindow();
private:
	bool showDemoWindow = false;
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight pointlight;
	DirectionalLight directionallight;
	//Model wall{ wnd.Gfx(),"Models\\brick_wall\\brick_wall.obj" };
	//Model nano{ wnd.Gfx(),"Models\\nano_textured\\nanosuit.obj" };
	//TestPlane plane;
	//TestCube
	TestSphere cube;
	SkyBox skyBox;
	std::unique_ptr<PreSkyBox> preSkyBox;
	std::unique_ptr<PreSkyBox> preSkyBoxBlur;
	std::unique_ptr<PreSkyBox> preSkyBoxMip;
	PrePlane prePlane;
};