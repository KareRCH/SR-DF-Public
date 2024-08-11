#pragma once

#include "Base.h"
#include "Engine_Define.h"



//class FComponentData
//{
//public:
//
//};
//
//class FObjectData
//{
//public:
//	vector<FComponentData>		m_vecComponent;
//};
//
//class FLayerData
//{
//public:
//	vector<FObjectData>			m_vecObject;
//};
//
//class FSceneData
//{
//public:
//	vector<FLayerData>			m_vecTable;
//};


class CProjectDataTable : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CProjectDataTable)
	
private:
	explicit CProjectDataTable() {}
	virtual ~CProjectDataTable() { Free(); }

public:
	virtual void	Free();

public:
	// ������Ʈ�� �ε��Ͽ� ������ �޸𸮿� �ε��ϴ� ��.
	// ���� ������ �ƴ� ������ ��ġ ������ �ε��մϴ�.
	void			Load_Project();

private:
	//map<string, FSceneData>		m_mapTable;
};

