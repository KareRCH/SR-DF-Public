#pragma once

#include "ImguiWin.h"

#include "imgui.h"
#include "Serialize_Core.h"
#include "Serialize_BaseClass.h"

#include "ImguiWin_ProtoTool.h"

const string g_strSceneExt = ".ascene";
const string g_strTerrainExt = ".aterrain";
const string g_strObjectExt = ".aobject";
const string g_strLightExt = ".alight";
const string g_strCameraExt = ".acamera";

const string g_strDataPath = "../Client/Resource/Data/";
const string g_strScenePath = g_strDataPath + "Scene/";
const string g_strObjectPath = g_strDataPath + "Object/";
const string g_strTerrainPath = g_strDataPath + "Terrain/";
const string g_strLightPath = g_strDataPath + "Light/";

BEGIN(Engine)
class CGameObject;
END

class CImguiWin_MapTool : public CImguiWin
{
	DERIVED_CLASS(CImguiWin, CImguiWin_MapTool)

private:
	explicit CImguiWin_MapTool();
	virtual ~CImguiWin_MapTool();

public:
	static CImguiWin_MapTool* Create();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;


private:
	ImGuiStyle				m_pStyle;

	_bool					m_bOpt_FullScreen;
	_bool					m_bOpt_Padding;

	ImGuiDockNodeFlags		m_DockSpace_Flags;
	ImGuiWindowFlags		m_Window_Flags;


private:
	void			Warning();

private:
	void			Layout_Browser(const ImGuiWindowFlags& iMain_Flags);
	void			Layout_Browser_Scene();
	void			Layout_Browser_Terrain();
	void			Layout_Browser_Object();
	void			Layout_Browser_Light();
	void			Layout_Browser_Camera();

private:
	void			Layout_Hierarchi(const ImGuiWindowFlags& iMain_Flags);

private:
	void			Layout_Property(const ImGuiWindowFlags& iMain_Flags);
	void			Layout_Property_Scene();
	void			Layout_Property_Layer();
	void			Layout_Property_Object();
	void			Layout_Property_Light();

private:
	void			Layout_Viewer(const ImGuiWindowFlags& iMain_Flags);

private:	// 뷰 추가 레이아웃
	vector<string>	m_vecCoord_Item = { u8"월드", u8"로컬", u8"오일러" };
	_uint			m_iIndex_CoordItem = 0;

	vector<string>	m_vecPivot_Item = { u8"오브젝트", u8"커서", u8"중간점" };
	_uint			m_iIndex_PivotItem = 0;

	vector<string>	m_vecSnap_Item = { u8"좌표", u8"점", u8"선", u8"면" };
	_uint			m_iIndex_SnapItem = 0;

	_bool			m_bSnap_Active = false;

	// 탭바
	enum ETAB_BROWSER
	{
		ETAB_BROWSER_SCENE,
		ETAB_BROWSER_TERRAIN,
		ETAB_BROWSER_OBJECT,
		ETAB_BROWSER_LIGHT,
		ETAB_BROWSER_CAMERA
	};
	ETAB_BROWSER	m_eTab_Browser = ETAB_BROWSER_SCENE;


private:	// 선택 타입
	enum ESELECTED_TYPE : _ulong
	{
		ESELECTED_TYPE_NONE,
		ESELECTED_TYPE_SCENE,
		ESELECTED_TYPE_LAYER,
		ESELECTED_TYPE_OBJECT,
		ESELECTED_TYPE_LIGHT
	};
	ESELECTED_TYPE				m_eSelectedProperty_Type = ESELECTED_TYPE_NONE;

private:	// 계층 관련 정의부
	enum EPRIORITY_OBJECT : _ulong
	{
		EPRIORITY_OBJECT_UPDATE,
		EPRIORITY_OBJECT_LATE,
		EPRIORITY_OBJECT_RENDER,
		EPRIORITY_OBJECT_END
	};
	struct FObjectData
	{
		string			strName = "";				// 현재 이름
		string			strOriginName = "";			// 프로토 타입 이름
		EGO_CLASS		eObjectID = ECLASS_NONE;	// 클래스 분류
		string			strClassName = "";
		CGameObject*	pObject = nullptr;

		string			strGroupKey;
		string			strTextureKey;

		_float			fPriority[EPRIORITY_OBJECT_END] = { 0.f, 0.f, 0.f };
		_bool			bUsePriority[EPRIORITY_OBJECT_END] = { true, true, true };

		_vec3			vPos = { 0.f,0.f,0.f };
		_vec3			vRot = { 0.f,0.f,0.f };
		_vec3			vScale = { 1.f,1.f,1.f };
		
		_vec3			vTexPos = { 0.f, 0.f, 0.f };
		_vec3			vTexRot = { 0.f, 0.f, 0.f };
		_vec3			vTexScale = { 1.f, 1.f, 1.f };

