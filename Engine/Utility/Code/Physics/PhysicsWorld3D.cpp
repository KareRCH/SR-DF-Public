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

	// �� ����, �Ϸ�
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

	// �� ���ϱ�, ��ü ��ġ ������� �浹ü ��ġ ����
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		(*iter)->Integrate(fTimeDelta);

		FCollisionPrimitive* pCol = static_cast<FCollisionPrimitive*>((*iter)->Get_Owner());
		pCol->Calculate_Transform();
		pCol->Set_Position(pCol->Get_Position());
		pCol->Calculate_Shape();

		
	}

	// ���� �߻���
	_uint iUsedContacts = Generate_Contacts();


	//// �� �߻���� ���� �߻���� ���� ����� �浹�� �ذ�
	//if (m_bCalculateIterations)
	//	m_ContactResolver.Set_Iterations(iUsedContacts * 4);
	//m_ContactResolver.ResolveContacts(m_pContacts, iUsedContacts, fTimeDelta);

	return 0;
}

_uint CPhysicsWorld3D::Generate_Contacts()
{
	_uint iLimit = m_iMaxContacts;
	FContact* pNextContact = m_pContacts;

	// �浹�߻�
	//for (auto iter = m_ConGenList.begin(); iter != m_ConGenList.end(); ++iter)
	//{
	//	_uint iUsed = (*iter)->Add_Contact(pNextContact, iLimit);
	//	iLimit -= iUsed;
	//	pNextContact += iUsed;

	//	// ����Ʈ�� �ʰ��ϸ� �� �̻� ����ó���� ���� �ʴ´�.
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
	list<FDistPoint> listDistance;			// �Ÿ���
	list<FEndPoint> listEndPoint;			// ù��, ���� ����
	list<FEndPoint> listEndPoint_Calc;			// ���� ����Ʈ
	list<pair<FRigidBody*, FRigidBody*>> listPairCollide;	// �浹 ��� ���

	// �Ÿ� �����, Square �Ÿ��� ����Ѵ�.
	// O(n)
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
		listDistance.push_back({ (*iter), ((*iter)->Get_Position() - FVector3(-100.f, -100.f, -100.f)).Magnitude() });
	// ���� O(log(n))
	listDistance.sort();

	// ������, ���� �����, �Ȱ��� Square�� ����Ѵ�.
	// O(n)
	for (auto iter = listDistance.begin(); iter != listDistance.end(); ++iter)
	{
		FCollisionPrimitive* pShape = static_cast<FCollisionPrimitive*>((*iter).pBody->Get_Owner());
		Real fHalfScale = (pShape->Get_Scale() * 0.5f).Magnitude();
		listEndPoint.push_back({ (*iter).pBody, (*iter).fDist - fHalfScale, true });
		listEndPoint.push_back({ (*iter).pBody, (*iter).fDist + fHalfScale, false });
	}
	// ���� O(log(n))
	listEndPoint.sort();

	// �浹�� ����� ��Ʈ
	for (auto iter = listEndPoint.begin(); iter != listEndPoint.end(); ++iter)
	{
		if ((*iter).bIsStart)
		{
			// ����� �浹�� ����
			for (auto iterCalc = listEndPoint_Calc.begin(); iterCalc != listEndPoint_Calc.end(); ++iterCalc)
			{
				// ��ŸƮ ����Ʈ�� �ִٸ� �浹 ���� �����Ѵ�.
				// ���̾� ����ũ�� ��ĥ �� ���� �����.
				FCollisionPrimitive* pColSrc = static_cast<FCollisionPrimitive*>((*iter).pBody->Get_Owner());
				FCollisionPrimitive* pColDst = static_cast<FCollisionPrimitive*>((*iterCalc).pBody->Get_Owner());


				// ����ũ�� �浹�Ǵ��� Ȯ��
				if (pColSrc->Get_CollisionMask() & pColDst->Get_CollisionLayer()
					|| pColDst->Get_CollisionMask() & pColSrc->Get_CollisionLayer())
				{
					// �ٿ�� �ڽ��� �浹�Ǵ��� Ȯ��
					if (pColSrc->BoundingBox.vMax >= pColDst->BoundingBox.vMin
						&& pColDst->BoundingBox.vMax >= pColSrc->BoundingBox.vMin)
					{
						listPairCollide.push_back({ (*iter).pBody, (*iterCalc).pBody });
					}
				}
			}
			listEndPoint_Calc.push_back((*iter));
		}
		// ������ ���� ��Ī�Ǵ� ���� ����.
		else
		{
			for (auto iterCalc = listEndPoint_Calc.begin(); iterCalc != listEndPoint_Calc.end(); ++iterCalc)
			{
				// StartPoint�� ��Ī�� �� �����Ѵ�.
				if ((*iter).pBody == (*iterCalc).pBody)
				{
					listEndPoint_Calc.erase(iterCalc);
					break;
				}
			}
		}
	}


	// �浹 ����ȭ, ���� �߰� ����
	// ��ȹ�� �浹��ü�� Ʈ���� ����� �ε��� �� ���� ��ü�� ���� ó���ϴ� ��.
	// ���⼭ �߻���Ų �浹�� ���� ���� �������� �߻��ϴ� 
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
		tColData.iContactsLeft = 1;	// �۵� ��ų��� �־����.

		// �ϳ��� �浹�� üũ�� �ϴ� ��쿡�� ����Ѵ�.
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


	// O(MxN)�� ������ ���� �˰���, ���
	//for (auto iterSrc = m_listBody.begin(); iterSrc != m_listBody.end(); ++iterSrc)
	//{
	//	// �翬�� ����, ���� �ݺ��ڷκ��� ���� ���� �����ٰ� ����.
	//	for (auto iterDst = (++iterSrc)--; iterDst != m_listBody.end(); ++iterDst)
	//	{
	//		bool bCollide = false;
	//		if ((*iterSrc) == (*iterDst))
	//			continue;

	//		// ���߿� ���� ���� ��ȣ������ ����
	//		if (m_bIsPaused)
	//			return 0;

	//		FCollisionPrimitive* pColSrc = static_cast<FCollisionPrimitive*>((*iterSrc)->Get_Owner());
	//		FCollisionPrimitive* pColDst = static_cast<FCollisionPrimitive*>((*iterDst)->Get_Owner());

	//		FCollisionData tColData;
	//		tColData.iContactsLeft = 1;	// �۵� ��ų��� �־����.

	//		// �ϳ��� �浹�� üũ�� �ϴ� ��쿡�� ����Ѵ�.
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
	cout << "�˻� ī��Ʈ : " << iDebugCount << endl;
	/*cout << "Src ī��Ʈ : " << iDebug_BodySrc << endl;
	cout << "Dst ī��Ʈ : " << ((iDebug_BodySrc) ? (iDebugCount / iDebug_BodySrc) : 0) << endl;*/
	cout << "Body ī��Ʈ : " << m_listBody.size() << endl;

	// ���� ���� ���� ��ȯ
	return m_iMaxContacts - iLimit;
}

FCollisionPrimitive* CPhysicsWorld3D::Test_Contact(FCollisionPrimitive* const pCollision)
{
	// ���� �浹ü�� ����Ʈ�ٵ��� �浹ü��� ���ؼ� �ϳ��� �浹�ϸ� ��ȯ�Ѵ�.
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

	// ���� �浹ü�� ����Ʈ�ٵ��� �浹ü��� ���ؼ� �ϳ��� �浹�ϸ� ��ȯ�Ѵ�.
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		FCollisionPrimitive* pCol = static_cast<FCollisionPrimitive*>(pCollision);
		FCollisionPrimitive* pColDst = static_cast<FCollisionPrimitive*>((*iter)->Get_Owner());
		if (pCol == pColDst)
			continue;

		_bool bCollide = false;
		FCollisionData tColData;
		tColData.iContactsLeft = 1;	// �۵� ��ų��� �־����.

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
