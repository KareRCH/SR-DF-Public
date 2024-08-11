#pragma once

#include "ImguiWin.h"

#include "imgui.h"
#include "Serialize_Core.h"
#include "Serialize_BaseClass.h"


const string g_strProtoPath = "../Client/Resource/Data/Proto/";

const string g_strProtoExt = ".aproto";

struct FComponentData
{

};

class FProtoData
{
public:
	virtual ~FProtoData() {}
public:
	string							strName = "";				// 프로토타입의 이름
	EGO_CLASS						eID = ECLASS_NONE;			// 프로토타입의 원본
	string							strClassName = "";

	string							strGroupKey = "";			// 텍스처 그룹키
	string							strTextureKey = "";			// 텍스처키

	_vec3							vPos = { 0.f, 0.f, 0.f };
	_vec3							vRot = { 0.f, 0.f, 0.f };
	_vec3							vScale = { 1.f, 1.f, 1.f };

	_vec3							vTexPos = { 0.f, 0.f, 0.f };
	_vec3							vTexRot = { 0.f, 0.f, 0.f };
	_vec3							vTexScale = { 1.f, 1.f, 1.f };

	_vec3							vColPos = { 0.f, 0.f, 0.f };
	_vec3							vColRot = { 0.f, 0.f, 0.f };
	_vec3							vColScale = { 1.f, 1.f, 1.f };

	vector<string>					vecUserString;				// 사용자 지정 문자열

	map<string, FComponentData>		mapComponent;

	
};

class FProtoCubeStructure : public FProtoData
{
public:
	virtual ~FProtoCubeStructure() {}
public:
	string			strGroupKey = "";
	string			strTextureKey = "";

	/*FProtoCubeStructure operator=(const FProtoData& tProto)
	{
		strName = tProto.strName;
		eID = tProto.eID;
		strTextureGroupKey = tProto.strTextureGroupKey;
		strTextureKey = tProto.strTextureKey;
		vPos = tProto.vPos;
		vRot = tProto.vRot;
		vScale = tProto.vScale;
		mapComponent = tProto.mapComponent;

		return *this;
	}*/
};

/// <summary>
/// 오브젝트의 프로토 타입을 제조하는 툴
/// </summary>
class CImguiWin_ProtoTool : public CImguiWin
{
	DERIVED_CLASS(CImguiWin, CImguiWin_ProtoTool)

private:
	explicit CImguiWin_ProtoTool();
	virtual ~CImguiWin_ProtoTool();

public:
	static CImguiWin_ProtoTool* Create();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;

private:
	void Layout_ObjectBrowser(const ImGuiWindowFlags& iMain_Flags);
	void Layout_Component(const ImGuiWindowFlags& iMain_Flags);
	void Layout_Property(const ImGuiWindowFlags& iMain_Flags);
	void Layout_Viewer(const ImGuiWindowFlags& iMain_Flags);


private:
	enum ESELECTED_TYPE : _uint
	{
		ESELECTED_TYPE_NONE,
		ESELECTED_TYPE_PROTO,
		ESELECTED_TYPE_END,
	};
	ImGuiStyle		m_pStyle;


private:
	void Save_Protos();
	void Export_Proto(const FSerialize_Proto& tProto);
	void Load_Protos();
	void Import_Proto(const string& strName, FSerialize_Proto& tProtoSerial, FProtoData& tProtoData);

private:	// 프로토
	_bool						m_bLoadProto_Init = false;
	string						m_strAdd_Proto = "";			// 씬 추가하기
	map<string, FProtoData*>	m_mapProtoData;					// 프로토 타입 데이터
	string						m_strCur_Proto = "";			// 현재 선택된 프로토 키
	string						m_strCur_Component = "";		// 현재 선택된 컴포넌트 키

	ESELECTED_TYPE				m_eSelected_Type = ESELECTED_TYPE_NONE;

	// 콤보박스 전용 오브젝트 선택
	vector<string>				m_vecObject_Type = {
		"None",
		"Player",
		"Brown",
		"Grey",
		"Boss",
		"Food",
		"Weapon",
		"Throw",
		"Interaction",
		"Building",
	};

private:	// 유틸리티
	void			Set_Button_ActiveColor();
	void			Set_Button_NonActiveColor();
	void			Set_Button_ReturnColor();

	void			Clamp_Vec3Translate(_vec3& vTranslate, _float fValue)
	{
		if (vTranslate.x < -fValue)
			vTranslate.x = -fValue;
		else if (vTranslate.x > fValue)
			vTranslate.x = fValue;

		if (vTranslate.y < -fValue)
			vTranslate.y = -fValue;
		else if (vTranslate.y > fValue)
			vTranslate.y = fValue;

		if (vTranslate.z < -fValue)
			vTranslate.z = -fValue;
		else if (vTranslate.z > fValue)
			vTranslate.z = fValue;
	}
	void			Clamp_Vec3Rot(_vec3& vRot, _float fValue)
	{
		if (vRot.x < -fValue)
			vRot.x = -fValue;
		else if (vRot.x > fValue)
			vRot.x = fValue;

		if (vRot.y < -fValue)
			vRot.y = -fValue;
		else if (vRot.y > fValue)
			vRot.y = fValue;

		if (vRot.z < -fValue)
			vRot.z = -fValue;
		else if (vRot.z > fValue)
			vRot.z = fValue;
	}
	void			Clamp_Vec3Scale(_vec3& vScale, _float fValue)
	{
		if (vScale.x < -fValue)
			vScale.x = -fValue;
		else if (vScale.x > fValue)
			vScale.x = fValue;

		if (vScale.y < -fValue)
			vScale.y = -fValue;
		else if (vScale.y > fValue)
			vScale.y = fValue;

		if (vScale.z < -fValue)
			vScale.z = -fValue;
		else if (vScale.z > fValue)
			vScale.z = fValue;
	}


};

