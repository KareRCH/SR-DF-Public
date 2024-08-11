#include "Renderer.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
	: m_hReadyResult(E_FAIL)
{
	D3DXMatrixIdentity(&m_matPersView);
	D3DXMatrixIdentity(&m_matOrthoView);
}

CRenderer::~CRenderer()
{
	Free();
}

void CRenderer::Free()
{
	Clear_RenderGroup();
}

HRESULT CRenderer::Ready_Renderer(const _uint iWidth, const _uint iHeight)
{
	// 뷰포트 사용가능 디폴트 8개
	_uint iViewportCount = static_cast<_uint>(EVIEWPORT::SIZE);
	m_vecViewport.reserve(iViewportCount);
	for (size_t i = 0; i < iViewportCount; i++)
	{
		D3DVIEWPORT9 UiViewPort({ 0, 0, iWidth, iHeight, 0.f, 1.f });
		m_vecViewport.push_back(UiViewPort);
	}
	
	// 뷰포트 사용가능 디폴트 8개
	_uint iViewportRTCount = static_cast<_uint>(EVIEWPORT_RT::SIZE);
	m_vecViewport_RT.reserve(iViewportRTCount);
	for (size_t i = 0; i < iViewportRTCount; i++)
	{
		D3DVIEWPORT9 UiViewPort({ 0, 0, iWidth, iHeight, 0.f, 1.f });
		m_vecViewport_RT.push_back(UiViewPort);
	}

	D3DXMatrixOrthoLH(&m_matOrthoProject, static_cast<float>(iWidth), static_cast<float>(iHeight), 0.f, 100.f);

	return m_hReadyResult = S_OK;
}

void CRenderer::Add_RenderGroup(RENDERID eType, CGameObject* pGameObject)
{
	if ((0 > eType || RENDER_END <= eType) || nullptr == pGameObject)
		return;

	m_RenderGroup[eType].push_back(pGameObject);
	pGameObject->AddRef();
}

void CRenderer::Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	// 렌더처리를 하는 종류에 따라 따로 모아서 처리한다.
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_AlphaTest(pGraphicDev); // + 성희 추가
	Render_UI(pGraphicDev);

	// 항상 처리 후 다음 프레임을 위해 초기화시킨다.
	Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_PRIORITY])
		iter->Render_GameObject();
}

// + 성희 추가
void CRenderer::Render_AlphaTest(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 160);

	for (auto& iter : m_RenderGroup[RENDER_ALPHATEST])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_NONALPHA])
		iter->Render_GameObject();
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	for (auto& iter : m_RenderGroup[RENDER_ALPHA])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetTransform(D3DTS_VIEW, &m_matOrthoView);						// 직교용 뷰 행렬 적용.
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matOrthoProject);			// 직교투영 행렬 적용.
	pGraphicDev->SetViewport(&m_vecViewport[0]);								// 뷰포트 설정.
	
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);						// Z버퍼 OFF
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);					// 알파렌더링 ON
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	for (auto& iter : m_RenderGroup[RENDER_UI])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);						// Z버퍼 ON
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);					// 알파렌더링 OFF

}