		_vec3			vColPos = { 0.f, 0.f, 0.f };
		_vec3			vColRot = { 0.f, 0.f, 0.f };
		_vec3			vColScale = { 1.f, 1.f, 1.f };

		vector<string>					vecUserString;				// 사용자 지정 문자열

		
	};
	struct FLayerData
	{
		string						strName = "";
		_float						fPriority = 0.f;
		vector<FObjectData>		vecObject;
	};
	struct FTerrainData
	{
		string			strName = "";
		_vec3			vVertexCnt = { 0.f, 0.f, 0.f };
		_vec3			vScale = { 1.f, 1.f, 1.f };
		_vec3			vOffset = { 0.f, 0.f, 0.f };
	};
	struct FSceneData
	{
		string						strName = "";
		FTerrainData				tTerrain;
		vector<FLayerData>		vecLayer;
	};

private:
	void			Reset_Hierarchi()
	{
		m_iSelected_Layer = -1;
		m_iSelected_Layer_Remain = -1;
		m_iSelected_Object = -1;
		m_eSelectedProperty_Type = ESELECTED_TYPE_NONE;
	}
	// 단순 인덱스 설정용
	void			Set_HierarchiIndex(_int iLayerIndex, _int iObjectIndex = -1)
	{
		if (iObjectIndex != -1)
			m_iSelected_Layer = iLayerIndex;
		m_iSelected_Layer_Remain = iLayerIndex;
		m_iSelected_Object = iObjectIndex;
		if (iObjectIndex != -1)
			m_eSelectedProperty_Type = ESELECTED_TYPE_OBJECT;
		else
			m_eSelectedProperty_Type = ESELECTED_TYPE_LAYER;
	}
private:
	void			Load_ObjectToScene();
	void			Create_LayerToScene(const FLayerData& tLayerData);
	void			Factory_GameObject(const _tchar* pLayerTag, const EGO_CLASS& eClassID, FObjectData& tObjectData);

	void			Duplicate_SelectedObject();
	void			Delete_AllFromScene();
	void			Delete_SelectedObjectFromScene();

	void			Set_SelectedObject();
	void			Reset_SelectedObject();

private:
	// 목록에 있는 씬 모두 저장
	void			Save_SceneAll();
	// 선택 씬만 저장
	void			Save_Scene() {}
	// 씬 내보내기 (단일 씬)
	void			Export_Scene(const FSerialize_Scene& tSceneSerial);
	// 전체 씬 로드
	void			Load_SceneAll();
	// 선택 씬만 로드
	void			Load_SelectedScene();
	// 파싱된 파일로부터 씬 정보 불러오기
	void			Import_Scene(const string& strName, FSerialize_Scene& tSceneSerial, FSceneData& tSceneData);

	// 프로토 로드 관련
	void			Load_ProtoAll();
	void			Import_Proto(const string& strName, FSerialize_Proto& tProtoSerial, FProtoData& tProtoData);

private:
	_bool						m_bScene_Init = true;

	string						m_strAdd_SceneName;				// 씬 추가하기
	_int						m_iSelected_Scene = -1;			// 선택된 씬
	_int						m_iLoaded_Scene = -1;			// 실제 로드된 씬
	_int						m_iLastLoaded_Scene = -1;
	_bool						m_bScene_Loaded = false;

	vector<FSceneData>			m_vecScene;						// 씬, 레이어, 오브젝트
	FSceneData					m_tBackupScene;					// 로드시 저장 데이터
	_int						m_iSelected_Layer = -1;
	_int						m_iSelected_Layer_Remain = -1;
	_int						m_iSelected_Object = -1;

	char						m_arrAddLayer_Buf[256] = "";
	_bool						m_bFocusedLayer_Edit = false;

	FObjectData*				m_pPickedObjectData = nullptr;


private:			// 터레인 관련
	void			Apply_Terrain(const string& strTerrainName);
	void			Save_Terrain(const _int iSelected_Scene);
	void			Export_Terrain(const FSerialize_Terrain& tTerrain);
	void			Load_Terrain(const _int iSelected_Scene, const string& strName);
	void			Import_Terrain(const _int iSelected_Scene, const string& strName, FSerialize_Terrain& tTerrain);

private:			// 프로토 관련
	void			Add_ObjectFromProto();
	//CGameObject*	Get_Object();

private:
	vector<FProtoData>			m_vecProto;
	_int						m_iSelected_Proto = -1;


private:	// 속성 관련
	_bool			m_bInput_Warning = false;


private:	// 유틸리티
	void			Set_Button_ActiveColor();
	void			Set_Button_NonActiveColor();
	void			Set_Button_ReturnColor();

