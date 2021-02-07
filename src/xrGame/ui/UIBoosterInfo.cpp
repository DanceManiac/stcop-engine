#include "stdafx.h"
#include "UIBoosterInfo.h"
#include "UIStatic.h"
#include "object_broker.h"
#include "../EntityCondition.h"
#include "..\actor.h"
#include "../ActorCondition.h"
#include "UIXmlInit.h"
#include "UIHelper.h"
#include "../string_table.h"
#include "../eatable_item.h"

CUIBoosterInfo::CUIBoosterInfo()
{
	m_booster_health_restore = NULL;
	m_booster_power_restore = NULL;
	m_booster_radiation_restore = NULL;
	m_booster_bleeding_restore = NULL;
	m_booster_max_weight = NULL;
	m_booster_radiation_protection = NULL;
	m_booster_telepatic_protection = NULL;
	m_booster_chemburn_protection = NULL;
	m_booster_burn_immunity = NULL;
	m_booster_shock_immunity = NULL;
	m_booster_radiation_immunity = NULL;
	m_booster_telepatic_immunity = NULL;
	m_booster_chemburn_immunity = NULL;

	m_booster_satiety = NULL;
	m_booster_anabiotic = NULL;
	m_booster_time = NULL;
}

CUIBoosterInfo::~CUIBoosterInfo()
{
	xr_delete(m_booster_health_restore);
	xr_delete(m_booster_power_restore);
	xr_delete(m_booster_radiation_restore);
	xr_delete(m_booster_bleeding_restore);
	xr_delete(m_booster_max_weight);
	xr_delete(m_booster_radiation_protection);
	xr_delete(m_booster_telepatic_protection);
	xr_delete(m_booster_chemburn_protection);
	xr_delete(m_booster_burn_immunity);
	xr_delete(m_booster_shock_immunity);
	xr_delete(m_booster_radiation_immunity);
	xr_delete(m_booster_telepatic_immunity);
	xr_delete(m_booster_chemburn_immunity);

	xr_delete(m_booster_satiety);
	xr_delete(m_booster_anabiotic);
	xr_delete(m_booster_time);
	xr_delete(m_Prop_line);
}

void CUIBoosterInfo::InitFromXml(CUIXml& xml)
{
	LPCSTR base = "booster_params";
	XML_NODE* stored_root = xml.GetLocalRoot();
	XML_NODE* base_node = xml.NavigateToNode(base, 0);
	if (!base_node)
		return;

	CUIXmlInit::InitWindow(xml, base, 0, this);
	xml.SetLocalRoot(base_node);

	m_Prop_line = xr_new<CUIStatic>();
	AttachChild(m_Prop_line);
	m_Prop_line->SetAutoDelete(false);
	CUIXmlInit::InitStatic(xml, "prop_line", 0, m_Prop_line);

	InitInfoItemXml(xml, m_booster_health_restore, "boost_health_restore", "ui_inv_health");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_power_restore, "boost_power_restore", "ui_inv_power");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_radiation_restore, "boost_radiation_restore", "ui_inv_radiation");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_bleeding_restore, "boost_bleeding_restore", "ui_inv_bleeding");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_max_weight, "boost_max_weight", "ui_inv_outfit_additional_weight");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_radiation_protection, "boost_radiation_protection", "ui_inv_outfit_radiation_protection");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_telepatic_protection, "boost_telepat_protection", "ui_inv_outfit_telepatic_protection");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_chemburn_protection, "boost_chemburn_protection", "ui_inv_outfit_chemical_burn_protection");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_burn_immunity, "boost_burn_immunity", "ui_inv_outfit_burn_immunity");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_shock_immunity, "boost_shock_immunity", "ui_inv_outfit_shock_immunity");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_radiation_immunity, "boost_radiation_immunity", "ui_inv_outfit_radiation_immunity");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_telepatic_immunity, "boost_telepat_immunity", "ui_inv_outfit_telepatic_immunity");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_chemburn_immunity, "boost_chemburn_immunity", "ui_inv_outfit_chemical_burn_immunity");
	xml.SetLocalRoot(base_node);

	InitInfoItemXml(xml, m_booster_satiety, "boost_satiety", "ui_inv_satiety");
	xml.SetLocalRoot(base_node);
	InitInfoItemXml(xml, m_booster_anabiotic, "boost_anabiotic", "ui_inv_survive_surge");
	xml.SetLocalRoot(base_node);

	m_booster_time = xr_new<UIBoosterInfoItem>();
	m_booster_time->Init(xml, "boost_time");
	m_booster_time->SetAutoDelete(false);
	LPCSTR name = CStringTable().translate("ui_inv_effect_time").c_str();
	m_booster_time->SetCaption(name);

	xml.SetLocalRoot(stored_root);
}
void CUIBoosterInfo::InitInfoItemXml(CUIXml& xml, UIBoosterInfoItem*& InfoItem, LPCSTR section, LPCSTR name)
{
	InfoItem = xr_new<UIBoosterInfoItem>();
	InfoItem->Init(xml, section);
	InfoItem->SetAutoDelete(false);
	InfoItem->SetCaption(CStringTable().translate(name).c_str());
};

