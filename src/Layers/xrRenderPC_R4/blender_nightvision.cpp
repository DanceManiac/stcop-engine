#include "stdafx.h"
#include "blender_nightvision.h"

void CBlender_nightvision::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:	//Dummy pass - To avoid crash
		C.r_Pass("stub_screen_space", "fft_blit", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);
		C.r_dx10Sampler("smp_base");
		C.r_End();
		break;
	case 1:	//Pass 1 - gen1
		C.r_Pass("stub_screen_space", "fft_nv_gen_1", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);		

		C.r_dx10Sampler("smp_base");		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 2:	//Pass 2 - gen2
		C.r_Pass("stub_screen_space", "fft_nv_gen_2", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);	
		
		C.r_dx10Sampler("smp_base");		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 3:	//Pass 3 - gen3
		C.r_Pass("stub_screen_space", "fft_nv_gen_3", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);	

		C.r_dx10Sampler("smp_base");		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;		
	}
}

CBlender_nightvision::CBlender_nightvision()
{
	description.CLS = 0;
}

CBlender_nightvision::~CBlender_nightvision()
{
}
