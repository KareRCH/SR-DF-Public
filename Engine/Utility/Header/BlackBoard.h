#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

/// <summary>
/// 블랙보드 전용 추상클래스입니다.
/// 정보만을 저장하는 클래스이며, 업데이트는 외부에서 일어난다.
/// 블랙보드 자체가 누군가의 정보를 얻어오는 행위는 하지 않으며
/// 가공된 데이터를 요청받을 때만 계산을 실시하도록 한다.
/// BlackBoard는 항상 FBlackBoardPtr를 통해서 참조할 것을 당부한다.
/// 그 외에 이 블랙보드를 상속받아 만드는 블랙보드의 구조는 어떻게 되든 상관 없다.
/// STL을 많이 우겨넣어서 그룹을 참조하도록 만들어도 좋고
/// 각종 계산을 때려넣어도 좋다.
/// 활용은 사용자에게 맡긴다.
/// </summary>
class ENGINE_DLL CBlackBoard abstract : public CBase
{
protected:
    explicit CBlackBoard();
    explicit CBlackBoard(const CBlackBoard& rhs);
    virtual ~CBlackBoard();

private:
    virtual void		Free() PURE;

protected:
    virtual HRESULT     Ready_BlackBoard() PURE;

public:
    void Set_Expired() { m_bExpired = true; }
    _bool IsExpired() { return m_bExpired; }

private:
    _bool   m_bExpired;             // 블랙보드 만료시 모든 참조포인터들이 연결을 끊을 수 있게 해준다.
    
};

END