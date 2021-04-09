#include "pch_script.h"
#include "actorcondition.h"
#include "actor.h"
#include "actorEffector.h"
#include "inventory.h"
#include "level.h"
#include "sleepeffector.h"
#include "game_base_space.h"
#include "autosave_manager.h"
#include "xrserver.h"
#include "ai_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "object_broker.h"
#include "weapon.h"
#include "eatable_item.h"

#include "PDA.h"
#include "ai/monsters/basemonster/base_monster.h"
#include "UIGameCustom.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIStatic.h"

#include "characterphysicssupport.h"

#define MAX_SATIETY					1.0f
#define START_SATIETY				0.5f

BOOL GodMode ()	
{ 
	if (GameID() == eGameIDSingle) 
		return psActorFlags.test(AF_GODMODE|AF_GODMODE_RT); 
	return FALSE;	
}

CActorCondition::CActorCondition(CActor *object) :
	inherited	(object)
{
	m_fJumpPower				= 0.f;
	m_fStandPower				= 0.f;
	m_fWalkPower				= 0.f;
	m_fJumpWeightPower			= 0.f;
	m_fWalkWeightPower			= 0.f;
	m_fOverweightWalkK			= 0.f;
	m_fOverweightJumpK			= 0.f;
	m_fAccelK					= 0.f;
	m_fSprintK					= 0.f;
	m_fAlcohol					= 0.f;
	m_fSatiety					= 1.0f;
	m_fThirst					= 1.0f;
	m_fToxicity					= 0.0f;

	VERIFY						(object);
	m_object					= object;
	m_condition_flags.zero		();
	m_death_effector			= NULL;

	m_zone_max_power[ALife::infl_rad]	= 1.0f;
	m_zone_max_power[ALife::infl_fire]	= 1.0f;
	m_zone_max_power[ALife::infl_acid]	= 1.0f;
	m_zone_max_power[ALife::infl_psi]	= 1.0f;
	m_zone_max_power[ALife::infl_electra] = 1.0f;

	m_zone_danger[ALife::infl_rad]	= 0.0f;
	m_zone_danger[ALife::infl_fire]	= 0.0f;
	m_zone_danger[ALife::infl_acid]	= 0.0f;
	m_zone_danger[ALife::infl_psi]	= 0.0f;
	m_zone_danger[ALife::infl_electra] = 0.0f;
	m_f_time_affected = Device.fTimeGlobal;

	m_max_power_restore_speed	= 0.0f;
	m_max_wound_protection		= 0.0f;
	m_max_fire_wound_protection = 0.0f;
}

CActorCondition::~CActorCondition()
{
	xr_delete(m_death_effector);
}

