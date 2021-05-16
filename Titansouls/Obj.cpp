#include "stdafx.h"
#include "Obj.h"


CObj::CObj()
	: m_bDead( false ), m_pImageKey( nullptr ), m_iDrawID( 0 ), m_eRenderID(RENDERID::END)
{
	ZeroMemory( &m_tInfo, sizeof( m_tInfo ) );
	ZeroMemory( &m_tRect, sizeof( m_tRect ) );
}


CObj::~CObj()
{
}

void CObj::Update_Rect()
{
	m_tRect.left = (LONG)(m_tInfo.fX - (m_tInfo.iCX >> 1));
	m_tRect.top = (LONG)(m_tInfo.fY - (m_tInfo.iCY >> 1));
	m_tRect.right = (LONG)(m_tInfo.fX + (m_tInfo.iCX >> 1));
	m_tRect.bottom = (LONG)(m_tInfo.fY + (m_tInfo.iCY >> 1));
}
