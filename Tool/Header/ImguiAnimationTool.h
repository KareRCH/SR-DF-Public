#pragma once

#include "ImguiWin.h"

// 게임 로직 오브젝트의 유형 (Enum)
enum OBJECTTYPE 
{
	NONE,
	TWO_HAND,
	TWO_OBJECT,
	RIGHT_OBJECT
};

// 객체 유형 키값 (Enum)
enum OBJECTKEY 
{
	KEY_NONE,
	KEY_GUN,
	KEY_THOMPSON,
	KEY_STEELPIPE,
	KEY_BEERBOTLE,
	KEY_FRYINGPAN
};

// 변경할 속성 값 모드
enum EDITMODE 
{
	EDITMODE_NONE,
	EDITMODE_SCALE,
	EDITMODE_ROTATION,
	EDITMODE_TRANSLATION,
	EDITMODE_TIME_VALUE,
	EDITMODE_TEXTURE
};

EDITMODE eEditMode = EDITMODE_NONE; // 변경할 모드 변수

// 자동 애니메이션 생성시 최소 및 최대 값을 입력 받는 구조체
struct AnimationProperties {
	float minTime = 0.0f;
	float maxTime = 20.f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	_vec3 minScale = { 0.1f, 0.1f, 0.f };
	_vec3 maxScale = { 5.f, 5.f, 5.f };
	_vec3 minRotation = { 0.f, 0.f, 0.f };
	_vec3 maxRotation = { 360.f, 360.f, 360.f };
	_vec3 minPosition = { 0.f, 0.f, 0.f };
	_vec3 maxPosition = { 300.f, 300.f, 300.f };
};

// 애니메이션 Info 구조체
typedef struct tagMyInfoAnimation
{
	_vec3	m_vScale;	// 크기를 담을 그릇
	_vec3	m_vRot;		// 회전을 담을 그릇
	_vec3	m_vPos;		// 위치를 담을 그릇

	OBJ_TYPE m_eObjectType; // 타입을 부여할 그릇 (ex : 한손, 양손)
	OBJ_NAME m_eObjectName; // 이름을 부여할 그릇 (ex : 권총, 쇠파이프)

}MYANIMATIONINFO;

class CImguiAnimationTool : public CImguiWin
{
	DERIVED_CLASS(CImguiWin, CImguiAnimationTool)

private:
	explicit CImguiAnimationTool();
	virtual ~CImguiAnimationTool();

public:
	static CImguiAnimationTool* Create();

public:
	// 이미지를 추가하는 함수
	void AddImage(const std::string& imagePath);

	// 애니메이션 정보 벡터 넘겨주기
	std::vector<KEYFRAME>* Get_Animation() { return &timeline[m_iCurType]; }
	
	KEYTYPE* Get_CurType() {
		return &m_iCurType;
	} void Set_CurType(KEYTYPE value) {
		m_iCurType = value;
	}

	//std::string	OpenImageFileDialog();
	//HRESULT OpenImageFileDialog(const _tchar* folderPath, LPDIRECT3DDEVICE9 pGraphicDev);
	//HRESULT	SaveData(const _tchar* mapTag);
	//void	SaveUI(wstring wstrFileName, wstring wstrFilePath);
	//CMyTexture* FindUI(const _tchar* szKey);
	//char를 wchar_t로 변경
	//void ConverCtoWC(char* str, wchar_t*& _cstr);

#pragma region 문자열 변환 함수
	//wstring을 char로 변경
	char* ConverWStringtoC(const wstring& wstr);

	//char를 wchar_t로 변경
	wchar_t* ConverCtoWC(char* str);

	//wchar_t를 char로 변경
	char* ConverWCtoC(wchar_t* str);

	//유니코드 변환
	string WstringToUTF8(const wstring& wstr);

	std::wstring ConvertToWideString(const std::string& ansiString);
	
	std::vector<const char*> ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector);