void CActorCondition::LoadCondition(LPCSTR entity_section)
{
	inherited::LoadCondition(entity_section);

	LPCSTR						section = READ_IF_EXISTS(pSettings,r_string,entity_section,"condition_sect",entity_section);

	m_fJumpPower				= pSettings->r_float(section,"jump_power");
	m_fStandPower				= pSettings->r_float(section,"stand_power");
	m_fWalkPower				= pSettings->r_float(section,"walk_power");
	m_fJumpWeightPower			= pSettings->r_float(section,"jump_weight_power");
	m_fWalkWeightPower			= pSettings->r_float(section,"walk_weight_power");
	m_fOverweightWalkK			= pSettings->r_float(section,"overweight_walk_k");
	m_fOverweightJumpK			= pSettings->r_float(section,"overweight_jump_k");
	m_fAccelK					= pSettings->r_float(section,"accel_k");
	m_fSprintK					= pSettings->r_float(section,"sprint_k");

	//порог силы и здоровья меньше которого актер начинает хромать
	m_fLimpingHealthBegin		= pSettings->r_float(section,	"limping_health_begin");
	m_fLimpingHealthEnd			= pSettings->r_float(section,	"limping_health_end");
	R_ASSERT					(m_fLimpingHealthBegin<=m_fLimpingHealthEnd);

	m_fLimpingPowerBegin		= pSettings->r_float(section,	"limping_power_begin");
	m_fLimpingPowerEnd			= pSettings->r_float(section,	"limping_power_end");
	R_ASSERT					(m_fLimpingPowerBegin<=m_fLimpingPowerEnd);

	m_fCantWalkPowerBegin		= pSettings->r_float(section,	"cant_walk_power_begin");
	m_fCantWalkPowerEnd			= pSettings->r_float(section,	"cant_walk_power_end");
	R_ASSERT					(m_fCantWalkPowerBegin<=m_fCantWalkPowerEnd);

	m_fCantSprintPowerBegin		= pSettings->r_float(section,	"cant_sprint_power_begin");
	m_fCantSprintPowerEnd		= pSettings->r_float(section,	"cant_sprint_power_end");
	R_ASSERT					(m_fCantSprintPowerBegin<=m_fCantSprintPowerEnd);

	m_fPowerLeakSpeed			= pSettings->r_float(section,"max_power_leak_speed");
	
	m_fV_Alcohol				= pSettings->r_float(section,"alcohol_v");

	m_fSatietyCritical			= pSettings->r_float(section,"satiety_critical");
	clamp						(m_fSatietyCritical, 0.0f, 1.0f);
	m_fV_Satiety				= pSettings->r_float(section, "satiety_v");		
	m_fV_SatietyHealth			= pSettings->r_float(section, "satiety_health_v");
	m_fV_SatietyPower = pSettings->r_float(section, "satiety_power_v");

	m_fV_Thirst = pSettings->r_float(section, "thirst_v");
	m_fThirstCritical = pSettings->r_float(section, "thirst_critical");

	m_fToxicityCritical = pSettings->r_float(section, "toxicity_critical");
	m_fV_Toxicity = pSettings->r_float(section, "toxicity_v");
	m_fV_ToxicityDamage = pSettings->r_float(section, "toxicity_damage_v");
	
	m_MaxWalkWeight				= pSettings->r_float(section,"max_walk_weight");

	m_zone_max_power[ALife::infl_rad]	= pSettings->r_float(section, "radio_zone_max_power" );
	m_zone_max_power[ALife::infl_fire]	= pSettings->r_float(section, "fire_zone_max_power" );
	m_zone_max_power[ALife::infl_acid]	= pSettings->r_float(section, "acid_zone_max_power" );
	m_zone_max_power[ALife::infl_psi]	= pSettings->r_float(section, "psi_zone_max_power" );
	m_zone_max_power[ALife::infl_electra]= pSettings->r_float(section, "electra_zone_max_power" );

	m_max_power_restore_speed = pSettings->r_float(section, "max_power_restore_speed" );
	m_max_wound_protection = READ_IF_EXISTS(pSettings,r_float,section,"max_wound_protection",1.0f);
	m_max_fire_wound_protection = READ_IF_EXISTS(pSettings,r_float,section,"max_fire_wound_protection",1.0f);

	VERIFY( !fis_zero(m_zone_max_power[ALife::infl_rad]) );
	VERIFY( !fis_zero(m_zone_max_power[ALife::infl_fire]) );
	VERIFY( !fis_zero(m_zone_max_power[ALife::infl_acid]) );
	VERIFY( !fis_zero(m_zone_max_power[ALife::infl_psi]) );
	VERIFY( !fis_zero(m_zone_max_power[ALife::infl_electra]) );
	VERIFY( !fis_zero(m_max_power_restore_speed) );
}

float CActorCondition::GetZoneMaxPower( ALife::EInfluenceType type) const
{
	if ( type < ALife::infl_rad || ALife::infl_electra < type )
	{
		return 1.0f;
	}
	return m_zone_max_power[type];
}

float CActorCondition::GetZoneMaxPower( ALife::EHitType hit_type ) const
{
	ALife::EInfluenceType iz_type = ALife::infl_max_count;
	switch( hit_type )
	{
	case ALife::eHitTypeRadiation:		iz_type = ALife::infl_rad;		break;
	case ALife::eHitTypeLightBurn:		iz_type = ALife::infl_fire;		break;
	case ALife::eHitTypeBurn:			iz_type = ALife::infl_fire;		break;
	case ALife::eHitTypeChemicalBurn:	iz_type = ALife::infl_acid;		break;
	case ALife::eHitTypeTelepatic:		iz_type = ALife::infl_psi;		break;
	case ALife::eHitTypeShock:			iz_type = ALife::infl_electra;	break;

	case ALife::eHitTypeStrike:
	case ALife::eHitTypeExplosion:
	case ALife::eHitTypeFireWound:
	case ALife::eHitTypeWound_2:
		return 1.0f;
	case ALife::eHitTypeWound:
		return m_max_wound_protection;
	default:
		NODEFAULT;
	}
	
	return GetZoneMaxPower( iz_type );
}

