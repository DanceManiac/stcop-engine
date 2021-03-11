#include "stdafx.h"
#include "blender_ssr.h"

void CBlender_ssr::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:	//Pass 1 - Raytracing
		C.r_Pass("stub_screen_space", "ssr_generate", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);		
		C.r_dx10Texture("s_position", r2_RT_P);
			
		C.r_dx10Sampler("smp_rtlinear");
		C.r_dx10Sampler("smp_nofilter");		
		C.r_End();
		break;
	case 1:	//Pass 2 - 
		break;
	case 2:	//Pass 3 - 
		break;		
	}
}

CBlender_ssr::CBlender_ssr()
{
	description.CLS = 0;
}

CBlender_ssr::~CBlender_ssr()
{
}
