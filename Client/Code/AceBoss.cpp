#include "stdafx.h"
#include "AceBoss.h"
#include <AceBuilding.h>

CAceBoss::CAceBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CAceBoss::CAceBoss(const CAceBoss& rhs)
	: Base(rhs)
{
}

CAceBoss::~CAceBoss()
{
}

CAceBoss* CAceBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("BOSS Monster Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);

	return pInstance;
}

CAceBoss* CAceBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
	{
		Safe_Release(pInstance);

		MSG_BOX("BOSS Monster Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CAceBoss::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//���� ����
	m_tSound.m_fTalkAge = 0.f;
	m_tSound.m_fTalkLife = 5.f;
	m_tSound.m_fTalkReapeat = 0.f;
	m_tSound.m_fSoundVolume = 0.6f;
	m_tSound.m_fSoundEffectVolume = 0.2f;
	m_tSound.m_bSoundOnce = FALSE;
	m_tSound.m_bSoundCheck = FALSE;


	// ������ �� �̹��� ���� 
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Idle_South");
	//m_pTransformComp->Set_Scale({ 3.f, 2.5f, 1.f });
	m_tFrame.fFrame = 0.f;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 12.f;
	m_tFrame.fRepeat = 0.f;

	// ����Ʈ ����
	m_fAwareness = m_tStat.fAwareness = 0.f;
	m_tStat.fMaxAwareness = 15.f;
	
	// �浹ó�� & ������ & ���� üũ ���� 
	m_bDazedState = FALSE;
	m_bDeadState = FALSE;
	m_bDazeToHeal = FALSE;
	m_bShitTrigger = FALSE;

	// ��������Ʈ ���� 
	Set_TeamID(ETEAM_BOSS);

	// �������
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 0.5f;

	// Status - ��Ÿ��� DISTANCE����ü�� ����
	m_tStat.fAttackFov = 90.f;
	m_tStat.fAttackDistance = m_tDistance.TotalDist;
	m_gHp.Cur = 100.f;
	m_gHp.Max = 100.f;

	// �浹��
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
	pSphereShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
//	m_pColliderComp->Set_Scale({ 0.1f, 0.1, 0.1 });

	// Phase - ���ϰ��� 
	m_ePhase = Engine::MonsterPhase::Intro;
	m_fTriggerHP = 90.f;

#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
	m_tState_Obj.Set_State(STATE_OBJ::IDLE);

	// ���
	m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CAceBoss::AI_Idle);
	m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CAceBoss::AI_Suspicious);
	m_tState_Obj.Add_Func(STATE_OBJ::REST, &CAceBoss::AI_Rest);
	m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CAceBoss::AI_Chase);

	// �߰�
	m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CAceBoss::AI_Run);
	m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CAceBoss::AI_Walk);
	m_tState_Obj.Add_Func(STATE_OBJ::RELOADING, &CAceBoss::AI_Reloading);

	// ����
	m_tState_Obj.Add_Func(STATE_OBJ::PRE_ATTACK, &CAceBoss::AI_PreAttack);
	m_tState_Obj.Add_Func(STATE_OBJ::SIDE_READY, &CAceBoss::AI_SideReady);
	m_tState_Obj.Add_Func(STATE_OBJ::ROLL, &CAceBoss::AI_Roll);

	// ����
	m_tState_Obj.Add_Func(STATE_OBJ::CLOSEATTACK, &CAceBoss::AI_CloseAttack);
	m_tState_Obj.Add_Func(STATE_OBJ::SHOOTING, &CAceBoss::AI_Shoot);
	m_tState_Obj.Add_Func(STATE_OBJ::HEAVYSHOOT, &CAceBoss::AI_HeavyShoot);

	m_tState_Obj.Add_Func(STATE_OBJ::PHASE1_INSTALL, &CAceBoss::AI_InstallPh1);
	m_tState_Obj.Add_Func(STATE_OBJ::PHASE2_INSTALL, &CAceBoss::AI_InstallPh2);
	m_tState_Obj.Add_Func(STATE_OBJ::PHASE2_BUFF, &CAceBoss::AI_SkillBuff);

	m_tState_Obj.Add_Func(STATE_OBJ::FALLING_STONE, &CAceBoss::AI_SkillStone);
	m_tState_Obj.Add_Func(STATE_OBJ::ENERGY_BALL, &CAceBoss::AI_SkillEnergyBall);
	m_tState_Obj.Add_Func(STATE_OBJ::RED_THUNDER, &CAceBoss::AI_SkillThunder);
	m_tState_Obj.Add_Func(STATE_OBJ::ROUND_FIRE, &CAceBoss::AI_SkillFire);

	// �ǰ�
	m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CAceBoss::AI_Hit);
	m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CAceBoss::AI_Dazed);
	m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CAceBoss::AI_FacePunch);
	m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CAceBoss::AI_Falling);
	m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CAceBoss::AI_CrotchHit);

	// ����
	m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CAceBoss::AI_Death);

#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting

	m_tState_Act.Set_State(STATE_ACT::IDLE);

	m_tState_Act.Add_Func(STATE_ACT::IDLE, &CAceBoss::Idle);
	m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CAceBoss::Approach);
	m_tState_Act.Add_Func(STATE_ACT::MOVING, &CAceBoss::Moving);
	m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CAceBoss::Attack);
	m_tState_Act.Add_Func(STATE_ACT::SHOOT, &CAceBoss::Shoot);
	m_tState_Act.Add_Func(STATE_ACT::FALLING, &CAceBoss::Falling);

	m_tState_Act.Add_Func(STATE_ACT::INSTALL, &CAceBoss::LaserInstall);
	m_tState_Act.Add_Func(STATE_ACT::BUFF, &CAceBoss::BuffActive);

	m_tState_Act.Add_Func(STATE_ACT::SKILL_STONE, &CAceBoss::SkillStone);
	m_tState_Act.Add_Func(STATE_ACT::SKILL_ENERGYBALL, &CAceBoss::SkillEnergyBall);
	m_tState_Act.Add_Func(STATE_ACT::SKILL_THUNDER, &CAceBoss::SkillThunder);
	m_tState_Act.Add_Func(STATE_ACT::SKILL_FIRE, &CAceBoss::SkillFire);


#pragma endregion

#pragma region �׼� Ű ���

	m_mapActionKey.Add_Action(ACTION_KEY::IDLE); // �ֶ� 
	m_mapActionKey.Add_Action(ACTION_KEY::RUN); // �ֶ� 
	m_mapActionKey.Add_Action(ACTION_KEY::WALK); //�߰�
	m_mapActionKey.Add_Action(ACTION_KEY::ROLL); 
	m_mapActionKey.Add_Action(ACTION_KEY::ATTACK);
	m_mapActionKey.Add_Action(ACTION_KEY::FALLING);

	m_mapActionKey.Add_Action(ACTION_KEY::CLOSEATK);
	m_mapActionKey.Add_Action(ACTION_KEY::SHOOT);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_LASER);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_STONE);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_FIRE);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_THUNDER);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_ENERGYBALL);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_BUFF);

#pragma endregion

	Engine::Play_Sound(L"Enemy", L"_TeddyBear.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

	return S_OK;
}