void CActorCondition::UpdateCondition()
{
	if (psActorFlags.test(AF_GODMODE_RT))
	{
		UpdateAlcohol();
		UpdateThirst();
		UpdateSatiety();
		UpdateBoosters();
		UpdateToxicity();
	}

	if (GodMode() || !object().g_Alive() || !object().Local() && m_object != Level().CurrentViewEntity())
		return;
	
	float base_weight = object().MaxCarryWeight();
	float cur_weight = object().inventory().TotalWeight();

	if ((object().mstate_real&mcAnyMove))
		ConditionWalk(cur_weight / base_weight, isActorAccelerated(object().mstate_real,object().IsZoomAimingMode()), (object().mstate_real&mcSprint) != 0);
	else
		ConditionStand( cur_weight / base_weight );

	float k_max_power = 1.0f + _min(cur_weight, base_weight) / base_weight + _max(0.0f, (cur_weight - base_weight) / 10.0f);
	SetMaxPower(GetMaxPower() - m_fPowerLeakSpeed * m_fDeltaTime * k_max_power);

	UpdateAlcohol();
		
	string512 pp_sect_name;
	shared_str ln = Level().name();
	if (ln.size())
	{
		CEffectorPP* ppe = object().Cameras().GetPPEffector((EEffectorPPType)effPsyHealth);		

		strconcat(sizeof(pp_sect_name),pp_sect_name, "effector_psy_health", "_", *ln);
		if (!pSettings->section_exist(pp_sect_name))
			xr_strcpy(pp_sect_name, "effector_psy_health");

		if (!fsimilar(GetPsyHealth(), 1.0f, 0.05f) && !ppe)
			AddEffector(m_object,effPsyHealth, pp_sect_name, GET_KOEFF_FUNC(this, &CActorCondition::GetPsy));
		else if (ppe)
			RemoveEffector(m_object,effPsyHealth);
	}

	UpdateThirst();
	UpdateSatiety();
	UpdateBoosters();
	UpdateToxicity();

	inherited::UpdateCondition();

	UpdateTutorialThresholds();

	if(GetHealth()<0.05f && m_death_effector==NULL)
	{
		if(pSettings->section_exist("actor_death_effector"))
			m_death_effector = xr_new<CActorDeathEffector>(this, "actor_death_effector");
	}
	if(m_death_effector && m_death_effector->IsActual())
	{
		m_death_effector->UpdateCL	();

		if(!m_death_effector->IsActual())
			m_death_effector->Stop();
	}

	AffectDamage_InjuriousMaterialAndMonstersInfluence();
}

void CActorCondition::UpdateBoosters()
{
	for (auto& it = m_BoostersList.begin(); it != m_BoostersList.end(); ++it)
	{
		it->fBoostTime -= m_fDeltaTime / Level().GetGameTimeFactor();
		if (it->fBoostTime <= 0.0f)
		{
			DisableBooster(*it);
			m_BoostersList.erase(it);
			break;
		}
	}
}

void CActorCondition::UpdateThirst()
{
	if (GetThirst() > 0)
		ChangeThirst(m_fV_Thirst * m_fDeltaTime);
}

void CActorCondition::UpdateSatiety()
{
	float V_satiety_koef = 1.0f + (GetMaxHealth() - GetHealth() + GetMaxPower() - GetPower()) / 2;

	if (GetSatiety() > 0)
		ChangeSatiety(m_fV_Satiety * V_satiety_koef * m_fDeltaTime);

	float satiety_health_koef = (m_fSatiety - m_fSatietyCritical) / (m_fSatiety >= m_fSatietyCritical ? 1 - m_fSatietyCritical : m_fSatietyCritical) 
		* (m_fSatiety >= m_fSatietyCritical ? m_fThirst : 2.0f - m_fThirst);

	if (CanBeHarmed() && !psActorFlags.test(AF_GODMODE_RT)) 
	{
		ChangeHealth(m_fV_SatietyHealth * satiety_health_koef * m_fDeltaTime);
		ChangePower(m_fV_SatietyPower * m_fSatiety * m_fThirst * m_fDeltaTime);
	}
}

