#include "stdafx.h"
#include "Weapon.h"
#include "xr_level_controller.h"
#include "level.h"
#include "actor.h"
#include "debug_renderer.h"
#include "Torch.h"
#include "player_hud.h"
#include "inventory.h"
#include "weaponBinocularsVision.h"

BOOL	b_toggle_weapon_aim = FALSE;

bool CWeapon::Action(u16 cmd, u32 flags)
{
	if (inherited::Action(cmd, flags)) return true;


	switch (cmd)
	{
	case kWPN_NV_CHANGE:
	{
		return bChangeNVSecondVPStatus();
	}
	case kWPN_FIRE:
	{
		//если оружие чем-то занято, то ничего не делать
		{
			if (IsPending())
				return				false;

			if (flags & CMD_START)
				FireStart();
			else
				FireEnd();
		};
	}
	return true;
	case kWPN_NEXT:
	{
		return SwitchAmmoType(flags);
	}

	case kWPN_ZOOM:
		if (IsZoomEnabled())
		{
			if (b_toggle_weapon_aim)
			{
				if (flags & CMD_START)
				{
					if (!IsZoomed())
					{
						if (!IsPending())
						{
							if (GetState() != eIdle)
								SwitchState(eIdle);
							OnZoomIn();
						}
					}
					else
						OnZoomOut();
				}
			}
			else
			{
				if (flags & CMD_START)
				{
					if (!IsZoomed() && !IsPending())
					{
						if (GetState() != eIdle)
							SwitchState(eIdle);
						OnZoomIn();
					}
				}
				else
					if (IsZoomed())
						OnZoomOut();
			}
			return true;
		}
		else
			return false;

	case kWPN_ZOOM_INC:
	case kWPN_ZOOM_DEC:
		if (IsZoomEnabled() && IsZoomed())
		{
			if (cmd == kWPN_ZOOM_INC)  ZoomInc();
			else					ZoomDec();
			return true;
		}
		else
			return false;
	}
	return false;
}

void CWeapon::OnEvent(NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_ADDON_CHANGE:
	{
		P.r_u8(m_flagsAddOnState);
		InitAddons();
		UpdateAddonsVisibility();
	}break;

	case GE_WPN_STATE_CHANGE:
	{
		u8				state;
		P.r_u8(state);
		P.r_u8(m_sub_state);
		//			u8 NewAmmoType = 
		P.r_u8();
		u8 AmmoElapsed = P.r_u8();
		u8 NextAmmo = P.r_u8();
		if (NextAmmo == undefined_ammo_type)
			m_set_next_ammoType_on_reload = undefined_ammo_type;
		else
			m_set_next_ammoType_on_reload = NextAmmo;

		if (OnClient()) SetAmmoElapsed(int(AmmoElapsed));
		OnStateSwitch(u32(state));
	}
	break;
	default:
	{
		inherited::OnEvent(P, type);
	}break;
	}
};

void CWeapon::OnStateSwitch(u32 S)
{
	inherited::OnStateSwitch(S);
	m_BriefInfo_CalcFrame = 0;
}

void CWeapon::OnAnimationEnd(u32 state)
{
	inherited::OnAnimationEnd(state);
}

void CWeapon::shedule_Update(u32 dT)
{
	inherited::shedule_Update(dT);
}

bool CWeapon::MovingAnimAllowedNow()
{
	return !IsZoomed();
}

bool CWeapon::IsHudModeNow()
{
	return (HudItemData() != NULL);
}

bool CWeapon::AllowBore()
{
	return true;
}

void CWeapon::UpdateCL()
{
	inherited::UpdateCL();
	UpdateHUDAddonsVisibility();
	//подсветка от выстрела
	UpdateLight();

	//нарисовать партиклы
	UpdateFlameParticles();
	UpdateFlameParticles2();

	if (!IsGameTypeSingle())
		make_Interpolation();

	if ((GetNextState() == GetState()) && IsGameTypeSingle() && H_Parent() == Level().CurrentEntity())
	{
		CActor* pActor = smart_cast<CActor*>(H_Parent());
		if (pActor && !pActor->AnyMove() && this == pActor->inventory().ActiveItem())
		{
			if (hud_adj_mode == 0 &&
				GetState() == eIdle &&
				(Device.dwTimeGlobal - m_dw_curr_substate_time > 20000) &&
				!IsZoomed() &&
				g_player_hud->attached_item(1) == NULL)
			{
				if (AllowBore())
					SwitchState(eBore);

				ResetSubStateTime();
			}
		}
	}

	if (m_zoom_params.m_pNight_vision && !need_renderable())
	{
		if (!m_zoom_params.m_pNight_vision->IsActive())
		{
			CActor* pA = smart_cast<CActor*>(H_Parent());
			R_ASSERT(pA);
			CTorch* pTorch = smart_cast<CTorch*>(pA->inventory().ItemFromSlot(TORCH_SLOT));
			if (pTorch && pTorch->GetNightVisionStatus())
			{
				m_bRememberActorNVisnStatus = pTorch->GetNightVisionStatus();
				pTorch->SwitchNightVision(false, false);
			}
			m_zoom_params.m_pNight_vision->Start(m_zoom_params.m_sUseZoomPostprocess, pA, false);
		}

	}
	else if (m_bRememberActorNVisnStatus)
	{
		m_bRememberActorNVisnStatus = false;
		EnableActorNVisnAfterZoom();
	}

	if (m_zoom_params.m_pVision)
		m_zoom_params.m_pVision->Update();
}

