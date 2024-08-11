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
	// 트랜스폼 컴포넌트
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	// 버퍼
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	// 텍스쳐 컴포넌트
	NULL_CHECK_RETURN(m_pRightHandTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_RightTexture", L"Proto_RightHandTextureComp"), E_FAIL);
	// 텍스쳐 컴포넌트
	//NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_LogoTexture", L"Proto_LogoTextureComp"), E_FAIL);
	// 텍스쳐 컴포넌트
	NULL_CHECK_RETURN(m_pLeftHandTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_DYNAMIC, L"Comp_LeftTexture", L"Proto_LeftHandTextureComp"), E_FAIL);

	////몬스터 - 작업할때 넣기 
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

	m_pTransformComp->Set_Pos({ 0.f, 0.f, 0.f });	// 이미지 위치
	m_pTransformComp->Set_Scale({ 100.f, 100.f, 1.f });	// 이미지 크기

	m_pLeftHandTextureComp->Set_Pos({ 0.f, 0.f, 0.f });

	return S_OK;
}

_int CBackGround::Update_GameObject(const _float& fTimeDelta)
{

	SUPER::Update_GameObject(fTimeDelta);

	KeyInput();

	// 비었는지 검사
	if (!m_vecAnimation[*m_eCurType]->empty())
	{
		// 프레임 재생 여부
		if (m_pAnimationTool->Get_FramePlaying())
		{
			// 현재 프레임을 시간(프레임)마다 증가시키기


			// 현재 프레임이 최대 프레임에 도달한 경우
			if (m_pAnimationTool->Get_currentTime() > (*m_vecAnimation[*m_eCurType])[m_vecAnimation[*m_eCurType]->size() - 1].time)
			{
				// 현재 프레임 초기화
				//m_pAnimationTool->Get_currentTime() = 0.f;

				// 반복 On/Off
				if (m_pAnimationTool->Get_Repetition())
				{
					m_pAnimationTool->Set_FramePlaying(false);
				}

				// 툴 시간 초기화
				m_pAnimationTool->Set_currentTime(0);
			}
		}
	}

	if (!m_vecAnimation[*m_eCurType]->empty()) // 비었는지 체크
	{
		//if (m_pAnimationTool->Get_FramePlaying()) // 재생 버튼을 눌렀을 경우만
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

				//m_fX = m_fSizeX * 0.5f; // 중점위치 
				//m_fY = m_fSizeY * 0.5f;

				//m_pTransformComp->Set_Pos({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.x,
				//							(*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.y,
				//							0.f });	// 이미지 위치

				//m_pTransformComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });	// 이미지 크기


				// Linear
				if (iFrameIndex + 1U < m_vecAnimation[*m_eCurType]->size())
				{
					// 키 프레임간 시간 변화율
					fFrameTimeDelta = (*m_vecAnimation[*m_eCurType])[iFrameIndex + 1U].time - (*m_vecAnimation[*m_eCurType])[iFrameIndex].time;
					// 현재 키 프레임시간부터 현재 시간 변화율
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
												0.f });	// 이미지 위치

					m_pTransformComp->Set_Scale({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.x + fSizeX_Delta, 	// 이미지 크기
												  (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.y + fSizeY_Delta,
												  1.f });

					m_pTransformComp->Set_Rotation({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vRot.x + fRotX_Delta, 	// 이미지 회전
													 (*m_vecAnimation[*m_eCurType])[iFrameIndex].vRot.y + fRotY_Delta,
													 (*m_vecAnimation[*m_eCurType])[iFrameIndex].vRot.z + fRotZ_Delta });
					
					// 현재 키타입
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
					m_pTransformComp->Set_Scale({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.x, 	// 이미지 크기
												  (*m_vecAnimation[*m_eCurType])[iFrameIndex].vScale.y,
												  1.f });

					m_pTransformComp->Set_Pos({ (*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.x,
												(*m_vecAnimation[*m_eCurType])[iFrameIndex].vPos.y,
												0.f });	// 이미지 위치

					// 현재 키타입
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

	// 텍스처의 트랜스폼을 월드 행렬, DYNAMIC으로 설정시 수동으로 해줄 필요가 없음
	m_pRightHandTextureComp->Readjust_Transform();
	// 텍스처 행렬 * 부모(게임오브젝트)의 행렬
	m_pRightHandTextureComp->Set_TransformToWorld(*m_pTransformComp->Get_Transform());
	// 위의 두개만 쓰면 텍스처 행렬과 부모 행렬을 별개로 두고 계산할 수 있음.

		// 텍스처의 트랜스폼을 월드 행렬, DYNAMIC으로 설정시 수동으로 해줄 필요가 없음
	//m_pLeftHandTextureComp->Readjust_Transform();
	// 텍스처 행렬 * 부모(게임오브젝트)의 행렬
	//m_pLeftHandTextureComp->Set_TransformToWorld(*m_pTransformComp->Get_Transform());
	// 위의 두개만 쓰면 텍스처 행렬과 부모 행렬을 별개로 두고 계산할 수 있음.
	
	// 이제부터 Render_Texture 함수 안에서 자동으로 텍스처의 행렬이 디바이스에 들어간다.(SetTransform(D3DTS_WORLD, 텍스처 행렬))
	m_pLeftHandTextureComp->Render_Texture(TextureNum[KEYTYPE_LEFTHAND], true);
	m_pBufferComp->Render_Buffer();

	m_pRightHandTextureComp->Render_Texture(TextureNum[KEYTYPE_RIGHTHAND], true);
	m_pBufferComp->Render_Buffer();

	// 이건 부모 행렬을 텍스처 행렬에 그대로 쓰는 방법, 텍스처 별개의 행렬이 필요없을 때 사용
	//m_pTextureComp->Set_Transform(m_pTransformComp->Get_Transform());

	// m_pBackTextureComp에 적용한 것과 m_pTextureComp에 적용한 것. 두개다 백그라운드에서 동일한 위치, 크기로 설정되므로 유효함.

	// Set_Transform : 단순 행렬 덮어쓰기
	// Readjust_Transform : 텍스처의 로컬 좌표, 회전, 크기를 텍스처의 월드 행렬(트랜스폼)에 적용한다.
	// Set_TransformToWorld : 텍스처 월드 행렬 * 부모 행렬, 행렬곱임

	//m_pTextureComp->Render_Texture(0, true);
	//m_pBufferComp->Render_Buffer();
}

void CBackGround::KeyInput()
{
	// 오른손
	if (Engine::IsKey_Pressed(DIK_1))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");
	}

	// 왼손
	if (Engine::IsKey_Pressed(DIK_O))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_Hand");
	}

	// 왼손
	if (Engine::IsKey_Pressed(DIK_K))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"OpenHand");
	}

	// 오른손 차징
	if (Engine::IsKey_Pressed(DIK_J))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"RightHand_Charging");
	}
	
	// 권총
	if (Engine::IsKey_Pressed(DIK_2))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Gun");
	}

	// 톰슨 기관총
	if (Engine::IsKey_Pressed(DIK_3))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Thompson");
	}

	// 쇠파이프
	if (Engine::IsKey_Pressed(DIK_4))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe");
	}

	// 맥주병
	if (Engine::IsKey_Pressed(DIK_5))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"BeerBottle");
	}

	// 프라이팬
	if (Engine::IsKey_Pressed(DIK_6))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan");
	}

	// 주먹 차징
	if (Engine::IsKey_Pressed(DIK_7))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"RightHand_Charging");
	}

	// 프라이팬 차징
	if (Engine::IsKey_Pressed(DIK_8))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan_Charging");
	}

	// 쇠파이프 차징
	if (Engine::IsKey_Pressed(DIK_9))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe_Charging");
	}

	// 발차기
	if (Engine::IsKey_Pressed(DIK_0))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Kick");
	}

	// 권총 회전
	if (Engine::IsKey_Pressed(DIK_U))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Gun_Spin");
	}

	// 라이터
	if (Engine::IsKey_Pressed(DIK_Y))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Righter");
	}

	// 오브젝트 버리기 (오브젝트가 있을 경우만 실행가능)
	if (Engine::IsKey_Pressed(DIK_F))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"UnderThrow_RightHand");
	}

	// 왼손 달리기
	if (Engine::IsKey_Pressed(DIK_H))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_RunHand");
	}

	// 오른손 달리기
	if (Engine::IsKey_Pressed(DIK_N))
	{
		m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_RunHand");
	}
}

// 애니메이션 불러오기
void CBackGround::LoadAnimationFromFile(const char* fileName)
{
	// .dat 파일 확장자를 추가한 파일 이름 생성
	std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

	// 파일을 UTF-8로 열기
	std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		// 파일을 열 수 없을 때의 오류 처리
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
