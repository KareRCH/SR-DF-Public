#include "BackGround.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include <ImguiMgr.h>

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CBackGround::CBackGround(const CBackGround& rhs)
	: Base(rhs)
{
}

CBackGround::~CBackGround()
{
}

CBackGround* CBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("BackGround Create Failed");
		return nullptr;
	}

	return pInstance;
}


HRESULT CBackGround::Add_Component()
{
	// Ʈ������ ������Ʈ
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	// ����
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	// �ؽ��� ������Ʈ
	NULL_CHECK_RETURN(m_pRightHandTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_RightTexture", L"Proto_RightHandTextureComp"), E_FAIL);
	// �ؽ��� ������Ʈ
	//NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_LogoTexture", L"Proto_LogoTextureComp"), E_FAIL);
	// �ؽ��� ������Ʈ
	NULL_CHECK_RETURN(m_pLeftHandTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_DYNAMIC, L"Comp_LeftTexture", L"Proto_LeftHandTextureComp"), E_FAIL);

	////���� - �۾��Ҷ� �ֱ� 
	//NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_MonsterTexture", L"Proto_BrownTextureComp"), E_FAIL);

	return S_OK;
}

HRESULT CBackGround::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pAnimationTool = dynamic_cast<CImguiAnimationTool*>
		(CImguiMgr::GetInstance()->Get_MapImguiWin()[L"AnimationTool"]);

	m_eCurType = m_pAnimationTool->Get_CurType();

	if (m_pAnimationTool->Get_Animation()->empty())
	{
		m_vecAnimation[*m_eCurType] = m_pAnimationTool->Get_Animation();
	}	
	m_fPriority[0] = -1.f;

	m_pTransformComp->Set_Pos({ 0.f, 0.f, 0.f });	// �̹��� ��ġ
	m_pTransformComp->Set_Scale({ 100.f, 100.f, 1.f });	// �̹��� ũ��

	m_pLeftHandTextureComp->Set_Pos({ 0.f, 0.f, 0.f });

	return S_OK;
}

