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

CUIBoosterInfo::CUIBoosterInfo()
{
	m_booster_health_restore = xr_new<UIBoosterInfoItem>();
	m_booster_satiety_restore = xr_new<UIBoosterInfoItem>();
	m_booster_power_restore = xr_new<UIBoosterInfoItem>();
	m_booster_radiation_restore = xr_new<UIBoosterInfoItem>();
	m_booster_bleeding_restore = xr_new<UIBoosterInfoItem>();
	m_booster_max_weight = xr_new<UIBoosterInfoItem>();
	m_booster_radiation_protection = xr_new<UIBoosterInfoItem>();
	m_booster_telepatic_protection = xr_new<UIBoosterInfoItem>();
	m_booster_chemburn_protection = xr_new<UIBoosterInfoItem>();
	m_booster_burn_immunity = xr_new<UIBoosterInfoItem>();
	m_booster_shock_immunity = xr_new<UIBoosterInfoItem>();
	m_booster_radiation_immunity = xr_new<UIBoosterInfoItem>();
	m_booster_telepatic_immunity = xr_new<UIBoosterInfoItem>();
	m_booster_chemburn_immunity = xr_new<UIBoosterInfoItem>();

	m_booster_satiety = xr_new<UIBoosterInfoItem>();
	m_booster_anabiotic = xr_new<UIBoosterInfoItem>();

	m_booster_time = xr_new<UIBoosterInfoItem>();

	m_Prop_line = xr_new<CUIStatic>();
}

CUIBoosterInfo::~CUIBoosterInfo()
{
	xr_delete(m_booster_health_restore);
	xr_delete(m_booster_satiety_restore);
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

	AttachChild(m_Prop_line);
	m_Prop_line->SetAutoDelete(false);
	CUIXmlInit::InitStatic(xml, "prop_line", 0, m_Prop_line);

	m_booster_health_restore->InitFromXML(xml, "boost_health_restore", "ui_inv_health");
	xml.SetLocalRoot(base_node);
	m_booster_satiety_restore->InitFromXML(xml, "boost_satiety_restore", "ui_inv_satiety");
	xml.SetLocalRoot(base_node);
	m_booster_power_restore->InitFromXML(xml, "boost_power_restore", "ui_inv_power");
	xml.SetLocalRoot(base_node);
	m_booster_radiation_restore->InitFromXML(xml, "boost_radiation_restore", "ui_inv_radiation");
	xml.SetLocalRoot(base_node);
	m_booster_bleeding_restore->InitFromXML(xml, "boost_bleeding_restore", "ui_inv_bleeding");
	xml.SetLocalRoot(base_node);
	m_booster_max_weight->InitFromXML(xml, "boost_max_weight", "ui_inv_outfit_additional_weight");
	xml.SetLocalRoot(base_node);
	m_booster_radiation_protection->InitFromXML(xml, "boost_radiation_protection", "ui_inv_outfit_radiation_protection");
	xml.SetLocalRoot(base_node);
	m_booster_telepatic_protection->InitFromXML(xml, "boost_telepat_protection", "ui_inv_outfit_telepatic_protection");
	xml.SetLocalRoot(base_node);
	m_booster_chemburn_protection->InitFromXML(xml, "boost_chemburn_protection", "ui_inv_outfit_chemical_burn_protection");
	xml.SetLocalRoot(base_node);
	m_booster_burn_immunity->InitFromXML(xml, "boost_burn_immunity", "ui_inv_outfit_burn_immunity");
	xml.SetLocalRoot(base_node);
	m_booster_shock_immunity->InitFromXML(xml, "boost_shock_immunity", "ui_inv_outfit_shock_immunity");
	xml.SetLocalRoot(base_node);
	m_booster_radiation_immunity->InitFromXML(xml, "boost_radiation_immunity", "ui_inv_outfit_radiation_immunity");
	xml.SetLocalRoot(base_node);
	m_booster_telepatic_immunity->InitFromXML(xml, "boost_telepat_immunity", "ui_inv_outfit_telepatic_immunity");
	xml.SetLocalRoot(base_node);
	m_booster_chemburn_immunity->InitFromXML(xml, "boost_chemburn_immunity", "ui_inv_outfit_chemical_burn_immunity");
	xml.SetLocalRoot(base_node);

	m_booster_satiety->InitFromXML(xml, "boost_satiety", "ui_inv_satiety");
	xml.SetLocalRoot(base_node);
	m_booster_anabiotic->InitFromXML(xml, "boost_anabiotic", "ui_inv_survive_surge");
	xml.SetLocalRoot(base_node);

	m_booster_time->InitFromXML(xml, "boost_time", "ui_inv_effect_time");
	xml.SetLocalRoot(stored_root);
}