void CUIBoosterInfo::UpdateInfo(const CEatableItem& object)
{
	DetachAll();
	AttachChild( m_Prop_line );

	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor )
		return;

	float val = 0.0f, max_val = 1.0f;
	float cur_h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;

	if (!fis_zero(object.m_Boosters.fHealthRestore))
		SetInfo(m_booster_health_restore, object.m_Boosters.fHealthRestore, cur_h);
	if (!fis_zero(object.m_Boosters.fPowerRestore))
		SetInfo(m_booster_power_restore, object.m_Boosters.fPowerRestore, cur_h);
	if (!fis_zero(object.m_Boosters.fRadiationRestore))
		SetInfo(m_booster_radiation_restore, object.m_Boosters.fRadiationRestore / -1.0f, cur_h);
	if (!fis_zero(object.m_Boosters.fBleedingRestore))
		SetInfo(m_booster_bleeding_restore, object.m_Boosters.fBleedingRestore, cur_h);
	if (!fis_zero(object.m_Boosters.fMaxWeight))
		SetInfo(m_booster_max_weight, object.m_Boosters.fMaxWeight, cur_h);
	if (!fis_zero(object.m_Boosters.fRadiationProtection))
		SetInfo(m_booster_radiation_protection, object.m_Boosters.fRadiationProtection / GetMaxValue(ALife::infl_rad), cur_h);
	if (!fis_zero(object.m_Boosters.fTelepaticProtection))
		SetInfo(m_booster_telepatic_protection, object.m_Boosters.fTelepaticProtection / GetMaxValue(ALife::infl_psi), cur_h);
	if (!fis_zero(object.m_Boosters.fChemburnProtection))
		SetInfo(m_booster_chemburn_protection, object.m_Boosters.fChemburnProtection / GetMaxValue(ALife::infl_acid), cur_h);
	if (!fis_zero(object.m_Boosters.fBurnImmunity))
		SetInfo(m_booster_burn_immunity, object.m_Boosters.fBurnImmunity / GetMaxValue(ALife::infl_fire), cur_h);
	if (!fis_zero(object.m_Boosters.fShockImmunity))
		SetInfo(m_booster_shock_immunity, object.m_Boosters.fShockImmunity / GetMaxValue(ALife::infl_electra), cur_h);
	if (!fis_zero(object.m_Boosters.fRadiationImmunity))
		SetInfo(m_booster_radiation_immunity, object.m_Boosters.fRadiationImmunity / GetMaxValue(ALife::infl_rad), cur_h);
	if (!fis_zero(object.m_Boosters.fTelepaticImmunity))
		SetInfo(m_booster_telepatic_immunity, object.m_Boosters.fTelepaticImmunity / GetMaxValue(ALife::infl_psi), cur_h);
	if (!fis_zero(object.m_Boosters.fChemburnImmunity))
		SetInfo(m_booster_chemburn_immunity, object.m_Boosters.fChemburnImmunity / GetMaxValue(ALife::infl_acid), cur_h);
	if (!fis_zero(object.m_fSatiety))
		SetInfo(m_booster_satiety, object.m_fSatiety, cur_h);

	if (object.object().cNameSect() == "drug_anabiotic")
	{	
		Fvector2 pos;
		pos.set(m_booster_anabiotic->GetWndPos());
		pos.y = cur_h;
		m_booster_anabiotic->SetWndPos(pos);

		cur_h += m_booster_anabiotic->GetWndSize().y;
		AttachChild(m_booster_anabiotic);
	}

	if (!fis_zero(object.m_Boosters.fBoostTime))
		SetInfo(m_booster_time, object.m_Boosters.fBoostTime, cur_h);

	SetHeight(cur_h);
}
void CUIBoosterInfo::SetInfo(UIBoosterInfoItem*& InfoItem, float value, float& cur_h)
{
	Fvector2 pos;
	InfoItem->SetValue(value);
	pos.set(InfoItem->GetWndPos());
	pos.y = cur_h;
	InfoItem->SetWndPos(pos);

	cur_h += InfoItem->GetWndSize().y;
	AttachChild(InfoItem);
};

