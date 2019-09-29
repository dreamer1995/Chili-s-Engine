#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <cmath>
#include <DirectXMath.h>
#include "GraphicsThrowMacros.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")


Graphics::Graphics( HWND hWnd,int width,int height )
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checking results of d3d functions
	HRESULT hr;

	// create device and front/back buffers, and swap chain and rendering context
	GFX_THROW_INFO( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	) );

	// gain access to texture subresource in swap chain (back buffer)
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO( pSwap->GetBuffer( 0,__uuidof(ID3D11Resource),&pBackBuffer ) );
	GFX_THROW_INFO( pDevice->CreateRenderTargetView( pBackBuffer.Get(),nullptr,&pTarget ) );

	///////////////////////// Map's Texture

	// Setup the texture description.
	// We will have our map be a square
	// We will need to have this texture bound as a render target AND a shader resource
	preCubemapHeight = (float)height;
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = height;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;


	// Create the texture
	GFX_THROW_INFO(pDevice->CreateTexture2D(&texDesc, NULL, &pPreCubeMapH));
	texDesc.Width = 64u;
	texDesc.Height = 64u;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&texDesc, NULL, &pPreCubeMap));
	texDesc.Width = 128u;
	texDesc.Height = 128u;
	texDesc.MipLevels = 5;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&texDesc, NULL, &pPreCubeMapM));


	/////////////////////// Map's Render Target
    // Setup the description of the render target view.
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 1;

    // Create the render target view.
	
	for (short int i = 0; i < 6; ++i)
	{
		// Create a render target view to the ith element.
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		GFX_THROW_INFO(pDevice->CreateRenderTargetView(pPreCubeMapH.Get(), &rtvDesc, &pPreMapTarget[i]));
	}

	/////////////////////// Map's Shader Resource View
    // Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;

    // Create the shader resource view.
	GFX_THROW_INFO(pDevice->CreateShaderResourceView(pPreCubeMapH.Get(), &srvDesc, &pPreMapShaderResourceViewH));
	GFX_THROW_INFO(pDevice->CreateShaderResourceView(pPreCubeMap.Get(), &srvDesc, &pPreMapShaderResourceView));

	srvDesc.TextureCube.MipLevels = 5;
	GFX_THROW_INFO(pDevice->CreateShaderResourceView(pPreCubeMapM.Get(), &srvDesc, &pPreMapShaderResourceViewM));
	
	//Create another RTV for LUT
	texDesc.Width = height;
	texDesc.Height = height;
	texDesc.MipLevels = 0;
	texDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	texDesc.MiscFlags = 0;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&texDesc, NULL, &pPreCubeMapLUT));

	D3D11_RENDER_TARGET_VIEW_DESC brdfLUTRTVDesc = {};
	brdfLUTRTVDesc.Format = texDesc.Format;
	brdfLUTRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pPreCubeMapLUT.Get(), &brdfLUTRTVDesc, &pPreLUTTarget));

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.TextureCube.MipLevels = 1;
	GFX_THROW_INFO(pDevice->CreateShaderResourceView(pPreCubeMapLUT.Get(), &srvDesc, &pPreMapShaderResourceViewLUT));

	// create depth stensil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	GFX_THROW_INFO( pDevice->CreateDepthStencilState( &dsDesc,&pDSStateDefault) );

	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsDesc, &pDSStateCube));

	// bind depth state
	pContext->OMSetDepthStencilState(pDSStateCube.Get(),1u );

	// create depth stensil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO( pDevice->CreateTexture2D( &descDepth,nullptr,&pDepthStencil ) );

	descDepth.Width = height;
	descDepth.Height = height;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	wrl::ComPtr<ID3D11Texture2D> pPreDepthStencil;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pPreDepthStencil));

	// create view of depth stensil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO( pDevice->CreateDepthStencilView(
		pDepthStencil.Get(),&descDSV,&pDSV
	) );

	descDSV.Flags = 0;
	GFX_THROW_INFO(pDevice->CreateDepthStencilView(
		pPreDepthStencil.Get(), &descDSV, &pPreDSV
	));

	// bind depth stensil view to OM
	pContext->OMSetRenderTargets( 1u,pTarget.GetAddressOf(),pDSV.Get() );
	   
	// configure viewport
	pDefaultVP.Width = (float)width;
	pDefaultVP.Height = (float)height;
	pDefaultVP.MinDepth = 0.0f;
	pDefaultVP.MaxDepth = 1.0f;
	pDefaultVP.TopLeftX = 0.0f;
	pDefaultVP.TopLeftY = 0.0f;

	pPreMapVP.Width = (float)height;
	pPreMapVP.Height = (float)height;
	pPreMapVP.MinDepth = 0.0f;
	pPreMapVP.MaxDepth = 1.0f;
	pPreMapVP.TopLeftX = 0.0f;
	pPreMapVP.TopLeftY = 0.0f;

	pContext->RSSetViewports(1u, &pPreMapVP);
	
	//Create the Rasterize State
	D3D11_RASTERIZER_DESC rasterDesc{};
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	GFX_THROW_INFO(pDevice->CreateRasterizerState(&rasterDesc, &pRasterStateDefault));

	rasterDesc.CullMode = D3D11_CULL_NONE;
	GFX_THROW_INFO(pDevice->CreateRasterizerState(&rasterDesc, &pRasterStateNoneSolid));

	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	GFX_THROW_INFO(pDevice->CreateRasterizerState(&rasterDesc, &pRasterStateBackWireframe));

	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	GFX_THROW_INFO(pDevice->CreateRasterizerState(&rasterDesc, &pRasterStateNoneWireframe));

	//Set the Rasterizer
	pContext->RSSetState(pRasterStateNoneSolid.Get());

	// init imgui d3d impl
	ImGui_ImplDX11_Init( pDevice.Get(),pContext.Get() );
}

Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

