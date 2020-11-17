#include "stdafx.h"
#include "Weapon.h"
#include "object_broker.h"
#include "inventory.h"

void CWeapon::save(NET_Packet& output_packet)
{
	inherited::save(output_packet);
	save_data(iAmmoElapsed, output_packet);
	/*save_data(m_cur_scope, output_packet);
	save_data(m_cur_silencer, output_packet);
	save_data(m_cur_glauncher, output_packet);
	save_data(m_flagsAddOnState, output_packet);*/
	save_data(m_ammoType, output_packet);
	save_data(m_zoom_params.m_bIsZoomModeNow, output_packet);
	save_data(m_bRememberActorNVisnStatus, output_packet);
	save_data(bNVsecondVPstatus, output_packet);
	save_data(m_fSecondRTZoomFactor, output_packet);
}

void CWeapon::load(IReader& input_packet)
{
	inherited::load(input_packet);
	load_data(iAmmoElapsed, input_packet);
	/*load_data(m_cur_scope, input_packet);
	load_data(m_cur_silencer, input_packet);
	load_data(m_cur_glauncher, input_packet);
	load_data(m_flagsAddOnState, input_packet);*/
	UpdateAddonsVisibility();
	load_data(m_ammoType, input_packet);
	load_data(m_zoom_params.m_bIsZoomModeNow, input_packet);

	if (m_zoom_params.m_bIsZoomModeNow)
		OnZoomIn();
	else
		OnZoomOut();

	load_data(m_bRememberActorNVisnStatus, input_packet);
	load_data(bNVsecondVPstatus, input_packet);
	load_data(m_fSecondRTZoomFactor, input_packet);
}

BOOL CWeapon::net_Spawn(CSE_Abstract* DC)
{
	m_fRTZoomFactor = m_zoom_params.m_fScopeZoomFactor;
	BOOL bResult = inherited::net_Spawn(DC);
	CSE_Abstract* e = (CSE_Abstract*)(DC);
	CSE_ALifeItemWeapon* E = smart_cast<CSE_ALifeItemWeapon*>(e);

	//iAmmoCurrent					= E->a_current;
	iAmmoElapsed = E->a_elapsed;
	//m_flagsAddOnState = E->m_addon_flags.get();
	m_ammoType = E->ammo_type;
	SetState(E->wpn_state);
	SetNextState(E->wpn_state);

	m_DefaultCartridge.Load(m_ammoTypes[m_ammoType].c_str(), m_ammoType);
	if (iAmmoElapsed)
	{
		m_fCurrentCartirdgeDisp = m_DefaultCartridge.param_s.kDisp;
		for (int i = 0; i < iAmmoElapsed; ++i)
			m_magazine.push_back(m_DefaultCartridge);
	}

	UpdateAddonsVisibility();
	InitAddons();

	m_dwWeaponIndependencyTime = 0;

	VERIFY((u32)iAmmoElapsed == m_magazine.size());
	m_bAmmoWasSpawned = false;

	return bResult;
}

void CWeapon::net_Destroy()
{
	inherited::net_Destroy();

	//удалить объекты партиклов
	StopFlameParticles();
	StopFlameParticles2();
	StopLight();
	Light_Destroy();

	while (m_magazine.size()) m_magazine.pop_back();
}

BOOL CWeapon::IsUpdating()
{
	bool bIsActiveItem = m_pInventory && m_pInventory->ActiveItem() == this;
	return bIsActiveItem || bWorking;// || IsPending() || getVisible();
}

void CWeapon::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);

	P.w_float_q8(GetCondition(), 0.0f, 1.0f);


	u8 need_upd = IsUpdating() ? 1 : 0;
	P.w_u8(need_upd);
	P.w_u16(u16(iAmmoElapsed));
	P.w_u8(0);
	P.w_u8(m_ammoType);
	P.w_u8((u8)GetState());
	P.w_u8((u8)IsZoomed());
}

void CWeapon::net_Import(NET_Packet& P)
{
	inherited::net_Import(P);

	float _cond;
	P.r_float_q8(_cond, 0.0f, 1.0f);
	SetCondition(_cond);

	u8 flags = 0;
	P.r_u8(flags);

	u16 ammo_elapsed = 0;
	P.r_u16(ammo_elapsed);

	u8 NewAddonState;
	P.r_u8();

	UpdateAddonsVisibility();

	u8 ammoType, wstate;
	P.r_u8(ammoType);
	P.r_u8(wstate);

	u8 Zoom;
	P.r_u8((u8)Zoom);


	if (H_Parent() && H_Parent()->Remote())
	{
		if (Zoom) OnZoomIn();
		else OnZoomOut();
	};
	switch (wstate)
	{
	case eFire:
	case eFire2:
	case eSwitch:
	case eReload:
	{
	}break;
	default:
	{
		if (ammoType >= m_ammoTypes.size())
			Msg("!! Weapon [%d], State - [%d]", ID(), wstate);
		else
		{
			m_ammoType = ammoType;
			SetAmmoElapsed((ammo_elapsed));
		}
	}break;
	}

	VERIFY((u32)iAmmoElapsed == m_magazine.size());
}