#pragma endregion

	// 이미지 로드 함수
	void LoadImg(const _tchar* folderPath);

	// 재귀 함수 (재귀하면서 폴더 하나하나 들어가서 찾아줌)
	void Replace(wstring& strCurrentDirectory, wstring strSearch, wstring strReplace);

	//void RenderSelectedFrameTexture(const int selectedItemIndex);
	
	void RenderTimeline(); // 애니메이션 타임라인

	float EvaluateAnimationAtTime(float time); // 애니메이션 시간

	// 애니메이션의 시작과 끝을 부드럽게 표현하기 위한 함수
	float ImEaseInQuad(float start, float end, float t);
	float ImEaseOutQuad(float start, float end, float t);

	// 미리보기 애니메이션 렌더링 함수
	void RenderPreviewAnimation(float value);

	// 선형 보간 함수
	float Lerp(float a, float b, float t);

	_vec3 Lerp(const _vec3& a, const _vec3& b, float t);

	_float Lerp2(const _float& a, const _float& b, float t);

public: // 애니메이션 함수
	// 오브젝트 설정 및 관리 함수
	void ObjectSetting();

	// 데이터 로드 함수
	//void LoadObjectInforamtionData();

	// 데이터 저장 함수
	//void SaveObjectInformationData();

	// 파일명 제거 함수
	void PathRemoveFileSpec(TCHAR* path);

	// 시간에 따른 애니메이션 자동 생성 함수
	void EditKeyframesForAnimation(int numFrames);

	//test
	//ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t);
	
	// 토글 버튼
	void ToggleButton(const char* str_id, bool* v);

	// 애니메이션 저장
	void SaveAnimationToFile(const char* fileName);

	// 애니메이션 불러오기
	void LoadAnimationFromFile(const char* fileName);

	// 키프레임 자동 생성 함수
	void KeyframeAutomaticGeneration();

	// 키프레임 자동 생성 함수(선형 보간)
	void CreateKeyframesWithLinearInterpolation(
		std::vector<Engine::KEYFRAME>& timeline, float minTime, float maxTime,
		_float minValue, _float maxValue,
		_vec3 minscaleValue, _vec3 maxscaleValue,
		_vec3 minrotationValue, _vec3 maxrotationValue,
		_vec3 mintranslationValue, _vec3 maxtranslationValue,
		_float _minTexture, _float _maxTexture,
		int numKeyframes);

	// 키프레임을 선택하여 해당 키프레임 수정
	void HandleKeyframeClick();

	// 단축키 모음 (키입력)
	void KeyInput();

	// 단축키로 키프레임의 속성 값 변경
	void SelectKeyframeValueChange();

	// 타임라인의 키프레임을 마우스로 선택하여 구분하는 함수
	void SelectKeyframeMouseL();

	// 키프레임 목록
	void KeyframeList();

	// 키프레임 수정 창
	void KeyframeChangeWindow();

	// 키프레임 미리보기 보간 (보류)
	void KeyframeAniV();

	// 키프레임 랜더링 및 편집 (순서 UI중 가장 마지막)
	void KeyframeRender_ValueChange();


	// 우클릭으로 키프레임 삭제하는 함수
	void KeyframeDeleteMouseR();

	// 현재 선택한 키프레임 값을 변경하는 함수 (창)
	void DrawSelectedKeyframeEditor(KEYFRAME& selectedKeyframe);

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;

public:
	GETSET_EX1(LPDIRECT3DTEXTURE9, m_pTexture, Texture, GET_REF)
	GETSET_EX2(_bool, isPlaying, FramePlaying, GET, SET)
	GETSET_EX2(_float, currentTime, currentTime, GET, SET)
	GETSET_EX2(_int, bRepetition, Repetition, GET, SET)
		
protected:
	virtual void Free();
	
