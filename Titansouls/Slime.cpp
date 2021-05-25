#include "stdafx.h"
#include "Slime.h"
#include "BmpMgr.h"
#include "ScrollMgr.h"
#include "SlimeShadow.h"
#include "SludgeHeart.h"
#include "ObjMgr.h"
#include "Player.h"

const int CSlime::m_iSizeStartX[5] = { 0, 10, 15, 18, 20 };
const int CSlime::m_iSize[5] = { 8, 5, 3, 2, 1 };
const float CSlime::m_fMozziMinX = 0.6f;
const float CSlime::m_fMozziMinY = 0.5f;
CSlime::CSlime()
	:m_pShadow(nullptr), m_iSizeLv(0), m_bHaveHeart(false), m_fMozziY(false), m_fMozziX(false)
	, m_fTargetX(0), m_fTargetY(0)
{
}

CSlime::~CSlime()
{
	Release();
}

void CSlime::Initialize()
{
	m_iMaxCX = m_iSize[m_iSizeLv] * DEFAULTCX;
	m_iMaxCY = m_iSize[m_iSizeLv] * DEFAULTCY;
	m_tInfo.iCX = m_iMaxCX;
	m_tInfo.iCY = m_iMaxCY;
	Update_Rect();
	m_pImageKey = L"Sludge";
	m_eRenderID = RENDERID::OBJECT;
	m_fPosZ = 30.f;
	m_iPatternFrame = 20;

	m_pShadow = static_cast<CSlimeShadow*>(CAbstractFactory<CSlimeShadow>::Create( m_tInfo.fX, m_tInfo.fY ));
	m_pShadow->Set_Slime( this );
	CObjMgr::Get_Instance()->Add_Object( m_pShadow, OBJID::SHADOW );
}

int CSlime::Update()
{
	m_pShadow->Set_SludgeHeart( m_pSludeHeart );
	if ( m_bDestroyed )
		return OBJ_DESTROYED;

	++m_iPatternFrame;

	//���� ����
	
	int iCurPatternFrame = m_iPatternFrame % 180;
	if ( iCurPatternFrame < 50 )
		m_fPosZ = DEFAULTCY * 3 * sin( iCurPatternFrame * PI / 50 );
	else
		m_fPosZ = 0.f;

	if ( m_bHaveHeart )
		m_pSludeHeart->Set_PosZ( m_fPosZ );

	if(m_bHaveHeart )
		Update_Heart();
	Update_Shadow();
	Update_Mozzi();

	if ( !m_bDead )
	{
		Update_Pattern();
		Update_DamageCollision();
	}
	else
	{
		Update_Dead();
	}
	Update_Rect();


	return OBJ_NOEVENT;
}

void CSlime::Late_Update()
{
}

void CSlime::Render( HDC _DC )
{
	int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
	int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Bmp( m_pImageKey );


	Update_Rect();

	GdiTransparentBlt( _DC
					   , m_tRect.left + iScrollX, (int)(m_tRect.top + iScrollY - (m_tInfo.iCY >> 1) - m_fPosZ)
					   , m_tInfo.iCX, m_tInfo.iCY
					   , hMemDC
					   , m_iSizeStartX[m_iSizeLv] * PIXELCX, 0 
					   , PIXELCX * m_iSize[m_iSizeLv], PIXELCY * m_iSize[m_iSizeLv]
					   , RGB( 255, 0, 255 ) );
	RenderCollision(_DC);
}

void CSlime::Release()
{
}

void CSlime::Set_Active()
{
	m_bActive = true;
	m_pShadow->Set_Active();
}

void CSlime::Reset_Size()
{
	m_iMaxCX = m_iSize[m_iSizeLv] * DEFAULTCX;
	m_iMaxCY = m_iSize[m_iSizeLv] * DEFAULTCY;
	m_tInfo.iCX = m_iMaxCX;
	m_tInfo.iCY = m_iMaxCY;
	Update_Rect();
	m_pShadow->Reset_Size();
}