HRESULT CAceBoss::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
	FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

	m_pTransformComp->Set_Pos(tObjectSerial.vPos);
	//m_pTextureComp->Set_Rotation(D3DXToRadian(tObjectSerial.vRotation));
	m_pTextureComp->Set_Scale(tObjectSerial.vScale);
	m_pTextureComp->Readjust_Transform();

	wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
	Set_ObjectName(strConvName);

	m_fPriority[0] = tObjectSerial.fPriority_Update;
	m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
	m_fPriority[2] = tObjectSerial.fPriority_Render;

	m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
	m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
	m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

	m_pColliderComp->Set_TransformToWorld(*m_pTransformComp->Get_Transform());

	return S_OK;
}

_int CAceBoss::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_tSound.m_iHpSection = _int((m_gHp.Cur)/20.f); // 20�������� ���� ���� ������ hpsection 

	Gravity(fTimeDelta);

	m_pTransformComp->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);

	// ����Ÿ�� 
	if (m_pTransformComp->Get_Pos().y < 1.5f && m_vSpeed.y < 0.f)
	{
		Height_On_Terrain(1.6f);
			m_IsOnGround = true;
	}
	else
		m_IsOnGround = false;

	// �״¸���� �ϳ����̶� �浹ü �������ʰ� �׳� 0 �Ǹ� ���� 
	if (m_gHp.Cur <= 0 && FALSE == m_bDeadState)
	{
		Engine::Play_Sound(L"FallenAces", L"DeadCaution.mp3", SOUND_EFFECT, 0.4f);
		m_tState_Obj.Set_State(STATE_OBJ::DEATH);
	}
	//���� ������ ���� 
	Change_Phase();
	
	//�轼��ȯ 
	if (m_gHp.Cur <= 55 && !m_bMakeGun)
	{
		Engine::Add_GameObject(L"GameLogic", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::WEAPON, L"TommyGun",
			m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y + 1.5f, m_pTransformComp->Get_Pos().z));

		m_bMakeGun = true;
	}

	// ������
	if (FALSE == m_bDeadState)
		Billboard(fTimeDelta);

	//���� �����
	m_tSound.m_fTalkAge += fTimeDelta;

	//���¸ӽ�
	m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;
	m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
	m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
	m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = 0.f;

		if (STATE_OBJ::SHOOTING == m_tState_Obj.Get_State() ||
			STATE_OBJ::RUN == m_tState_Obj.Get_State() ||
			STATE_OBJ::REST == m_tState_Obj.Get_State() )
		{
			m_AttackOnce = false;
			m_tFrame.fRepeat += 1;
		}
	}

	if (m_tSound.m_fTalkAge > m_tSound.m_fTalkLife)
	{
		m_tSound.m_fTalkAge = 0.f;
		m_tSound.m_fTalkReapeat += 1.f;
	}

	//������ ���ε� 
	Update_InternalData();

	//���� ������Ʈ �ڵ� - �ݶ��̴� ��ġ ������Ʈ 
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); 

	// Renderer 
	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CAceBoss::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

#pragma region �⺻ override 

void CAceBoss::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &((*m_pTextureComp->Get_Transform()) * (*m_pTransformComp->Get_Transform())));
	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

#pragma region �浹 �޽� �ݶ��̴�
	//MeshSphereColider(pSphereShape->fRadius, 32, 16);
#pragma endregion

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CAceBoss::Add_Component()
{
	// �浹 ������Ʈ 
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderCapsuleComp"), E_FAIL);

	// ���� ���� ���
	m_pColliderComp->EnterToPhysics(0);

	// �浹 �Լ� ����
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// �浹 ���̾�, ����ũ ����
	m_pColliderComp->Set_CollisionLayer(LAYER_BOSSMONSTER); // �� Ŭ������ ���� �浹���̾� 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER | LAYER_WALL | LAYER_PLAYER_ATTACK); // ��� �浹�ؾ��ϴ� ���̾�� - ����ü���� �浹�ҿ��� 

	m_pColliderComp->Set_Scale(_vec3(0.75f, 3.0f, 0.75f));

	return S_OK;
}

void CAceBoss::Free()
{
	SUPER::Free();
}

#pragma endregion 

#pragma region �浹 

void CAceBoss::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
	CSolid* pSolid = dynamic_cast<CSolid*>(pDst);
	if (pSolid)
	{
		_vec3 vNormal(_float(pContact->vContactNormal.x), _float(pContact->vContactNormal.y), _float(pContact->vContactNormal.z));
		m_pTransformComp->Set_Pos((m_pTransformComp->Get_Pos() - vNormal * static_cast<_float>(pContact->fPenetration)));
		if (D3DXVec3Dot(&(-vNormal), &_vec3({ 0.f, -1.f, 0.f })) < 0.f)
			m_IsOnGround = true;
	}
}
void CAceBoss::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	if (Get_IsMonsterDeath()) // �׾����� ���̻� �浹�Ͼ���ʰ� ���� ���� 
		return;

	else if (25 >= m_gHp.Cur && FALSE == m_bDazedState)
		m_tState_Obj.Set_State(STATE_OBJ::DAZED);

	CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

	if (nullptr == pAceObj)
		return;

	// ���� �� : ������  ������� : �÷��̾�, ���� 
	if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // ��������� ���
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pAceObj);

		if (nullptr == pPlayer)
		{
			if (Random_variable(50))
				m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
			if (Random_variable(50))
				m_tState_Obj.Set_State(STATE_OBJ::FALLING_STONE);
			CPlayerAttackUnion* pPlayerAttack = dynamic_cast<CPlayerAttackUnion*>(pAceObj);

			if (nullptr == pPlayerAttack)
			{
				if (Random_variable(50))
					m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
				if (Random_variable(50))
					m_tState_Obj.Set_State(STATE_OBJ::FALLING_STONE);
				CMonsterAttackUnion* pMonsterAttack = dynamic_cast<CMonsterAttackUnion*>(pAceObj);

				if (nullptr == pMonsterAttack)
					return;
				else
				{
					//====���� ����ü�� �浹  - �ڱ��ڽ��� ��ų�� ������ =============================
					return;
				}
			}
			else
			{
				//==== �÷��̾� Atk Union��  �浹 =============================
				//���Ͱ� �ϴ� ������ �������� ������. ���� ������ ������ �����ӿ��� ������ �������� 

				m_bCollisionOn = true;
				Add_BasicEffect(m_pOwner); // ����Ʈ �߰�

				// �ǰݽ� �ݰݸ��� �غ��� �ȵǸ� ��Ʈ�� �Ѿ
				

				if (m_tStat.iDazedHP >= m_gHp.Cur && FALSE == m_bDazedState)
				{
					OutputDebugString(L"��Brown - �浹���� DAZED ����   \n");
					m_tState_Obj.Set_State(STATE_OBJ::DAZED);
					return;
				}
				else if (STATE_RIGHTHAND::KICK == ePlayerRighthand)
					m_tState_Obj.Set_State(STATE_OBJ::FALLING);
				else if (PSITDONW_ATTACK == m_ePlayer_AttackState) // �� + kick = falling �� 
					m_tState_Obj.Set_State(STATE_OBJ::CROTCHHIT);
				else
				{
					if (Random_variable(60))
						m_tState_Obj.Set_State(STATE_OBJ::FACEPUNCH);
					else
						m_tState_Obj.Set_State(STATE_OBJ::HIT);
				}
			}
		}
		else
		{
			//==== �÷��̾�� �浹 =====================================
			m_bCollisionOn = true;

			if (Random_variable(50))
				m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
			if (Random_variable(50))
				m_tState_Obj.Set_State(STATE_OBJ::FALLING_STONE);

			switch (ePlayerRighthand)
			{
			case Engine::STATE_RIGHTHAND::RUN_HAND:	//�޸��� 
				Add_BasicEffect(m_pOwner); // ����Ʈ �߰�
				m_tState_Obj.Set_State(STATE_OBJ::FALLING); 
				break;

			case Engine::STATE_RIGHTHAND::KICK:	//������ 
				Add_BasicEffect(m_pOwner); // ����Ʈ �߰�
				m_tState_Obj.Set_State(STATE_OBJ::FALLING); 
				break;

			default:
				break;
			}
		}
	}
	else if (Check_Relation(pAceObj, this) == ERELATION::NUETRAL) // ������Ʈ �浹 
		return;	//m_tState_Obj.Set_State(STATE_OBJ::HIT);

	// �ǰ��� 40���� Ȯ���� �ݰ� ���� - �ݰ��� ������ �ݹ�Ȯ��  

}
void CAceBoss::OnCollisionExited(CGameObject* pDst)
{
	m_bCollisionOn = false;
}

