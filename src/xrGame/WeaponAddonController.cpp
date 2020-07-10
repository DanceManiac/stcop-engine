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

int CWeapon::GetScopeX()
{
	if (bUseAltScope)
	{
		if (m_eScopeStatus != ALife::eAddonPermanent && IsScopeAttached())
		{
			return pSettings->r_s32(GetNameWithAttachment(), "scope_x");
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return pSettings->r_s32(m_addons_list[m_cur_scope], "scope_x");
	}
}

int CWeapon::GetScopeY()
{
	if (bUseAltScope)
	{
		if (m_eScopeStatus != ALife::eAddonPermanent && IsScopeAttached())
		{
			return pSettings->r_s32(GetNameWithAttachment(), "scope_y");
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return pSettings->r_s32(m_addons_list[m_cur_scope], "scope_y");
	}
}

const shared_str CWeapon::GetScopeName() const
{
	if (bUseAltScope)
	{
		return m_addons_list[m_cur_scope];
	}
	else
	{
		return pSettings->r_string(m_addons_list[m_cur_scope], "scope_name");
	}
}

shared_str CWeapon::GetNameWithAttachment()
{
	string64 str;
	if (pSettings->line_exist(m_section_id.c_str(), "parent_section"))
	{
		shared_str parent = pSettings->r_string(m_section_id.c_str(), "parent_section");
		xr_sprintf(str, "%s_%s", parent.c_str(), GetScopeName().c_str());
	}
	else
	{
		xr_sprintf(str, "%s_%s", m_section_id.c_str(), GetScopeName().c_str());
	}
	return (shared_str)str;
}

float CWeapon::Weight() const
{
	float res = CInventoryItemObject::Weight();
	if (IsGrenadeLauncherAttached() && GetGrenadeLauncherName().size()) {
		res += pSettings->r_float(GetGrenadeLauncherName(), "inv_weight");
	}
	if (IsScopeAttached() && m_addons_list.size()) {
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