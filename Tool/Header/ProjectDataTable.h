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
	// 프로젝트를 로드하여 정보를 메모리에 로드하는 것.
	// 실제 정보가 아닌 파일의 위치 정보를 로드합니다.
	void			Load_Project();

private:
	//map<string, FSceneData>		m_mapTable;
};

