#pragma once
#include "GameObject.h"
#include "Engine_Define.h"

// 오류중 코드 페이지 949는 주로 한글을 의미함.
// 시스템 내의 모든 파티클에 영향을 주는 전역 특성들을 관리
// 파티클의 갱신과 디스플레이 소멸, 생성 등을 관장하는 역할

BEGIN(Engine)

class ENGINE_DLL CPsystem : public CGameObject
{
	DERIVED_CLASS(CGameObject, CPsystem)

public:
	explicit CPsystem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPsystem(const CPsystem& rhs);
	virtual ~CPsystem();

public:
	HRESULT		Ready_GameObject(const _tchar* texFileName);	// 포인트 스프라이트를 저장하기위한 버텍스 버퍼를 만들고 텍스처를 만드는등 초기화작업 처리하는곳 - 용책의 bool init에 해당 
	_int		Update_GameObject(const _float& fTimeDelta);
	void		LateUpdate_GameObject();
	void		Render_GameObject();

protected:
	virtual void ResetParticle(Attribute* attribute) PURE; // 파티클마다 리셋되는 방식이 다르다.
	
public:
	virtual void AddParticle(); // 시스템에 파티클을 추가 
	virtual void Free();
	virtual void RemoveDeadParticles(); // 속성 리스트 _particle을 검색하여 죽은 파티클을 리스트에서 제거.

private:
	virtual void ResetList(); //시스템 내의 모든 파티클 속성을 리셋
	virtual void preRender();	// 렌더링에 앞서 지정해야 할 초기 렌더 상태를 지정
	virtual void postRender();  // 특정 파티클 시스템이 지정했을 수 있는 렌더 상태를 복구하는 데 이용

public:

protected:
	_bool isEmpty() { return m_ParticleList.empty(); }   // 현재 시스템에 파티클이 없는 경우 true 리턴.
	_bool isDead(); // 시스템 내의 파티클이 모두 죽은 경우 true 리턴.
	
	void Get_RandomVector(_vec3* out, _vec3* min, _vec3* max)
	{
		out->x = Get_RandomFloat(min->x, max->x);
		out->y = Get_RandomFloat(min->y, max->y);
		out->z = Get_RandomFloat(min->z, max->z);
	}

	_float Get_RandomFloat(_float lowBound, _float hightBound)
	{
		if (lowBound >= hightBound)
			return lowBound;

		_float f = (rand() % 10000) * 0.0001f;

		return (f * (hightBound - lowBound)) + lowBound;
	}

protected:
	DWORD m_dSize;       // 버텍스 버퍼가 보관할 수 있는 파티클의 수- 실제 파티클 시스템 내의 파티클 수와는 독립적.
	DWORD m_dOffset;     // 파티클 시스템의 렌더링에 이용.
	DWORD m_dBatchSize;  // 파티클 시스템의 렌더링에 이용.

	LPDIRECT3DVERTEXBUFFER9			m_pVB;
	LPDIRECT3DTEXTURE9              m_pTexture;
	BoundingBox						m_BoundingBox;    // 파티클이 이동할 수 있는 부피를 제한하는 데 이용. -> 원하는 영역을 경계상자로 정의하면 이 영역을 벗어난 파티클들을 곧바로 소멸.
	
	list<Attribute>					m_ParticleList;   // 시스템 내 파티클 속성의 리스트.
	_vec3							m_vOrigin;		  // 시스템 내에서 파티클이 시작되는 곳.
	_float							m_fEmitRate;	  // 시스템에 새로운 파티클이 추가되는 비율. 보통은 초당 파티클 수로 기록.
	_float							m_fSize;		  // 시스템 내 모든 파티클의 크기
	_int							m_iMaxParticles;  // 주어진 시간 동안 시스템이 가질 수 있는 최대 파티클의 수.

};

END

// 우리는 파티클을 만들고 제거하고 갱신하는 데 list<Attribute> 리스트를 이용.
// 파티클을 그릴 준비가 완료되면 리스트 노드의 일부를 버텍스 버퍼로 복사하고 파티클 드로잉 과정을 거친다.
// 이어 다음 단계의 데이터를 복사하고 다시 파티클을 그리며, 모든 파티클을 그릴 때까지 이 과정을 반복