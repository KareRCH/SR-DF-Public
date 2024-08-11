#include "GameObject.h"

_ulonglong CGameObject::g_dwID_Count = 0ULL;

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev)
    , m_bIsDead(false), m_strObjectName(L" "), m_dwObjectID(g_dwID_Count++), m_dwObjectState(0U)
{
    m_pGraphicDev->AddRef();

    // 기본 우선도 설정
    ZeroMemory(m_fPriority, static_cast<_uint>(EPRIORITY::SIZE));
    for (_uint i = 0; i < static_cast<_uint>(EPRIORITY::SIZE); i++)
        m_bUsePriority[i] = true;
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_pGraphicDev(rhs.m_pGraphicDev)
    , m_bIsDead(false), m_strObjectName(rhs.m_strObjectName), m_dwObjectID(g_dwID_Count++), m_dwObjectState(rhs.m_dwObjectState)
{
    m_pGraphicDev->AddRef();
}

CGameObject::~CGameObject()
{

}

void CGameObject::Free()
{
    for (size_t i = 0; i < ID_END; i++)
    {
        for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
        m_mapComponent[i].clear();
    }

    Safe_Release(m_pGraphicDev);
}

CComponent* CGameObject::Get_Component(COMPONENTID eID, const _tchar* pComponentTag)
{
    // 외부에서 컴포넌트에 대한 주소를 얻어올 수 있도록 하는 함수
    // 오브젝트 -> 컴포넌트
    CComponent* pComponent = Find_Component(pComponentTag, eID);
    
    NULL_CHECK_RETURN(pComponent, nullptr);

    return pComponent;
}

HRESULT CGameObject::Bind_Component(COMPONENTID eID, CComponent*& pComponent, const _tchar* pComponentTag)
{
    auto result = m_mapComponent[eID].emplace(pComponentTag, pComponent);

    // 컴포넌트 연결 실패
    if (!result.second)
        return E_FAIL;

    pComponent->Set_Owner(this);

    return S_OK;
}

HRESULT CGameObject::Ready_GameObject()
{
    return S_OK;
}

_int CGameObject::Update_GameObject(const _float& fTimeDelta)
{
    // 프레임 업데이트를 수행해야하는 컴포넌트 집단에 대해 업데이트 수행
    for (auto& iter : m_mapComponent[ID_DYNAMIC])
        iter.second->Update_Component(fTimeDelta);
    
    return 0;
}

void CGameObject::LateUpdate_GameObject()
{
    // 프레임 업데이트를 수행해야하는 컴포넌트 집단에 대해 업데이트 수행
    for (auto& iter : m_mapComponent[ID_DYNAMIC])
        iter.second->LateUpdate_Component();
}

void Engine::CGameObject::Compute_ViewZ(const _vec3* pPos)
{
    _matrix matCamWorld;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);

    D3DXMatrixInverse(&matCamWorld, 0, &matCamWorld);

    _vec3 vCamPos;
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));
    //m_fViewZ = D3DXVec3Length(&(vCamPos - *pPos));
}

void CGameObject::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
    if (m_MeshBox == nullptr)
    D3DXCreateBox(m_pGraphicDev, _Width, _Height, Depth, &m_MeshBox, NULL);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    m_MeshBox->DrawSubset(0);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CGameObject::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
    LPD3DXBUFFER* MeshBuffer = nullptr;

    if (m_MeshSphere == nullptr)
        D3DXCreateSphere(m_pGraphicDev, Radius, Slices, Stacks, &m_MeshSphere, MeshBuffer);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    m_MeshSphere->DrawSubset(0);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}


HRESULT CGameObject::Delete_Component(CComponent* pComponent)
{
    for (size_t i = 0; i <= (size_t)ID_STATIC; i++)
    {
        auto iter = find_if(m_mapComponent[i].begin(), m_mapComponent[i].end(),
            [&pComponent](auto& pairComp) {
                return pairComp.second == pComponent;
            });
        if (iter == m_mapComponent[i].end())
            continue;

        m_mapComponent[i].erase(iter);
        Safe_Release(pComponent);
        pComponent = nullptr;
    }

    return S_OK;
}

CComponent* CGameObject::Find_Component(const _tchar* pComponentTag, COMPONENTID eID)
{
    // 내부적으로 컴포넌트를 태그를 통해 찾도록 지원하는 함수
    //auto iter = find_if(m_mapComponent[eID].begin(), m_mapComponent[eID].end(), CTag_Finder(pComponentTag));
    auto iter = m_mapComponent[eID].find(pComponentTag);

    if (iter == m_mapComponent[eID].end())
        return nullptr;

    return iter->second;
}

