#include "VIBufferComp.h"

CVIBufferComp::CVIBufferComp()
    : m_pVB(nullptr), m_pIB(nullptr), m_dwVtxCnt(0), m_dwTriCnt(0), m_dwVtxSize(0), m_dwFVF(0), m_dwIdxSize(0)
{
}

CVIBufferComp::CVIBufferComp(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev), 
    m_pVB(nullptr), m_pIB(nullptr), m_dwVtxCnt(0), m_dwTriCnt(0), m_dwVtxSize(0), m_dwFVF(0), m_dwIdxSize(0)
{
}

CVIBufferComp::CVIBufferComp(const CVIBufferComp& rhs)
    : Base(rhs)
    , m_pVB(rhs.m_pVB)
    , m_pIB(rhs.m_pIB)
    , m_dwVtxCnt(rhs.m_dwVtxCnt)
    , m_dwTriCnt(rhs.m_dwTriCnt)
    , m_dwVtxSize(rhs.m_dwVtxSize)
    , m_dwFVF(rhs.m_dwFVF)
    , m_dwIdxSize(rhs.m_dwIdxSize)
    , m_IdxFmt(rhs.m_IdxFmt)
{
    m_pVB->AddRef();
    m_pIB->AddRef();
}

CVIBufferComp::~CVIBufferComp()
{
}

HRESULT CVIBufferComp::Ready_Buffer()
{
    FAILED_CHECK_RETURN(m_pGraphicDev->CreateVertexBuffer(m_dwVtxCnt * m_dwVtxSize,	// 생성할 버퍼의 크기
        0,		// 생성하고자 하는 버텍스 버퍼의 종류(0인 경우 정적 버퍼, D3DUSAGE_DYNAMIC)
        m_dwFVF,	// 버텍스 속성 옵션
        D3DPOOL_MANAGED,	// 메모리 풀 방식(정적 버퍼인 경우 MANAGED)
        &m_pVB,				// 결과 컴 객체
        NULL), E_FAIL);

    FAILED_CHECK_RETURN(m_pGraphicDev->CreateIndexBuffer(m_dwTriCnt * m_dwIdxSize,	// 생성할 버퍼의 크기
        0,		// 생성하고자 하는 인덱스 버퍼의 종류(0인 경우 정적 버퍼, D3DUSAGE_DYNAMIC)
        m_IdxFmt,	// 인덱스 속성 옵션
        D3DPOOL_MANAGED,	// 메모리 풀 방식(정적 버퍼인 경우 MANAGED)
        &m_pIB,				// 결과 컴 객체
        NULL), E_FAIL);

    return S_OK;
}

void CVIBufferComp::Render_Buffer()
{
    // 디바이스 장치와 링크시키려는 버퍼를 지정하는 함수

    // 1인자 : 몇 번 스트림에 버퍼를 전송할 것인가
    // 2인자 : 어떤 대상을 디바이스 장치와 링크 시킬 것인가
    // 3인자 : 어디서부터 전달할 것인가
    // 4인자 : 어떤 크기로 전달할 것인가
    m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);

    // 매 프레임마다 정점을 속성 값을 장치에게 전달
    m_pGraphicDev->SetFVF(m_dwFVF);

    m_pGraphicDev->SetIndices(m_pIB);

    m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
    // 1. 삼각형을 그리는 방식
    // 2. 버텍스의 인덱스 값(몇 번 인덱스의 정점부터 그릴 것인가)
    // 3. 버텍스 버퍼에 들어가 있는 정점들 중 몇 번째 부터 그릴 것인가
    // 4. 내가 사용하고자하는 버텍스의 개수
    // 5. 인덱스 버퍼에 들어가 있는 인덱스 중 몇 번때부터 그릴 것인가
    // 6. 그리려는 삼각형 개수

    // 실제 그리기 함수
    // 1인자 : 삼각형을 그리는 방식
    // 2인자 : 몇 번째 버텍스부터 그릴 것인가
    // 3인자 : 그리려는 삼각형 개수

    //m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwTriCnt);
}

void CVIBufferComp::Free()
{
    Safe_Release(m_pIB);
    Safe_Release(m_pVB);

    CComponent::Free();
}
