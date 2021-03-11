#include "stdafx.h"
#include "blender_ao.h"

void CBlender_ao::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:	//Pass 1 - AO generation
		C.r_Pass("stub_screen_space", "ambient_occlusion", FALSE, FALSE, FALSE);	
		C.r_dx10Texture("s_position", r2_RT_P);
			
		C.r_dx10Sampler("smp_rtlinear");
		C.r_dx10Sampler("smp_nofilter");	
		jitter(C);

		C.r_End();
		break;
	case 1:	//Pass 2 - AO blurring - horizontal
		C.r_Pass("stub_screen_space", "ambient_occlusion_blur", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_ao", r2_RT_ao);	
		C.r_dx10Texture("s_position", r2_RT_P);
		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_dx10Sampler("smp_nofilter");
		
		C.r_End();
		break;	
	case 2:	//Pass 2 - AO blurring - vertical
		C.r_Pass("stub_screen_space", "ambient_occlusion_blur", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_ao", r2_RT_ao_blur);	
		C.r_dx10Texture("s_position", r2_RT_P);
		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_dx10Sampler("smp_nofilter");
		
		C.r_End();
		break;			
	}
}

CBlender_ao::CBlender_ao()
{
	description.CLS = 0;
}

CBlender_ao::~CBlender_ao()
{
}
