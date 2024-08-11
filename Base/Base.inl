CBase::CBase()
	: m_dwRefCnt(0)
{

}
CBase::~CBase()
{

}

unsigned	long		CBase::AddRef()
{
	return ++m_dwRefCnt;
}
CBase*					CBase::Get_ByRef()
{
	AddRef();
	return this;
}
template <class T>
T*						CBase::Get_ByRef()
{
	AddRef();
	return dynamic_cast<T*>(this);
}
unsigned	long		CBase::Get_RefCount()
{
	return m_dwRefCnt;
}
unsigned	long		CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}

	return m_dwRefCnt--;
}