#include "stdafx.h"

void CRenderTarget::phase_gasmask()
{	//Thx OGSR for base RT function
	float w = float(Device.dwWidth);
	float h = float(Device.dwHeight);	
	
	u32 Offset = 0;
	float d_Z = EPS_S;
	float d_W = 1.0f;
	u32 C = color_rgba(0, 0, 0, 255);
	
	//Set output RT
	ref_rt& dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color; //OGSR workaround... sukiiii ukrali rendertargeta
	u_setrt(dest_rt, 0, 0, HW.pBaseZB);

	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	//No half pixel offset
	constexpr Fvector2 p0{ 0.0f, 0.0f }, p1{ 1.0f, 1.0f };

	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	RCache.set_Element(s_gasmask->E[pp_gasmask_state]);

	RCache.set_Geometry(g_combine);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	
	//Copy our RT
	HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());		
}
