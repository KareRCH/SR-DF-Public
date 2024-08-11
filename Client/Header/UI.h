#pragma once

#include "AceGameObject.h"
#include "Engine_Define.h"

#include "BlackBoard_Player.h"
#include "BlackBoard_Monster.h"
#include "BlackBoardPtr.h"
#include "BlackBoardMgr.h"

#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

/// <summary>
/// UI의 기본이 되는 추상 클래스
/// </summary>
class CUI abstract : public CAceGameObject
{
	DERIVED_CLASS(CAceGameObject, CUI)

protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI(const CUI& rhs);
	virtual ~CUI();

protected:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() PURE;
	virtual void	Free();

protected:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTransformComponent*	m_pTransformComp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;

	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;	// 블랙보드 플레이어
	FBlackBoardPtr<CBlackBoard_Monster>		m_wpBlackBoard_Monster; // 블랙보드용 

protected:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_matrix				m_ViewMatrix, m_ProjMatrix;
	FRAME				m_tFrame;
	_tchar				debugString[100];
	
};

