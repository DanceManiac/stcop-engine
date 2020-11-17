#include "stdafx.h"
#include "Weapon.h"

float CWeapon::Weight() const
{
	float res = CInventoryItemObject::Weight();
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

	if (iAmmoElapsed)
	{
		float w = pSettings->r_float(m_ammoTypes[m_ammoType].c_str(), "cost");
		float bs = pSettings->r_float(m_ammoTypes[m_ammoType].c_str(), "box_size");

		res += iFloor(w * (iAmmoElapsed / bs));
	}
	return res;

}