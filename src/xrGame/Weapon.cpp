#include "stdafx.h"
#include "Weapon.h"
#include "actor.h"
#include "ui/UIWindow.h"

ENGINE_API extern float psHUD_FOV_def;

CWeapon::CWeapon()
{
	SetState(eHidden);
	SetNextState(eHidden);
	m_sub_state = eSubstateReloadBegin;
	m_bTriStateReload = false;
	SetDefaults();

	m_Offset.identity();
	m_StrapOffset.identity();

	m_iAmmoCurrentTotal = 0;
	m_BriefInfo_CalcFrame = 0;

	iAmmoElapsed = -1;
	iMagazineSize = -1;
	m_ammoType = 0;

	eHandDependence = hdNone;

	m_zoom_params.m_fCurrentZoomFactor = g_fov;
	m_zoom_params.m_fZoomRotationFactor = 0.f;
	m_zoom_params.m_pVision = NULL;
	m_zoom_params.m_pNight_vision = NULL;

	m_pCurrentAmmo = NULL;

	m_pFlameParticles2 = NULL;
	m_sFlameParticles2 = NULL;

	m_fCurrentCartirdgeDisp = 1.f;

	m_strap_bone0 = 0;
	m_strap_bone1 = 0;
	m_StrapOffset.identity();
	m_strapped_mode = false;
	m_can_be_strapped = false;
	m_ef_main_weapon_type = u32(-1);
	m_ef_weapon_type = u32(-1);
	m_UIScope = NULL;
	m_set_next_ammoType_on_reload = undefined_ammo_type;
	m_crosshair_inertion = 0.f;
	m_activation_speed_is_overriden = false;
	m_cur_scope = NULL;
	m_bRememberActorNVisnStatus = false;

	//Mortan: new params
	bUseAltScope = false;
	bScopeIsHasTexture = false;
	bNVsecondVPavaible = false;
	bNVsecondVPstatus = false;

	m_nearwall_last_hud_fov = psHUD_FOV_def;
	m_fZoomStepCount = 3.0f;
	m_fZoomMinKoeff = 0.3f;
	m_fLR_MovingFactor = 0.f;
	m_fLR_CameraFactor = 0.f;
	m_fLR_InertiaFactor = 0.f;
	m_fUD_InertiaFactor = 0.f;

	m_zoom_params.m_f3dZoomFactor = 0.0f;
	m_zoom_params.m_fSecondVPFovFactor = 0.0f;

	m_fSecondRTZoomFactor = -1.0f;

	NeedUpdateHudParams = false;
}

CWeapon::~CWeapon()
{
	xr_delete(m_UIScope);
	delete_data(m_addons_list);
}