private:
	LPDIRECT3DTEXTURE9 m_pTexture = nullptr;
	LPDIRECT3DSURFACE9 m_pRenderTargetSurface = nullptr;

	//map<const _tchar*, CMyTexture*> m_mapTexture;

	MYANIMATIONINFO m_AniInfo;

	// 문자열 키값
	vector<wstring>	uniqueObj;
	vector<wstring>	uniqueState;

	// 애니메이션 정보를 담을 벡터
	vector<MYANIMATIONINFO>	m_vecAnimationInfo;

	// 애니메이션 정보를 담을 벡터
	vector<KEYFRAME>	m_vecAnimationKeyframe;

	// 파일을 담을 맵
	map<wstring, vector<wstring>> m_MapFile;

	// 텍스처를 담을 맵
	map<pair<wstring, wstring>, LPDIRECT3DTEXTURE9> m_MapTexture;

	//map<OBJECTKEY, Engine::CGameObject*> m_mapObject;

	//OBJ_TYPE m_eObjectType;
	//OBJ_NAME m_eObjName;

	OBJECTTYPE m_eObjName[RIGHT_OBJECT];

private: // 멤버 변수
	float m_fX = 0.0f;
	float m_fY = 0.0f;
	float m_fZ = 0.0f;

	bool my_tool_active;
	float my_color[4];

	const char* items[100]; // ListBox에 추가할 아이템 배열
	int itemIndex = 0;

	float g_animationTime = 0.f;
	int g_currentFrame = 0;

	int	m_iObjectCreatIndex = -1;
	int selectedItemIndex = -1; // 선택된 항목 인덱스, 기본값 -1은 아무 항목도 선택되지 않음을 나타냅니다.

	bool isMouseDragging = false; // 마우스 드래그 상태 저장
	_vec2 mouseClickPos; // 클릭된 좌표 저장

	bool	m_bTest;

private: // 애니메이션 툴 변수

	// 애니메이션의 타입 (왼손, 오른손)
	KEYTYPE m_iCurType = KEYTYPE_RIGHTHAND;

	// 애니메이션 타임 라인
	std::vector<KEYFRAME> timeline[KEYTYPE_END];


	// 애니메이션 타임라인
	float currentTime = 0.0f; // 현재 시간 값
	float MaxTime = 20.f;
	float fDisplayTime = 1.f;	// 눈금마다의 시간 값 표시

	int iOldIndex = -1;

	_bool isDraggingTimeline = false;
	_bool isPlaying = false;		// 재생 여부

	// 설정
	_bool m_bSheild = false;
	_bool m_bSheildFrame = false;
	_bool m_bCharging = false;
	_bool m_bChargingFrame = false;


	_bool m_bRendomValue = false; // 벨류 값 랜덤으로 주기
	_bool m_bIndividualTexture = false; // 텍스처 값 순차적으로 주기

	float playbackSpeed = 1.0f; // 재생 속도

#pragma region 최소, 최대 값
	// 크기
	float	fMin_Scale = -5000.1f;	// 최소
	float	fMax_Scale = 5000.1f;	// 최대

	// 회전
	float	fMin_Rot = -180.f;		// 최소
	float	fMax_Rot = 180.f;	// 최대

	// 이동
	float	fMin_Pos = -5000.1f;		// 최소
	float	fMax_Pos = 5000.1f;	// 최대

	// 시간
	float	fMin_Time = 0.f;	// 최소
	float	fMax_Time = MaxTime;// 최대

	// 벨류
	float	fMin_Value = 0.f;	// 최소
	float	fMax_Value = 1.f;	// 최대
#pragma endregion

#pragma region 최소, 최대 자동 생성 값

	_vec2 _v2Time = { 0.f, 0.f };
	_vec2 _v2Value = { 0.f, 0.f };
	_vec3 minScale = { 0.f, 0.f, 0.f };
	_vec3 maxScale = { 0.f, 0.f, 0.f };
	_vec3 minRot = { 0.f, 0.f, 0.f };
	_vec3 maxRot = { 0.f, 0.f, 0.f };
	_vec3 minPos = { 0.f, 0.f, 0.f };
	_vec3 maxPos = { 0.f, 0.f, 0.f };

	// 텍스처 프레임
	float	fMin_Texture = 0.f;	// 최소
	float	fMax_Texture = 0.f;	// 최대

	_float numKeyframes = 0.f;
