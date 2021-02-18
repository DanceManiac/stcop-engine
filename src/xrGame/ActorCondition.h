// ActorCondition.h: класс состояния игрока
//

#pragma once

#include "EntityCondition.h"
#include "actor_defs.h"
#include "actor.h"
#include "inventory.h"

struct SBooster
{
	shared_str sSectionName;
	float fSatietyRestore;
	float fThirstRestore;
	float fAlcoholRestore;
	float fHealthRestore;
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
	float fToxicityRestore;
	float fBoostTime;
};

template <typename _return_type>
class CScriptCallbackEx;
class CActor;
class CActorDeathEffector;

class CActorCondition : public CEntityCondition {
private:
	typedef CEntityCondition inherited;
	enum
	{
		eCriticalPowerReached = (1 << 0),
		eCriticalMaxPowerReached = (1 << 1),
		eCriticalBleedingSpeed = (1 << 2),
		eCriticalSatietyReached = (1 << 3),
		eCriticalRadiationReached = (1 << 4),
		eWeaponJammedReached = (1 << 5),
		ePhyHealthMinReached = (1 << 6),
		eCantWalkWeight = (1 << 7),
		eCantWalkWeightReached = (1 << 8),
	};
	Flags16											m_condition_flags;
private:
	CActor* m_object;
	CActorDeathEffector* m_death_effector;
	void				UpdateTutorialThresholds();
	void 		UpdateSatiety();
	void 		UpdateThirst();
	void 		UpdateToxicity();
public:
	CActorCondition(CActor* object);
	virtual				~CActorCondition();

	virtual void		LoadCondition(LPCSTR section);
	virtual void		reinit();

	virtual CWound* ConditionHit(SHit* pHDS);
	virtual void		UpdateCondition();
	void		UpdateBoosters();

	virtual void 		ChangeAlcohol(const float value) { m_fAlcohol += value; };
	virtual void 		ChangeSatiety(const float value) { clamp(m_fSatiety += value, 0.0f, 1.0f); };
	virtual void 		ChangeThirst(const float value) { clamp(m_fThirst += value, 0.0f, 1.0f); };
	virtual void 		ChangeToxicity(const float value) { clamp(m_fToxicity += value, 0.0f, 1.0f); };

	void 				ChangeBoostParameters(const SBooster& B, bool positive);
	IC void				BoostMaxWeight(const float value) { m_object->inventory().SetMaxWeight(object().inventory().GetMaxWeight() + value); m_MaxWalkWeight += value; };
	IC void				BoostSatietyRestore(const float value) { m_fV_Satiety += value; };
	IC void				BoostThirstRestore(const float value) { m_fV_Thirst += value; };
	IC void				BoostAlcoholRestore(const float value) { m_fV_Alcohol += value; };
	IC void				BoostHpRestore(const float value) { m_change_v.m_fV_HealthRestore += value; };
	IC void				BoostPowerRestore(const float value) { m_fV_Power += value; };
	IC void				BoostRadiationRestore(const float value) { m_change_v.m_fV_Radiation += value; };
	IC void				BoostBleedingRestore(const float value) { m_change_v.m_fV_WoundIncarnation += value; };
	IC void				BoostBurnImmunity(const float value) { m_fBoostBurnImmunity += value; };
	IC void				BoostShockImmunity(const float value) { m_fBoostShockImmunity += value; };
	IC void				BoostRadiationImmunity(const float value) { m_fBoostRadiationImmunity += value; };
	IC void				BoostTelepaticImmunity(const float value) { m_fBoostTelepaticImmunity += value; };
	IC void				BoostChemicalBurnImmunity(const float value) { m_fBoostChemicalBurnImmunity += value; };
	IC void				BoostExplImmunity(const float value) { m_fBoostExplImmunity += value; };
	IC void				BoostStrikeImmunity(const float value) { m_fBoostStrikeImmunity += value; };
	IC void				BoostFireWoundImmunity(const float value) { m_fBoostFireWoundImmunity += value; };
	IC void				BoostWoundImmunity(const float value) { m_fBoostWoundImmunity += value; };
	IC void				BoostRadiationProtection(const float value) { m_fBoostRadiationProtection += value; };
	IC void				BoostTelepaticProtection(const float value) { m_fBoostTelepaticProtection += value; };
	IC void				BoostChemicalBurnProtection(const float value) { m_fBoostChemicalBurnProtection += value; };
	IC void				BoostToxicityRestore(const float value) { m_fV_Toxicity += value; };
	std::list<SBooster>   BoostersList;

