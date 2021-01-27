#include "stdafx.h"

void CRenderTarget::phase_smaa()
{
	float w = float(Device.dwWidth);
	float h = float(Device.dwHeight);	
	
	u32 Offset = 0;
	float d_Z = EPS_S;
	float d_W = 1.0f;
	u32 C = color_rgba(0, 0, 0, 255);
	FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	//No half pixel offset
	constexpr Fvector2 p0{ 0.0f, 0.0f }, p1{ 1.0f, 1.0f };

	//////////////////////////////////////////////////////////////////////////
	//Edge detection
	u_setrt(rt_smaa_edgetex, 0, 0, 0);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x1, 0, 0, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
	RCache.Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0L);

	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	RCache.set_Element(s_smaa->E[0]);
	RCache.set_Geometry(g_combine);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	//////////////////////////////////////////////////////////////////////////
	//Blending	
	u_setrt(rt_smaa_blendtex, 0, 0, 0);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(TRUE, D3DCMP_EQUAL, 0x1, 0, 0, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
	RCache.Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0L);

	pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	RCache.set_Element(s_smaa->E[1]);
	RCache.set_Geometry(g_combine);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);	
	
	//////////////////////////////////////////////////////////////////////////
	//Final stage	
	ref_rt& dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;	

	u_setrt(dest_rt, 0, 0, HW.pBaseZB);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);
	
	pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	RCache.set_Element(s_smaa->E[2]);
	RCache.set_Geometry(g_combine);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);	
	
	//Copy our RT
	HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());		
}
