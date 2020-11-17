#include "stdafx.h"
#include "Weapon.h"
#include "level.h"
#include "entity_alive.h"
#include "actor.h"
#include "player_hud.h"
#include "inventory.h"
#include "ui/UIWindow.h"
#include "weaponBinocularsVision.h"

void CWeapon::UpdateXForm()
{
	if (Device.dwFrame == dwXF_Frame)
		return;

	dwXF_Frame = Device.dwFrame;

	if (!H_Parent())
		return;

	// Get access to entity and its visual
	CEntityAlive* E = smart_cast<CEntityAlive*>(H_Parent());

	if (!E) {
		if (!IsGameTypeSingle())
			UpdatePosition(H_Parent()->XFORM());

		return;
	}

	const CInventoryOwner* parent = smart_cast<const CInventoryOwner*>(E);
	if (parent && parent->use_simplified_visual())
		return;

	if (parent->attached(this))
		return;

	IKinematics* V = smart_cast<IKinematics*>	(E->Visual());
	VERIFY(V);

	// Get matrices
	int						boneL = -1, boneR = -1, boneR2 = -1;

	// this ugly case is possible in case of a CustomMonster, not a Stalker, nor an Actor
	E->g_WeaponBones(boneL, boneR, boneR2);

	if (boneR == -1)		return;

	if ((HandDependence() == hd1Hand) || (GetState() == eReload) || (!E->g_Alive()))
		boneL = boneR2;

	V->CalculateBones_Invalidate();
	V->CalculateBones(TRUE);
	Fmatrix& mL = V->LL_GetTransform(u16(boneL));
	Fmatrix& mR = V->LL_GetTransform(u16(boneR));
	// Calculate
	Fmatrix					mRes;
	Fvector					R, D, N;
	D.sub(mL.c, mR.c);

	if (fis_zero(D.magnitude())) {
		mRes.set(E->XFORM());
		mRes.c.set(mR.c);
	}
	else {
		D.normalize();
		R.crossproduct(mR.j, D);

		N.crossproduct(D, R);
		N.normalize();

		mRes.set(R, N, D, mR.c);
		mRes.mulA_43(E->XFORM());
	}

	UpdatePosition(mRes);
}

void CWeapon::UpdateFireDependencies_internal()
{
	if (Device.dwFrame != dwFP_Frame)
	{
		dwFP_Frame = Device.dwFrame;

		UpdateXForm();

		if (GetHUDmode())
		{
			HudItemData()->setup_firedeps(m_current_firedeps);
			VERIFY(_valid(m_current_firedeps.m_FireParticlesXForm));
		}
		else
		{
			// 3rd person or no parent
			Fmatrix& parent = XFORM();
			Fvector& fp = vLoadedFirePoint;
			Fvector& fp2 = vLoadedFirePoint2;
			Fvector& sp = vLoadedShellPoint;

			parent.transform_tiny(m_current_firedeps.vLastFP, fp);
			parent.transform_tiny(m_current_firedeps.vLastFP2, fp2);
			parent.transform_tiny(m_current_firedeps.vLastSP, sp);

			m_current_firedeps.vLastFD.set(0.f, 0.f, 1.f);
			parent.transform_dir(m_current_firedeps.vLastFD);

			m_current_firedeps.m_FireParticlesXForm.set(parent);
			VERIFY(_valid(m_current_firedeps.m_FireParticlesXForm));
		}
	}
}

void CWeapon::ForceUpdateFireParticles()
{
	if (!GetHUDmode())
	{//update particlesXFORM real bullet direction

		if (!H_Parent())		return;

		Fvector					p, d;
		smart_cast<CEntity*>(H_Parent())->g_fireParams(this, p, d);

		Fmatrix						_pxf;
		_pxf.k = d;
		_pxf.i.crossproduct(Fvector().set(0.0f, 1.0f, 0.0f), _pxf.k);
		_pxf.j.crossproduct(_pxf.k, _pxf.i);
		_pxf.c = XFORM().c;

		m_current_firedeps.m_FireParticlesXForm.set(_pxf);
	}
}

