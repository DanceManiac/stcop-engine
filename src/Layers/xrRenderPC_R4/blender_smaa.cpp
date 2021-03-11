#include "stdafx.h"
#include "blender_smaa.h"

void CBlender_smaa::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:	//Pass 1 - Edge detection
		C.r_Pass("smaa_edge_detection", "smaa_edge_detection", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);		
		C.r_dx10Texture("s_position", r2_RT_P);
			
		C.r_dx10Sampler("smp_rtlinear");
		C.r_dx10Sampler("smp_nofilter");		
		C.r_End();
		break;
	case 1:	//Pass 2 - Blending
		C.r_Pass("smaa_blending_weight", "smaa_blending_weight", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);	

		C.r_dx10Texture("s_edgetex", r2_RT_smaa_edgetex);
		C.r_dx10Texture("s_areatex", "fft_shaders\\smaa\\smaa_area_tex");
		C.r_dx10Texture("s_searchtex", "fft_shaders\\smaa\\smaa_search_tex");
		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_dx10Sampler("smp_nofilter");
		C.r_End();
		break;
	case 2:	//Pass 3 - Final stage
		C.r_Pass("smaa_neighbour_blend", "smaa_neighbour_blend", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);	
		
		C.r_dx10Texture("s_blendtex", r2_RT_smaa_blendtex);
		
		C.r_dx10Sampler("smp_rtlinear");
		C.r_dx10Sampler("smp_nofilter");
		C.r_End();
		break;		
	}
}

CBlender_smaa::CBlender_smaa()
{
	description.CLS = 0;
}

CBlender_smaa::~CBlender_smaa()
{
}