bool CWeapon::show_crosshair()
{
	return !IsPending() && (!IsZoomed() || !ZoomHideCrosshair());
}

bool CWeapon::show_indicators()
{
	return !(IsZoomed() && ZoomTexture());
}

float CWeapon::GetConditionToShow() const
{
	return	(GetCondition());
}

BOOL CWeapon::ParentMayHaveAimBullet()
{
	CObject* O = H_Parent();
	CEntityAlive* EA = smart_cast<CEntityAlive*>(O);
	return EA->cast_actor() != 0;
}

BOOL CWeapon::ParentIsActor()
{
	CObject* O = H_Parent();
	if (!O)
		return FALSE;

	CEntityAlive* EA = smart_cast<CEntityAlive*>(O);
	if (!EA)
		return FALSE;

	return EA->cast_actor() != 0;
}

extern u32 hud_adj_mode;

bool CWeapon::ZoomHideCrosshair()
{
	if (hud_adj_mode != 0)
		return false;

	return m_zoom_params.m_bHideCrosshairInZoom || ZoomTexture();
}

void CWeapon::debug_draw_firedeps()
{
	if (hud_adj_mode == 5 || hud_adj_mode == 6 || hud_adj_mode == 7)
	{
		CDebugRenderer& render = Level().debug_renderer();

		if (hud_adj_mode == 5)
			render.draw_aabb(get_LastFP(), 0.005f, 0.005f, 0.005f, D3DCOLOR_XRGB(255, 0, 0));

		if (hud_adj_mode == 6)
			render.draw_aabb(get_LastFP2(), 0.005f, 0.005f, 0.005f, D3DCOLOR_XRGB(0, 0, 255));

		if (hud_adj_mode == 7)
			render.draw_aabb(get_LastSP(), 0.005f, 0.005f, 0.005f, D3DCOLOR_XRGB(0, 255, 0));
	}
}

u8 CWeapon::GetCurrentHudOffsetIdx()
{
	CActor* pActor = smart_cast<CActor*>(H_Parent());
	if (!pActor)		return 0;

	bool b_aiming = ((IsZoomed() && m_zoom_params.m_fZoomRotationFactor <= 1.f) ||
		(!IsZoomed() && m_zoom_params.m_fZoomRotationFactor > 0.f));

	if (!b_aiming)
		return		0;
	else
		return		1;
}

void CWeapon::SwitchState(u32 S)
{
	if (OnClient()) return;

#ifndef MASTER_GOLD
	if (bDebug)
	{
		Msg("---Server is going to send GE_WPN_STATE_CHANGE to [%d], weapon_section[%s], parent[%s]",
			S, cNameSect().c_str(), H_Parent() ? H_Parent()->cName().c_str() : "NULL Parent");
	}
#endif // #ifndef MASTER_GOLD

	SetNextState(S);
	if (CHudItem::object().Local() && !CHudItem::object().getDestroy() && m_pInventory && OnServer())
	{
		// !!! Just single entry for given state !!!
		NET_Packet		P;
		CHudItem::object().u_EventGen(P, GE_WPN_STATE_CHANGE, CHudItem::object().ID());
		P.w_u8(u8(S));
		P.w_u8(u8(m_sub_state));
		P.w_u8(m_ammoType);
		P.w_u8(u8(iAmmoElapsed & 0xff));
		P.w_u8(m_set_next_ammoType_on_reload);
		CHudItem::object().u_EventSend(P, net_flags(TRUE, TRUE, FALSE, TRUE));
	}
}

const float& CWeapon::hit_probability() const
{
	VERIFY((g_SingleGameDifficulty >= egdNovice) && (g_SingleGameDifficulty <= egdMaster));
	return					(m_hit_probability[egdNovice]);
}

void CWeapon::Hit(SHit* pHDS)
{
	inherited::Hit(pHDS);
}

u32	CWeapon::ef_main_weapon_type() const
{
	VERIFY(m_ef_main_weapon_type != u32(-1));
	return	(m_ef_main_weapon_type);
}

u32	CWeapon::ef_weapon_type() const
{
	VERIFY(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}

void CWeapon::EnableActorNVisnAfterZoom()
{
	CActor* pA = smart_cast<CActor*>(H_Parent());
	if (IsGameTypeSingle() && !pA)
		pA = g_actor;

	if (pA)
	{
		CTorch* pTorch = smart_cast<CTorch*>(pA->inventory().ItemFromSlot(TORCH_SLOT));
		if (pTorch)
		{
			pTorch->SwitchNightVision(true, false);
			pTorch->GetNightVision()->PlaySounds(CNightVisionEffector::eIdleSound);
		}
	}
}