#pragma endregion 

#pragma region ������ 
// ������� ������Ʈ 
void CAceBoss::Update_InternalData()
{  
	// ������ ���� ���, ���� �ڵ�� �ʼ�
	if (!m_wpBlackBoard_Monster.Get_BlackBoard())
	{
		m_wpBlackBoard_Monster.Set_BlackBoard(Engine::Get_BlackBoard(L"MonsterUnion"));
		// ���� ����
		if (!m_wpBlackBoard_Monster.Get_BlackBoard())
			return;
	}

	// ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
	CBlackBoard_Monster* pBlackBoard = m_wpBlackBoard_Monster.Get_BlackBoard();

	// ���⼭���� �������� ������ ������Ʈ �Ѵ�.
	pBlackBoard->Set_ControlLight(m_bLightOn) ;

	if (m_bCollisionOn)
	{
		pBlackBoard->Get_MonsterHP() = m_gHp;
		pBlackBoard->Get_Owner() = L"Malone";

		Engine::Add_GameObject(L"UI", L"UI_MonsterHpBar", CUI_MonsterHP::Create(m_pGraphicDev));
	}
}

//�����忡�� �ٿ�ε� 
void CAceBoss::Update_BlackBoard()
{
	//// ������ ���� ���, ���� �ڵ�� �ʼ�
	//if (!m_wpBlackBoard_Player.Get_BlackBoard())
	//{
	//	m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"BossLight"));
	//	// ���� ����
	//	if (!m_wpBlackBoard_Player.Get_BlackBoard())
	//		return;
	//}

	//// ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
	//CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

	//// ���⼭���� �������� ������ ���´�.
	//m_fHp = pBlackBoard->Get_HP().Cur;
}

#pragma endregion 

#pragma region ��Ÿ�Լ� 
void CAceBoss::Change_Phase()
{
	if (50.f >= m_gHp.Cur) // 50 ���� = Phase 2
		m_ePhase = Engine::MonsterPhase::Phase2;
	else if (95.f >= m_gHp.Cur && 50.f < m_gHp.Cur) // 50 ~ 95 = Phase 1
		m_ePhase = Engine::MonsterPhase::Phase1;
	else
		m_ePhase = Engine::MonsterPhase::Intro;
}

void CAceBoss::LightControl(const _float& fTimeDelta)
{
	if (m_bLightOn)
	{
		m_tFrame.fAge += 1.f * fTimeDelta;  

		if (m_tFrame.fAge >= m_tFrame.fLifeTime)
		{
			m_bLightOn = false;  //�ð�����ϸ� ����Ʈ ���� 
			m_tFrame.fAge = 0.f;
		}
	}
}


#pragma endregion 

#pragma region ���¸ӽ� : idle ~ Death

void CAceBoss::AI_Idle(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��BOSS - ���¸ӽ� : idle ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;

		m_tSound.m_fTalkReapeat = 0.f;

		Engine::Add_GameObject(L"GameLogic", CBossLight::Create(m_pGraphicDev, this));
    }

	if (m_tState_Obj.Can_Update())
	{

		if (m_tSound.m_fTalkAge > m_tSound.m_fTalkLife)
		{
			if (1 == m_tSound.m_fTalkReapeat)
				Engine::Play_Sound(L"Enemy", L"_HeardSomething.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
			else if (2 <= m_tSound.m_fTalkReapeat)
				Engine::Play_Sound(L"Enemy", L"_HearingGhosts.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		}

		if (Detect_Player())
		{
			m_tSound.m_fTalkReapeat = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
		}
	}

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��BOSS - ���¸ӽ� : idle �� \n");
    }
}

void CAceBoss::AI_Suspicious(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"BackIdle");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
		Engine::Add_GameObject(L"GameLogic", CEffect_Awareness::Create(m_pGraphicDev,
								m_pTransformComp->Get_Pos().x + 0.1f, 
								m_pTransformComp->Get_Pos().y + 1.3f, 
								m_pTransformComp->Get_Pos().z, 
								CEffect_Awareness::TYPE::BOSS, this));

		Engine::Play_Sound(L"Enemy", L"_HearingThingsAgain.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
	}

	if (m_tState_Obj.Can_Update())
	{
		if (Detect_Player()) 
		{
			_float fWeight = 3.0f / Calc_Distance();

			m_tStat.fAwareness += fDeltaTime * 6.f * (1 + fWeight); // ����ġ �߰��� 

			if (m_tStat.fMaxAwareness <= m_tStat.fAwareness)
			{
				m_tStat.fAwareness = m_tStat.fMaxAwareness; 
				m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
			}
		}
		else 
		{
			m_tStat.fAwareness -= fDeltaTime * 2.f;

			if (0 >= m_tStat.fAwareness)
			{
				m_tStat.fAwareness = 0.f;
				m_tState_Obj.Set_State(STATE_OBJ::IDLE);
			}
		}
	};

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Suspicious ��   \n");
	}
}