void CActorCondition::UpdateAlcohol()
{
	ChangeAlcohol(m_fV_Alcohol * m_fDeltaTime);

	if (!psActorFlags.test(AF_GODMODE_RT)) 
	{
		CEffectorCam* ce = Actor()->Cameras().GetCamEffector((ECamEffectorType)effAlcohol);
		if (GetAlcohol() > 0.0001f && !ce)
			AddEffector(m_object, effAlcohol, "effector_alcohol", GET_KOEFF_FUNC(this, &CActorCondition::GetAlcohol));
		else if (ce)
			RemoveEffector(m_object, effAlcohol);
	}
	else 
	{
		CEffectorCam* ce = Actor()->Cameras().GetCamEffector((ECamEffectorType)effAlcohol);
		if (ce)
			RemoveEffector(m_object, effAlcohol);
	}
}

void CActorCondition::UpdateToxicity()
{
	ChangeToxicity(m_fV_Toxicity * m_fDeltaTime * (m_fV_Toxicity < 0 ? m_fThirst : 1));

	if (CanBeHarmed() && !psActorFlags.test(AF_GODMODE_RT)) 
	{
		if (GetToxicity() >= m_fToxicityCritical)
			ChangeHealth(m_fV_ToxicityDamage * m_fDeltaTime);
	}
}

void CActorCondition::AffectDamage_InjuriousMaterialAndMonstersInfluence()
{
	float one = 0.1f;
	float tg  = Device.fTimeGlobal;
	if (m_f_time_affected + one > tg)
		return;

	clamp(m_f_time_affected, tg - (one * 3), tg);

	float psy_influence	= 0;
	float fire_influence = 0;
	float radiation_influence =	GetInjuriousMaterialDamage(); // Get Radiation from Material

	// Add Radiation and Psy Level from Monsters
	CPda* const pda	= m_object->GetPDA();

	if (pda)
	{
		typedef xr_vector<CObject*>				monsters;

		for (monsters::const_iterator it = pda->feel_touch.begin(); it != pda->feel_touch.end(); ++it)
		{
			CBaseMonster* const	monster		=	smart_cast<CBaseMonster*>(*it);
			if (!monster || !monster->g_Alive()) 
				continue;

			psy_influence += monster->get_psy_influence();
			radiation_influence	+= monster->get_radiation_influence();
			fire_influence += monster->get_fire_influence();
		}
	}

	struct 
	{
		ALife::EHitType	type;
		float value;
	} 
	hits[] = {{ ALife::eHitTypeRadiation, radiation_influence* one }, { ALife::eHitTypeTelepatic, psy_influence* one }, { ALife::eHitTypeBurn, fire_influence* one }};

 	NET_Packet np;

	while ( m_f_time_affected + one < tg )
	{
		m_f_time_affected += one;

		for (int i=0; i<sizeof(hits)/sizeof(hits[0]); ++i)
		{
			float			damage	= hits[i].value;
			ALife::EHitType	type	=hits[i].type;

			if ( damage > EPS )
			{
				SHit HDS = SHit(damage, Fvector().set(0,1,0), NULL, BI_NONE, Fvector().set(0,0,0), 0.0f, type, 0.0f, false);

				HDS.GenHeader(GE_HIT, m_object->ID());
				HDS.Write_Packet( np );
				CGameObject::u_EventSend( np );
			}
		}
	}
}

float CActorCondition::GetInjuriousMaterialDamage()
{
	u16 mat_injurios = m_object->character_physics_support()->movement()->injurious_material_idx();

	if(mat_injurios!=GAMEMTL_NONE_IDX)
	{
		const SGameMtl* mtl	= GMLib.GetMaterialByIdx(mat_injurios);
		return mtl->fInjuriousSpeed;
	}
	else return 0.0f;
}

void CActorCondition::SetZoneDanger( float danger, ALife::EInfluenceType type )
{
	VERIFY(type != ALife::infl_max_count);
	m_zone_danger[type] = danger;
	clamp(m_zone_danger[type], 0.0f, 1.0f);
}

float CActorCondition::GetZoneDanger() const
{
	float sum = 0.0f;
	for (u8 i = 1; i < ALife::infl_max_count; ++i)
		sum += m_zone_danger[i];

	clamp(sum, 0.0f, 1.5f);
	return sum;
}

