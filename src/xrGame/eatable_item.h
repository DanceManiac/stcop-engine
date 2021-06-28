#pragma once

#include "inventory_item.h"

class CPhysicItem;
class CEntityAlive;

struct SBooster 
{
	float fHealthRestore;
	float fSatietyRestore;
	float fPowerRestore;
	float fRadiationRestore;
	float fBleedingRestore;
	float fMaxWeight;
	float fBurnImmunity;
	float fShockImmunity;
	float fRadiationImmunity;
	float fTelepaticImmunity;
	float fChemburnImmunity;
	float fExplosionImmunity;
	float fStrikeImmunity;
	float fFireWoundImmunity;
	float fWoundImmunity;
	float fRadiationProtection;
	float fTelepaticProtection;
	float fChemburnProtection;
	float fBoostTime;
};

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
	int				PortionsNum()	const { return m_nPortions; }
	LPCSTR				UseSoundName()	const { return m_sUseSoundName; }

protected:
	int						m_nPortions;
	LPCSTR					m_sUseSoundName;

public:
	SBooster m_Boosters;

	float m_fHealth;
	float m_fPower;
	float m_fSatiety;
	float m_fRadiation;
	float m_fWoundsHeal;
	float m_fMaxPowerUp;
	float m_fAlcohol;

};

