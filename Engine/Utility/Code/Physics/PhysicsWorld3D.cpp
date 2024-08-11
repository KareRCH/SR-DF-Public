#include "PhysicsWorld3D.h"

#include "Collide.h"
#include "Contact.h"

CPhysicsWorld3D::CPhysicsWorld3D()
{
}

CPhysicsWorld3D::~CPhysicsWorld3D()
{
	Free();
}

CPhysicsWorld3D* CPhysicsWorld3D::Create(_uint iMaxContacts, _uint iIterations)
{
	ThisClass* pInstance = new ThisClass();

	if (FAILED(pInstance->Ready_Physics(iMaxContacts, iIterations)))
	{
		Safe_Release(pInstance);

		MSG_BOX("PhysicsWorld3D Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPhysicsWorld3D::Free()
{

}

HRESULT CPhysicsWorld3D::Ready_Physics(_uint iMaxContacts, _uint iIterations)
{
	m_ContactResolver.Set_Iterations(iMaxContacts, iIterations);

	return S_OK;
}

void CPhysicsWorld3D::StartFrame_Physics()
{
	if (m_bIsPaused)
		return;

	// 힘 제거, 완료
	/*for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		(*iter)->Clear_Accumulators();
		(*iter)->CalculateDerivedData();
	}*/
}

_int CPhysicsWorld3D::Update_Physics(const Real& fTimeDelta)
{
	if (m_bIsPaused)
		return 0;

	// 힘 더하기, 강체 위치 기반으로 충돌체 위치 수정
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		(*iter)->Integrate(fTimeDelta);

		FCollisionPrimitive* pCol = static_cast<FCollisionPrimitive*>((*iter)->Get_Owner());
		pCol->Calculate_Transform();
		pCol->Set_Position(pCol->Get_Position());
		pCol->Calculate_Shape();

		
	}

	// 접촉 발생기
	_uint iUsedContacts = Generate_Contacts();


	//// 힘 발생기와 접촉 발생기로 나온 결과로 충돌을 해결
	//if (m_bCalculateIterations)
	//	m_ContactResolver.Set_Iterations(iUsedContacts * 4);
	//m_ContactResolver.ResolveContacts(m_pContacts, iUsedContacts, fTimeDelta);

	return 0;
}

_uint CPhysicsWorld3D::Generate_Contacts()
{
	_uint iLimit = m_iMaxContacts;
	FContact* pNextContact = m_pContacts;

	// 충돌발생
	//for (auto iter = m_ConGenList.begin(); iter != m_ConGenList.end(); ++iter)
	//{
	//	_uint iUsed = (*iter)->Add_Contact(pNextContact, iLimit);
	//	iLimit -= iUsed;
	//	pNextContact += iUsed;

	//	// 리미트를 초과하면 더 이상 접촉처리를 하지 않는다.
	//	if (iLimit <= 0) break;
	//}

	struct FDistPoint
	{
		FRigidBody* pBody = nullptr;
		Real fDist = -1.f;

		bool operator < (const FDistPoint& other) const
		{
			return fDist < other.fDist;
		}
	};

	struct FEndPoint
	{
		FRigidBody* pBody = nullptr;
		Real fPoint = -1.f;
		bool bIsStart = false;

		bool operator < (const FEndPoint& other) const
		{
			return fPoint < other.fPoint;
		}
	};

	// D-SAP
	list<FDistPoint> listDistance;			// 거리점
	list<FEndPoint> listEndPoint;			// 첫점, 끝점 저장
	list<FEndPoint> listEndPoint_Calc;			// 계산용 리스트
	list<pair<FRigidBody*, FRigidBody*>> listPairCollide;	// 충돌 계산 페어

	// 거리 만들기, Square 거리로 계산한다.
	// O(n)
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
		listDistance.push_back({ (*iter), ((*iter)->Get_Position() - FVector3(-100.f, -100.f, -100.f)).Magnitude() });
	// 정렬 O(log(n))
	listDistance.sort();

	// 시작점, 끝점 만들기, 똑같이 Square로 계산한다.
	// O(n)
	for (auto iter = listDistance.begin(); iter != listDistance.end(); ++iter)
	{
		FCollisionPrimitive* pShape = static_cast<FCollisionPrimitive*>((*iter).pBody->Get_Owner());
		Real fHalfScale = (pShape->Get_Scale() * 0.5f).Magnitude();
		listEndPoint.push_back({ (*iter).pBody, (*iter).fDist - fHalfScale, true });
		listEndPoint.push_back({ (*iter).pBody, (*iter).fDist + fHalfScale, false });
	}
	// 정렬 O(log(n))
	listEndPoint.sort();

	// 충돌쌍 만드는 파트
	for (auto iter = listEndPoint.begin(); iter != listEndPoint.end(); ++iter)
	{
		if ((*iter).bIsStart)
		{
			// 계산후 충돌쌍 생성
			for (auto iterCalc = listEndPoint_Calc.begin(); iterCalc != listEndPoint_Calc.end(); ++iterCalc)
			{
				// 스타트 포인트가 있다면 충돌 쌍을 생성한다.
				// 레이어 마스크가 겹칠 때 쌍을 만든다.
				FCollisionPrimitive* pColSrc = static_cast<FCollisionPrimitive*>((*iter).pBody->Get_Owner());
				FCollisionPrimitive* pColDst = static_cast<FCollisionPrimitive*>((*iterCalc).pBody->Get_Owner());


				// 마스크로 충돌되는지 확인
				if (pColSrc->Get_CollisionMask() & pColDst->Get_CollisionLayer()
					|| pColDst->Get_CollisionMask() & pColSrc->Get_CollisionLayer())
				{
					// 바운딩 박스로 충돌되는지 확인
					if (pColSrc->BoundingBox.vMax >= pColDst->BoundingBox.vMin
						&& pColDst->BoundingBox.vMax >= pColSrc->BoundingBox.vMin)
					{
						listPairCollide.push_back({ (*iter).pBody, (*iterCalc).pBody });
					}
				}
			}
			listEndPoint_Calc.push_back((*iter));
		}
		// 끝점일 때는 매칭되는 점을 뺀다.
		else
		{
			for (auto iterCalc = listEndPoint_Calc.begin(); iterCalc != listEndPoint_Calc.end(); ++iterCalc)
			{
				// StartPoint와 매칭될 때 삭제한다.
				if ((*iter).pBody == (*iterCalc).pBody)
				{
					listEndPoint_Calc.erase(iterCalc);
					break;
				}
			}
		}
	}


	// 충돌 최적화, 추후 추가 예정
	// 계획은 충돌객체를 트리로 만들어 부딪힐 것 같은 객체에 대해 처리하는 것.
	// 여기서 발생시킨 충돌에 대한 것은 엔진에서 발생하는 
	_int iDebugCount = 0;
	_int iDebug_BodySrc = 0;
	for (auto iter = listPairCollide.begin(); iter != listPairCollide.end(); ++iter)
	{
		bool bCollide = false;
		if (m_bIsPaused)
			return 0;

		FCollisionPrimitive* pColSrc = static_cast<FCollisionPrimitive*>((*iter).first->Get_Owner());
		FCollisionPrimitive* pColDst = static_cast<FCollisionPrimitive*>((*iter).second->Get_Owner());

		FCollisionData tColData;
		tColData.iContactsLeft = 1;	// 작동 시킬라면 넣어야함.

		// 하나라도 충돌을 체크를 하는 경우에만 계산한다.
		if (pColSrc->Get_CollisionMask() & pColDst->Get_CollisionLayer()
			|| pColDst->Get_CollisionMask() & pColSrc->Get_CollisionLayer())
			bCollide = FCollisionDetector::CollsionPrimitive(pColSrc, pColDst, &tColData);
		else
			bCollide = false;

		if (bCollide)
		{
			if (pColSrc->Get_CollisionMask() & pColDst->Get_CollisionLayer())
				pColSrc->Handle_CollsionEvent(pColDst->Get_Owner(), &tColData.tContacts);
			tColData.tContacts.Reverse_BodyData();
			if (pColDst->Get_CollisionMask() & pColSrc->Get_CollisionLayer())
				pColDst->Handle_CollsionEvent(pColSrc->Get_Owner(), &tColData.tContacts);
		}
		++iDebugCount;
	}


	// O(MxN)의 굉장히 느린 알고리즘, 폐기
	//for (auto iterSrc = m_listBody.begin(); iterSrc != m_listBody.end(); ++iterSrc)
	//{
	//	// 재연산 방지, 현재 반복자로부터 다음 것을 가져다가 쓴다.
	//	for (auto iterDst = (++iterSrc)--; iterDst != m_listBody.end(); ++iterDst)
	//	{
	//		bool bCollide = false;
	//		if ((*iterSrc) == (*iterDst))
	//			continue;

	//		// 도중에 연결 정지 신호들어오면 종료
	//		if (m_bIsPaused)
	//			return 0;

	//		FCollisionPrimitive* pColSrc = static_cast<FCollisionPrimitive*>((*iterSrc)->Get_Owner());
	//		FCollisionPrimitive* pColDst = static_cast<FCollisionPrimitive*>((*iterDst)->Get_Owner());

	//		FCollisionData tColData;
	//		tColData.iContactsLeft = 1;	// 작동 시킬라면 넣어야함.

	//		// 하나라도 충돌을 체크를 하는 경우에만 계산한다.
	//		if (pColSrc->Get_CollisionMask() & pColDst->Get_CollisionLayer()
	//			|| pColDst->Get_CollisionMask() & pColSrc->Get_CollisionLayer())
	//			bCollide = FCollisionDetector::CollsionPrimitive(pColSrc, pColDst, &tColData);
	//		else
	//			bCollide = false;

	//		if (bCollide)
	//		{
	//			if (pColSrc->Get_CollisionMask() & pColDst->Get_CollisionLayer())
	//				pColSrc->Handle_CollsionEvent(pColDst->Get_Owner(), &tColData.tContacts);
	//			tColData.tContacts.Reverse_BodyData();
	//			if (pColDst->Get_CollisionMask() & pColSrc->Get_CollisionLayer())
	//				pColDst->Handle_CollsionEvent(pColSrc->Get_Owner(), &tColData.tContacts);
	//		}
	//		++iDebugCount;
	//	}
	//	++iDebug_BodySrc;
	//}
	/*wstringstream ss;
	wstring str;
	ss << iDebugCount;
	str = L"Physics CheckCount : " + ss.str() + L"\n";
	OutputDebugString(str.c_str());*/
	cout << "검사 카운트 : " << iDebugCount << endl;
	/*cout << "Src 카운트 : " << iDebug_BodySrc << endl;
	cout << "Dst 카운트 : " << ((iDebug_BodySrc) ? (iDebugCount / iDebug_BodySrc) : 0) << endl;*/
	cout << "Body 카운트 : " << m_listBody.size() << endl;

	// 사용된 접촉 수를 반환
	return m_iMaxContacts - iLimit;
}

FCollisionPrimitive* CPhysicsWorld3D::Test_Contact(FCollisionPrimitive* const pCollision)
{
	// 받은 충돌체로 리스트바디의 충돌체들과 비교해서 하나라도 충돌하면 반환한다.
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		FCollisionPrimitive* pCol = static_cast<FCollisionPrimitive*>((*iter)->Get_Owner());
		if (FCollisionDetector::CollsionPrimitive(pCollision, pCol, nullptr))
			return pCol;
	}

	return nullptr;
}

list_collide_test CPhysicsWorld3D::Test_Contacts(FCollisionPrimitive* const pCollision)
{
	list_collide_test listCollision;

	// 받은 충돌체로 리스트바디의 충돌체들과 비교해서 하나라도 충돌하면 반환한다.
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		FCollisionPrimitive* pCol = static_cast<FCollisionPrimitive*>(pCollision);
		FCollisionPrimitive* pColDst = static_cast<FCollisionPrimitive*>((*iter)->Get_Owner());
		if (pCol == pColDst)
			continue;

		_bool bCollide = false;
		FCollisionData tColData;
		tColData.iContactsLeft = 1;	// 작동 시킬라면 넣어야함.

		if (pCol->Get_CollisionMask() & pColDst->Get_CollisionLayer())
			bCollide = FCollisionDetector::CollsionPrimitive(pCol, pColDst, &tColData);
		else
			continue;

		if (bCollide)
		{
			listCollision.push_back(pair<FCollisionPrimitive*, FContact>(pColDst, tColData.tContacts));
		}
	}

	return listCollision;
}

void CPhysicsWorld3D::Add_RigidBody(FRigidBody* pBody)
{
	auto iter = find_if(m_listBody.begin(), m_listBody.end(),
		[&pBody](FRigidBody*& pDstBody) {
			return pDstBody == pBody;
		});
	if (iter != m_listBody.end())
		return;
	
	m_listBody.push_back(pBody);
}

void CPhysicsWorld3D::Delete_RigidBody(FRigidBody* pBody)
{
	auto iter = find_if(m_listBody.begin(), m_listBody.end(),
		[&pBody](FRigidBody*& pDstBody) {
			return pDstBody == pBody;
		});
	if (iter != m_listBody.end())
		m_listBody.erase(iter);
}