CWound* CActorCondition::ConditionHit(SHit* pHDS)
{
	if (GodMode()) return NULL;
		return inherited::ConditionHit(pHDS);
}

void CActorCondition::PowerHit(float power, bool apply_outfit)
{
	m_fPower -=	apply_outfit ? HitPowerEffect(power) : power;
	clamp (m_fPower, 0.f, 1.f);
}

void CActorCondition::ConditionJump(float weight)
{
	float power	= m_fJumpPower;
	power += m_fJumpWeightPower*weight*(weight>1.f?m_fOverweightJumpK:1.f);
	m_fPower -=	HitPowerEffect(power);
}

void CActorCondition::ConditionWalk(float weight, bool accel, bool sprint)
{	
	float power	= m_fWalkPower;
	power += m_fWalkWeightPower*weight*(weight>1.f?m_fOverweightWalkK:1.f);
	power *= m_fDeltaTime*(accel?(sprint?m_fSprintK:m_fAccelK):1.f);
	m_fPower -=	HitPowerEffect(power);
}

void CActorCondition::ConditionStand(float weight)
{	
	float power	= m_fStandPower;
	power *= m_fDeltaTime;
	m_fPower -= power;
}

bool CActorCondition::IsCantWalk() const
{
	if(m_fPower < m_fCantWalkPowerBegin)
		m_bCantWalk		= true;
	else if(m_fPower > m_fCantWalkPowerEnd)
		m_bCantWalk		= false;
	return m_bCantWalk;
}

bool CActorCondition::IsCantWalkWeight()
{
	if(!GodMode())
	{
		if( object().inventory().TotalWeight() > m_object->MaxWalkWeight())
		{
			m_condition_flags.set(eCantWalkWeight, TRUE);
			return true;
		}
	}
	m_condition_flags.set(eCantWalkWeight, FALSE);
	return false;
}

bool CActorCondition::IsCantSprint() const
{
	if(m_fPower< m_fCantSprintPowerBegin)
		m_bCantSprint = true;
	else if(m_fPower > m_fCantSprintPowerEnd)
		m_bCantSprint = false;
	return m_bCantSprint;
}

bool CActorCondition::IsLimping() const
{
	if(m_fPower< m_fLimpingPowerBegin || GetHealth() < m_fLimpingHealthBegin)
		m_bLimping = true;
	else if(m_fPower > m_fLimpingPowerEnd && GetHealth() > m_fLimpingHealthEnd)
		m_bLimping = false;
	return m_bLimping;
}
extern bool g_bShowHudInfo;

void CActorCondition::save(NET_Packet &output_packet)
{
	inherited::save		(output_packet);
	save_data			(m_fAlcohol, output_packet);
	save_data			(m_condition_flags, output_packet);
	save_data			(m_fSatiety, output_packet);
	save_data			(m_fThirst, output_packet);
	save_data			(m_fToxicity, output_packet);

	save_data((u8)m_BoostersList.size(), output_packet);

	for (auto& it = m_BoostersList.cbegin(); it != m_BoostersList.cend(); ++it)
	{
		//B.sSectionName = input_packet.r_s8();
		save_data(it->fSatietyRestore, output_packet);
		save_data(it->fThirstRestore, output_packet);
		save_data(it->fAlcoholRestore, output_packet);
		save_data(it->fHealthRestore, output_packet);
		save_data(it->fPowerRestore, output_packet);
		save_data(it->fRadiationRestore, output_packet);
		save_data(it->fBleedingRestore, output_packet);
		save_data(it->fMaxWeight, output_packet);
		save_data(it->fBurnImmunity, output_packet);
		save_data(it->fShockImmunity, output_packet);
		save_data(it->fRadiationImmunity, output_packet);
		save_data(it->fTelepaticImmunity, output_packet);
		save_data(it->fChemburnImmunity, output_packet);
		save_data(it->fExplosionImmunity, output_packet);
		save_data(it->fStrikeImmunity, output_packet);
		save_data(it->fFireWoundImmunity, output_packet);
		save_data(it->fWoundImmunity, output_packet);
		save_data(it->fRadiationProtection, output_packet);
		save_data(it->fTelepaticProtection, output_packet);
		save_data(it->fChemburnProtection, output_packet);
		save_data(it->fToxicityRestore, output_packet);
		save_data(it->fSpeedFactor, output_packet);
		save_data(it->fDispFactor, output_packet);

		save_data(it->fBoostTime, output_packet);
	}
}