void CAceBoss::AI_Reloading(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Reloading ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Reloading");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 8.f;
		m_tFrame.fFrame = 0.f;

		m_tSound.m_fTalkReapeat = 0.f;
		
		if(Engine::MonsterPhase::Phase2 == m_ePhase) // �ӵ� ��� 
			m_tFrame.fFrameSpeed = 11.f;

		if(0 == m_tSound.m_iHpSection)
			Engine::Play_Sound(L"Enemy", L"_Reloading_20.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		else if (1 == m_tSound.m_iHpSection)
			Engine::Play_Sound(L"Enemy", L"_Reloading_40.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		else if (2 == m_tSound.m_iHpSection)
			Engine::Play_Sound(L"Enemy", L"_Reloading_60.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		else if (3 == m_tSound.m_iHpSection)
			Engine::Play_Sound(L"Enemy", L"_Reloading_80.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		else if (4 == m_tSound.m_iHpSection)
			Engine::Play_Sound(L"Enemy", L"_Reloading_100.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		else if (5 == m_tSound.m_iHpSection && 0 == m_tSound.m_fTalkReapeat)
			Engine::Play_Sound(L"Enemy", L"_HugeMistake.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		else
			Engine::Play_Sound(L"Enemy", L"_Reloading_100.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

		m_tSound.m_bSoundOnce = true;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tSound.m_bSoundOnce && m_tFrame.fFrame >= 6.f)
		{
			Engine::Play_Sound(L"Enemy", L"_Reloading.wav", SOUND_ENEMY_EFFECT, m_tSound.m_fSoundEffectVolume);
			m_tSound.m_bSoundOnce = false;
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::CHASE);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Reloading ��   \n");
	}
}

void CAceBoss::AI_Rest(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Rest ����   \n");
		m_tStat.fAwareness = m_tStat.fMaxAwareness;
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady"); // �� �̹��� �ٲٱ� ? 
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			if (m_bShitTrigger == TRUE)
			{
				if (m_tFrame.fRepeat >= 2)
				{
					if (MonsterPhase::Phase1 == m_ePhase)
					{
						m_bShitTrigger = false;
						m_tFrame.fRepeat = 0.f;
						m_tState_Obj.Set_State(STATE_OBJ::ENERGY_BALL); // EnergyBall
					}
					else if (MonsterPhase::Phase2 == m_ePhase)
					{
						m_bShitTrigger = false;
						m_tFrame.fRepeat = 0.f;
						m_tState_Obj.Set_State(STATE_OBJ::RED_THUNDER); // EnergyBall
					}
					else if (MonsterPhase::Intro == m_ePhase)
						m_tState_Obj.Set_State(STATE_OBJ::CHASE);
				}
			}
			else if (m_bShitTrigger == FALSE)
				m_tState_Obj.Set_State(STATE_OBJ::CHASE);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Rest �� \n");
	}
}

void CAceBoss::AI_Chase(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		m_tStat.fAwareness = m_tStat.fMaxAwareness;
		m_AttackOnce = FALSE;
		
	}

	if (m_tState_Obj.Can_Update())
	{
		_float CurDist = Calc_Distance();

		if (Engine::MonsterPhase::Intro == m_ePhase)
		{

			// ������ ��ü�� ���� �þ߿� �����Ƿ� ó������ �޸��ų� ��������
			if (m_tDistance.INTRO_ShootDist < CurDist) // 15 < a �ʰ� 
			{
				//OutputDebugString(L"�ڡڡڡڡ� Chase - ��Ʈ�� - WALK \n");
				m_tState_Obj.Set_State(STATE_OBJ::WALK);
			}
			else if (m_tDistance.INTRO_CloseAtkDist < CurDist && 
					 m_tDistance.INTRO_ShootDist >= CurDist ) // 1.5 < a <= 15����
			{
				//OutputDebugString(L"�ڡڡڡڡ�  Chase - ��Ʈ�� - SHOOTING \n");
				m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
			}
			else if (m_tDistance.INTRO_CloseAtkDist >= CurDist) // a <= 1.5  
			{
				//OutputDebugString(L"�ڡڡڡڡ�  Chase - ��Ʈ�� - CLOSEATTACK \n");
				m_tState_Obj.Set_State(STATE_OBJ::CLOSEATTACK);
			}
		}
		else if (Engine::MonsterPhase::Phase1 == m_ePhase)
		{
			_int HP = _int(m_gHp.Cur);

			if (!m_bPhaseStart) //1ȸ�� ��ġ�� 
			{
				m_fTriggerHP = 90.f;
				m_bPhaseStart = TRUE;
				//m_tState_Obj.Set_State(STATE_OBJ::PHASE1_INSTALL);
			}
			else if(m_fTriggerHP > HP) // 15���� ���������� ������ ���� 
			{
				Engine::Play_Sound(L"Enemy", L"_ShitShitShitA.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
				m_fTriggerHP -= 15.f;
				m_bShitTrigger = TRUE;
				m_tState_Obj.Set_State(STATE_OBJ::REST); // EnergyBall
			}
			else if (Random_variable(65)) //60% Ȯ���� ��ų������ ����. 
			{
				m_tState_Obj.Set_State(STATE_OBJ::FALLING_STONE); // FallingStone
			}
			// ======= ������ʹ� �Ÿ��� �Ϲݰ��� =======
			else if (m_tDistance.Ph1_RunDist < CurDist) // 13 < a �ʰ� 
			{							
				m_tState_Obj.Set_State(STATE_OBJ::RUN);
			}
			else if (m_tDistance.Ph1_ShootDist < CurDist &&	m_tDistance.Ph1_RunDist >= CurDist) // 5 < a <= 13����
			{							
				m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
			}
			else if (m_tDistance.Ph1_CloseAtkDist < CurDist &&	m_tDistance.Ph1_RollDist >= CurDist) // 2 < a <= 5����
			{							
				m_tState_Obj.Set_State(STATE_OBJ::ROLL);
			}
			else if (m_tDistance.Ph1_CloseAtkDist >= CurDist) // a <= 2 
			{							
				m_tState_Obj.Set_State(STATE_OBJ::CLOSEATTACK);
			}

		}
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)
		{
			_int HP = _int(m_gHp.Cur);

			if (m_bPhaseStart) // ���� ���۽�ų 
			{
				m_fTriggerHP = 50.f;
				m_bPhaseStart = FALSE;
				m_tState_Obj.Set_State(STATE_OBJ::PHASE2_BUFF);
			}
			else if (m_fTriggerHP > HP) // 12���� hp���������� 
			{
				Engine::Play_Sound(L"Enemy", L"_ShitShitShitB.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
				m_fTriggerHP -= 12.f;
				m_bShitTrigger = TRUE;
				m_tState_Obj.Set_State(STATE_OBJ::REST); // Thunder
			}
			//else if (m_gHp.Cur < 25.f) // ������ ��ġ�Ұ� ����ġ�ʾƼ� �׳� �ּ�ó�� 
			//{
			//	m_tState_Obj.Set_State(STATE_OBJ::PHASE2_INSTALL); // EnergyBall
			//}
			else if (Random_variable(75)) //75% Ȯ���� ��ų������ ����. 
			{
				// ���� 25���� Ȯ���� fire, 50���� Ȯ���� ��������, �Ѵ� �ȵǸ� �������� 
				// ��� Ȯ���� �������� 
				if(Random_variable(25)) 
					m_tState_Obj.Set_State(STATE_OBJ::ROUND_FIRE); // FallingStone
				else if(Random_variable(50))
					m_tState_Obj.Set_State(STATE_OBJ::ENERGY_BALL); // FallingStone
				else
					m_tState_Obj.Set_State(STATE_OBJ::FALLING_STONE); // FallingStone
			}
			// ======= ������ʹ� �Ÿ��� �Ϲݰ��� =======
			else if (m_tDistance.Ph2_RunDist < CurDist) // 15 < a �ʰ� 
			{
				m_tState_Obj.Set_State(STATE_OBJ::RUN);
			}
			else if (m_tDistance.Ph2_SideReadyDist < CurDist && m_tDistance.Ph2_RunDist >= CurDist) // 5 < a <= 13����
			{
				m_tState_Obj.Set_State(STATE_OBJ::SIDE_READY);
			}
			else if (m_tDistance.Ph2_RollDist < CurDist && m_tDistance.Ph2_SideReadyDist >= CurDist) // 2 < a <= 5����
			{
				m_tState_Obj.Set_State(STATE_OBJ::ROLL);
			}
			else if (m_tDistance.Ph2_ShootDist < CurDist && m_tDistance.Ph2_RollDist >= CurDist) // 2 < a <= 5����
			{
				m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
			}
			else if (m_tDistance.Ph1_CloseAtkDist >= CurDist) // a <= 2 
			{
				m_tState_Obj.Set_State(STATE_OBJ::PRE_ATTACK);
			}

		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Chase ��   \n");
	}
}

void CAceBoss::AI_Run(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Run ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Run"); // �� �̹��� �ٲٱ� ? 
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 11.f;
		m_tFrame.fFrame = 0.f;

		m_tSound.m_bSoundOnce = true;
		m_tSound.m_bSoundCheck = true;
		m_AttackOnce = true;

		if(1 >= m_tSound.m_iHpSection)
			Engine::Play_Sound(L"Enemy", L"_MySightA.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		else if (3 >= m_tSound.m_iHpSection && 1 < m_tSound.m_iHpSection)
			Engine::Play_Sound(L"Enemy", L"_MySightB.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		else if (4 <= m_tSound.m_iHpSection)
			Engine::Play_Sound(L"Enemy", L"_MySightC.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

		if(Engine::MonsterPhase::Phase2 == m_ePhase)
			m_tFrame.fFrameSpeed = 14.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		// ���� - Phase���� ���ǵ尡 �ٸ� 
		if (m_tFrame.fFrame >= 1 && m_tSound.m_bSoundOnce && m_tSound.m_bSoundCheck && m_AttackOnce) // TT
		{
			m_tSound.m_bSoundOnce = false;
			m_tSound.m_bSoundCheck = false;
			m_AttackOnce = false;
			Engine::Play_Sound(L"Enemy", L"_FootstepRun.wav", SOUND_ENEMY_EFFECT, m_tSound.m_fSoundVolume);
		}
		else if (m_tFrame.fFrame >= 3 && !m_tSound.m_bSoundOnce && !m_tSound.m_bSoundCheck)
		{
			m_tSound.m_bSoundOnce = true;
			m_tSound.m_bSoundCheck = false;
			Engine::Play_Sound(L"Enemy", L"_FootstepRun.wav", SOUND_ENEMY_EFFECT, m_tSound.m_fSoundVolume);
		} 
		else if (m_tFrame.fFrame >=6 && m_tSound.m_bSoundOnce && !m_tSound.m_bSoundCheck)
		{
			m_tSound.m_bSoundOnce = false;
			m_tSound.m_bSoundCheck = true;
			Engine::Play_Sound(L"Enemy", L"_FootstepRun.wav", SOUND_ENEMY_EFFECT, m_tSound.m_fSoundVolume);
		}
		else if (m_tFrame.fFrame >= 9 && !m_tSound.m_bSoundOnce && m_tSound.m_bSoundCheck)
		{
			m_tSound.m_bSoundOnce = true;
			m_tSound.m_bSoundCheck = true;
			Engine::Play_Sound(L"Enemy", L"_FootstepRun.wav", SOUND_ENEMY_EFFECT, m_tSound.m_fSoundVolume);
		}


		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� Run > Run����Ű  \n");
			m_mapActionKey[ACTION_KEY::RUN].Act();
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� Run -> SHOOTING �ٽ� ����   \n");
			m_tFrame.fRepeat = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Run �� \n");
	}
}

void CAceBoss::AI_Walk(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Walk ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Walk"); // �� �̹��� �ٲٱ� ? 
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 14.f;
		m_tFrame.fFrame = 0.f;
		m_tSound.m_bSoundOnce = true;
		m_tSound.m_bSoundCheck = true;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tFrame.fFrame >= 3 && m_tSound.m_bSoundOnce && m_tSound.m_bSoundCheck)
		{
			m_tSound.m_bSoundOnce = false;
			m_tSound.m_bSoundCheck = false;
			Engine::Play_Sound(L"Enemy", L"_FootstepWalk.wav", SOUND_ENEMY_EFFECT, m_tSound.m_fSoundVolume);
		}
		if (m_tFrame.fFrame >= 14 && !m_tSound.m_bSoundOnce)
		{
			m_tSound.m_bSoundOnce = true;
			Engine::Play_Sound(L"Enemy", L"_FootstepWalk.wav", SOUND_ENEMY_EFFECT, m_tSound.m_fSoundVolume);
		}

		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� Walk > walk����Ű  \n");
			m_mapActionKey[ACTION_KEY::WALK].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� walk -> Chase �ٽ� ����   \n");
			m_tState_Obj.Set_State(STATE_OBJ::CHASE);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Walk �� \n");
	}
}

void CAceBoss::AI_PreAttack(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Pre_Attack ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Pre_Attack"); // �� �̹��� �ٲٱ� ? 
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrame = 0.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		m_fSideAge += 1.f * fDeltaTime;

		if (m_fSideAge > m_fSideTime)
		{
			m_fSideAge = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::CLOSEATTACK);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Pre_Attack �� \n");
	}
}

void CAceBoss::AI_SideReady(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : SideReady ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"SideReady");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrame = 0.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		m_fSideAge += 1.f * fDeltaTime;

		if (m_fSideAge > m_fSideTime)
		{
			m_fSideAge = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::HEAVYSHOOT);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : CloseAttack �� \n");
	}
}

void CAceBoss::AI_Roll(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Roll ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Roll");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 11.f;
		m_tFrame.fFrame = 0.f;

		if (Engine::MonsterPhase::Phase2 == m_ePhase)
		{
			m_tFrame.fFrameSpeed = 14.f;
			Engine::Play_Sound(L"Enemy", L"_MyTurfB.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		}
		else if (Engine::MonsterPhase::Phase1 == m_ePhase)
			Engine::Play_Sound(L"Enemy", L"_MyTurfA.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			m_mapActionKey[ACTION_KEY::ROLL].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Roll �� \n");
	}
}


void CAceBoss::AI_CloseAttack(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : CloseAttack ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"CloseAttack");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� CloseAttack > CLOSEATK ����Ű  \n");
			m_mapActionKey[ACTION_KEY::CLOSEATK].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� CloseAttack -> Chase �ٽ� ����   \n");
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : CloseAttack �� \n");
	}
}

void CAceBoss::AI_Shoot(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Shooting ����   \n");

		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shooting");
		m_tFrame.fFrameEnd = float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 13.f;
		m_tFrame.fFrame = 0.f;

		if (Engine::MonsterPhase::Phase2 == m_ePhase)
			m_tFrame.fFrameSpeed = 16.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tFrame.fFrame <= 1)
		{
			m_bLightOn = true;
		}

		LightControl(fDeltaTime);

		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� Shoot > SHOOT����Ű  \n");
			m_mapActionKey[ACTION_KEY::SHOOT].Act();
		}

		if (4 <= m_tFrame.fRepeat)
		{
			m_bLightOn = FALSE;
			m_tFrame.fRepeat = 0;
			//m_iBulletCnt -= 1; // ������ ���� 
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� Shooting -> Chase �ٽ� ����   \n");
			m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Shooting ��   \n");
	}
}

