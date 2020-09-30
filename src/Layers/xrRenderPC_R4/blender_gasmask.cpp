#include "stdafx.h"
#include "blender_gasmask.h"

void CBlender_gasmask::Compile(CBlender_Compile& C)
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
	case 1:	//Pass 1 - damage - 0-20%
		C.r_Pass("stub_screen_space", "fft_gasmask", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);		
		C.r_dx10Texture("s_gasmask", "fft_shaders\\gasmask\\gasmask_1");
		C.r_dx10Texture("s_gasmask_breath", "fft_shaders\\gasmask\\gasmask_breath");
		
		C.r_dx10Sampler("smp_base");		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 2:	//Pass 2 - damage - 20-40%
		C.r_Pass("stub_screen_space", "fft_gasmask", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);	
		C.r_dx10Texture("s_gasmask", "fft_shaders\\gasmask\\gasmask_2");
		C.r_dx10Texture("s_gasmask_breath", "fft_shaders\\gasmask\\gasmask_breath");
				
		C.r_dx10Sampler("smp_base");		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 3:	//Pass 3 - damage - 40-60%
		C.r_Pass("stub_screen_space", "fft_gasmask", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);	
		C.r_dx10Texture("s_gasmask", "fft_shaders\\gasmask\\gasmask_3");
		C.r_dx10Texture("s_gasmask_breath", "fft_shaders\\gasmask\\gasmask_breath");
				
		C.r_dx10Sampler("smp_base");		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 4:	//Pass 4 - damage - 60-80%
		C.r_Pass("stub_screen_space", "fft_gasmask", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);	
		C.r_dx10Texture("s_gasmask", "fft_shaders\\gasmask\\gasmask_4");
		C.r_dx10Texture("s_gasmask_breath", "fft_shaders\\gasmask\\gasmask_breath");
				
		C.r_dx10Sampler("smp_base");		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;	
	case 5:	//Pass 5 - damage - 80-100%
		C.r_Pass("stub_screen_space", "fft_gasmask", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);	
		C.r_dx10Texture("s_gasmask", "fft_shaders\\gasmask\\gasmask_5");
		C.r_dx10Texture("s_gasmask_breath", "fft_shaders\\gasmask\\gasmask_breath");
				
		C.r_dx10Sampler("smp_base");		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;		
	}
}

CBlender_gasmask::CBlender_gasmask()
{
	description.CLS = 0;
}

CBlender_gasmask::~CBlender_gasmask()
{
}
