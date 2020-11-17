#include "stdafx.h"
#include "Weapon.h"

void CWeapon::InitAddons()
{
}

bool CWeapon::bChangeNVSecondVPStatus()
{
	if (!bNVsecondVPavaible || !IsZoomed())
		return false;

	bNVsecondVPstatus = !bNVsecondVPstatus;

	return true;
}

void CWeapon::UpdateAddonsHudParams()
{

}