bool CWeapon::need_renderable()
{
	return Render->currentViewPort == MAIN_VIEWPORT && !(IsZoomed() && ZoomTexture() && !IsRotatingToZoom());
}

void CWeapon::UpdateAddonsTransform(bool for_hud)
{
	for (auto& mesh : m_attaches)
	{
		if(for_hud)
			mesh->UpdateRenderPos(HudItemData()->m_model->dcast_RenderVisual(), for_hud, HudItemData()->m_item_transform);
		else
			mesh->UpdateRenderPos(Visual(), for_hud, XFORM());
	}
}

void CWeapon::renderable_Render()
{
	UpdateXForm();

	//нарисовать подсветку

	for(auto mesh : m_attaches)
		mesh->Render(false);

	RenderLight();

	//если мы в режиме снайперки, то сам HUD рисовать не надо
	if (IsZoomed() && !IsRotatingToZoom() && ZoomTexture())
		RenderHud(FALSE);
	else
		RenderHud(TRUE);

	inherited::renderable_Render();
}

void CWeapon::UpdatePosition(const Fmatrix& trans)
{
	Position().set(trans.c);
	XFORM().mul(trans, m_strapped_mode ? m_StrapOffset : m_Offset);
	VERIFY(!fis_zero(DET(renderable.xform)));
}

void CWeapon::SaveAttachableParams()
{
	if (!m_dbgItem)	return;

	LPCSTR sect_name = m_dbgItem->item().m_section_id.c_str();
	string_path fname;
	FS.update_path(fname, "$game_data$", make_string("_world\\%s.ltx", sect_name).c_str());

	CInifile* pHudCfg = new CInifile(fname, FALSE, FALSE, TRUE);

	pHudCfg->w_string(sect_name, "position", make_string("%f,%f,%f", m_Offset.c.x, m_Offset.c.y, m_Offset.c.z).c_str());
	Fvector ypr;
	m_Offset.getHPB(ypr.x, ypr.y, ypr.z);
	ypr.mul(180.f / PI);
	pHudCfg->w_string(sect_name, "orientation", make_string("%f,%f,%f", ypr.x, ypr.y, ypr.z).c_str());

	if (pSettings->line_exist(sect_name, "strap_position") && pSettings->line_exist(sect_name, "strap_orientation"))
	{
		pHudCfg->w_string(sect_name, "strap_position", make_string("%f,%f,%f", m_StrapOffset.c.x, m_StrapOffset.c.y, m_StrapOffset.c.z).c_str());
		m_StrapOffset.getHPB(ypr.x, ypr.y, ypr.z);
		ypr.mul(180.f / PI);
		pHudCfg->w_string(sect_name, "strap_orientation", make_string("%f,%f,%f", ypr.x, ypr.y, ypr.z).c_str());
	}

	xr_delete(pHudCfg);
	Msg("data saved to %s", fname);
	Sleep(250);

}
void CWeapon::ParseCurrentItem(CGameFont* F)
{
	F->OutNext("WEAPON IN STRAPPED MOD [%d]", m_strapped_mode);
}

bool CWeapon::render_item_ui_query()
{
	bool b_is_active_item = (m_pInventory->ActiveItem() == this);
	bool res = b_is_active_item && IsZoomed() && ZoomHideCrosshair() && ZoomTexture() && !IsRotatingToZoom();
	return res;
}

void CWeapon::render_item_ui()
{
	if (m_zoom_params.m_pVision)
		m_zoom_params.m_pVision->Draw();

	ZoomTexture()->Update();
	ZoomTexture()->Draw();
}

void CWeapon::render_hud_mode()
{
	for(auto mesh : m_attaches)
		mesh->Render(true);

	RenderLight();
}

