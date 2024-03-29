#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App()
	:
	wnd( 1600,900,"The Donkey Fart Box" ),
	pointlight(wnd.Gfx()),
	directionallight(wnd.Gfx()),
	cam(wnd.Gfx()),
	prePlane(wnd.Gfx(), 1.0f),
	skyBox(wnd.Gfx(), 10.0f)
{
	ground.SetRotation(0.0f, PI * 0.5f, 0.0f);
	//plane = std::make_unique<WaterPlane>(wnd.Gfx(), 5.0f);
	//plane->SetRotation(0.0f, PI * 0.5f, 0.0f);
	//causticPlaneNormal = std::make_unique<CausticPlaneNormal>(wnd.Gfx(), 5.0f);
	//causticPlane = std::make_unique<CausticPlane>(wnd.Gfx(), 1.0f);
	//causticPlane->SetRotation(0.0f, PI * 0.5f, 0.0f);

	//wall.SetRootTransform(dx::XMMatrixTranslation(pointlight.GetPos().x - 3.0f, pointlight.GetPos().y, pointlight.GetPos().z - 2.0f));
	//plane.SetPos({ -5.0f,17.0f,-1.0f });
	//uvPannel = std::make_unique<UVPannel>(wnd.Gfx(), 1, gun.UVPos, gun.indices);
	wnd.Gfx().SetViewPort('P');
	wnd.Gfx().SetStencilState('C');
	wnd.Gfx().SetRasterState('N');
	preSkyBox = std::make_unique<PreSkyBox>(wnd.Gfx(), 10.0f);
	preSkyBoxBlur = std::make_unique<PreSkyBox>(wnd.Gfx(), 10.0f, 'B');
	preSkyBoxMip= std::make_unique<PreSkyBox>(wnd.Gfx(), 10.0f, 'M');

	wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.5f, 400.0f));

	dx::XMMATRIX viewmatrix[6] = 
	{
		dx::XMMatrixLookAtLH({ 0.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }),
		dx::XMMatrixLookAtLH({ 0.0f,0.0f,0.0f }, { -1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }),
		dx::XMMatrixLookAtLH({ 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,-1.0f }),
		dx::XMMatrixLookAtLH({ 0.0f,0.0f,0.0f }, { 0.0f,-1.0f,0.0f }, { 0.0f,0.0f,1.0f }),
		dx::XMMatrixLookAtLH({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,1.0f }, { 0.0f,1.0f,0.0f }),
		dx::XMMatrixLookAtLH({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-1.0f }, { 0.0f,1.0f,0.0f })
	};

	for (short int i = 0; i < 6; i++)
	{
		wnd.Gfx().SetPreRenderTarget(i);
		wnd.Gfx().SetCamera(viewmatrix[i]);
		preSkyBox->Draw(wnd.Gfx());
	}

	wnd.Gfx().SaveHDCubemapSRV();
	for (short int i = 0; i < 6; i++)
	{
		wnd.Gfx().SetPreRenderTarget(i);
		wnd.Gfx().SetCamera(viewmatrix[i]);
		preSkyBoxBlur->Draw(wnd.Gfx());
	}


	for (short int i = 0; i < 5; i++)
	{
		wnd.Gfx().SetCubemapSRVMip(i);
		preSkyBoxMip->pmc.roughness = (float)i / 4.0f;
		preSkyBoxMip->ChangeSphereMaterialState(wnd.Gfx());
		for (short j = 0; j < 6; j++)
		{
			wnd.Gfx().SetPreRenderTarget(j);
			wnd.Gfx().SetCamera(viewmatrix[j]);
			preSkyBoxMip->Draw(wnd.Gfx());
		}
	}

	wnd.Gfx().SetLUTRT();
	prePlane.Draw(wnd.Gfx());

	wnd.Gfx().SetRenderTarget();
	wnd.Gfx().SetRasterState();
	wnd.Gfx().SetStencilState();

	wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 400.0f));

	//wnd.Gfx().SetViewPort('U');
	wnd.Gfx().SetViewPort();
}