void CAceBoss::AI_HeavyShoot(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Shooting ����   \n");

		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shooting");
		m_tFrame.fFrameEnd = float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		m_fSideAge += 1.f * fDeltaTime;

		if (m_fSideAge > m_fSideTime) // �ܴ����·� ��� - HeavyShot�̹Ƿ� 
		{
			m_tFrame.fFrame = 0.f;
		}
		else
		{
			if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
			{
				//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� Shoot > SHOOT����Ű  \n");
				m_mapActionKey[ACTION_KEY::SHOOT].Act();
			}

			if (m_tFrame.fFrame <= 1)
			{
				m_bLightOn = true;
			}
			LightControl(fDeltaTime);
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_fSideAge = 0.f;
			m_bLightOn = FALSE;
			m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Shooting ��   \n");
	}
}

void CAceBoss::AI_SkillStone(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Phase1 Skill ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Evasion");
		m_tFrame.fFrameEnd = float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 8.f;
		m_tFrame.fFrame = 0.f;

		Engine::Play_Sound(L"Enemy", L"_MeleeA.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
	}

	if (m_tState_Obj.Can_Update())
	{
		_int HP = _int(m_gHp.Cur);

		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			m_mapActionKey[ACTION_KEY::SKILL_STONE].Act();
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_SkillEnergyBall(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Phase1 EnergyBall ����   \n");

		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Evasion");
		m_tFrame.fFrameEnd = float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 8.f;
		m_tFrame.fFrame = 0.f;
		Engine::Play_Sound(L"Enemy", L"_MeleeB.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			m_mapActionKey[ACTION_KEY::SKILL_ENERGYBALL].Act();
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_InstallPh1(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : 1���� ��ġ�� ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"SetOn");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;

		Engine::Play_Sound(L"Enemy", L"_DoYouKnowA.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� SKILL_LASER ����Ű  \n");
			m_mapActionKey[ACTION_KEY::SKILL_LASER].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : CloseAttack �� \n");
	}

}
void CAceBoss::AI_SkillThunder(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : Thunder ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shout");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
		Engine::Play_Sound(L"Enemy", L"_MeleeB.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

		//������ �����ؾߵǼ� Create�Ҷ����� �ڲ� �ҷ����°Ͷ����� ���⼭ ȿ���� �߰� 
		Engine::Play_Sound(L"Enemy", L"_Thunder.wav", SOUND_ENEMY_THUNDER, m_tSound.m_fSoundVolume);
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� SKILL_THUNDER ����Ű  \n");
			m_mapActionKey[ACTION_KEY::SKILL_THUNDER].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::CHASE);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : SKILL_THUNDER �� \n");
	}
}

void CAceBoss::AI_SkillFire(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� :SKILL_FIRE ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shout");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
		Engine::Play_Sound(L"Enemy", L"_MeleeA.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� SKILL_FIRE ����Ű  \n");
			m_mapActionKey[ACTION_KEY::SKILL_FIRE].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::CHASE);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : SKILL_FIRE �� \n");
	}
}

