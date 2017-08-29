#pragma once

namespace graphic
{

	// Direct3D11 Initialize
	bool InitDirectX11(
		const HWND hWnd
		, const int width
		, const int height
		, OUT ID3D11Device **pd3dDevice
		, OUT ID3D11DeviceContext **pImmediateContext
		, OUT IDXGISwapChain **pSwapChain
		, OUT ID3D11RenderTargetView **pRenderTargetView
		, OUT ID3D11Texture2D **pDepthStencil
		, OUT ID3D11DepthStencilView **pDepthStencilView 
	);


	// Direct3D11 Deferred Context Initialize
	bool InitDirectX11Deferred(
		ID3D11Device *pd3dDevice
		, const HWND hWnd
		, const int width
		, const int height		
		, OUT ID3D11DeviceContext **pDeferredContext
		, OUT IDXGISwapChain **pSwapChain
		, OUT ID3D11RenderTargetView **pRenderTargetView
		, OUT ID3D11Texture2D **pDepthStencil
		, OUT ID3D11DepthStencilView **pDepthStencilView
	);

}