	// хромание при потере сил и здоровья
	virtual	bool		IsLimping() const;
	virtual bool		IsCantWalk() const;
	virtual bool		IsCantWalkWeight();
	virtual bool		IsCantSprint() const;

	void		PowerHit(float power, bool apply_outfit);
	float		GetPower() const { return m_fPower; }

	void		ConditionJump(float weight);
	void		ConditionWalk(float weight, bool accel, bool sprint);
	void		ConditionStand(float weight);
	IC		float		MaxWalkWeight() const { return m_MaxWalkWeight; }

	float	xr_stdcall	GetAlcohol() { return m_fAlcohol; }
	float	xr_stdcall	GetPsy() { return 1.0f - GetPsyHealth(); }
	float				GetThirst() { return m_fThirst; }
	float				GetSatiety() { return m_fSatiety; }
	IC float GetConditionPower() const { return m_fV_Power * m_fSatiety * m_fThirst; };

	void		AffectDamage_InjuriousMaterialAndMonstersInfluence();
	float		GetInjuriousMaterialDamage();

	void		SetZoneDanger(float danger, ALife::EInfluenceType type);
	float		GetZoneDanger() const;

public:
	IC		CActor& object() const
	{
		VERIFY(m_object);
		return			(*m_object);
	}
	virtual void			save(NET_Packet& output_packet);
	virtual void			load(IReader& input_packet);
	//	IC		float const&	Satiety					()	{ return m_fSatiety; }
	IC		float const& V_Satiety() { return m_fV_Satiety; }
	IC		float const& V_SatietyPower() { return m_fV_Power; }
	IC		float const& V_SatietyHealth() { return m_fV_SatietyHealth; }
	IC		float const& SatietyCritical() { return m_fSatietyCritical; }

	float	GetZoneMaxPower(ALife::EInfluenceType type) const;
	float	GetZoneMaxPower(ALife::EHitType hit_type) const;

	bool	DisableSprint(SHit* pHDS);
	bool	PlayHitSound(SHit* pHDS);
	float	HitSlowmo(SHit* pHDS);
	virtual void ApplyBooster(const CEatableItem& object) override;
	float	GetMaxPowerRestoreSpeed() { return m_max_power_restore_speed; };
	float	GetMaxWoundProtection() { return m_max_wound_protection; };
	float	GetMaxFireWoundProtection() { return m_max_fire_wound_protection; };

protected:
	float m_fAlcohol;
	float m_fV_Alcohol;
	//--
	float m_fSatiety;
	float m_fV_Satiety;
	float m_fV_SatietyHealth;
	float m_fSatietyCritical;
	//--
	float m_fThirst;
	float m_fV_Thirst;
	float m_fV_ThirstPower;
	//--
	float m_fToxicity;
	float m_fV_Toxicity;
	float m_fV_ToxicityDamage;
	float m_fToxicityCritical;
	//--
	float m_fV_Power;
	float m_fPowerLeakSpeed;

	float m_fJumpPower;
	float m_fStandPower;
	float m_fWalkPower;
	float m_fJumpWeightPower;
	float m_fWalkWeightPower;
	float m_fOverweightWalkK;
	float m_fOverweightJumpK;
	float m_fAccelK;
	float m_fSprintK;

	float	m_MaxWalkWeight;
	float	m_zone_max_power[ALife::infl_max_count];
	float	m_zone_danger[ALife::infl_max_count];
	float	m_f_time_affected;
	float	m_max_power_restore_speed;
	float	m_max_wound_protection;
	float	m_max_fire_wound_protection;

	mutable bool m_bLimping;
	mutable bool m_bCantWalk;
	mutable bool m_bCantSprint;

	//порог силы и здоровья меньше которого актер начинает хромать
	float m_fLimpingPowerBegin;
	float m_fLimpingPowerEnd;
	float m_fCantWalkPowerBegin;
	float m_fCantWalkPowerEnd;

	float m_fCantSprintPowerBegin;
	float m_fCantSprintPowerEnd;

	float m_fLimpingHealthBegin;
	float m_fLimpingHealthEnd;

	ref_sound m_use_sound;
};

class CActorDeathEffector
{
	CActorCondition* m_pParent;
	ref_sound				m_death_sound;
	bool					m_b_actual;
	float					m_start_health;
	void xr_stdcall			OnPPEffectorReleased();
public:
	CActorDeathEffector(CActorCondition* parent, LPCSTR sect);
	~CActorDeathEffector() {};
	void	UpdateCL();
	IC bool	IsActual() { return m_b_actual; }
	void	Stop();
};