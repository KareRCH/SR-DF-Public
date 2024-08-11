#pragma once

#include "Serialize_Core.h"

using namespace rapidjson;


BEGIN(Engine)

#define RPJSON_IS_MEMBER(value, name)	\
			(value.HasMember(name))

#define RPJSON_RECIEVE_STRING(value, name)	\
			((RPJSON_IS_MEMBER(value, name) && !value[name].IsNull() && value[name].IsString()) ? value[name].GetString() : "")

#define RPJSON_RECIEVE_BOOL(value, name)	\
			((RPJSON_IS_MEMBER(value, name) && !value[name].IsNull() && value[name].IsBool()) ? value[name].GetBool() : false)

#define RPJSON_RECIEVE_FLOAT(value, name)	\
			((RPJSON_IS_MEMBER(value, name) && !value[name].IsNull() && value[name].IsFloat()) ? value[name].GetFloat() : 0.f)

#define RPJSON_RECIEVE_INT(value, name)	\
			((RPJSON_IS_MEMBER(value, name) && !value[name].IsNull() && value[name].IsInt()) ? value[name].GetInt() : 0)

#define RPJSON_IS_MEMARRAY(value, name)		\
			(value.HasMember(name) && value[name].IsArray())

/// <summary>
/// 컴포넌트가 가지는 직렬화 데이터
/// </summary>
class ENGINE_DLL FSerialize_Component
{
public:
	FSeialize_Header	tHeader;
	//string
};



/// <summary>
/// 오브젝트가 가지는 직렬화 데이터
/// </summary>
class ENGINE_DLL FSerialize_GameObject
{
public:
	FSeialize_Header				tHeader;
	EGO_CLASS						eID;						// 실제 오브젝트의 원본 태그
	string							strClassName;				// 원본이 되는 클래스의 이름

	string							strGroupKey;
	string							strTextureKey;

	set<string>						bTag;						// 태그

	_float							fPriority_Update;			// 업데이트 우선도
	_bool							bUsePriority_Update;		// 업데이트 우선도 사용
	_float							fPriority_LateUpdate;
	_bool							bUsePriority_LateUpdate;
	_float							fPriority_Render;
	_bool							bUsePriority_Render;

	_vec3							vPos;
	_vec3							vRotation;
	_vec3							vScale;

	_vec3							vTexPos = { 0.f, 0.f, 0.f };
	_vec3							vTexRot = { 0.f, 0.f, 0.f };
	_vec3							vTexScale = { 1.f, 1.f, 1.f };

	_vec3							vColPos = { 0.f, 0.f, 0.f };
	_vec3							vColRot = { 0.f, 0.f, 0.f };
	_vec3							vColScale = { 1.f, 1.f, 1.f };

	vector<string>					vecUserString;				// 사용자 지정 문자열

	vector<FSerialize_Component>	vecComponent;
};


class ENGINE_DLL FSerialize_Proto
{
public:
	FSeialize_Header				tHeader;
	EGO_CLASS						eID;
	string							strClassName = "";

	string							strGroupKey = "";
	string							strTextureKey = "";

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

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf,
		const ESERIALIZE_PROCESS eProcess,
		const _bool bPrettyWriter = false) const;
	
	_bool Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble = false);
};




/// <summary>
/// 터레인이 가지는 직렬화 데이터
/// </summary>
class ENGINE_DLL FSerialize_Terrain
{
	
public:
	FSeialize_Header			tHeader;
	_vec3						vVertexCount = { 128.f, 128.f, 128.f };	// 버텍스 개수
	_vec3						vScale = { 1.f, 1.f, 1.f };				// 스케일, 정점간 너비
	_vec3						vInvOffset = { 0.f, 0.f, 0.f };			// 중점, 역버전
	

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf, _bool bPrettyWriter = false) const;
	_bool Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble = false);
	
};

/// <summary>
/// 레이어가 가지는 직렬화 데이터
/// </summary>
class ENGINE_DLL FSerialize_Layer
{
public:
	FSeialize_Header				tHeader;
	_float							fPriority;		// 우선도
	vector<FSerialize_GameObject>	vecGameObject;	// 오브젝트 정보
};

/// <summary>
/// 씬이 가지는 직렬화 데이터
/// </summary>
class ENGINE_DLL FSerialize_Scene
{
public:
	FSeialize_Header			tHeader;
	string						refTerrainName;		// 씬의 지형 정보가 담긴, 터레인 자료에 대한 참조
	vector<FSerialize_Layer>	vecLayer;			// 씬의 레이어 정보 

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf,
		const ESERIALIZE_PROCESS eProcess,
		const _bool bPrettyWriter = false) const;
	

	_bool Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble = false);
	
};


END