shared_str wpn_scope = "wpn_scope";
shared_str wpn_silencer = "wpn_silencer";
shared_str wpn_grenade_launcher = "wpn_launcher";

void CWeapon::UpdateAddonsVisibility()
{
	IKinematics* pWeaponVisual = smart_cast<IKinematics*>(Visual()); R_ASSERT(pWeaponVisual);

	UpdateHUDAddonsVisibility();

	pWeaponVisual->CalculateBones_Invalidate();
	pWeaponVisual->CalculateBones(TRUE);
}

void CWeapon::UpdateHUDAddonsVisibility()
{
	if (!GetHUDmode()) return;
}

VisualAddonHelper::VisualAddonHelper()
{
	hud_model = NULL;
	world_model = NULL;
	m_boneName = NULL;
	m_sectionId = NULL;
	m_meshName = NULL;
	m_meshHUDName = NULL;
	m_renderPos.identity();
	isRoot = false;
}

void VisualAddonHelper::UpdateRenderPos(IRenderVisual* model, bool hud, Fmatrix parent)
{
	if(!model) return;

	u16 bone_id = model->dcast_PKinematics()->LL_BoneID(m_boneName);
	Fmatrix bone_trans = model->dcast_PKinematics()->LL_GetTransform(bone_id);
	m_renderPos.identity();
	m_renderPos.mul(parent, bone_trans);
}

void VisualAddonHelper::PrepareRender(bool hud)
{
	if(hud && !hud_model)
		hud_model = ::Render->model_Create(m_meshHUDName.c_str());
	else if (!hud && !world_model)
		world_model = ::Render->model_Create(m_meshName.c_str());
	for (auto& child : m_childs)
		child->PrepareRender(hud);
}

void VisualAddonHelper::Render(bool hud)
{
	if((hud && !hud_model) || (!hud && !world_model))
		PrepareRender(hud);
		
	::Render->set_Transform(&m_renderPos);
	::Render->add_Visual(hud? hud_model: world_model);

	for (auto& child : m_childs)
		child->UpdateRenderPos(hud ? hud_model : world_model, hud, m_renderPos);//child->UpdateRenderPos(hud ? hud_model : world_model, hud);

	for (auto& child : m_childs)
		child->Render(hud);
}

void VisualAddonHelper::Load(shared_str &section)
{
	m_sectionId		= section;
	m_boneName		= READ_IF_EXISTS(pSettings,r_string, section,"bone_slot","wpn_body");
	m_sectionParent = READ_IF_EXISTS(pSettings,r_string, section,"mesh_require",NULL);
	isRoot			= READ_IF_EXISTS(pSettings, r_bool, section, "is_root", false);
	m_meshName		= pSettings->r_string(section,"visual");
	m_meshHUDName	= pSettings->r_string(section,"hud_visual");
}

VisualAddonHelper* VisualAddonHelper::create_and_attach_to_parent(shared_str sect, xr_vector<VisualAddonHelper*>& m_attaches)
{
	VisualAddonHelper* addon = xr_new<VisualAddonHelper>();
	addon->Load(sect);

	if (addon->m_sectionParent != NULL)
	{
		if (!FindParentAndAttach(addon->m_sectionParent, m_attaches))
		{
			VisualAddonHelper* parent = create_and_attach_to_parent(addon->m_sectionParent, m_attaches);
			if (parent)
				parent->m_childs.push_back(addon);
		}
	}
	else
	{
		m_attaches.push_back(addon);
	}

	return addon;
}

bool VisualAddonHelper::FindParentAndAttach(shared_str section, xr_vector<VisualAddonHelper*>& m_attaches)
{
	for (auto& it : m_attaches)
	{
		if (it->m_sectionId == section)
		{
			it->m_childs.push_back(this);
			return true;
		}
		else
		{
			return FindParentAndAttach(section, it->m_childs);
		}
	}

	return false;
}