/// ----------------------------------------------------------------

UIBoosterInfoItem::UIBoosterInfoItem()
{
	m_caption				= NULL;
	m_value					= NULL;
	m_magnitude				= 1.0f;
	m_show_sign				= false;
	
	m_unit_str._set			("");
	m_texture_minus._set	("");
	m_texture_plus._set		("");
}
UIBoosterInfoItem::~UIBoosterInfoItem()
{
}

void UIBoosterInfoItem::Init(CUIXml& xml, LPCSTR section)
{
	CUIXmlInit::InitWindow(xml, section, 0, this);
	xml.SetLocalRoot(xml.NavigateToNode(section));

	m_caption   = UIHelper::CreateStatic(xml, "caption", this);
	m_value     = UIHelper::CreateTextWnd(xml, "value",   this);
	m_magnitude = xml.ReadAttribFlt("value", 0, "magnitude", 1.0f);
	m_show_sign = (xml.ReadAttribInt("value", 0, "show_sign", 1) == 1);
	
	LPCSTR unit_str = xml.ReadAttrib("value", 0, "unit_str", "");
	m_unit_str._set(CStringTable().translate(unit_str));
	
	LPCSTR texture_minus = xml.Read("texture_minus", 0, "");
	if(texture_minus && xr_strlen(texture_minus))
	{
		m_texture_minus._set(texture_minus);
		
		LPCSTR texture_plus = xml.Read("caption:texture", 0, "");
		m_texture_plus._set(texture_plus);
		VERIFY(m_texture_plus.size());
	}
}

void UIBoosterInfoItem::SetCaption(LPCSTR name)
{
	m_caption->TextItemControl()->SetText(name);
}

void UIBoosterInfoItem::SetValue(float value)
{
	value *= m_magnitude;
	string32 buf;
	if(m_show_sign)
		xr_sprintf(buf, "%+.0f", value);
	else
		xr_sprintf(buf, "%.0f", value);
	
	LPSTR str;
	if(m_unit_str.size())
		STRCONCAT(str, buf, " ", m_unit_str.c_str());
	else
		STRCONCAT(str, buf);

	m_value->SetText(str);

	bool positive = (value >= 0.0f);
	m_value->SetTextColor(color_rgba(170,170,170,255));

	if(m_texture_minus.size())
	{
		if(positive)
			m_caption->InitTexture(m_texture_plus.c_str());
		else
			m_caption->InitTexture(m_texture_minus.c_str());
	}
}