void Graphics::EndFrame()
{
	// imgui frame end
	if( imguiEnabled )
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
	}

	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if( FAILED( hr = pSwap->Present( 1u,0u ) ) )
	{
		if( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw GFX_DEVICE_REMOVED_EXCEPT( pDevice->GetDeviceRemovedReason() );
		}
		else
		{
			throw GFX_EXCEPT( hr );
		}
	}
}

void Graphics::BeginFrame( float red,float green,float blue ) noexcept
{
	// imgui begin frame
	if( imguiEnabled )
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView( pTarget.Get(),color );
	pContext->ClearDepthStencilView( pDSV.Get(),D3D11_CLEAR_DEPTH,1.0f,0u );
}

void Graphics::DrawIndexed( UINT count ) noxnd
{
	GFX_THROW_INFO_ONLY( pContext->DrawIndexed( count,0u,0u ) );
}

void Graphics::SetProjection( DirectX::FXMMATRIX proj ) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::SetCamera( DirectX::FXMMATRIX cam ) noexcept
{
	camera = cam;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}

void Graphics::EnableImgui() noexcept
{
	imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
	imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const noexcept
{
	return imguiEnabled;
}

void Graphics::SetStencilState() noexcept
{
	pContext->OMSetDepthStencilState(pDSStateDefault.Get(), 1u);
}

void Graphics::SetStencilState(char type) noexcept
{
	if (type == 'C')
	{
		pContext->OMSetDepthStencilState(pDSStateCube.Get(), 1u);
	}
}

void Graphics::SetRasterState() noexcept
{
	pContext->RSSetState(pRasterStateDefault.Get());
}

void Graphics::SetRasterState(char type) noexcept
{
	if (type == 'N')
	{
		pContext->RSSetState(pRasterStateNoneSolid.Get());
	}
	if (type == 'W')
	{
		pContext->RSSetState(pRasterStateBackWireframe.Get());
	}
	if (type == 'D')
	{
		pContext->RSSetState(pRasterStateNoneWireframe.Get());
	}
}

void Graphics::SetRenderTarget() noexcept
{
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());
}

