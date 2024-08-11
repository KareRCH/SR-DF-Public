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

#pragma region ������� ��Ʈ 

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
    // ���Ͱ� �÷��̾� �ٶ󺸴� ���� 
    _vec3 vDir = vPlayerPos - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);
}


void CMonsterAttackUnion::Knockback_Player(const _float& fTimeDelta, _float _fSpeed)
{
    // ����ü�� �÷��̾� �ٶ󺸴� ���� 
    // ���ʿ� �÷��̾� �ڸ� ����Ű�� �����̹Ƿ� ���� �ӷ¸� �־��ָ�ȴ�. 

    _vec3 Dir = vPlayerPos - m_pTransformComp->Get_Pos();
    D3DXVec3Normalize(&Dir, &Dir);

    m_pPlayerTransformcomp->Move_Pos(&Dir, fTimeDelta, _fSpeed);
}

void CMonsterAttackUnion::Change_MonsterHp(_float _fAttack)
{

}

_bool CMonsterAttackUnion::Attack_Occurrence(CGameObject* pDst, _float fAttack)
{
    // ���� ����Ǯ��ü�� �������� = ������, ������ 
    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

    if (pAceObj == nullptr)
       return false;

    // Check_Relation(pDST, pSrc) = Dst������ Src�� ��ȣ�� 
    // ���� ���� �浹ü�� ���� ������ ��ȣ�� �˻� 

    // ������ - ���� > ���� : �÷��̾�
    // ������ - ����   > ���� : ����, �÷��̾� 
    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // ��������� ���,
    {
        // �����϶�
        CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);

        if (pMonster == nullptr) // �̰� NULL = �÷��̾� 
        {
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(pAceObj);

            if (pPlayer == nullptr)// �̷��� null? 
                return false;
            else
            {
                // �浹 = �÷��̾�
                OutputDebugString(L"�ڡڡ� MonsterAttackUnion���� ����� - ������ ���� <> Player �浹 \n");
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
            // �浹 = ���� 
            m_bOwnerDead = pMonster->Get_IsMonsterDeath();
         
            if (false == m_bOwnerDead)
            {
                OutputDebugString(L"�ڡڡ� MonsterAttackUnion���� ����� - ������ ���� <> Monster �浹 \n");

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

    // ������� �ƴϸ� �׳� ���α� 
    // Set_Dead()�� �ؾ��ϹǷ� s_ok�� ���� 
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
    // + - �Ѵ� �����ϰ� �ٲ� 
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