_int CBackGround::Update_GameObject(const _float& fTimeDelta)
{

	SUPER::Update_GameObject(fTimeDelta);

	KeyInput();

	// ������� �˻�
	if (!m_vecAnimation[*m_eCurType]->empty())
	{
		// ������ ��� ����
		if (m_pAnimationTool->Get_FramePlaying())
		{
			// ���� �������� �ð�(������)���� ������Ű��


			// ���� �������� �ִ� �����ӿ� ������ ���
			if (m_pAnimationTool->Get_currentTime() > (*m_vecAnimation[*m_eCurType])[m_vecAnimation[*m_eCurType]->size() - 1].time)
			{
				// ���� ������ �ʱ�ȭ
				//m_pAnimationTool->Get_currentTime() = 0.f;

				// �ݺ� On/Off
				if (m_pAnimationTool->Get_Repetition())
				{
					m_pAnimationTool->Set_FramePlaying(false);
				}

				// �� �ð� �ʱ�ȭ
				m_pAnimationTool->Set_currentTime(0);
			}
		}
	}

	if (!m_vecAnimation[*m_eCurType]->empty()) // ������� üũ
	{
		//if (m_pAnimationTool->Get_FramePlaying()) // ��� ��ư�� ������ ��츸
		//{
			if (m_pAnimationTool->Get_currentTime() >= 0.f &&
				m_pAnimationTool->Get_currentTime() <= m_vecAnimation[*m_eCurType]->back().time)
			{
				//m_eAnimationInfo = m_vecAnimation[(int)m_iFrameCount].front();
				_uint iFrameIndex = 0U;
				for (_uint i = m_vecAnimation[*m_eCurType]->size() - (_uint)1; i >= 0; i--)
				{
					if ((*m_vecAnimation[*m_eCurType])[i].time <= m_pAnimationTool->Get_currentTime())
					{
						iFrameIndex = i;
						break;
					}
				}

				// Constant
				//m_fSizeX = (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.x;
				//m_fSizeY = (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.y;

				//m_fX = m_fSizeX * 0.5f; // ������ġ 
				//m_fY = m_fSizeY * 0.5f;

				//m_pTransformComp->Set_Pos({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.x,
				//							(*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.y,
				//							0.f });	// �̹��� ��ġ

				//m_pTransformComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });	// �̹��� ũ��


				// Linear
				if (iFrameIndex + 1U < m_vecAnimation[*m_eCurType]->size())
				{
					// Ű �����Ӱ� �ð� ��ȭ��
					fFrameTimeDelta = (*m_vecAnimation[*m_eCurType])[iFrameIndex + 1U].time - (*m_vecAnimation[*m_eCurType])[iFrameIndex].time;
					// ���� Ű �����ӽð����� ���� �ð� ��ȭ��
					fCurFrameTimeDelta = (m_pAnimationTool->Get_currentTime() - (*m_vecAnimation[*m_eCurType])[iFrameIndex].time);

					fSizeX_Delta = (*m_vecAnimation[*m_eCurType])[iFrameIndex + 1U].vScale.x - (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.x;
					fSizeX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
					fSizeY_Delta = (*m_vecAnimation[*m_eCurType])[iFrameIndex + 1U].vScale.y - (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.y;
					fSizeY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

					fRotX_Delta = (*m_vecAnimation[*m_eCurType])[iFrameIndex + 1U].vRot.x - (*m_vecAnimation[*m_eCurType])[iFrameIndex].vRot.x;
					fRotX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
					fRotY_Delta = (*m_vecAnimation[*m_eCurType])[iFrameIndex + 1U].vRot.y - (*m_vecAnimation[*m_eCurType])[iFrameIndex].vRot.y;
					fRotY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
					fRotZ_Delta = (*m_vecAnimation[*m_eCurType])[iFrameIndex + 1U].vRot.z - (*m_vecAnimation[*m_eCurType])[iFrameIndex].vRot.z;
					fRotZ_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

					fPosX_Delta = (*m_vecAnimation[*m_eCurType])[iFrameIndex + 1U].vPos.x - (*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.x;
					fPosX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
					fPosY_Delta = (*m_vecAnimation[*m_eCurType])[iFrameIndex + 1U].vPos.y - (*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.y;
					fPosY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

					m_pTransformComp->Set_Pos({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.x + fPosX_Delta,
												(*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.y + fPosY_Delta,
												0.f });	// �̹��� ��ġ

					m_pTransformComp->Set_Scale({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.x + fSizeX_Delta, 	// �̹��� ũ��
												  (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.y + fSizeY_Delta,
												  1.f });

					m_pTransformComp->Set_Rotation({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vRot.x + fRotX_Delta, 	// �̹��� ȸ��
													 (*m_vecAnimation[*m_eCurType])[iFrameIndex].vRot.y + fRotY_Delta,
													 (*m_vecAnimation[*m_eCurType])[iFrameIndex].vRot.z + fRotZ_Delta });
					
					// ���� ŰŸ��
					switch (*m_eCurType)
					{
						case KEYTYPE_LEFTHAND:
						{
							TextureNum[KEYTYPE_LEFTHAND] = (*m_vecAnimation[*m_eCurType])[iFrameIndex].texureframe;
							break;
						}
						case KEYTYPE_RIGHTHAND:
						{
							TextureNum[KEYTYPE_RIGHTHAND] = (*m_vecAnimation[*m_eCurType])[iFrameIndex].texureframe;
							break;
						}
					}
					
				}
				else
				{
					m_pTransformComp->Set_Scale({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.x, 	// �̹��� ũ��
												  (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.y,
												  1.f });

					m_pTransformComp->Set_Pos({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.x,
												(*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.y,
												0.f });	// �̹��� ��ġ

					// ���� ŰŸ��
					switch (*m_eCurType)
					{
					case KEYTYPE_LEFTHAND:
					{
						TextureNum[KEYTYPE_LEFTHAND] = (*m_vecAnimation[*m_eCurType])[iFrameIndex].texureframe;
						break;
					}
					case KEYTYPE_RIGHTHAND:
					{
						TextureNum[KEYTYPE_RIGHTHAND] = (*m_vecAnimation[*m_eCurType])[iFrameIndex].texureframe;
						break;
					}
					}
					
				}
				
			}
		//}

	}

	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CBackGround::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CBackGround::Render_GameObject()
{

	// �ؽ�ó�� Ʈ�������� ���� ���, DYNAMIC���� ������ �������� ���� �ʿ䰡 ����
	m_pRightHandTextureComp->Readjust_Transform();
	// �ؽ�ó ��� * �θ�(���ӿ�����Ʈ)�� ���
	m_pRightHandTextureComp->Set_TransformToWorld(*m_pTransformComp->Get_Transform());
	// ���� �ΰ��� ���� �ؽ�ó ��İ� �θ� ����� ������ �ΰ� ����� �� ����.

		// �ؽ�ó�� Ʈ�������� ���� ���, DYNAMIC���� ������ �������� ���� �ʿ䰡 ����
	//m_pLeftHandTextureComp->Readjust_Transform();
	// �ؽ�ó ��� * �θ�(���ӿ�����Ʈ)�� ���
	//m_pLeftHandTextureComp->Set_TransformToWorld(*m_pTransformComp->Get_Transform());
	// ���� �ΰ��� ���� �ؽ�ó ��İ� �θ� ����� ������ �ΰ� ����� �� ����.
	
	// �������� Render_Texture �Լ� �ȿ��� �ڵ����� �ؽ�ó�� ����� ����̽��� ����.(SetTransform(D3DTS_WORLD, �ؽ�ó ���))
	m_pLeftHandTextureComp->Render_Texture(TextureNum[KEYTYPE_LEFTHAND], true);
	m_pBufferComp->Render_Buffer();

	m_pRightHandTextureComp->Render_Texture(TextureNum[KEYTYPE_RIGHTHAND], true);
	m_pBufferComp->Render_Buffer();

	// �̰� �θ� ����� �ؽ�ó ��Ŀ� �״�� ���� ���, �ؽ�ó ������ ����� �ʿ���� �� ���
	//m_pTextureComp->Set_Transform(m_pTransformComp->Get_Transform());

	// m_pBackTextureComp�� ������ �Ͱ� m_pTextureComp�� ������ ��. �ΰ��� ��׶��忡�� ������ ��ġ, ũ��� �����ǹǷ� ��ȿ��.

	// Set_Transform : �ܼ� ��� �����
	// Readjust_Transform : �ؽ�ó�� ���� ��ǥ, ȸ��, ũ�⸦ �ؽ�ó�� ���� ���(Ʈ������)�� �����Ѵ�.
	// Set_TransformToWorld : �ؽ�ó ���� ��� * �θ� ���, ��İ���

	//m_pTextureComp->Render_Texture(0, true);
	//m_pBufferComp->Render_Buffer();
}

void CBackGround::KeyInput()
{
	// ������
	if (Engine::IsKey_Pressed(DIK_1))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");
	}

	// �޼�
	if (Engine::IsKey_Pressed(DIK_O))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_Hand");
	}

	// �޼�
	if (Engine::IsKey_Pressed(DIK_K))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"OpenHand");
	}

	// ������ ��¡
	if (Engine::IsKey_Pressed(DIK_J))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"RightHand_Charging");
	}
	
	// ����
	if (Engine::IsKey_Pressed(DIK_2))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Gun");
	}

	// �轼 �����
	if (Engine::IsKey_Pressed(DIK_3))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Thompson");
	}

	// ��������
	if (Engine::IsKey_Pressed(DIK_4))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe");
	}

	// ���ֺ�
	if (Engine::IsKey_Pressed(DIK_5))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"BeerBottle");
	}

	// ��������
	if (Engine::IsKey_Pressed(DIK_6))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan");
	}

	// �ָ� ��¡
	if (Engine::IsKey_Pressed(DIK_7))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"RightHand_Charging");
	}

	// �������� ��¡
	if (Engine::IsKey_Pressed(DIK_8))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan_Charging");
	}

	// �������� ��¡
	if (Engine::IsKey_Pressed(DIK_9))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe_Charging");
	}

	// ������
	if (Engine::IsKey_Pressed(DIK_0))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Kick");
	}

	// ���� ȸ��
	if (Engine::IsKey_Pressed(DIK_U))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Gun_Spin");
	}

	// ������
	if (Engine::IsKey_Pressed(DIK_Y))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Righter");
	}

	// ������Ʈ ������ (������Ʈ�� ���� ��츸 ���డ��)
	if (Engine::IsKey_Pressed(DIK_F))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"UnderThrow_RightHand");
	}

	// �޼� �޸���
	if (Engine::IsKey_Pressed(DIK_H))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_RunHand");
	}

	// ������ �޸���
	if (Engine::IsKey_Pressed(DIK_N))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_RunHand");
	}
}

// �ִϸ��̼� �ҷ�����
void CBackGround::LoadAnimationFromFile(const char* fileName)
{
	// .dat ���� Ȯ���ڸ� �߰��� ���� �̸� ����
	std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

	// ������ UTF-8�� ����
	std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		// ������ �� �� ���� ���� ���� ó��
		return;
	}

	m_vecAnimation[*m_eCurType]->clear();
	KEYFRAME keyframe;

	while (file >> keyframe.time >> keyframe.value >> keyframe.type >>
		keyframe.isEaseIn >> keyframe.isEaseOut >>
		keyframe.vScale.x >> keyframe.vScale.y >> keyframe.vScale.z >>
		keyframe.vRot.x >> keyframe.vRot.y >> keyframe.vRot.z >>
		keyframe.vPos.x >> keyframe.vPos.y >> keyframe.vPos.z) {
		m_vecAnimation[*m_eCurType]->push_back(keyframe);
	}

	file.close();
}

void CBackGround::Free()
{
	SUPER::Free();


}
