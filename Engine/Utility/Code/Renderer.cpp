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
	// ����Ʈ ��밡�� ����Ʈ 8��
	_uint iViewportCount = static_cast<_uint>(EVIEWPORT::SIZE);
	m_vecViewport.reserve(iViewportCount);
	for (size_t i = 0; i < iViewportCount; i++)
	{
		D3DVIEWPORT9 UiViewPort({ 0, 0, iWidth, iHeight, 0.f, 1.f });
		m_vecViewport.push_back(UiViewPort);
	}
	
	// ����Ʈ ��밡�� ����Ʈ 8��
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
	// ����ó���� �ϴ� ������ ���� ���� ��Ƽ� ó���Ѵ�.
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_AlphaTest(pGraphicDev); // + ���� �߰�
	Render_UI(pGraphicDev);

	// �׻� ó�� �� ���� �������� ���� �ʱ�ȭ��Ų��.
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

// + ���� �߰�
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
	pGraphicDev->SetTransform(D3DTS_VIEW, &m_matOrthoView);						// ������ �� ��� ����.
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matOrthoProject);			// �������� ��� ����.
	pGraphicDev->SetViewport(&m_vecViewport[0]);								// ����Ʈ ����.
	
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);						// Z���� OFF
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);					// ���ķ����� ON
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	for (auto& iter : m_RenderGroup[RENDER_UI])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);						// Z���� ON
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);					// ���ķ����� OFF

}

