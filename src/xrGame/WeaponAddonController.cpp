#include "stdafx.h"
#include "Weapon.h"

bool CWeapon::IsGrenadeLauncherAttached() const
{
	return (ALife::eAddonAttachable == m_eGrenadeLauncherStatus &&
		0 != (m_flagsAddOnState & CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher)) ||
		ALife::eAddonPermanent == m_eGrenadeLauncherStatus;
}

bool CWeapon::IsScopeAttached() const
{
	return (ALife::eAddonAttachable == m_eScopeStatus &&
		0 != (m_flagsAddOnState & CSE_ALifeItemWeapon::eWeaponAddonScope)) ||
		ALife::eAddonPermanent == m_eScopeStatus;

}

bool CWeapon::IsSilencerAttached() const
{
	return (ALife::eAddonAttachable == m_eSilencerStatus &&
		0 != (m_flagsAddOnState & CSE_ALifeItemWeapon::eWeaponAddonSilencer)) ||
		ALife::eAddonPermanent == m_eSilencerStatus;
}

bool CWeapon::GrenadeLauncherAttachable()
{
	return (ALife::eAddonAttachable == m_eGrenadeLauncherStatus);
}
bool CWeapon::ScopeAttachable()
{
	return (ALife::eAddonAttachable == m_eScopeStatus);
}
bool CWeapon::SilencerAttachable()
{
	return (ALife::eAddonAttachable == m_eSilencerStatus);
}

int CWeapon::GetAddonIcon(u8 idx, bool x)
{
	if (m_addons_list.size() > idx && !m_addons_list.empty())
		return READ_IF_EXISTS(pSettings, r_s32, m_addons_list[idx], x ? "icon_x" : "icon_y", 0);
	else
		return 0;
}

int CWeapon::GetScopeX()
{
	return GetAddonIcon(m_cur_scope, true);
}

int CWeapon::GetScopeY()
{
	return GetAddonIcon(m_cur_scope, false);
}

int	CWeapon::GetSilencerX()
{
	return GetAddonIcon(m_cur_silencer, true);
}
int	CWeapon::GetSilencerY()
{
	return GetAddonIcon(m_cur_silencer, false);
}
int	CWeapon::GetGrenadeLauncherX()
{
	return GetAddonIcon(m_cur_glauncher, true);
}

int	CWeapon::GetGrenadeLauncherY()
{
	return GetAddonIcon(m_cur_glauncher, false);
}

shared_str CWeapon::GetAddonName(u8 idx) const
{

	if (m_addons_list.size() > idx && !m_addons_list.empty())
	{
		return READ_IF_EXISTS(pSettings, r_string, m_addons_list[idx], "addon_name", m_addons_list[idx]);
	}
	else
		return m_section_id;	
}

shared_str& CWeapon::GetScopeName() const
{
	return GetAddonName(m_cur_scope);
}

shared_str& CWeapon::GetSilencerName() const
{
	return GetAddonName(m_cur_silencer);
}

shared_str& CWeapon::GetGrenadeLauncherName() const
{
	return GetAddonName(m_cur_glauncher);
}

float CWeapon::Weight() const
{
	float res = CInventoryItemObject::Weight();
	if (IsGrenadeLauncherAttached() && GetGrenadeLauncherName().size()) {
		res += pSettings->r_float(GetGrenadeLauncherName(), "inv_weight");
	}
	if (IsScopeAttached() && GetScopeName().size()) {
		res += pSettings->r_float(GetScopeName(), "inv_weight");
	}
	if (IsSilencerAttached() && GetSilencerName().size()) {
		res += pSettings->r_float(GetSilencerName(), "inv_weight");
	}

	if (iAmmoElapsed)
	{
		float w = pSettings->r_float(m_ammoTypes[m_ammoType].c_str(), "inv_weight");
		float bs = pSettings->r_float(m_ammoTypes[m_ammoType].c_str(), "box_size");

		res += w * (iAmmoElapsed / bs);
	}
	return res;
}


u32 CWeapon::Cost() const
{
	u32 res = CInventoryItem::Cost();
	if (IsGrenadeLauncherAttached() && GetGrenadeLauncherName().size()) {
		res += pSettings->r_u32(GetGrenadeLauncherName(), "cost");
	}
	if (IsScopeAttached() && m_addons_list.size()) {
		res += pSettings->r_u32(GetScopeName(), "cost");
	}
	if (IsSilencerAttached() && GetSilencerName().size()) {
		res += pSettings->r_u32(GetSilencerName(), "cost");
	}

	if (iAmmoElapsed)
	{
		float w = pSettings->r_float(m_ammoTypes[m_ammoType].c_str(), "cost");
		float bs = pSettings->r_float(m_ammoTypes[m_ammoType].c_str(), "box_size");

		res += iFloor(w * (iAmmoElapsed / bs));
	}
	return res;

}