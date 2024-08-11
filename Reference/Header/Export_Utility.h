#pragma once

#include "Management.h"

#include "ProtoMgr.h"
#include "Renderer.h"
#include "LightMgr.h"
#include "PhysicsMgr.h"
#include "TextureMgr.h"
#include "KeyMgr.h"
#include "BlackBoardMgr.h"

#include "Camera.h"

#include "CalculatorComponent.h"
#include "TriColorComp.h"
#include "RcColorComp.h"
#include "MeshColComp.h"
#include "RcBufferComp.h"
#include "TerrainBufferComp.h"
#include "CameraComponent.h"
#include "CubeBufferComp.h"
#include "ColliderComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "ITeamAgent.h"
#include "Collide.h"
#include "Contact.h"


BEGIN(Engine)

// Management
inline CComponent*	Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag);

inline CGameObject* Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag);
inline CGameObject* Get_GameObject(const char* pLayerTag, const char* pObjTag);
inline HRESULT		Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagementInstance, EMANAGE_SCENE eType);

inline HRESULT		Set_Scene(CScene* pScene);
inline HRESULT		Set_Scene(wstring strSceneName);
inline HRESULT		Clear_CurrentScene();	// 현재 씬만 클리어
inline HRESULT		Clear_Scene();			// All Delete
inline _int			Update_Scene(const _float& fTimeDelta);
inline void			LateUpdate_Scene();
inline void			Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

// 게임 런타임용 오브젝트 생성기
inline void			Add_GameObject(const _tchar* pLayerTag, CGameObject* const pObj);
// 빌트인용 이름 지정 가능 오브젝트 생성기
inline void			Add_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag, CGameObject* const pObj);
inline void			Add_Layer(const _tchar* pLayerTag, CLayer* const pLayer);


// ProtoMgr
inline HRESULT			Ready_Proto(const _tchar* pProtoTag, CComponent* pComponent);
template <typename T>	
inline T*				Clone_Proto(const _tchar* pProtoTag);
// 클론시 저장할 포인터 변수를 받는 오버로딩 함수
inline CComponent*		Clone_Proto(const _tchar* pProtoTag, CComponent*& prComponent);

// Renderer
inline _bool		IsReady_Renderer();
inline CRenderer*	Get_Renderer();
inline HRESULT		Ready_Renderer(const _uint iWidth = WINCX, const _uint iHeight = WINCY);
inline void			Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
inline void			Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
inline void			Clear_RenderGroup();

// LightMgr
inline HRESULT		Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,	const D3DLIGHT9 * pLightInfo,	const _uint & iIndex);

// PhysicsMgr
inline HRESULT		Ready_PhysicsMgr(const _uint iMaxPhysicsWorld3D);
inline void			StartFrame_PhysicsMgr();
inline _int			Update_PhysicsMgr(const Real& fTimeDelta);
inline void			Stop_PhysicsSimulation(const _uint iIndex);
inline void			Play_PhysicsSimulation(const _uint iIndex);
inline void			Add_ColliderToPhysicsWorld(const _uint iIndex, FCollisionPrimitive* pCollider);
inline void			Delete_ColliderToPhysicsWorld(const _uint iIndex, FCollisionPrimitive* pCollider);

inline list<pair<CGameObject*, FContact>>	IntersectTests_Collider_GetGameObject(const _uint iWorldID, _vec3 vPos, CColliderComponent* pSrc, _ulong iMask);
inline list<pair<CGameObject*, FContact>>	IntersectTests_Sphere_GetGameObject(const _uint iWorldID, _vec3 vPos, _float fRadius, _ulong iMask);
inline list<pair<CGameObject*, FContact>>	IntersectTests_Box_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vHalfSize, _ulong iMask);
inline list<pair<CGameObject*, FContact>>	IntersectTests_Capsule_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vNormal, _float fRadius, _ulong iMask);
inline list<pair<CGameObject*, FContact>>	IntersectTests_Line_GetGameObject(const _uint iWorldID, _vec3 vStart, _vec3 vEnd, _ulong iMask);
inline list<pair<CGameObject*, FContact>>	IntersectTests_Ray_GetGameObject(const _uint iWorldID, _vec3 vOrigin, _vec3 vNormal, _ulong iMask);



// TextureMgr
inline HRESULT	Ready_TextureMgr(LPDIRECT3DDEVICE9 pGraphicDev);
inline HRESULT	Ready_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey = L"", const _range<_uint>& iCntRange = _range<_uint>(0U, 0U));


// KeyMgr
inline HRESULT	Ready_KeyMgr();
inline void		Update_KeyMgr();
inline void		LateUpdate_KeyMgr();
// 키 입력
inline bool		IsKey_Pressing(const int& iKey);
inline bool		IsKey_Pressing(const int&& iKey);
inline bool		IsKey_Pressed(const int& iKey);
inline bool		IsKey_Pressed(const int&& iKey);
inline bool		IsKey_Released(const int& iKey);
inline bool		IsKey_Released(const int&& iKey);
// 마우스 입력
inline bool		IsMouse_Pressing(const MOUSEKEYSTATE& iMouse);
inline bool		IsMouse_Pressing(const MOUSEKEYSTATE&& iMouse);
inline bool		IsMouse_Pressed(const MOUSEKEYSTATE& iMouse);
inline bool		IsMouse_Pressed(const MOUSEKEYSTATE&& iMouse);
inline bool		IsMouse_Released(const MOUSEKEYSTATE& iMouse);
inline bool		IsMouse_Released(const MOUSEKEYSTATE&& iMouse);





#pragma region 블랙보드 매니저
inline HRESULT		Ready_BlackBoardMgr();
inline _int			Update_BlackBoardMgr();
inline HRESULT		Add_BlackBoard(const wstring& strBoardName, CBlackBoard* pBlackBoard);
inline CBlackBoard* Get_BlackBoard(const wstring& strBoardName);
#pragma endregion






inline void		Release_Utility();

#include "Export_Utility.inl"

END