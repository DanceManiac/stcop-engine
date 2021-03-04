// EngineAPI.cpp: implementation of the CEngineAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EngineAPI.h"
#include "../xrcdb/xrXRC.h"

#include "securom_api.h"

extern xr_token* vid_quality_token;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void __cdecl dummy(void)
{
};
CEngineAPI::CEngineAPI()
{
    hGame = 0;
    hRender = 0;
    hTuner = 0;
    pCreate = 0;
    pDestroy = 0;
    tune_pause = dummy;
    tune_resume = dummy;
}

CEngineAPI::~CEngineAPI()
{
    // destroy quality token here
    if (vid_quality_token)
    {
		//xr_free(vid_quality_token[0].name);
        xr_free(vid_quality_token);
        vid_quality_token = NULL;
    }
}

extern u32 renderer_value; //con cmd
ENGINE_API int g_current_renderer = 0;

ENGINE_API bool is_enough_address_space_available()
{
    SYSTEM_INFO system_info;

    SECUROM_MARKER_HIGH_SECURITY_ON(12)

        GetSystemInfo(&system_info);

    SECUROM_MARKER_HIGH_SECURITY_OFF(12)

        return (*(u32*)&system_info.lpMaximumApplicationAddress) > 0x90000000;
}

#ifndef DEDICATED_SERVER

void CEngineAPI::InitializeNotDedicated()
{
    SECUROM_MARKER_HIGH_SECURITY_ON(2)

    LPCSTR r4_name = "xrRender_R4.dll";

    if (psDeviceFlags.test(rsR4))
    {
        // try to initialize R4
        Log("Loading DLL:", r4_name);
        hRender = LoadLibrary(r4_name);
        if (0 == hRender)
        {
            // try to load R1
            Msg("! ...Failed - incompatible hardware/pre-Vista OS.");
        }
    }

    SECUROM_MARKER_HIGH_SECURITY_OFF(2)
}
#endif // DEDICATED_SERVER


void CEngineAPI::Initialize(void)
{
    //////////////////////////////////////////////////////////////////////////
    // render
    InitializeNotDedicated();
    Device.ConnectToRender();

    // game
    {
        LPCSTR g_name = "xrGame.dll";
        Log("Loading DLL:", g_name);
        hGame = LoadLibrary(g_name);
        if (0 == hGame) R_CHK(GetLastError());
        R_ASSERT2(hGame, "Game DLL raised exception during loading or there is no game DLL at all");
        pCreate = (Factory_Create*)GetProcAddress(hGame, "xrFactory_Create");
        R_ASSERT(pCreate);
        pDestroy = (Factory_Destroy*)GetProcAddress(hGame, "xrFactory_Destroy");
        R_ASSERT(pDestroy);
    }

    //////////////////////////////////////////////////////////////////////////
    // vTune
    tune_enabled = FALSE;
    if (strstr(Core.Params, "-tune"))
    {
        LPCSTR g_name = "vTuneAPI.dll";
        Log("Loading DLL:", g_name);
        hTuner = LoadLibrary(g_name);
        if (0 == hTuner) R_CHK(GetLastError());
        R_ASSERT2(hTuner, "Intel vTune is not installed");
        tune_enabled = TRUE;
        tune_pause = (VTPause*)GetProcAddress(hTuner, "VTPause");
        R_ASSERT(tune_pause);
        tune_resume = (VTResume*)GetProcAddress(hTuner, "VTResume");
        R_ASSERT(tune_resume);
    }
}

void CEngineAPI::Destroy(void)
{
    if (hGame) { FreeLibrary(hGame); hGame = 0; }
    if (hRender) { FreeLibrary(hRender); hRender = 0; }
    pCreate = 0;
    pDestroy = 0;
    Engine.Event._destroy();
    XRC.r_clear_compact();
}

extern "C" {
    typedef bool __cdecl SupportsAdvancedRendering(void);
    typedef bool _declspec(dllexport) SupportsDX10Rendering();
    typedef bool _declspec(dllexport) SupportsDX11Rendering();
};

void CEngineAPI::CreateRendererList()
{
    // TODO: ask renderers if they are supported!
    if (vid_quality_token != NULL) return;
    bool bSupports_r2 = false;
    bool bSupports_r2_5 = false;
    bool bSupports_r3 = false;
    bool bSupports_r4 = true;

    LPCSTR r4_name = "xrRender_R4.dll";

	// try to initialize R4
	Log("Loading DLL:", r4_name);
	// Hide "d3d10.dll not found" message box for XP
	SetErrorMode(SEM_FAILCRITICALERRORS);
	hRender = LoadLibrary(r4_name);
	// Restore error handling
	SetErrorMode(0);
	if (hRender)
	{
		SupportsDX11Rendering* test_dx11_rendering = (SupportsDX11Rendering*)GetProcAddress(hRender, "SupportsDX11Rendering");
		R_ASSERT(test_dx11_rendering);
		bSupports_r4 = test_dx11_rendering();
		FreeLibrary(hRender);
	}

    hRender = 0;

    vid_quality_token = xr_alloc<xr_token>(1);
    vid_quality_token[0].id = 0;
    vid_quality_token[0].name = "renderer_r4";
}