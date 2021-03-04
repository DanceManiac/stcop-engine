// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "hwcaps.h"
#include "stats_manager.h"
#include <d3d11.h>

enum ViewPort;

struct HWViewPortRTZB
{
	HWViewPortRTZB()
	{
		baseRT = nullptr;
		baseZB = nullptr;
	};
	ID3D11RenderTargetView* baseRT;	//	combine with DX9 pBaseRT via typedef
	ID3D11DepthStencilView* baseZB;
};


class  CHW	:	public pureAppActivate, public pureAppDeactivate
{
//	Functions section
public:
	CHW();
	~CHW();

	void					CreateD3D				();
	void					DestroyD3D				();
	void					CreateDevice			(HWND hw, bool move_window);

	void					DestroyDevice			();

	void					Reset					(HWND hw);
	void					SwitchVP(ViewPort vp);

	void					selectResolution		(u32 &dwWidth, u32 &dwHeight, BOOL bWindowed);
	D3DFORMAT				selectDepthStencil		(D3DFORMAT);
	u32						selectPresentInterval	();
	u32						selectGPU				();
	u32						selectRefresh			(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt);
	void					updateWindowProps		(HWND hw);
	BOOL					support					(D3DFORMAT fmt, DWORD type, DWORD usage);

	void					Validate(void)	{};

//#include <dxgi1_2.h>

public:
	ViewPort				storedVP;
	xr_map<ViewPort, HWViewPortRTZB> viewPortsRTZB;

public:
	IDXGIAdapter*			m_pAdapter;	//	pD3D equivalent
	ID3D11Device*			pDevice;	//	combine with DX9 pDevice via typedef
	ID3D11DeviceContext*    pContext;	//	combine with DX9 pDevice via typedef
	IDXGISwapChain*         m_pSwapChain;
	//IDXGISwapChain1*        m_pSwapChain1;
	ID3D11RenderTargetView*	pBaseRT;	//	combine with DX9 pBaseRT via typedef
	ID3D11DepthStencilView*	pBaseZB;

	CHWCaps					Caps;

	D3D_DRIVER_TYPE			m_DriverType;	//	DevT equivalent
	DXGI_SWAP_CHAIN_DESC	m_ChainDesc;	//	DevPP equivalent
	//DXGI_SWAP_CHAIN_DESC1	m_ChainDesc1;	//	DevPP equivalent
	bool					m_bUsePerfhud;
	D3D_FEATURE_LEVEL		FeatureLevel;
	stats_manager			stats_manager;

	void			UpdateViews();
	DXGI_RATIONAL	selectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt);

	virtual	void	OnAppActivate();
	virtual void	OnAppDeactivate();

private:
	bool					m_move_window;
};

extern ECORE_API CHW		HW;

#endif // !defined(AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
