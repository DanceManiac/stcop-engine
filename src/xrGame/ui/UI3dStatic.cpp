// UI3dStatic.cpp: класс статического элемента, который рендерит 
// 3d объект в себя
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ui3dstatic.h"
#include "../gameobject.h"
#include "../HUDManager.h"
#include "../Include/xrRender/RenderVisual.h"
#include "xrEngine/CustomHUD.h"
#include "UIGameCustom.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUI3dStatic:: CUI3dStatic()
{
	m_x_angle = m_y_angle = m_z_angle = 0;
	mesh = NULL;
	Enable(false);
}

 CUI3dStatic::~ CUI3dStatic()
{

}


//расстояние от камеры до вещи, перед глазами
#define DIST  (VIEWPORT_NEAR + 0.1f)


void CUI3dStatic::FromScreenToItem(int x_screen, int y_screen, 
								   float& x_item, float& y_item)
{
		int x = x_screen;
		int y = y_screen;

        int halfwidth  = Device.dwWidth/2;
        int halfheight = Device.dwHeight/2;

	    float size_y = VIEWPORT_NEAR * tanf( deg2rad(Device.fFOV) * 0.5f);
        float size_x = size_y / (Device.fASPECT);

        float r_pt      = float(x-halfwidth) * size_x / (float) halfwidth;
        float u_pt      = float(halfheight-y) * size_y / (float) halfheight;

		x_item  = r_pt * DIST / VIEWPORT_NEAR;
		y_item = u_pt * DIST / VIEWPORT_NEAR;
}

void MouseRayFromPoint(Fvector& start, Fvector& direction, const Ivector2& point)
{
	int halfwidth = Device.dwWidth * 0.5f;
	int halfheight = Device.dwHeight * 0.5f;

	if (!halfwidth || !halfheight) return;

	Ivector2 point2;
	point2.set(point.x - halfwidth, halfheight - point.y);

	start.set(Device.vCameraPosition);

	float size_y = VIEWPORT_NEAR * tan(deg2rad(Device.fFOV) * 0.5f);
	float size_x = size_y / Device.fASPECT;

	float r_pt = float(point2.x) * size_x / (float)halfwidth;
	float u_pt = float(point2.y) * size_y / (float)halfheight;

	direction.mul(Device.mView.k, VIEWPORT_NEAR);
	direction.mad(direction, Device.mView.j, u_pt);
	direction.mad(direction, Device.mView.i, r_pt);
	direction.normalize();
}

//прорисовка
void  CUI3dStatic::Draw()
{
	if(m_pCurrentItem)
	{
		//SetHeight(Device.dwHeight);
		//SetWidth(Device.dwWidth);

		//SetWndPos(Fvector2().set(0.0f, 0.0f));

		Frect rect;
		//GetAbsoluteRect(rect);
		HUD().GetGameUI()->GetWindow().GetAbsoluteRect(rect);
		// Apply scale
		//rect.top	= static_cast<int>(rect.top * GetScaleY());
		rect.top	= static_cast<int>(rect.top);
		//rect.left	= static_cast<int>(rect.left * GetScaleX());
		rect.left	= static_cast<int>(rect.left);
		//rect.bottom	= static_cast<int>(rect.bottom * GetScaleY());
		rect.bottom	= static_cast<int>(rect.bottom);
		//rect.right	= static_cast<int>(rect.right * GetScaleX());
		rect.right	= static_cast<int>(rect.right);

		Fmatrix translate_matrix;
		Fmatrix scale_matrix;
		
		Fmatrix rx_m; 
		Fmatrix ry_m; 
		Fmatrix rz_m; 

		Fmatrix matrix;
		matrix.identity();

		//поместить объект в центр сферы
		translate_matrix.identity();
		translate_matrix.translate( - m_pCurrentItem->Visual()->getVisData().sphere.P.x,
			 					    - m_pCurrentItem->Visual()->getVisData().sphere.P.y,
								    - m_pCurrentItem->Visual()->getVisData().sphere.P.z);

		matrix.mulA_44(translate_matrix);


		rx_m.identity();
		rx_m.rotateX(0.0f);
		ry_m.identity();
		ry_m.rotateY(11.0f);
		rz_m.identity();
		rz_m.rotateZ(0.0f);


		matrix.mulA_44(rx_m);
		matrix.mulA_44(ry_m);
		matrix.mulA_44(rz_m);
		

		
		float x1, y1, x2, y2;

		FromScreenToItem(rect.left, rect.top, x1, y1);
		FromScreenToItem(rect.right, rect.bottom, x2, y2);

		float normal_size;
		normal_size =_abs(x2-x1)<_abs(y2-y1)?_abs(x2-x1):_abs(y2-y1);
		
				
		float radius = m_pCurrentItem->Visual()->getVisData().sphere.R;

		float scale = normal_size/(radius*2);

		scale_matrix.identity();
		scale_matrix.scale( scale, scale,scale);

		matrix.mulA_44(scale_matrix);
        

		float right_item_offset, up_item_offset;

		
		///////////////////////////////	
		
		/*FromScreenToItem(rect.left + iFloor(GetWidth()/2 * GetScaleX()),
						 rect.top + iFloor(GetHeight()/2 * GetScaleY()), 
						 right_item_offset, up_item_offset);*/
		FromScreenToItem(rect.left,
						 rect.top, 
						 right_item_offset, up_item_offset);

		translate_matrix.identity();
		translate_matrix.translate(right_item_offset,
								   up_item_offset,
								   DIST);

		matrix.mulA_44(translate_matrix);

		Fmatrix camera_matrix;
		camera_matrix.identity();
		camera_matrix = Device.mView;
		camera_matrix.invert();



		matrix.mulA_44(camera_matrix);


		Fmatrix	M = Fidentity;
		Fmatrix	S;
		S.scale(0.04f, 0.04f, 0.04f);
		M.mulB_44(S);

		Fvector start, dir;
		Ivector2 pt;

		static int _wh = 50;
		static float _kl = 1.0f;

		pt.x = _wh;
		pt.y = iFloor(Device.dwHeight - _wh);

		MouseRayFromPoint(M.c, dir, pt);
		M.c.mad(dir, _kl);

		M.mulA_44(camera_matrix);

		::Render->set_Object(NULL); 
		::Render->set_Transform(&M);
		::Render->add_3d_static(m_pCurrentItem->Visual());
	}
}

void CUI3dStatic::SetGameObject(CGameObject* pItem)
{
	m_pCurrentItem = pItem;
	Enable(true);
}