void CAceBoss::AI_SkillBuff(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : 2���� ���� ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shout");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
		Engine::Play_Sound(L"Enemy", L"_BuffCall.wav", SOUND_ENEMY, 0.5f);
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"�ڡڡڡڡ� ���¸ӽ� SKILL_BUFF ����Ű  \n");
			m_mapActionKey[ACTION_KEY::SKILL_BUFF].Act();
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::CHASE);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : CloseAttack �� \n");
	}
}

void CAceBoss::AI_InstallPh2(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : 1���� ��ġ�� ����   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"SetOn");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			m_mapActionKey[ACTION_KEY::SKILL_LASER].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_Hit(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : �Ϲ��ǰݻ��� ����   \n");
		m_tStat.fAwareness = m_tStat.fMaxAwareness;
		m_tFrame.fFrame = 0.f;

		if (Random_variable(50))
		{
			m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Hit_A");
			Engine::Play_Sound(L"Enemy", L"_HitA.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		}
		else
		{
			m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Hit_B");
			Engine::Play_Sound(L"Enemy", L"_HitB.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);
		}
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	}

	if (m_tState_Obj.Can_Update())
	{
		m_fSideAge += 1.f * fDeltaTime;

		if (m_fSideAge > m_fSideTime)
		{
			m_fSideAge = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_Dazed(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : 1���� ��ġ�� ����   \n");
		m_tStat.fAwareness = m_tStat.fMaxAwareness;
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Dazed");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
		m_tFrame.fLifeTime = 1.f;
		m_bDazedState = TRUE;
		Engine::Play_Sound(L"Enemy", L"_Dazed.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

		if (Engine::MonsterPhase::Phase2 == m_ePhase)
			m_tFrame.fFrameSpeed = 12.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		m_tFrame.fAge += 1.f * fDeltaTime;

		if (m_tFrame.fAge > m_tFrame.fLifeTime)
		{
			m_tFrame.fLifeTime = 0.f;
			m_tFrame.fAge = 0.f;
			m_bDazeToHeal = TRUE;
		}
		if (TRUE == m_bDazeToHeal)
		{
	
			if (m_gHp.Update(fDeltaTime * 6.f, 40.f, TRUE)) // ������, �����ϸ� bool��ȯ 
			{
				m_gHp.Cur = 40.f;
				m_bDazedState = FALSE;
				m_tState_Obj.Set_State(STATE_OBJ::CHASE);
			}
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_Falling(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : 1���� ��ġ�� ����   \n");
		m_tStat.fAwareness = m_tStat.fMaxAwareness;
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Falling");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;

		Engine::Play_Sound(L"Enemy", L"_Falling.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			m_mapActionKey[ACTION_KEY::FALLING].Act();
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tFrame.fFrame = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_FacePunch(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : 1���� ��ġ�� ����   \n");
		m_tStat.fAwareness = m_tStat.fMaxAwareness;
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"FacePunch");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
		
		if (Engine::MonsterPhase::Phase2 == m_ePhase)
			m_tFrame.fFrameSpeed = 14.f;

		Engine::Play_Sound(L"Enemy", L"_FacePunch.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tFrame.fFrame = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::REST);

		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_CrotchHit(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : �ϴ��ǰ� ����   \n");
		m_tStat.fAwareness = m_tStat.fMaxAwareness;
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"CrotchHit");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrame = 0.f;
		Engine::Play_Sound(L"Enemy", L"_CrotchHit.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

	}

	if (m_tState_Obj.Can_Update())
	{
		m_fSideAge += 1.f * fDeltaTime;

		if (m_fSideAge > m_fSideTime)
		{
			m_fSideAge = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_Death(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - ���¸ӽ� : 1���� ��ġ�� ����   \n");
		m_tStat.fAwareness = m_tStat.fMaxAwareness;
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Death");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fFrame = 0.f;
		m_bDeadState = TRUE;
		m_bLightOn = FALSE;

		Engine::Play_Sound(L"Enemy", L"_Death.wav", SOUND_ENEMY, 0.5f);
	}

	if (m_tState_Obj.Can_Update())
	{

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tFrame.fFrame = m_tFrame.fFrameEnd -1.f;

		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

#pragma endregion 

#pragma region �ൿ�ӽ�
void CAceBoss::Idle(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered()) // 1�� bIsEnter false�� �ٲ� 
	{
		//OutputDebugString(L"��BOSS - �ൿ : Idle ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (m_mapActionKey[ACTION_KEY::FALLING].IsOnAct()) 
			m_tState_Act.Set_State(STATE_ACT::FALLING);

		if (Engine::MonsterPhase::Intro == m_ePhase)
		{
			if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct()) // WALK
				m_tState_Act.Set_State(STATE_ACT::APPROACH);
			else if (m_mapActionKey[ACTION_KEY::SHOOT].IsOnAct()) // Shooting
				m_tState_Act.Set_State(STATE_ACT::SHOOT);
			else if (m_mapActionKey[ACTION_KEY::CLOSEATK].IsOnAct()) // CloseAtk
				m_tState_Act.Set_State(STATE_ACT::ATTACK);
			//else if (m_mapActionKey[ACTION_KEY::ATTACK].IsOnAct()); //CloseAtk
			//{
			//	//OutputDebugString(L"�ڡڡڡڡ� �ൿ idle -> ATTACK ����   \n"); 
			//	m_tState_Act.Set_State(STATE_ACT::ATTACK);
			//}

			// Q. ��ü�� ���⼭ �ڲ� ATTACK���� ? ACT���ߴµ��� ?? ??????????
		}// ������� ��ų������Ʈ 
		// ===== ���� ���� ��ų =====
		else if (m_mapActionKey[ACTION_KEY::SKILL_BUFF].IsOnAct()) // ���� 
			m_tState_Act.Set_State(STATE_ACT::BUFF);
		else if (m_mapActionKey[ACTION_KEY::SKILL_LASER].IsOnAct()) // Laser ��ġ 
			m_tState_Act.Set_State(STATE_ACT::INSTALL);
		// ===== �Ϲݽ�ų ===== 
		else if (m_mapActionKey[ACTION_KEY::SKILL_STONE].IsOnAct())
			m_tState_Act.Set_State(STATE_ACT::SKILL_STONE);
		else if (m_mapActionKey[ACTION_KEY::SKILL_ENERGYBALL].IsOnAct())
			m_tState_Act.Set_State(STATE_ACT::SKILL_ENERGYBALL);
		else if (m_mapActionKey[ACTION_KEY::SKILL_THUNDER].IsOnAct())
			m_tState_Act.Set_State(STATE_ACT::SKILL_THUNDER);
		else if (m_mapActionKey[ACTION_KEY::SKILL_FIRE].IsOnAct())
			m_tState_Act.Set_State(STATE_ACT::SKILL_FIRE);
		// ===== 1������ �Ϲݰ��ݶ��� 
		else if (Engine::MonsterPhase::Phase1 == m_ePhase)
		{
			if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct()) // WALK
				m_tState_Act.Set_State(STATE_ACT::APPROACH);
			else if (m_mapActionKey[ACTION_KEY::SHOOT].IsOnAct()) // Shooting
				m_tState_Act.Set_State(STATE_ACT::SHOOT);
			else if (m_mapActionKey[ACTION_KEY::ROLL].IsOnAct()) // Shooting
				m_tState_Act.Set_State(STATE_ACT::MOVING);
			else if (m_mapActionKey[ACTION_KEY::CLOSEATK].IsOnAct()) // CloseAtk
				m_tState_Act.Set_State(STATE_ACT::ATTACK);
		}
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)//Shooting + CloseAtk 
		{
			if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct()) // WALK
				m_tState_Act.Set_State(STATE_ACT::APPROACH);
			else if (m_mapActionKey[ACTION_KEY::SHOOT].IsOnAct()) // Shooting
				m_tState_Act.Set_State(STATE_ACT::SHOOT);
			else if (m_mapActionKey[ACTION_KEY::ROLL].IsOnAct()) // Shooting
				m_tState_Act.Set_State(STATE_ACT::MOVING);
			else if (m_mapActionKey[ACTION_KEY::CLOSEATK].IsOnAct()) // CloseAtk
				m_tState_Act.Set_State(STATE_ACT::ATTACK);
		}	
	}
	
	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - �ൿ : Idle ��   \n");
	}
}

void CAceBoss::Approach(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - �ൿ : Approach ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
		D3DXVec3Normalize(&vDir, &vDir);

		if (Engine::MonsterPhase::Intro == m_ePhase)// WALK
				m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 4.f);
		else if (Engine::MonsterPhase::Phase1 == m_ePhase) // RUN
			m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 12.f);
		else if (Engine::MonsterPhase::Phase2 == m_ePhase) //RUN
			m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 16.f);

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - �ൿ : Approaching ��   \n");
	}
}

void CAceBoss::Moving(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - �ൿ : Moving  - Roll ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		vDir = m_pTransformComp->Get_Right();
		D3DXVec3Normalize(&vDir, &vDir);

		if (Engine::MonsterPhase::Phase1 == m_ePhase)//CloseAtk 
			m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 5.f);
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)//CloseAtk 
			m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 7.f);

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - �ൿ : Attack ��   \n");
	}
}

