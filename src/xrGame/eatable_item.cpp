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

CEatableItem::CEatableItem()
{
	m_iPortionsNum = -1;
	m_physic_item	= 0;
}

CEatableItem::~CEatableItem()
{
}

DLL_Pure *CEatableItem::_construct	()
{
	m_physic_item	= smart_cast<CPhysicItem*>(this);
	return			(inherited::_construct());
}

void CEatableItem::Load(LPCSTR section)
{
	inherited::Load(section);

	m_fHealth = pSettings->r_float(section, "eat_health");
	m_fPower = pSettings->r_float(section, "eat_power");
	m_fSatiety = pSettings->r_float(section, "eat_satiety");
	m_fRadiation = pSettings->r_float(section, "eat_radiation");
	m_fWoundsHeal = pSettings->r_float(section, "wounds_heal_perc");
	clamp(m_fWoundsHeal, 0.f, 1.f);
	m_fMaxPowerUp = READ_IF_EXISTS(pSettings, r_float, section, "eat_max_power", 0.0f);
	m_fAlcohol = READ_IF_EXISTS(pSettings, r_float, section, "eat_alcohol", 0.0f);
	//m_fTimeTotal = READ_IF_EXISTS(pSettings, r_float, sect.c_str(), "apply_time_sec", -1.0f);
	m_iPortionsNum = pSettings->r_s32	(section, "eat_portions_num");
	VERIFY (m_iPortionsNum<10000);

	m_sUseSoundName = pSettings->r_string(section, "use_sound");
}

BOOL CEatableItem::net_Spawn				(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC)) return FALSE;

	return TRUE;
};

bool CEatableItem::Useful() const
{
	if(!inherited::Useful()) return false;

	//проверить не все ли еще съедено
	if(m_iPortionsNum == 0) return false;

	return true;
}

void CEatableItem::OnH_A_Independent() 
{
	inherited::OnH_A_Independent();
	if(!Useful()) {
		if (object().Local() && OnServer())	object().DestroyObject	();
	}	
}

void CEatableItem::OnH_B_Independent(bool just_before_destroy)
{
	if(!Useful()) 
	{
		object().setVisible(FALSE);
		object().setEnabled(FALSE);
		if (m_physic_item)
			m_physic_item->m_ready_to_destroy	= true;
	}
	inherited::OnH_B_Independent(just_before_destroy);
}

bool CEatableItem::UseBy (CEntityAlive* entity_alive)
{
	CInventoryOwner* IO	= smart_cast<CInventoryOwner*>(entity_alive);
	R_ASSERT		(IO);
	R_ASSERT		(m_pInventory==IO->m_inventory);
	R_ASSERT		(object().H_Parent()->ID()==entity_alive->ID());

	entity_alive->conditions().ApplyInfluence(*this);

	for(u8 i = 0; i<(u8)eBoostMaxCount; i++)
	{
		if(pSettings->line_exist(m_physic_item->cNameSect().c_str(), ef_boosters_section_names[i]))
		{
			SBooster B;
			B.Load(m_physic_item->cNameSect(), (EBoostParams)i);
			entity_alive->conditions().ApplyBooster(B, m_physic_item->cNameSect());
		}
	}

	if (!IsGameTypeSingle() && OnServer())
	{
		NET_Packet				tmp_packet;
		CGameObject::u_EventGen	(tmp_packet, GEG_PLAYER_USE_BOOSTER, entity_alive->ID());
		tmp_packet.w_u16		(object_id());
		Level().Send			(tmp_packet);
	}
	
	if(m_iPortionsNum > 0)
		--m_iPortionsNum;
	else
		m_iPortionsNum = 0;

	return true;
}