	void			Clamp_Vec3(_vec3& vVec, _float fValue)
	{
		if (vVec.x < -fValue)
			vVec.x = -fValue;
		else if (vVec.x > fValue)
			vVec.x = fValue;

		if (vVec.y < -fValue)
			vVec.y = -fValue;
		else if (vVec.y > fValue)
			vVec.y = fValue;

		if (vVec.z < -fValue)
			vVec.z = -fValue;
		else if (vVec.z > fValue)
			vVec.z = fValue;
	}
	void			Create_CamAxis(_vec3& refRight, _vec3& refLook, _vec3& refUp)
	{
		refUp = Get_Up();
		refLook = Get_Look() - Get_Pos();

		D3DXVec3Normalize(&refLook, &refLook);

		D3DXVec3Cross(&refRight, &refUp, &refLook);
		D3DXVec3Normalize(&refRight, &refRight);

		D3DXVec3Cross(&refUp, &refLook, &refRight);
		D3DXVec3Normalize(&refUp, &refUp);
	}

public:		// 트랜스폼 영역, Transform에서 옮겨온 거임
	GETSET_EX2(_vec3, m_vCamTranslate[INFO_RIGHT], Right, GET_REF, SET_C)
	GETSET_EX2(_vec3, m_vCamTranslate[INFO_UP], Up, GET_REF, SET_C)
	GETSET_EX2(_vec3, m_vCamTranslate[INFO_LOOK], Look, GET_REF, SET_C)
	GETSET_EX2(_vec3, m_vCamTranslate[INFO_POS], Pos, GET_REF, SET_C)

private:		// 카메라
	void			Input_Camera(const _float& fTimeDelta);

	_bool			m_bCanInput_Camera = false;

	ImVec2			m_fPrevDrag_Translate = { 0.f, 0.f };
	ImVec2			m_fDrag_Translate = { 0.f, 0.f };

	ImVec2			m_fPrevDrag_Rotate = { 0.f, 0.f };
	ImVec2			m_fDrag_Rotate = { 0.f, 0.f };

	_vec3			m_vCamTranslate[INFO_END];
	_vec3			m_vRot = { 0.f, 0.f, 0.f };
	_vec3			m_vScale = { 1.f, 1.f, 1.f };
	_matrix			m_matView;

	_matrix			m_matProj;

	_float			m_fFov = D3DXToRadian(60.f);
	_float			m_fAspect = (float)WINCX / WINCY;
	_float			m_fNear = 0.1f;
	_float			m_fFar = 1000.f;

private:		// 뷰어에서 오브젝트 이동
	ImVec2			m_vViewerContent_Size;

	enum class EEDIT_MODE
	{
		NONE,
		MOUSE_TRANSLATE,
		MOUSE_ROTATE,
		TRANSFORM,
	};

	enum ETRANSFORM_MODE
	{
		ETRANSFORM_MODE_NONE,
		ETRANSFORM_MODE_MOVE,
		ETRANSFORM_MODE_ROT,
		ETRANSFORM_MODE_SCALE,
	};

	enum ETRANSFORM_AXIS
	{
		ETRANSFORM_AXIS_NONE,
		ETRANSFORM_AXIS_X,
		ETRANSFORM_AXIS_Y,
		ETRANSFORM_AXIS_Z,
		ETRANSFORM_PLANE_X,
		ETRANSFORM_PLANE_Y,
		ETRANSFORM_PLANE_Z,
		ETRANSFORM_AXIS_ALL
	};

private:
	// 트랜스폼 에딧이 끝났을 때 호출하는 함수
	void End_EditTransform()
	{

	}

	EEDIT_MODE			m_eEdit_Mode = EEDIT_MODE::NONE;

	_bool				m_bIsTransform_Start = false;
	ETRANSFORM_MODE		m_eTransform_Mode = ETRANSFORM_MODE_NONE;
	ETRANSFORM_AXIS		m_eTransform_Axis = ETRANSFORM_AXIS_NONE;
	ETRANSFORM_AXIS		m_eTransform_PrevAxis = ETRANSFORM_AXIS_NONE;		// 되돌리기용

	_vec2				m_vTransform_ObjectStart = { 0.f, 0.f };
	_vec2				m_vTransform_MouseStart = { 0.f, 0.f };
	_vec2				m_vTransform_MouseEnd = { 0.f, 0.f };
	_float				m_fTransform_LengthStart = 0.f;
	_float				m_fTransform_LengthEnd = 0.f;

	_vec3				m_vTransform_Translate;
	_vec3				m_vTransform_Translate_Saved;

	_vec3				m_vTransform_Rotate;
	_vec3				m_vTransform_Rotate_Saved;

	_vec3				m_vTransform_Scale;
	_vec3				m_vTransform_Scale_Saved;

};