void CActorCondition::load(IReader &input_packet)
{
	inherited::load		(input_packet);
	load_data			(m_fAlcohol, input_packet);
	load_data			(m_condition_flags, input_packet);
	load_data			(m_fSatiety, input_packet);
	load_data			(m_fThirst, input_packet);
	load_data			(m_fToxicity, input_packet);

	for (u8 cntr = input_packet.r_u8(); cntr > 0; --cntr)
	{
		SBooster B;
		load_data(B.fSatietyRestore, input_packet);
		load_data(B.fThirstRestore, input_packet);
		load_data(B.fAlcoholRestore, input_packet);
		load_data(B.fHealthRestore, input_packet);
		load_data(B.fPowerRestore, input_packet);
		load_data(B.fRadiationRestore, input_packet);
		load_data(B.fBleedingRestore, input_packet);
		load_data(B.fMaxWeight, input_packet);
		load_data(B.fBurnImmunity, input_packet);
		load_data(B.fShockImmunity, input_packet);
		load_data(B.fRadiationImmunity, input_packet);
		load_data(B.fTelepaticImmunity, input_packet);
		load_data(B.fChemburnImmunity, input_packet);
		load_data(B.fExplosionImmunity, input_packet);
		load_data(B.fStrikeImmunity, input_packet);
		load_data(B.fFireWoundImmunity, input_packet);
		load_data(B.fWoundImmunity, input_packet);
		load_data(B.fRadiationProtection, input_packet);
		load_data(B.fTelepaticProtection, input_packet);
		load_data(B.fChemburnProtection, input_packet);
		load_data(B.fToxicityRestore, input_packet);
		load_data(B.fSpeedFactor, input_packet);
		load_data(B.fDispFactor, input_packet);

		load_data(B.fBoostTime, input_packet);
		EnableBooster(B);
	}
}

void CActorCondition::reinit	()
{
	inherited::reinit	();
	m_bLimping					= false;
	m_fSatiety					= 1.f;
	m_fThirst					= 1.f;
}

void CActorCondition::EnableBooster(const SBooster& B)
{
	BoostSatietyRestore(B.fSatietyRestore);
	BoostThirstRestore(B.fThirstRestore);
	BoostAlcoholRestore(B.fAlcoholRestore);
	BoostHpRestore(B.fHealthRestore);
	BoostPowerRestore(B.fPowerRestore);
	BoostRadiationRestore(B.fRadiationRestore);
	BoostBleedingRestore(B.fBleedingRestore);
	BoostMaxWeight(B.fMaxWeight);
	BoostBurnImmunity(B.fBurnImmunity);
	BoostShockImmunity(B.fShockImmunity);
	BoostRadiationImmunity(B.fRadiationImmunity);
	BoostTelepaticImmunity(B.fTelepaticImmunity);
	BoostChemicalBurnImmunity(B.fChemburnImmunity);
	BoostExplImmunity(B.fExplosionImmunity);
	BoostStrikeImmunity(B.fStrikeImmunity);
	BoostFireWoundImmunity(B.fFireWoundImmunity);
	BoostWoundImmunity(B.fWoundImmunity);
	BoostRadiationProtection(B.fRadiationProtection);
	BoostTelepaticProtection(B.fTelepaticProtection);
	BoostChemicalBurnProtection(B.fChemburnProtection);
	BoostToxicityRestore(B.fToxicityRestore);
	BoostSpeedFactor(B.fSpeedFactor);
	BoostDispFactor(B.fDispFactor);

	m_BoostersList.push_back(B);
}
void CActorCondition::DisableBooster(const SBooster& B)
{
	BoostSatietyRestore(-B.fSatietyRestore);
	BoostThirstRestore(-B.fThirstRestore);
	BoostAlcoholRestore(-B.fAlcoholRestore);
	BoostHpRestore(-B.fHealthRestore);
	BoostPowerRestore(-B.fPowerRestore);
	BoostRadiationRestore(-B.fRadiationRestore);
	BoostBleedingRestore(-B.fBleedingRestore);
	BoostMaxWeight(-B.fMaxWeight);
	BoostBurnImmunity(-B.fBurnImmunity);
	BoostShockImmunity(-B.fShockImmunity);
	BoostRadiationImmunity(-B.fRadiationImmunity);
	BoostTelepaticImmunity(-B.fTelepaticImmunity);
	BoostChemicalBurnImmunity(-B.fChemburnImmunity);
	BoostExplImmunity(-B.fExplosionImmunity);
	BoostStrikeImmunity(-B.fStrikeImmunity);
	BoostFireWoundImmunity(-B.fFireWoundImmunity);
	BoostWoundImmunity(-B.fWoundImmunity);
	BoostRadiationProtection(-B.fRadiationProtection);
	BoostTelepaticProtection(-B.fTelepaticProtection);
	BoostChemicalBurnProtection(-B.fChemburnProtection);
	BoostToxicityRestore(-B.fToxicityRestore);
	BoostSpeedFactor(-B.fSpeedFactor);
	BoostDispFactor(-B.fDispFactor);
}

