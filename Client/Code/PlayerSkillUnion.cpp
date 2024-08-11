#include "stdafx.h"
#include "PlayerSkillUnion.h"
#include "Player.h"
#include "AceMonster.h"

//보스
#include "AceBoss.h"
// 몬스터
#include "Gray.h"
#include "Brown.h"

CPlayerSkillUnion::CPlayerSkillUnion(LPDIRECT3DDEVICE9 pGraphicDev)
    :Base(pGraphicDev)
{
}

CPlayerSkillUnion::CPlayerSkillUnion(const CPlayerSkillUnion& rhs)
    : Base(rhs)
{
}

CPlayerSkillUnion::~CPlayerSkillUnion()
{
}

HRESULT CPlayerSkillUnion::Ready_GameObject()
{
    //srand((_uint)time(NULL));

    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_PlayerRightTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

    return S_OK;
}

_int CPlayerSkillUnion::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    return S_OK;
}

void CPlayerSkillUnion::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CPlayerSkillUnion::Render_GameObject()
{
}

void CPlayerSkillUnion::Free()
{
    SUPER::Free();
}

void CPlayerSkillUnion::MeshBoxColider(FLOAT _Width, FLOAT _Height, FLOAT Depth)
{
    SUPER::MeshBoxColider(_Width, _Height, Depth);
}

void CPlayerSkillUnion::MeshSphereColider(FLOAT Radius, UINT Slices, UINT Stacks)
{
   // SUPER::MeshSphereColider(Radius, Slices, Stacks);
}

#pragma region 셋팅

void CPlayerSkillUnion::Height_On_Terrain(_float Height)
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
        pTerrainBufferComp->Get_VtxPos(),
        pTerrainBufferComp->Get_VertexCountX() + 1U,
        pTerrainBufferComp->Get_VertexCountZ() + 1U,
        pTerrainBufferComp->Get_Scale(),
        pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + Height, vPos.z);
}

void CPlayerSkillUnion::Billboard()
{
    // 해당 공격이 플레이어를 바라보는 벡터 
    _vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    // D3DX_PI : 파이만큼 빼주지 않으면 빌보드시 좌우 반전이 일어남
    m_pTransformComp->Set_RotationY(rad - D3DX_PI);
}

HRESULT CPlayerSkillUnion::Update_PlayerPos()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    return S_OK;
}


void CPlayerSkillUnion::Recoil(const _float& fTimeDelta, _float fSpeed)
{
    // 공격체(총알)가 플레이어 바라보는 방향
    _vec3 Dir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
    D3DXVec3Normalize(&Dir, &Dir);

    m_pPlayerTransformcomp->Move_Pos(&Dir, fTimeDelta, fSpeed);
}

void CPlayerSkillUnion::Change_PlayerHp(_float pAttack)
{
    // + - 둘다 가능하게 바꿈 
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
    PlayerHp = pPlayer->Get_PlayerHP();

    PlayerHp.Cur += pAttack;

    if (PlayerHp.Cur <= 0)
        PlayerHp.Cur = 0.f;
    if (PlayerHp.IsMax())
        PlayerHp.Cur = PlayerHp.Max;

    pPlayer->Set_PlayerHP(PlayerHp);
}

void CPlayerSkillUnion::Change_MonsterHp(_float pAttack, CGameObject* _AttackTarget, PLAYER_ATTACK_STATE _AttackState)
{
    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(_AttackTarget);

    if (pAceObj == nullptr)
        return;

    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE)
    {
        // 타겟 지정
        CAceMonster* pMonster = dynamic_cast<CAceMonster*>(_AttackTarget);

        // 공격받은 몬스터(타겟)가 있을 경우
        if (nullptr != pMonster)
        {
            // 몬스터 체력 받아오기
            MonsterHp = pMonster->Get_MonsterHP();

            // 몬스터 체력 계산
            MonsterHp.Cur += pAttack;

            if (MonsterHp.Cur <= 0)
                MonsterHp.Cur = 0.f;
            if (MonsterHp.IsMax())
                MonsterHp.Cur = MonsterHp.Max;

            pMonster->Set_MonsterHP(MonsterHp); // 계산이 끝난 몬스터의 체력으로 넣어줌
            pMonster->Set_Player_AttackState(_AttackState); // 공격받은 유형을 넣어줌
        }
    }
}

void CPlayerSkillUnion::Monster_Select(CGameObject* _GameObject)
{
    CBrown* pBrown = nullptr;
    pBrown = dynamic_cast<CBrown*>(_GameObject);

    CGray* pGray = nullptr;
    pGray = dynamic_cast<CGray*>(_GameObject);

    CAceBoss* pBoss = nullptr;
    pBoss = dynamic_cast<CAceBoss*>(_GameObject);

    if (pBrown != nullptr)
        Set_Monster(pBrown);

    if (pGray != nullptr)
        Set_Monster(pGray);

    if (pBoss != nullptr)
        Set_Monster(pBoss);
}

_vec3 CPlayerSkillUnion::Target_Pos(CGameObject* _Target)
{
    _vec3 vNoTarget = { 0.f, 0.f, 0.f };
    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(_Target);

    if (pAceObj == nullptr)
        return vNoTarget;

    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE)
    {
        // 타겟 지정
        CAceMonster* pMonster = dynamic_cast<CAceMonster*>(_Target);

        // 공격받은 몬스터(타겟)가 있을 경우
        if (nullptr != pMonster)
        {
            return pMonster->Get_MonsterPos();
        }
    }

    return vNoTarget;
}

#pragma endregion