void CSlime::Update_DamageCollision()
{
	if ( m_fPosZ <= 2.f && m_fPosZ >= -2.f )
	{
		m_bIsCheckOverlape = true;
		m_pShadow->Set_IsCheckOverlap( true );
	}
	else
	{
		m_bIsCheckOverlape = false;
		m_pShadow->Set_IsCheckOverlap( false );
	}
}

void CSlime::Update_Dead()
{
	m_pShadow->Set_Dead( true );
}

void CSlime::Update_Pattern()
{
	int iCurPatternFrame = m_iPatternFrame % 180;
	if ( iCurPatternFrame == 0 )
	{
		m_fTargetX=m_pPlayer->Get_Info().fX;
		m_fTargetY = m_pPlayer->Get_Info().fY + DEFAULTCY;
	}

	if ( iCurPatternFrame < 50 )
	{
		if ( m_bActive )
		{
			float fDeltaX = MyMath::Clamp<float>(  m_fTargetX - m_tInfo.fX, -5.f, 5.f );
			float fDeltaY = MyMath::Clamp<float>(  m_fTargetY - m_tInfo.fY, -5.f, 5.f );
			m_tInfo.fX += fDeltaX;
			m_tInfo.fY += fDeltaY;

			if ( m_bHaveHeart )
				m_pSludeHeart->Add_Pos( fDeltaX, fDeltaY );
		}
	}
}

void CSlime::Update_Shadow()
{
	m_pShadow->Set_Pos( m_tInfo.fX, m_tInfo.fY );
	m_pShadow->Set_iSizeLv( m_iSizeLv );
}

void CSlime::Update_Heart()
{
	m_pSludeHeart->Set_Pos( m_tInfo.fX, m_tInfo.fY );
	m_pSludeHeart->Set_PosZ( m_fPosZ + (m_tInfo.iCY >> 2) );
}

void CSlime::Update_Mozzi()
{
	int iCurPatternFrame = m_iPatternFrame % 180;

	if ( iCurPatternFrame < 40 )
	{
		m_fMozziX = MyMath::FInterpTo( m_fMozziX, m_fMozziMinX, DELTATIME_S, (iCurPatternFrame + 30) / 8.f);
		m_fMozziY = MyMath::FInterpTo( m_fMozziY, 1.f, DELTATIME_S, (iCurPatternFrame+30) / 8.f );
		m_tInfo.iCX = m_iMaxCX * m_fMozziX;
		m_pShadow->Set_MozziX( m_fMozziX );
		m_tInfo.iCY = m_iMaxCY * m_fMozziY;
	}
	else if ( iCurPatternFrame < 80 )
	{
		m_fMozziX = MyMath::FInterpTo( m_fMozziX, 1.f, DELTATIME_S, iCurPatternFrame / 8.f );
		m_fMozziY = MyMath::FInterpTo( m_fMozziY, m_fMozziMinY, DELTATIME_S, iCurPatternFrame / 8.f );
		m_tInfo.iCX = m_iMaxCX * m_fMozziX;
		m_pShadow->Set_MozziX( m_fMozziX );
		m_tInfo.iCY = m_iMaxCY * m_fMozziY;
	}
	else if (iCurPatternFrame < 100 )
	{
		m_fMozziX = MyMath::FInterpTo( m_fMozziX, 0.9f, DELTATIME_S, iCurPatternFrame / 8.f );
		m_fMozziY = MyMath::FInterpTo( m_fMozziY, 0.9f, DELTATIME_S, iCurPatternFrame / 8.f );
		m_tInfo.iCX = m_iMaxCX * m_fMozziX;
		m_pShadow->Set_MozziX( m_fMozziX );
		m_tInfo.iCY = m_iMaxCY * m_fMozziY;
	}
	else if ( iCurPatternFrame > 160 )
	{
		m_fMozziX = MyMath::FInterpTo( m_fMozziX, 1.f, DELTATIME_S, iCurPatternFrame / 8.f );
		m_fMozziY = MyMath::FInterpTo( m_fMozziY, m_fMozziMinY, DELTATIME_S, iCurPatternFrame / 8.f );
		m_tInfo.iCX = m_iMaxCX * m_fMozziX;
		m_pShadow->Set_MozziX( m_fMozziX );
		m_tInfo.iCY = m_iMaxCY * m_fMozziY;
	}
}
