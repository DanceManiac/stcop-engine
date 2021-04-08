#pragma once
#include "UIWindow.h"
#include "../EntityCondition.h"
#include "../ActorCondition.h"
#include "Actor.h"

class CUIXml;
class CUIStatic;
class CUITextWnd;
class UIBoosterInfoItem;

class CUIBoosterInfo : public CUIWindow
{
public:
					CUIBoosterInfo		();
	virtual			~CUIBoosterInfo		();
			void	InitFromXml			(CUIXml& xml);
			void	InitInfoItemXml		(CUIXml& xml, UIBoosterInfoItem*& InfoItem, LPCSTR section, LPCSTR name);
			void	UpdateInfo			(const CEatableItem& object);
			void	SetInfo				(UIBoosterInfoItem*& InfoItem, float value, float& cur_h);
			float	GetMaxValue(ALife::EInfluenceType type)
			{
				CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
				if (actor)
					return actor->conditions().GetZoneMaxPower(type);
			};

protected:
	UIBoosterInfoItem* m_booster_health_restore;
	UIBoosterInfoItem* m_booster_power_restore;
	UIBoosterInfoItem* m_booster_radiation_restore;
	UIBoosterInfoItem* m_booster_bleeding_restore;
	UIBoosterInfoItem* m_booster_max_weight;
	UIBoosterInfoItem* m_booster_speed_factor;
	UIBoosterInfoItem* m_booster_radiation_protection;
	UIBoosterInfoItem* m_booster_telepatic_protection;
	UIBoosterInfoItem* m_booster_chemburn_protection;
	UIBoosterInfoItem* m_booster_burn_immunity;
	UIBoosterInfoItem* m_booster_shock_immunity;
	UIBoosterInfoItem* m_booster_radiation_immunity;
	UIBoosterInfoItem* m_booster_telepatic_immunity;
	UIBoosterInfoItem* m_booster_chemburn_immunity;

	UIBoosterInfoItem* m_booster_satiety_restore;
	UIBoosterInfoItem* m_booster_thirst_restore;
	UIBoosterInfoItem* m_booster_toxicity_restore;

	UIBoosterInfoItem*	m_booster_satiety;
	UIBoosterInfoItem*  m_booster_thirst;
	UIBoosterInfoItem*  m_booster_toxicity;
	UIBoosterInfoItem*	m_booster_anabiotic;
	UIBoosterInfoItem*	m_booster_time;

	CUIStatic*			m_Prop_line;

}; // class CUIBoosterInfo

// -----------------------------------

class UIBoosterInfoItem : public CUIWindow
{
public:
				UIBoosterInfoItem	();
	virtual		~UIBoosterInfoItem();
		
		void	Init				( CUIXml& xml, LPCSTR section );
		void	SetCaption			( LPCSTR name );
		void	SetValue			( float value );
	
private:
	CUIStatic*	m_caption;
	CUITextWnd*	m_value;
	float		m_magnitude;
	bool		m_show_sign;
	shared_str	m_unit_str;
	shared_str	m_texture_minus;
	shared_str	m_texture_plus;

}; // class UIBoosterInfoItem
