#include "TextureComponent.h"

#include "TextureMgr.h"

CTextureComponent::CTextureComponent()
{
}

CTextureComponent::CTextureComponent(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTextureComponent::CTextureComponent(const CTextureComponent& rhs)
    : Base(rhs)
{
    // 벡터는 공간 예약 후 복사
    size_t	iSize = rhs.m_vecTexture.size();
    m_vecTexture.reserve(iSize);

    m_vecTexture = rhs.m_vecTexture;

    // 텍스처별로 레퍼카운트 증가
    for (size_t i = 0; i < iSize; ++i)
        m_vecTexture[i]->AddRef();
}

CTextureComponent::~CTextureComponent()
{
}

CTextureComponent* CTextureComponent::Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eID, const _tchar* pTextureKey, const _tchar* pStateKey)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Component(eID, pTextureKey, pStateKey)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Texture Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CTextureComponent::Clone()
{
	return new ThisClass(*this);
}

void CTextureComponent::Free()
{
	SUPER::Free();
}

HRESULT CTextureComponent::Ready_Component(TEXTUREID eID, const _tchar* pTextureKey, const _tchar* pStateKey)
{
	return Receive_Texture(eID, pTextureKey, pStateKey);
}

_int CTextureComponent::Update_Component(const _float& fTimeDelta)
{
	SUPER::Update_Component(fTimeDelta);

	Readjust_Transform();

	return 0;
}

void CTextureComponent::LateUpdate_Component()
{
}

void CTextureComponent::Render_Component(const _uint& iIndex, _bool bLocalTransform)
{
	Render_Texture(iIndex, bLocalTransform);
}

void CTextureComponent::Render_Texture(const _uint& iIndex, _bool bLocalTransform)
{
	// 텍스처의 월드 트랜스폼으로 출력
	if (bLocalTransform)
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matTransform);

	// 인덱스 오버플로우 막기
	if (m_vecTexture.size() <= iIndex)
		return;

	m_pGraphicDev->SetTexture(0, m_vecTexture[iIndex]);
}

HRESULT CTextureComponent::Receive_Texture(TEXTUREID eID, const _tchar* pTextureKey, const _tchar* pStateKey)
{
	return CTextureMgr::GetInstance()->Transfer_Texture(&m_vecTexture, eID, pTextureKey, pStateKey);
}