void Graphics::SetPreRenderTarget(short int i) noexcept
{
	const float color[] = { 0.0f,0.0f,0.0f,1.0f };
	pContext->ClearRenderTargetView(pPreMapTarget[i].Get(), color);
	pContext->ClearDepthStencilView(pPreDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	pContext->OMSetRenderTargets(1u, pPreMapTarget[i].GetAddressOf(), pPreDSV.Get());
}

void Graphics::SetViewPort() noexcept
{
	pContext->RSSetViewports(1u, &pDefaultVP);
}

void Graphics::SetViewPort(char type) noexcept
{
	if (type == 'P')
	{
		pContext->RSSetViewports(1u, &pPreMapVP);
	}
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Graphics::GetShaderResourceView(char type) const noexcept
{
	if (type == 'H')
	{
		return pPreMapShaderResourceViewH;
	}
	else if (type == 'M')
	{
		return pPreMapShaderResourceViewM;
	}
	else if (type == 'L')
	{
		return pPreMapShaderResourceViewLUT;
	}
	else
	{
		return pPreMapShaderResourceView;
	}
}

void Graphics::SaveHDCubemapSRV()
{
	HRESULT hr;

	pPreMapVP.Width = 64.0f;
	pPreMapVP.Height = 64.0f;
	pContext->RSSetViewports(1u, &pPreMapVP);
	for (short int i = 0; i < 6; ++i)
	{
		// Create a render target view to the ith element.
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		GFX_THROW_INFO(pDevice->CreateRenderTargetView(pPreCubeMap.Get(), &rtvDesc, &pPreMapTarget[i]));
	}
}

void Graphics::SetCubemapSRVMip(short int i)
{
	HRESULT hr;

	float mipHeight = 128.0f * (float)pow(0.5, i);
	pPreMapVP.Width = mipHeight;
	pPreMapVP.Height = mipHeight;
	pContext->RSSetViewports(1u, &pPreMapVP);
	rtvDesc.Texture2DArray.MipSlice = i;
	for (short int k = 0; k < 6; ++k)
	{
		// Create a render target view to the ith element.
		rtvDesc.Texture2DArray.FirstArraySlice = k;
		GFX_THROW_INFO(pDevice->CreateRenderTargetView(pPreCubeMapM.Get(), &rtvDesc, &pPreMapTarget[k]));
	}
}

void Graphics::SetLUTRT() noexcept
{
	pPreMapVP.Width = preCubemapHeight;
	pPreMapVP.Height = preCubemapHeight;
	pContext->RSSetViewports(1u, &pPreMapVP);

	const float color[] = { 0.0f,0.0f,0.0f,1.0f };
	pContext->ClearRenderTargetView(pPreLUTTarget.Get(), color);
	pContext->ClearDepthStencilView(pPreDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);

	pContext->OMSetRenderTargets(1u, pPreLUTTarget.GetAddressOf(), pPreDSV.Get());
}

// Graphics exception stuff
Graphics::HrException::HrException( int line,const char * file,HRESULT hr,std::vector<std::string> infoMsgs ) noexcept
	:
	Exception( line,file ),
	hr( hr )
{
	// join all info messages with newlines into single string
	for( const auto& m : infoMsgs )
	{
		info += m;
		info.push_back( '\n' );
	}
	// remove final newline if exists
	if( !info.empty() )
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if( !info.empty() )
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString( hr );
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription( hr,buf,sizeof( buf ) );
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
Graphics::InfoException::InfoException( int line,const char * file,std::vector<std::string> infoMsgs ) noexcept
	:
	Exception( line,file )
{
	// join all info messages with newlines into single string
	for( const auto& m : infoMsgs )
	{
		info += m;
		info.push_back( '\n' );
	}
	// remove final newline if exists
	if( !info.empty() )
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
