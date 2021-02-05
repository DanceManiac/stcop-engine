#pragma once

class CBlender_ao : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "FFT: AO renderphase"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_ao();
	virtual ~CBlender_ao();
};
