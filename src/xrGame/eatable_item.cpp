////////////////////////////////////////////////////////////////////////////
//	Module 		: eatable_item.cpp
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Yuri Dobronravin
//	Description : Eatable item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "eatable_item.h"
#include "xrmessages.h"
#include "physic_item.h"
#include "Level.h"
#include "entity_alive.h"
#include "EntityCondition.h"
#include "InventoryOwner.h"

const int PORTIONS_NUM_MAX = 9999;

CEatableItem::CEatableItem()
{
	m_nPortions = 0;
	m_physic_item = nullptr;
}

CEatableItem::~CEatableItem()
{
}

DLL_Pure* CEatableItem::_construct()
{
	m_physic_item = smart_cast<CPhysicItem*>(this);
	return			(inherited::_construct());
}

void CEatableItem::Load(LPCSTR section)
{
	inherited::Load(section);

	m_Boosters.fHealthRestore = pSettings->r_float(section, "boost_health_restore");
	m_Boosters.fSatietyRestore = pSettings->r_float(section, "boost_satiety_restore");
	m_Boosters.fPowerRestore = pSettings->r_float(section, "boost_power_restore");
	m_Boosters.fRadiationRestore = pSettings->r_float(section, "boost_radiation_restore");
	m_Boosters.fBleedingRestore = pSettings->r_float(section, "boost_bleeding_restore");
	m_Boosters.fMaxWeight = pSettings->r_float(section, "boost_max_weight");
	m_Boosters.fBurnImmunity = pSettings->r_float(section, "boost_burn_immunity");
	m_Boosters.fShockImmunity = pSettings->r_float(section, "boost_shock_immunity");
	m_Boosters.fRadiationImmunity = pSettings->r_float(section, "boost_radiation_immunity");
	m_Boosters.fTelepaticImmunity = pSettings->r_float(section, "boost_telepat_immunity");
	m_Boosters.fChemburnImmunity = pSettings->r_float(section, "boost_chemburn_immunity");
	m_Boosters.fExplosionImmunity = pSettings->r_float(section, "boost_explosion_immunity");
	m_Boosters.fStrikeImmunity = pSettings->r_float(section, "boost_strike_immunity");
	m_Boosters.fFireWoundImmunity = pSettings->r_float(section, "boost_fire_wound_immunity");
	m_Boosters.fWoundImmunity = pSettings->r_float(section, "boost_wound_immunity");;
	m_Boosters.fRadiationProtection = pSettings->r_float(section, "boost_radiation_protection");
	m_Boosters.fTelepaticProtection = pSettings->r_float(section, "boost_telepat_protection");
	m_Boosters.fChemburnProtection = pSettings->r_float(section, "boost_chemburn_protection");
	m_Boosters.fBoostTime = pSettings->r_float(section, "boost_time");


	m_fHealth = pSettings->r_float(section, "eat_health");
	m_fPower = pSettings->r_float(section, "eat_power");
	m_fSatiety = pSettings->r_float(section, "eat_satiety");
	m_fRadiation = pSettings->r_float(section, "eat_radiation");
	m_fWoundsHeal = pSettings->r_float(section, "wounds_heal_perc");
	clamp(m_fWoundsHeal, 0.f, 1.f);
	m_fMaxPowerUp = READ_IF_EXISTS(pSettings, r_float, section, "eat_max_power", 0.0f);
	m_fAlcohol = READ_IF_EXISTS(pSettings, r_float, section, "eat_alcohol", 0.0f);

	m_sUseSoundName = pSettings->r_string(section, "use_sound");
	m_nPortions = pSettings->r_s32(section, "eat_portions_num");
	VERIFY(m_nPortions <= PORTIONS_NUM_MAX);
}

BOOL CEatableItem::net_Spawn(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC))
		return FALSE;
	else
		return TRUE;
};

bool CEatableItem::Useful() const
{
	if (!inherited::Useful() || m_nPortions == 0)
		return false;
	else
		return true;
}

void CEatableItem::OnH_A_Independent()
{
	inherited::OnH_A_Independent();

	if (!Useful() && object().Local() && OnServer())
		object().DestroyObject();
}

void CEatableItem::OnH_B_Independent(bool just_before_destroy)
{
	if (!Useful())
	{
		object().setVisible(FALSE);
		object().setEnabled(FALSE);
		if (m_physic_item)
			m_physic_item->m_ready_to_destroy = true;
	}

	inherited::OnH_B_Independent(just_before_destroy);
}

bool CEatableItem::UseBy(CEntityAlive* entity_alive)
{
	CInventoryOwner* IO = smart_cast<CInventoryOwner*>(entity_alive);
	R_ASSERT(IO);
	R_ASSERT(m_pInventory == IO->m_inventory);
	R_ASSERT(object().H_Parent()->ID() == entity_alive->ID());

	entity_alive->conditions().ApplyBooster(*this);
		
	clamp(--m_nPortions, 0, PORTIONS_NUM_MAX);

	return true;
}