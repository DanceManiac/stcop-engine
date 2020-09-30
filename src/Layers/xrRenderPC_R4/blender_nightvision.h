#pragma once

class CBlender_nightvision : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "FFT: Nightvision renderphase"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_nightvision();
	virtual ~CBlender_nightvision();
};
