#pragma once

#include "inventory_item.h"
#include "EntityCondition.h"
#include "ActorCondition.h"

class CPhysicItem;
class CEntityAlive;

class CEatableItem : public CInventoryItem {
private:
	typedef CInventoryItem	inherited;

protected:
	CPhysicItem* m_physic_item;

public:
	CEatableItem();
	virtual					~CEatableItem();
	virtual	DLL_Pure* _construct();
	virtual CEatableItem* cast_eatable_item() { return this; }

	virtual void			Load(LPCSTR section);
	virtual bool			Useful() const;

	virtual BOOL			net_Spawn(CSE_Abstract* DC);
				
	virtual void			OnH_B_Independent(bool just_before_destroy);
	virtual void			OnH_A_Independent();
	virtual	bool			UseBy(CEntityAlive* npc);
	virtual	bool			Empty() { return PortionsNum() == 0; };
	int				PortionsNum()	const { return m_iPortionsNum; };

public:
	float m_fHealth;
	float m_fPower;
	float m_fSatiety;
	float m_fRadiation;
	float m_fWoundsHeal;
	float m_fMaxPowerUp;
	float m_fAlcohol;
	float m_fToxicity;

	float m_fBoostBurnImmunity;
	float m_fBoostShockImmunity;
	float m_fBoostRadiationImmunity;
	float m_fBoostTelepaticImmunity;
	float m_fBoostChemicalBurnImmunity;
	float m_fBoostExplImmunity;
	float m_fBoostStrikeImmunity;
	float m_fBoostFireWoundImmunity;
	float m_fBoostWoundImmunity;
	float m_fBoostRadiationProtection;
	float m_fBoostTelepaticProtection;
	float m_fBoostChemicalBurnProtection;
	float m_fBoostToxicity;

	SBooster m_Boosters;

	shared_str m_sUseSoundName;

protected:
	int	m_iPortionsNum;
};