void CActorCondition::UpdateTutorialThresholds()
{
	string256						cb_name;
	static float _cPowerThr			= pSettings->r_float("tutorial_conditions_thresholds","power");
	static float _cPowerMaxThr		= pSettings->r_float("tutorial_conditions_thresholds","max_power");
	static float _cBleeding			= pSettings->r_float("tutorial_conditions_thresholds","bleeding");
	static float _cSatiety			= pSettings->r_float("tutorial_conditions_thresholds","satiety");
	static float _cRadiation		= pSettings->r_float("tutorial_conditions_thresholds","radiation");
	static float _cWpnCondition		= pSettings->r_float("tutorial_conditions_thresholds","weapon_jammed");
	static float _cPsyHealthThr		= pSettings->r_float("tutorial_conditions_thresholds","psy_health");

	bool b = true;
	if(b && !m_condition_flags.test(eCriticalPowerReached) && GetPower()<_cPowerThr)
	{
		m_condition_flags.set(eCriticalPowerReached, TRUE);
		b = false;
		xr_strcpy(cb_name,"_G.on_actor_critical_power");
	}

	if(b && !m_condition_flags.test(eCriticalMaxPowerReached) && GetMaxPower()<_cPowerMaxThr)
	{
		m_condition_flags.set(eCriticalMaxPowerReached, TRUE);
		b = false;
		xr_strcpy(cb_name,"_G.on_actor_critical_max_power");
	}

	if(b && !m_condition_flags.test(eCriticalBleedingSpeed) && BleedingSpeed()>_cBleeding)
	{
		m_condition_flags.set(eCriticalBleedingSpeed, TRUE);
		b = false;
		xr_strcpy(cb_name,"_G.on_actor_bleeding");
	}

	if(b && !m_condition_flags.test(eCriticalSatietyReached) && GetSatiety()<_cSatiety)
	{
		m_condition_flags.set(eCriticalSatietyReached, TRUE);
		b = false;
		xr_strcpy(cb_name,"_G.on_actor_satiety");
	}

	if(b && !m_condition_flags.test(eCriticalRadiationReached) && GetRadiation()>_cRadiation)
	{
		m_condition_flags.set(eCriticalRadiationReached, TRUE);
		b = false;
		xr_strcpy(cb_name,"_G.on_actor_radiation");
	}

	if(b && !m_condition_flags.test(ePhyHealthMinReached) && GetPsyHealth()<_cPsyHealthThr)
	{
		m_condition_flags.set(ePhyHealthMinReached, TRUE);
		b = false;
		xr_strcpy(cb_name,"_G.on_actor_psy");
	}

	if(b && m_condition_flags.test(eCantWalkWeight) && !m_condition_flags.test(eCantWalkWeightReached))
	{
		m_condition_flags.set(eCantWalkWeightReached, TRUE);
		b = false;
		xr_strcpy(cb_name,"_G.on_actor_cant_walk_weight");
	}

	if(b && !m_condition_flags.test(eWeaponJammedReached)&&m_object->inventory().GetActiveSlot()!=NO_ACTIVE_SLOT)
	{
		PIItem item	= m_object->inventory().ItemFromSlot(m_object->inventory().GetActiveSlot());
		CWeapon* pWeapon = smart_cast<CWeapon*>(item); 
		if(pWeapon&&pWeapon->GetCondition()<_cWpnCondition)
		{
			m_condition_flags.set(eWeaponJammedReached, TRUE);b=false;
			xr_strcpy(cb_name,"_G.on_actor_weapon_jammed");
		}
	}
	
	if(!b)
	{
		luabind::functor<LPCSTR> fl;
		R_ASSERT(ai().script_engine().functor<LPCSTR>(cb_name,fl));
		fl();
	}
}

