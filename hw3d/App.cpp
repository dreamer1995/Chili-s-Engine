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
	wnd(1280, 720, "The Donkey Fart Box"),
	pointlight(wnd.Gfx()),
	directionallight(wnd.Gfx()),
	cam(wnd.Gfx()),
	plane(wnd.Gfx(), 3.0f),
	cube(wnd.Gfx(), 4.0f),
	skyBox(wnd.Gfx(), 10.0f)
{
	plane.SetPos({ -5.0f,17.0f,-1.0f });
	cube.SetPos(pointlight.GetPos());
	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,9.0f / 16.0f,0.5f,40.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	pointlight.Bind( wnd.Gfx() );
	directionallight.Bind(wnd.Gfx());
	cam.Bind(wnd.Gfx());
		
	nano.Draw( wnd.Gfx() );
	//nano2.Draw( wnd.Gfx() );
	pointlight.Draw( wnd.Gfx() );
	directionallight.Draw(wnd.Gfx());
	cube.Draw(wnd.Gfx());

	static bool isWireframe = false;
	if (isWireframe)
	{
		wnd.Gfx().SetRasterState('D');
	}
	else
	{
		wnd.Gfx().SetRasterState('N');
	}
	plane.Draw( wnd.Gfx() );
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

	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cam.Translate({ 0.0f,0.0f,dt * cameraSpeed });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -dt * cameraSpeed,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.0f,0.0f,-dt * cameraSpeed });
		}
		if (wnd.kbd.KeyIsPressed('D'))
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

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{

		if (!wnd.CursorEnabled() && wnd.mouse.RightIsPressed())
		{
			cam.Rotate((float)delta->x, (float)delta->y);
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
			if (cam.yaw <= 0)
			{
				directionallight.Rotate((float)delta->x, (float)delta->y);
			}
			else
			{
				directionallight.Rotate((float)delta->x, -(float)delta->y);
			}

			
		}
	}
		
	// imgui windows
	cam.SpawnControlWindow();
	pointlight.SpawnControlWindow();
	directionallight.SpawnControlWindow(wnd.Gfx());
	ShowImguiDemoWindow();
	nano.ShowWindow( "Model 1" );
	//nano2.ShowWindow( "Model 2" );
	plane.SpawnControlWindow( wnd.Gfx() );
	cube.SpawnControlWindow(wnd.Gfx());
	skyBox.SpawnControlWindow(wnd.Gfx());

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