void CAceBoss::Attack(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - �ൿ : Attack ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (Engine::MonsterPhase::Intro == m_ePhase || Engine::MonsterPhase::Phase1 == m_ePhase )//CloseAtk 
		{
			if (STATE_OBJ::CLOSEATTACK == m_tState_Obj.Get_State())
			{
				//OutputDebugString(L" �ൿ INTRO - Attack -> CloseAttack ����   \n");
				if (!m_AttackOnce)
				{
					_vec3 vLook = m_pTransformComp->Get_Look();
					D3DXVec3Normalize(&vLook, &vLook);

					vDir = m_pTransformComp->Get_Pos() + vLook * 1;

					Engine::Add_GameObject(L"GameLogic", CMonsterPunch::Create(m_pGraphicDev,
						vDir.x, vDir.y, vDir.z, CMonsterPunch::TYPE::NORMAL, this, (ETEAM_ID)Get_TeamID()));

					m_AttackOnce = TRUE;
				}
			}
		}
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)//Shooting + CloseAtk 
		{
			if (STATE_OBJ::CLOSEATTACK == m_tState_Obj.Get_State())
			{
				//OutputDebugString(L" �ൿ INTRO - Attack -> CloseAttack ����   \n");
				if (!m_AttackOnce)
				{
					_vec3 vLook = m_pTransformComp->Get_Look();
					D3DXVec3Normalize(&vLook, &vLook);

					vDir = m_pTransformComp->Get_Pos() + vLook * 1;

					Engine::Add_GameObject(L"GameLogic", CMonsterPunch::Create(m_pGraphicDev,
						vDir.x, vDir.y, vDir.z, CMonsterPunch::TYPE::HEAVY, this, (ETEAM_ID)Get_TeamID()));

					m_AttackOnce = TRUE;
				}
			}
		}

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - �ൿ : Attack ��   \n");
	}
}

