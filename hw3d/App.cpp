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
	wnd( 1280,720,"The Donkey Fart Box" ),
	light( wnd.Gfx() )
{
	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,9.0f / 16.0f,0.5f,40.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	light.Bind( wnd.Gfx(),cam.GetMatrix() );
		
	nano.Draw( wnd.Gfx() );
	//nano2.Draw( wnd.Gfx() );
	light.Draw( wnd.Gfx() );

	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
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
				if (wnd.kbd.KeyIsPressed(VK_MENU))
				{
					wnd.EnableCursor();
					wnd.mouse.DisableRaw();
				}
				break;
			}
			case Mouse::Event::Type::LPress:
			{
				if (wnd.kbd.KeyIsPressed(VK_MENU))
				{
					wnd.DisableCursor();
					wnd.mouse.EnableRaw();
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

	if( !wnd.CursorEnabled() )
	{
		if( wnd.kbd.KeyIsPressed( 'W' ) )
		{
			cam.Translate( { 0.0f,0.0f,dt * cameraSpeed } );
		}
		if( wnd.kbd.KeyIsPressed( 'A' ) )
		{
			cam.Translate( { -dt * cameraSpeed,0.0f,0.0f } );
		}
		if( wnd.kbd.KeyIsPressed( 'S' ) )
		{
			cam.Translate( { 0.0f,0.0f,-dt * cameraSpeed } );
		}
		if( wnd.kbd.KeyIsPressed( 'D' ) )
		{
			cam.Translate( { dt * cameraSpeed,0.0f,0.0f } );
		}
		if(wnd.kbd.KeyIsPressed( 'E' ))
		{
			cam.Translate( { 0.0f,dt * cameraSpeed,0.0f } );
		}
		if(wnd.kbd.KeyIsPressed( 'Q' ))
		{
			cam.Translate( { 0.0f,-dt * cameraSpeed,0.0f } );
		}
	}

	if (wnd.kbd.KeyIsPressed('F'))
	{
		cam.LookZero();
	}
	
	while (const auto delta = wnd.mouse.ReadRawDelta())
	{

		if (!wnd.CursorEnabled() && wnd.mouse.RightIsPressed())
		{
			cam.Rotate((float)delta->x, (float)delta->y);
		}
		else if (!wnd.CursorEnabled() && wnd.kbd.KeyIsPressed(VK_MENU) && wnd.mouse.LeftIsPressed())
		{
			cam.RotateAround((float)delta->x, (float)delta->y);
		}
		else if (!wnd.CursorEnabled() && wnd.mouse.WheelIsPressed())
		{
			cam.Translate({ -(float)delta->x * dt,(float)delta->y * dt,0.0f });
		}
	}
		
	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	nano.ShowWindow( "Model 1" );
	//nano2.ShowWindow( "Model 2" );

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