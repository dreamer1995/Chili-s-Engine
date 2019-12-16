#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include "DxgiInfoManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>
#include "ConditionalNoexcept.h"
//#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\D3DX11.h"
//#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64\\d3dx11.lib")

namespace Bind
{
	class Bindable;
}

class Graphics
{
	friend Bind::Bindable;
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	};
	class HrException : public Exception
	{
	public:
		HrException( int line,const char* file,HRESULT hr,std::vector<std::string> infoMsgs = {} ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException( int line,const char* file,std::vector<std::string> infoMsgs ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics( HWND hWnd,int width,int height );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	~Graphics();
	void EndFrame();
	void BeginFrame( float red,float green,float blue ) noexcept;
	void DrawIndexed( UINT count ) noxnd;
	void SetProjection( DirectX::FXMMATRIX proj ) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera( DirectX::FXMMATRIX cam ) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;

	void SetStencilState() noexcept;
	void SetStencilState(char type) noexcept;

	void SetRasterState() noexcept;
	void SetRasterState(char type) noexcept;

	void SetAlphaBlendState() noexcept;
	void SetAlphaBlendState(char type) noexcept;

	void CreateMapRenderTarget(char type);
	void SetRenderTarget() noexcept;
	void SetPreRenderTarget(short int i) noexcept;
	void SetMapRenderTarget(char type = 'D') noexcept;

	void SetViewPort() noexcept;
	void SetViewPort(char type) noexcept;
	void SetViewPort(float x, float y) noexcept;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView(char type = 'D') const noexcept;

	void SaveHDCubemapSRV();
	void SetCubemapSRVMip(short int i);
	void SetLUTRT() noexcept;

	void UnbindTessellationShader() noexcept;
	void UnbindShaderResource(UINT slotP, UINT slotD) noexcept;
private:
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	bool imguiEnabled = true;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pPreDSV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pShadowDSV;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSStateDefault;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSStateCube;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterStateDefault;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterStateBackSolid;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterStateNoneSolid;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterStateBackWireframe;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterStateNoneWireframe;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pPreCubeMap;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pPreCubeMapH;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pPreCubeMapM;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pPreMapLUT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pMapCaustics;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pMapCausticsNormal;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pMap3DTarget[6];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pMap2DTarget;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pPreMapShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pPreMapShaderResourceViewH;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pPreMapShaderResourceViewM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pPreMapShaderResourceViewLUT;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pMapShaderResourceViewCaustics;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pMapShaderResourceViewCausticsNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pMapShaderResourceViewShadow;

	float preCubemapHeight;
	D3D11_VIEWPORT pDefaultVP;
	D3D11_VIEWPORT pPreMapVP;

	D3D11_RENDER_TARGET_VIEW_DESC texture3DRTVDesc = {};
	D3D11_RENDER_TARGET_VIEW_DESC texture2DRTVDesc = {};

	Microsoft::WRL::ComPtr<ID3D11BlendState> pAlphaEnable;
	Microsoft::WRL::ComPtr<ID3D11BlendState> pAlphaDisable;

};