bool CActorCondition::DisableSprint(SHit* pHDS)
{
	return	(pHDS->hit_type != ALife::eHitTypeTelepatic)	&& 
			(pHDS->hit_type != ALife::eHitTypeChemicalBurn)	&&
			(pHDS->hit_type != ALife::eHitTypeBurn)			&&
			(pHDS->hit_type != ALife::eHitTypeLightBurn)	&&
			(pHDS->hit_type != ALife::eHitTypeRadiation)	;
}

bool CActorCondition::PlayHitSound(SHit* pHDS)
{
	switch (pHDS->hit_type)
	{
		case ALife::eHitTypeTelepatic:
			return false;
			break;
		case ALife::eHitTypeShock:
		case ALife::eHitTypeStrike:
		case ALife::eHitTypeWound:
		case ALife::eHitTypeExplosion:
		case ALife::eHitTypeFireWound:
		case ALife::eHitTypeWound_2:
			return true;
			break;

		case ALife::eHitTypeRadiation:
		case ALife::eHitTypeBurn:
		case ALife::eHitTypeLightBurn:
		case ALife::eHitTypeChemicalBurn:
			return (pHDS->damage()>0.017f); //field zone threshold
			break;
		default:
			return true;
	}
}

float CActorCondition::HitSlowmo(SHit* pHDS)
{
	float ret;
	if(pHDS->hit_type==ALife::eHitTypeWound || pHDS->hit_type==ALife::eHitTypeStrike )
	{
		ret	= pHDS->damage();
		clamp(ret,0.0f,1.f);
	}
	else ret = 0.0f;

	return ret;	
}

void CActorCondition::ApplyBooster(const CEatableItem& object)
{
	if (m_object->Local() && m_object == Level().CurrentViewEntity())
	{
		if (object.m_sUseSoundName != nullptr)
		{
			if (m_use_sound._feedback())
				m_use_sound.stop();

			m_use_sound.create(object.m_sUseSoundName.c_str(), st_Effect, sg_SourceType);
			m_use_sound.play(NULL, sm_2D);
		}
	}

	//Non-temporary boosters
	ChangeHealth(object.m_fHealth);
	ChangePower(object.m_fPower);
	ChangeSatiety(object.m_fSatiety);
	ChangeThirst(object.m_fThirst);
	ChangeRadiation(object.m_fRadiation);
	ChangeBleeding(object.m_fWoundsHeal);
	SetMaxPower(GetMaxPower() + object.m_fMaxPowerUp);
	ChangeAlcohol(object.m_fAlcohol);
	ChangeToxicity(object.m_fToxicity);

	//Temporary boosters
	EnableBooster(object.m_Boosters);
}

void disable_input();
void enable_input();
void hide_indicators();
void show_indicators();

CActorDeathEffector::CActorDeathEffector(CActorCondition* parent, LPCSTR sect)	// -((
:m_pParent(parent)
{
	Actor()->SetWeaponHideState(INV_STATE_BLOCK_ALL,true);
	hide_indicators();

	AddEffector(Actor(), effActorDeath, sect);
	disable_input();
	LPCSTR snd = pSettings->r_string(sect, "snd");
	m_death_sound.create(snd,st_Effect,0);
	m_death_sound.play_at_pos(0,Fvector().set(0,0,0),sm_2D);

	SBaseEffector* pe = Actor()->Cameras().GetPPEffector((EEffectorPPType)effActorDeath);
	pe->m_on_b_remove_callback = SBaseEffector::CB_ON_B_REMOVE(this, &CActorDeathEffector::OnPPEffectorReleased);
	m_b_actual = true;	
	m_start_health = m_pParent->health();
}

void CActorDeathEffector::UpdateCL()
{
	m_pParent->SetHealth(m_start_health);
}

void CActorDeathEffector::OnPPEffectorReleased()
{
	m_b_actual = false;	
	Msg("111");
	m_pParent->SetHealth(-1.0f);
}

void CActorDeathEffector::Stop()
{
	RemoveEffector(Actor(),effActorDeath);
	m_death_sound.destroy();
	enable_input();
	show_indicators();
}