void App::DoFrame()
{
	//wnd.Gfx().SetViewPort();
	const auto dt = timer.Mark() * speed_factor;
	//wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().BeginFrame(0.0f, 0.0f, 0.0f);
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	pointlight.Bind( wnd.Gfx() );
	directionallight.Bind(wnd.Gfx());
	cam.Bind(wnd.Gfx());
		
	//wall.Draw( wnd.Gfx() );
	//nano.Draw( wnd.Gfx() );
	//nano2.Draw( wnd.Gfx() );
	pointlight.Draw( wnd.Gfx() );
	directionallight.Draw(wnd.Gfx());

	static bool isWireframe = false;
	if (isWireframe)
	{
		wnd.Gfx().SetRasterState('D');
	}
	else
	{
		wnd.Gfx().SetRasterState('N');
	}

	ground.Draw(wnd.Gfx());

	////Render Water Plane
	//wnd.Gfx().SetViewPort('C');

	//wnd.Gfx().CreateMapRenderTarget('N');
	//wnd.Gfx().UnbindShaderResource(3u, 21u);
	//wnd.Gfx().UnbindShaderResource(6u, 21u);
	//wnd.Gfx().SetMapRenderTarget();

	//causticPlaneNormal->Bind(wnd.Gfx(), dt);
	//causticPlaneNormal->Bind(wnd.Gfx(), plane->pmc.speed, plane->pmc.roughness, plane->pmc.flatten1, plane->pmc.flatten2,
	//	plane->vmc.amplitude, plane->vmc.speed, plane->vmc.wavelength, plane->vmc.omega, plane->vmc.Q,
	//	plane->vmc.directionX, plane->vmc.directionZ,
	//	plane->pmc.normalMappingEnabled);
	//causticPlaneNormal->Draw(wnd.Gfx());

	//wnd.Gfx().CreateMapRenderTarget('D');
	//wnd.Gfx().SetMapRenderTarget();

	//wnd.Gfx().SetAlphaBlendState('A');

	//causticPlane->Bind(wnd.Gfx(), dt);
	//causticPlane->Bind(wnd.Gfx(), plane->pmc.depth,
	//	plane->vmc.amplitude, plane->vmc.speed, plane->vmc.wavelength, plane->vmc.omega, plane->vmc.Q,
	//	plane->vmc.directionX, plane->vmc.directionZ);
	//causticPlane->Draw(wnd.Gfx());

	//wnd.Gfx().UnbindTessellationShader();

	//wnd.Gfx().SetAlphaBlendState();

	//wnd.Gfx().SetRenderTarget();

	//wnd.Gfx().SetViewPort();

	//plane->ChangeSphereMaterialState(wnd.Gfx(), skyBox.pitch, skyBox.yaw, skyBox.roll);
	//plane->Bind(wnd.Gfx(), dt);
	//plane->Draw(wnd.Gfx());

	wnd.Gfx().SetStencilState('C');

	if (skyBox.show)
	{
		skyBox.SetPos(cam.pos);
		skyBox.Draw(wnd.Gfx());
	}

	if (isWireframe)
	{
		wnd.Gfx().SetRasterState('W');
	}
	else
	{
		wnd.Gfx().SetRasterState();
	}

	wnd.Gfx().SetStencilState();

	cube.ChangeSphereMaterialState(wnd.Gfx(), skyBox.pitch, skyBox.yaw, skyBox.roll);
	cube.Draw(wnd.Gfx());

	//wnd.Gfx().SetRasterState('N');
	//wnd.Gfx().SetAlphaBlendState('A');
	//causticPlane->Draw(wnd.Gfx());

	//wnd.Gfx().UnbindTessellationShader();
	//wnd.Gfx().SetAlphaBlendState();
	//wnd.Gfx().SetRasterState();

	//if (uvPannel->showUV)
	//{
	//	wnd.Gfx().SetViewPort('P');
	//	wnd.Gfx().SetRasterState('W');
	//	uvPannel->Draw(wnd.Gfx());
	//	wnd.Gfx().SetViewPort();
	//	wnd.Gfx().SetRasterState();
	//}

	static bool isRotate = true;

	while( const auto e = wnd.kbd.ReadKey() )
	{
		if( !e->IsPress() )
		{
			continue;
		}

		switch( e->GetCode() )
		{
			/*case VK_ESCAPE:
				if( wnd.CursorEnabled() )
				{
					wnd.DisableCursor();
					wnd.mouse.EnableRaw();
				}
				else
				{
					wnd.EnableCursor();
					wnd.mouse.DisableRaw();
				}
				break;*/
			case VK_F1:
				showDemoWindow = true;
				break;
			case VK_F3:
				isWireframe = true;
				break;
			case VK_F4:
				isWireframe = false;
				break;
			//case VK_F5:
			//	if (uvPannel->showUV)
			//	{
			//		uvPannel->showUV = false;
			//	}
			//	else
			//	{
			//		uvPannel->showUV = true;
			//	}
			//	break;
			case VK_SPACE:
				if (isRotate)
				{
					isRotate = false;
				}
				else
				{
					isRotate = true;
				}
			break;
		}
	}

	static float cameraSpeed = 1.0f;

	while (!wnd.mouse.IsEmpty())
	{
		const auto e = wnd.mouse.Read();

		switch (e->GetType())
		{
		case Mouse::Event::Type::RPress:
		{
			wnd.DisableCursor();
			wnd.mouse.EnableRaw();
			break;
		}
		case Mouse::Event::Type::RRelease:
		{
			wnd.EnableCursor();
			wnd.mouse.DisableRaw();
			break;
		}

		case Mouse::Event::Type::LRelease:
		{
			wnd.EnableCursor();
			wnd.mouse.DisableRaw();
			break;
		}
		case Mouse::Event::Type::LPress:
		{
			if (wnd.kbd.KeyIsPressed(VK_MENU))
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
				break;
			}
			
			if (wnd.kbd.KeyIsPressed('L') || wnd.kbd.KeyIsPressed(VK_SHIFT))
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
				break;
			}
			break;
		}
		case Mouse::Event::Type::WheelUp:
		{
			if (wnd.mouse.RightIsPressed())
			{
				cameraSpeed += 0.3;
			}
			else
			{
				cam.Translate({ 0.0f,0.0f,10.0f * dt });
			}
			break;
		}
		case Mouse::Event::Type::WheelDown:
		{
			if (wnd.mouse.RightIsPressed())
			{
				cameraSpeed -= 0.3;
			}
			else
			{
				cam.Translate({ 0.0f,0.0f,10.0f * -dt });
			}
			break;
		}
		case Mouse::Event::Type::WheelPress:
		{
			wnd.DisableCursor();
			wnd.mouse.EnableRaw();
			break;
		}
		case Mouse::Event::Type::WheelRelease:
		{
			wnd.EnableCursor();
			wnd.mouse.DisableRaw();
			break;
		}
		}
		cameraSpeed = std::clamp(cameraSpeed, 0.3f, 9.9f);
	}

	if ( !wnd.CursorEnabled() )
	{
		if ( wnd.kbd.KeyIsPressed('W') )
		{
			cam.Translate({ 0.0f,0.0f,dt * cameraSpeed });
		}
		if ( wnd.kbd.KeyIsPressed('A') )
		{
			cam.Translate({ -dt * cameraSpeed,0.0f,0.0f });
		}
		if ( wnd.kbd.KeyIsPressed('S') )
		{
			cam.Translate({ 0.0f,0.0f,-dt * cameraSpeed });
		}
		if ( wnd.kbd.KeyIsPressed('D') )
		{
			cam.Translate({ dt * cameraSpeed,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('E'))
		{
			cam.Translate({ 0.0f,dt * cameraSpeed,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('Q'))
		{
			cam.Translate({ 0.0f,-dt * cameraSpeed,0.0f });
		}
	}

	if (wnd.kbd.KeyIsPressed('F'))
	{
		cam.LookZero({ pointlight.GetPos().x, pointlight.GetPos().y, pointlight.GetPos().z });
	}

	while( const auto delta = wnd.mouse.ReadRawDelta() )
	{
		if (!wnd.CursorEnabled() && wnd.mouse.RightIsPressed())
		{
			cam.Rotate( (float)delta->x,(float)delta->y );
		}
		else if (!wnd.CursorEnabled() && wnd.kbd.KeyIsPressed(VK_MENU) && wnd.mouse.LeftIsPressed())
		{
			cam.RotateAround((float)delta->x, (float)delta->y, { pointlight.GetPos().x, pointlight.GetPos().y, pointlight.GetPos().z });
		}
		else if (!wnd.CursorEnabled() && wnd.mouse.WheelIsPressed())
		{
			static float mKeyMoveSpeed = 0.1f;
			cam.Translate({ -(float)delta->x * dt * mKeyMoveSpeed,(float)delta->y * dt * mKeyMoveSpeed,0.0f });
		}
		else if (!wnd.CursorEnabled() && (wnd.kbd.KeyIsPressed('L') || wnd.kbd.KeyIsPressed(VK_SHIFT)) && wnd.mouse.LeftIsPressed())
		{
			directionallight.Rotate((float)delta->x, (float)delta->y);
		}
	}
	
	//if (isRotate)
	//{
	//	gun.SetRotation(gun.roll, gun.pitch, gun.yaw + dt);
	//}

	// imgui windows
	cam.SpawnControlWindow();
	pointlight.SpawnControlWindow();
	directionallight.SpawnControlWindow(wnd.Gfx());
	ShowImguiDemoWindow();
	//wall.ShowWindow("Wall");
	//nano.ShowWindow( "Model 1" );
	//nano2.ShowWindow( "Model 2" );
	//plane.SpawnControlWindow( wnd.Gfx() );
	//causticPlane->SpawnControlWindow(wnd.Gfx());
	//plane->SpawnControlWindow(wnd.Gfx());
	skyBox.SpawnControlWindow(wnd.Gfx());
	cube.SpawnControlWindow(wnd.Gfx());
	ground.SpawnControlWindow(wnd.Gfx());

	// present
	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow()
{
	if( showDemoWindow )
	{
		ImGui::ShowDemoWindow( &showDemoWindow );
	}
}

App::~App()
{}


int App::Go()
{
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}