void CAceBoss::Shoot(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - �ൿ : Shoot ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (STATE_OBJ::SHOOTING == m_tState_Obj.Get_State())
		{
			if (!m_AttackOnce)
			{
				Engine::Play_Sound(L"Enemy", L"_Shooting.wav", SOUND_ENEMY_EFFECT, m_tSound.m_fSoundEffectVolume);

				Engine::Add_GameObject(L"GameLogic", CMonsterBullet::Create(m_pGraphicDev,
					m_pTransformComp->Get_Pos().x,
					m_pTransformComp->Get_Pos().y,
					m_pTransformComp->Get_Pos().z, CMonsterBullet::TYPE::NORMAL, this, (ETEAM_ID)Get_TeamID()));

				m_AttackOnce = TRUE;
			}
		}
		else if (STATE_OBJ::HEAVYSHOOT == m_tState_Obj.Get_State())
		{
			if (!m_AttackOnce)
			{
				Engine::Play_Sound(L"Enemy", L"_HeavyShoot.wav", SOUND_ENEMY_EFFECT, m_tSound.m_fSoundEffectVolume);

				Engine::Add_GameObject(L"GameLogic", CMonsterBullet::Create(m_pGraphicDev,
					m_pTransformComp->Get_Pos().x,
					m_pTransformComp->Get_Pos().y,
					m_pTransformComp->Get_Pos().z, CMonsterBullet::TYPE::NORMAL, this, (ETEAM_ID)Get_TeamID()));

				m_AttackOnce = TRUE;
			}
		}
		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - �ൿ : Shoot ��   \n");
	}
}

void CAceBoss::Falling(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - �ൿ�ӽ� :FALLING ����   \n");
	}

	// ����
	{
		vDir = m_pPlayerTransformcomp->Get_Look();

		D3DXVec3Normalize(&vDir, &vDir);

		m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 7.f);

		if (STATE_OBJ::FALLING != m_tState_Obj.Get_State())
			m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		////OutputDebugString(L"��BOSS - �ൿ�ӽ� : GOHOME ��   \n");
	}
}

void CAceBoss::LaserInstall(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - �ൿ : LaserInstall ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (Engine::MonsterPhase::Phase1 == m_ePhase)
		{
			if (!m_AttackOnce)
			{
				_float PosZ = m_pTransformComp->Get_Pos().z;
				_float PosY = m_pTransformComp->Get_Pos().y;

				for (_int i = 0; i < 5; ++i)
				{
					Engine::Add_GameObject(L"GameLogic", CRedLaser::Create(m_pGraphicDev,
						m_pTransformComp->Get_Pos().x,
						Get_RandomFloat(PosY - 1.f, PosY + 5.f),
						Get_RandomFloat(PosZ - 3.f, PosZ + 3.f), this, (ETEAM_ID)Get_TeamID()));
				}
				m_AttackOnce = TRUE;
			}
		}
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)
		{
			if (!m_AttackOnce)
			{
				//������ ��ġ ���� 


				m_AttackOnce = TRUE;
			}
		}

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - �ൿ : LaserInstall ��   \n");
	}
}

void CAceBoss::BuffActive(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - �ൿ : BuffActive ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (Engine::MonsterPhase::Phase2 == m_ePhase)
		{
			if (!m_bBuffActive)
			{
				Engine::Add_GameObject(L"GameLogic", CBlueBuff::Create(m_pGraphicDev,
					m_pTransformComp->Get_Pos().x,
					m_pTransformComp->Get_Pos().y,
					m_pTransformComp->Get_Pos().z, m_ePhase, this, (ETEAM_ID)Get_TeamID()));
			}
		
			m_bBuffActive = TRUE;
		}

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"��BOSS - �ൿ : BuffActive ��   \n");
	}
}

void CAceBoss::SkillStone(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - Skill - SKILL_STONE ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (!m_AttackOnce)
		{
			_int iCnt = 0;

			if (Engine::MonsterPhase::Phase1 == m_ePhase)
				iCnt = 4;
			else if (Engine::MonsterPhase::Phase2 == m_ePhase)
				iCnt = 6;

			for (_int i = 0; i < iCnt; ++i)
			{
				_vec3 randomCenter;
				_vec3 m_pPlayerPos = m_pPlayerTransformcomp->Get_Pos();

				GetRandomPointInCircle(&randomCenter, &m_pPlayerPos, 6.f);

				Engine::Add_GameObject(L"GameLogic", CFallingStone::Create(m_pGraphicDev,
					randomCenter.x,
					randomCenter.y,
					randomCenter.z, m_ePhase, this, (ETEAM_ID)Get_TeamID()));
			}

		}
		m_AttackOnce = true;

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
	}
}

void CAceBoss::SkillEnergyBall(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - Skill - EnergyBall ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{	
		if (!m_AttackOnce)
		{
			_int iCnt = 0;

			if (Engine::MonsterPhase::Phase1 == m_ePhase)
				iCnt = 4;
			else if (Engine::MonsterPhase::Phase2 == m_ePhase)
				iCnt = 5;
			
			// ���� ��ġ���� Right ���� �������� spacing ������ �ΰ� ��ȯ ��ġ ���
			// �¿� 3���� ��6��, �������� ��� ������ ��Ÿ���̱⵵��
			for (_int i = 1; i < iCnt; ++i)
			{
				_vec3 Pos1 = m_pTransformComp->Get_Pos() + m_pTransformComp->Get_Right() * _float(i);
				_vec3 Pos2 = m_pTransformComp->Get_Pos() - m_pTransformComp->Get_Right() * _float(i);

				Engine::Add_GameObject(L"GameLogic", CEnergyBall::Create(m_pGraphicDev,
					Pos1.x, Pos1.y, Pos1.z, m_ePhase, this, (ETEAM_ID)Get_TeamID()));

				Engine::Add_GameObject(L"GameLogic", CEnergyBall::Create(m_pGraphicDev,
					Pos2.x, Pos2.y, Pos2.z, m_ePhase, this, (ETEAM_ID)Get_TeamID()));
			}
			m_AttackOnce = true;
		}
		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
	}
}

void CAceBoss::SkillThunder(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - Skill - SKILL_STONE ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (!m_AttackOnce)
		{
			for (_int i = 0 ; i < 5; ++i)
			{
				_vec3 randomCenter;
				_vec3 m_pPlayerPos = m_pPlayerTransformcomp->Get_Pos();

				GetRandomPointInCircle(&randomCenter, &m_pPlayerPos, 8.f);

				Engine::Add_GameObject(L"GameLogic", CRedThunder::Create(m_pGraphicDev,
					randomCenter.x * 2 ,
					randomCenter.y,
					randomCenter.z * 2, m_ePhase, this, (ETEAM_ID)Get_TeamID() ));
			}

			m_AttackOnce = true;
		}
		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
	}
}

void CAceBoss::SkillFire(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"��BOSS - Skill - SKILL_FIRE ����   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (!m_AttackOnce)
		{
			Engine::Play_Sound(L"Enemy", L"_SkillFire.wav", SOUND_ENEMY_FIRE, m_tSound.m_fSoundVolume);

			_vec3 Center = m_pTransformComp->Get_Pos();
			_vec3 spawnPosition;

			for (_int i = 0; i < 8; ++i)
			{
				_float angle = static_cast<_float>(i) / static_cast<_float>(8) * D3DX_PI * 2.0f;

				spawnPosition.x = Center.x + 4.f * cosf(angle);
				spawnPosition.y = Center.y;
				spawnPosition.z = Center.z + 4.f * sinf(angle);

				Engine::Add_GameObject(L"GameLogic", CSpawnFire::Create(m_pGraphicDev,
					spawnPosition.x, spawnPosition.y, spawnPosition.z, m_ePhase, this, (ETEAM_ID)Get_TeamID()));

			}
			m_AttackOnce = TRUE;
		}
		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
	}
}

#pragma endregion 