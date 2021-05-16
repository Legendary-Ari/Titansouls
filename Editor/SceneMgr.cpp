#include "stdafx.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Hallway.h"

CSceneMgr* CSceneMgr::m_pInstance = nullptr;
CSceneMgr::CSceneMgr()
	: m_pScene(nullptr), m_ePreScene( SCENEID::ID::SCENDID_END), m_eCurScene( SCENEID::ID::SCENDID_END )
{
}


CSceneMgr::~CSceneMgr()
{
	Release();
}

void CSceneMgr::Update()
{
	m_pScene->Update();
}

void CSceneMgr::Late_Update()
{
	m_pScene->Late_Update();
}

void CSceneMgr::Render(HDC _DC)
{
	m_pScene->Render(_DC);
}

void CSceneMgr::Release()
{
	SAFE_DELETE(m_pScene);
}

void CSceneMgr::Scene_Change( SCENEID::ID _eID)
{
	m_eCurScene = _eID;

	if (m_ePreScene != m_eCurScene)
	{
		Release();

		switch (m_eCurScene)
		{
		case SCENEID::ID::HALLWAY_UNDER:
			m_pScene = new CHallway;
			break;
		case SCENEID::ID::GOL_LATH:
			break;
		}
		m_pScene->Initialize();
		m_ePreScene = m_eCurScene;
	}
}
