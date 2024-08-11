#ifndef Base_h__
#define Base_h__

class _declspec(dllexport) CBase
{
protected:
	inline	explicit CBase();
	inline  virtual ~CBase();

public:
	inline	unsigned	long		AddRef();
	inline	unsigned	long		Get_RefCount();
	inline	unsigned	long		Release();

	// RefCount�� ������Ű�鼭 ���� ���� �Լ�.
	// [����] ��������� �ش��ϴ� ������ ���ÿ�.

	inline	CBase*					Get_ByRef();
	template <class T>
	inline	T*						Get_ByRef();

private:
	unsigned long			m_dwRefCnt;

public:
	inline virtual void	Free() = 0;
};

#include "Base.inl"

#endif // Base_h__