void CUIBoosterInfo::UpdateInfo(const CEatableItem& object)
{
	DetachAll();
	AttachChild(m_Prop_line);

	CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if (!actor)
		return;

	CurH = m_Prop_line->GetHeight();

	if (!fis_zero(object.m_Boosters.fHealthRestore))
		SetInfo(m_booster_health_restore, object.m_Boosters.fHealthRestore);
	if (!fis_zero(object.m_Boosters.fSatietyRestore))
		SetInfo(m_booster_satiety_restore, object.m_Boosters.fSatietyRestore);
	if (!fis_zero(object.m_Boosters.fPowerRestore))
		SetInfo(m_booster_power_restore, object.m_Boosters.fPowerRestore);
	if (!fis_zero(object.m_Boosters.fRadiationRestore))
		SetInfo(m_booster_radiation_restore, object.m_Boosters.fRadiationRestore / -1.0f);
	if (!fis_zero(object.m_Boosters.fBleedingRestore))
		SetInfo(m_booster_bleeding_restore, object.m_Boosters.fBleedingRestore);
	if (!fis_zero(object.m_Boosters.fMaxWeight))
		SetInfo(m_booster_max_weight, object.m_Boosters.fMaxWeight);
	if (!fis_zero(object.m_Boosters.fRadiationProtection))
		SetInfo(m_booster_radiation_protection, object.m_Boosters.fRadiationProtection / GetMaxValue(ALife::infl_rad));
	if (!fis_zero(object.m_Boosters.fTelepaticProtection))
		SetInfo(m_booster_telepatic_protection, object.m_Boosters.fTelepaticProtection / GetMaxValue(ALife::infl_psi));
	if (!fis_zero(object.m_Boosters.fChemburnProtection))
		SetInfo(m_booster_chemburn_protection, object.m_Boosters.fChemburnProtection / GetMaxValue(ALife::infl_acid));
	if (!fis_zero(object.m_Boosters.fBurnImmunity))
		SetInfo(m_booster_burn_immunity, object.m_Boosters.fBurnImmunity / GetMaxValue(ALife::infl_fire));
	if (!fis_zero(object.m_Boosters.fShockImmunity))
		SetInfo(m_booster_shock_immunity, object.m_Boosters.fShockImmunity / GetMaxValue(ALife::infl_electra));
	if (!fis_zero(object.m_Boosters.fRadiationImmunity))
		SetInfo(m_booster_radiation_immunity, object.m_Boosters.fRadiationImmunity / GetMaxValue(ALife::infl_rad));
	if (!fis_zero(object.m_Boosters.fTelepaticImmunity))
		SetInfo(m_booster_telepatic_immunity, object.m_Boosters.fTelepaticImmunity / GetMaxValue(ALife::infl_psi));
	if (!fis_zero(object.m_Boosters.fChemburnImmunity))
		SetInfo(m_booster_chemburn_immunity, object.m_Boosters.fChemburnImmunity / GetMaxValue(ALife::infl_acid));
	if (!fis_zero(object.m_fSatiety))
		SetInfo(m_booster_satiety, object.m_fSatiety);

	if (object.object().cNameSect() == "drug_anabiotic")
		SetInfo(m_booster_anabiotic, NULL);

	if (!fis_zero(object.m_Boosters.fBoostTime))
		SetInfo(m_booster_time, object.m_Boosters.fBoostTime);

	SetHeight(CurH);
}

void CUIBoosterInfo::SetInfo(UIBoosterInfoItem*& InfoItem, float value)
{
	if (value != NULL)
		InfoItem->SetValue(value);

	Fvector2 pos = InfoItem->GetWndPos();
	pos.y = CurH;
	InfoItem->SetWndPos(pos);

	CurH += InfoItem->GetHeight();
	AttachChild(InfoItem);
}

UIBoosterInfoItem::UIBoosterInfoItem()
{
	m_caption = nullptr;
	m_value = nullptr;
	m_magnitude = 1.0f;
	m_show_sign = false;

	m_unit_str._set("");
	m_texture_minus._set("");
	m_texture_plus._set("");
}

UIBoosterInfoItem::~UIBoosterInfoItem()
{
}

void UIBoosterInfoItem::InitFromXML(CUIXml& xml, LPCSTR section, LPCSTR name)
{
	CUIXmlInit::InitWindow(xml, section, 0, this);
	xml.SetLocalRoot(xml.NavigateToNode(section));

	m_caption = UIHelper::CreateStatic(xml, "caption", this);
	m_value = UIHelper::CreateTextWnd(xml, "value", this);
	m_magnitude = xml.ReadAttribFlt("value", 0, "magnitude", 1.0f);
	m_show_sign = (xml.ReadAttribInt("value", 0, "show_sign", 1) == 1);

	LPCSTR unit_str = xml.ReadAttrib("value", 0, "unit_str", "");
	m_unit_str._set(CStringTable().translate(unit_str));

	LPCSTR texture_minus = xml.Read("texture_minus", 0, "");
	if (texture_minus && xr_strlen(texture_minus))
	{
		m_texture_minus._set(texture_minus);

		LPCSTR texture_plus = xml.Read("caption:texture", 0, "");
		m_texture_plus._set(texture_plus);
		VERIFY(m_texture_plus.size());
	}

	SetAutoDelete(false);
	m_caption->TextItemControl()->SetText(CStringTable().translate(name).c_str());
}

void UIBoosterInfoItem::SetValue(float value)
{
	value *= m_magnitude;
	string32 buf;
	if (m_show_sign)
		xr_sprintf(buf, "%+.0f", value);
	else
		xr_sprintf(buf, "%.0f", value);

	LPSTR str;
	if (m_unit_str.size())
		STRCONCAT(str, buf, " ", m_unit_str.c_str());
	else
		STRCONCAT(str, buf);

	m_value->SetText(str);

	m_value->SetTextColor(color_rgba(170, 170, 170, 255));

	if (m_texture_minus.size())
	{
		if (value >= 0.0f)
			m_caption->InitTexture(m_texture_plus.c_str());
		else
			m_caption->InitTexture(m_texture_minus.c_str());
	}
}
