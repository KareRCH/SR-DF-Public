#pragma once
#include "GameObject.h"
#include "Engine_Define.h"

// ������ �ڵ� ������ 949�� �ַ� �ѱ��� �ǹ���.
// �ý��� ���� ��� ��ƼŬ�� ������ �ִ� ���� Ư������ ����
// ��ƼŬ�� ���Ű� ���÷��� �Ҹ�, ���� ���� �����ϴ� ����

BEGIN(Engine)

class ENGINE_DLL CPsystem : public CGameObject
{
	DERIVED_CLASS(CGameObject, CPsystem)

public:
	explicit CPsystem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPsystem(const CPsystem& rhs);
	virtual ~CPsystem();

public:
	HRESULT		Ready_GameObject(const _tchar* texFileName);	// ����Ʈ ��������Ʈ�� �����ϱ����� ���ؽ� ���۸� ����� �ؽ�ó�� ����µ� �ʱ�ȭ�۾� ó���ϴ°� - ��å�� bool init�� �ش� 
	_int		Update_GameObject(const _float& fTimeDelta);
	void		LateUpdate_GameObject();
	void		Render_GameObject();

protected:
	virtual void ResetParticle(Attribute* attribute) PURE; // ��ƼŬ���� ���µǴ� ����� �ٸ���.
	
public:
	virtual void AddParticle(); // �ý��ۿ� ��ƼŬ�� �߰� 
	virtual void Free();
	virtual void RemoveDeadParticles(); // �Ӽ� ����Ʈ _particle�� �˻��Ͽ� ���� ��ƼŬ�� ����Ʈ���� ����.

private:
	virtual void ResetList(); //�ý��� ���� ��� ��ƼŬ �Ӽ��� ����
	virtual void preRender();	// �������� �ռ� �����ؾ� �� �ʱ� ���� ���¸� ����
	virtual void postRender();  // Ư�� ��ƼŬ �ý����� �������� �� �ִ� ���� ���¸� �����ϴ� �� �̿�

public:

protected:
	_bool isEmpty() { return m_ParticleList.empty(); }   // ���� �ý��ۿ� ��ƼŬ�� ���� ��� true ����.
	_bool isDead(); // �ý��� ���� ��ƼŬ�� ��� ���� ��� true ����.
	
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
	DWORD m_dSize;       // ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ�� ��- ���� ��ƼŬ �ý��� ���� ��ƼŬ ���ʹ� ������.
	DWORD m_dOffset;     // ��ƼŬ �ý����� �������� �̿�.
	DWORD m_dBatchSize;  // ��ƼŬ �ý����� �������� �̿�.

	LPDIRECT3DVERTEXBUFFER9			m_pVB;
	LPDIRECT3DTEXTURE9              m_pTexture;
	BoundingBox						m_BoundingBox;    // ��ƼŬ�� �̵��� �� �ִ� ���Ǹ� �����ϴ� �� �̿�. -> ���ϴ� ������ �����ڷ� �����ϸ� �� ������ ��� ��ƼŬ���� ��ٷ� �Ҹ�.
	
	list<Attribute>					m_ParticleList;   // �ý��� �� ��ƼŬ �Ӽ��� ����Ʈ.
	_vec3							m_vOrigin;		  // �ý��� ������ ��ƼŬ�� ���۵Ǵ� ��.
	_float							m_fEmitRate;	  // �ý��ۿ� ���ο� ��ƼŬ�� �߰��Ǵ� ����. ������ �ʴ� ��ƼŬ ���� ���.
	_float							m_fSize;		  // �ý��� �� ��� ��ƼŬ�� ũ��
	_int							m_iMaxParticles;  // �־��� �ð� ���� �ý����� ���� �� �ִ� �ִ� ��ƼŬ�� ��.

};

END

// �츮�� ��ƼŬ�� ����� �����ϰ� �����ϴ� �� list<Attribute> ����Ʈ�� �̿�.
// ��ƼŬ�� �׸� �غ� �Ϸ�Ǹ� ����Ʈ ����� �Ϻθ� ���ؽ� ���۷� �����ϰ� ��ƼŬ ����� ������ ��ģ��.
// �̾� ���� �ܰ��� �����͸� �����ϰ� �ٽ� ��ƼŬ�� �׸���, ��� ��ƼŬ�� �׸� ������ �� ������ �ݺ