#pragma endregion

	//
	bool m_bChargePossible;	// 차지 가능 여부
	bool m_bShieldPossible;	// 방어 가능 여부

	float m_fFullChargeFrame;	// 풀차지시 프레임
	float m_fShieldFrame;		// 쉴드시 프레임

#pragma region 애니메이션 타임 라인

	// 키프레임 변경 관련 함수
	float oldKeyframeIndex = 0.f;

#pragma endregion

	float oldTime = 0.f;	// 이전 시간 값 저장
	float newTime = 0.f;	// 새로운 시간 값 (원하는 값으로 변경)
	float originalIndex;

	float vValueSize = 1.f;	// 변경을 줄 값의 크기
	float fValueCurve = 0.1f; // 변경을 줄 값을 조절하는 변수


	int closestKeyframeIndex = -1; // # 현재 마우스로 선택한 키프레임 인덱스

	// 선택한 키프레임 인덱스를 저장하는 변수 (선택한 키프렘의 인덱스를 초기화합니다.)
	int selectedKeyframeIndex = -1;

	// 키프레임 드래그
	bool	isDraggingKeyframe = false;
	int		draggingKeyframeIndex = 0;

	// 애니메이션 재생
	int animationFrame = 0;
	float playbackTime = 0.f;

	float initialKeyframeX = 0.f; // 마우스 드래그 시작 위치

	// 애니메이션 반복 여부
	int bRepetition = 1;

	// 애니메이션 저장 파일명
	char cFileSaveName[128] = "Animation";

	// 애니메이션 불러오기 파일명
	char cFileLoadName[128] = "Animation";

	// 이전에 그려진 빨간색 원을 저장하기 위한 변수
	_vec2 lastRedCirclePos;

	// ==============폴더 경로==============
	// 이미지 경로 목록을 저장하는 벡터
	std::vector<std::string> imagePaths = 
	{
		"../Client/Resource/Texture/Player",
		"../Client/Resource/Texture/Monster",
		"../Client/Resource/Texture/Item",
		"../Client/Resource/Texture/BUTTON",
		"../Client/Resource/Texture/crosshair",
		"../Client/Resource/Texture/cursor",
		"../Client/Resource/Texture/effect",
		"../Client/Resource/Texture/Explosion",
		"../Client/Resource/Texture/Font",
		"../Client/Resource/Texture/idk",
		"../Client/Resource/Texture/LIGHT",
		"../Client/Resource/Texture/Lights",
		"../Client/Resource/Texture/MAP",
		"../Client/Resource/Texture/number",
		"../Client/Resource/Texture/Scene",
		"../Client/Resource/Texture/SkyBox",
		"../Client/Resource/Texture/Terrain",
		"../Client/Resource/Texture/Tile",
		"../Client/Resource/Texture/UI",
		"../Client/Resource/Texture/Weapon",
		"../Client/Resource/Texture/Woman"
	};

	std::string m_cPath[5] = {
		"Apple",
		"Banana",
		"Cherry",
		"Date"
	};
	int selectedPathIndex = 0; // 선택된 이미지 경로 인덱스

	float fScaleSpeed = 0.1f; // 크기 조정 속도 조절
};

#pragma region 단축키

// "A" 키 : 현재 마우스 위치에 키프레임을 생성
// "R" 키 : 현재 시간(시간선)을 0으로 초기화 [처음으로 이동]
// "T" 키 : 현재 시간(시간 선)을 마우스 위치로 변경
// "Space" 키 : 애니메이션 재생/정지
// "0" 키 : 재생 속도 초기화
// "-" 키 : 재생 속도 -
// "+" 키 : 재생 속도 +

#pragma endregion