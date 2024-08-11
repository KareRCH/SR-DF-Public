#include "stdafx.h"
#include "MonsterAttackUnion.h"
#include "Player.h"

CMonsterAttackUnion::CMonsterAttackUnion(LPDIRECT3DDEVICE9 pGraphicDev)
    :Base(pGraphicDev)
{
}

CMonsterAttackUnion::CMonsterAttackUnion(const CMonsterAttackUnion& rhs)
    : Base(rhs)
{
}

CMonsterAttackUnion::~CMonsterAttackUnion()
{
}

HRESULT CMonsterAttackUnion::Ready_GameObject()
{
    srand((_uint)time(NULL));

    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL); 
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

    return S_OK;
}

_int CMonsterAttackUnion::Update_GameObject(const _float& fTimeDelta)
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    vPlayerPos = m_pPlayerTransformcomp->Get_Pos();

    SUPER::Update_GameObject(fTimeDelta);

    return S_OK;
}

void CMonsterAttackUnion::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CMonsterAttackUnion::Render_GameObject()
{
}

void CMonsterAttackUnion::Free()
{
    SUPER::Free();
}

#pragma region 공통셋팅 파트 

void CMonsterAttackUnion::Height_On_Terrain(_float Height)
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

void CMonsterAttackUnion::Billboard()
{
    // 몬스터가 플레이어 바라보는 벡터 
    _vec3 vDir = vPlayerPos - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);
}


void CMonsterAttackUnion::Knockback_Player(const _float& fTimeDelta, _float _fSpeed)
{
    // 공격체가 플레이어 바라보는 방향 
    // 애초에 플레이어 뒤를 가리키는 벡터이므로 순수 속력만 넣어주면된다. 

    _vec3 Dir = vPlayerPos - m_pTransformComp->Get_Pos();
    D3DXVec3Normalize(&Dir, &Dir);

    m_pPlayerTransformcomp->Move_Pos(&Dir, fTimeDelta, _fSpeed);
}

void CMonsterAttackUnion::Change_MonsterHp(_float _fAttack)
{

}

_bool CMonsterAttackUnion::Attack_Occurrence(CGameObject* pDst, _float fAttack)
{
    // 현재 어택풀자체에 들어간공격팀 = 몬스터팀, 보스팀 
    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

    if (pAceObj == nullptr)
       return false;

    // Check_Relation(pDST, pSrc) = Dst에대한 Src의 우호도 
    // 현재 팀이 충돌체에 대한 팀단위 우호도 검사 

    // 현재팀 - 몬스터 > 적대 : 플레이어
    // 현재팀 - 보스   > 적대 : 몬스터, 플레이어 
    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // 적대관계의 경우,
    {
        // 몬스터일때
        CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);

        if (pMonster == nullptr) // 이게 NULL = 플레이어 
        {
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(pAceObj);

            if (pPlayer == nullptr)// 이래도 null? 
                return false;
            else
            {
                // 충돌 = 플레이어
                OutputDebugString(L"★★★ MonsterAttackUnion에서 계산중 - 몬스터팀 공격 <> Player 충돌 \n");
                m_gPlayerHp = pPlayer->Get_PlayerHP();
                m_gPlayerHp.Cur -= fAttack;

                if (m_gPlayerHp.Cur <= 0)
                    m_gPlayerHp.Cur = 0.f;
                if (m_gPlayerHp.IsMax())
                    m_gPlayerHp.Cur = m_gPlayerHp.Max;

                pPlayer->Set_PlayerHP(m_gPlayerHp);
                
                Engine::Add_GameObject(L"UI", CUI_PlayerHurt::Create(m_pGraphicDev));

                return true;
            }
        }
        else
        {
            // 충돌 = 몬스터 
            m_bOwnerDead = pMonster->Get_IsMonsterDeath();
         
            if (false == m_bOwnerDead)
            {
                OutputDebugString(L"★★★ MonsterAttackUnion에서 계산중 - 몬스터팀 공격 <> Monster 충돌 \n");

                m_gMonsterHp = pMonster->Get_MonsterHP();
                m_gMonsterHp.Cur -= m_fAttack;

                if (m_gMonsterHp.Cur <= 0)
                    m_gMonsterHp.Cur = 0.f;
                if (m_gMonsterHp.IsMax())
                    m_gMonsterHp.Cur = m_gMonsterHp.Max;

                pMonster->Set_MonsterHP(m_gMonsterHp);

                Engine::Add_GameObject(L"UI", CUI_PlayerHurt::Create(m_pGraphicDev));
                return true;
            }
            else
                return false;
        }
    }

    // 적대관계 아니면 그냥 냅두기 
    // Set_Dead()는 해야하므로 s_ok로 설정 
    return true;
}

void CMonsterAttackUnion::Owner_Dead(CGameObject* pOwner)
{
    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pOwner);

    if (pAceObj == nullptr)
        return;
    else
    {
        CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);
        m_bOwnerDead = pMonster->Get_IsMonsterDeath();

        if (m_bOwnerDead)
            Set_Dead();
    }
}

void CMonsterAttackUnion::Change_PlayerHp(_float _fAttack)
{
    // + - 둘다 가능하게 바꿈 
   // PlayerHp = pPlayer->Get_PlayerHP();
   //
   // PlayerHp.Cur += _fAttack;
   //
   // if (PlayerHp.Cur <= 0)
   //     PlayerHp.Cur = 0.f;
   // if (PlayerHp.IsMax())
   //     PlayerHp.Cur = PlayerHp.Max;
   //
   // pPlayer->Set_PlayerHP(PlayerHp);
}

#pragma endregion