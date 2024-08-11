#pragma once

#include "Base.h"
#include "Engine_Define.h"

#include <mutex>
#include <future>
#include <thread>

BEGIN(Engine)

class ENGINE_DLL FSoundData
{
	THIS_CLASS(FSoundData)
private:
	FSoundData() {}
	~FSoundData() {}

public:
	static FSoundData* Create()
	{
		ThisClass* pInstance = new ThisClass;

		return pInstance;
	}
	void Free()
	{
		mapSound.clear();

		delete this;
	}

public:
	using map_sound = _unmap<wstring, FMOD_SOUND*>;
	GETSET_EX1(map_sound, mapSound, MapSound, GET_REF)

private:
	map_sound mapSound;
};

class ENGINE_DLL CSoundMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CSoundMgr)

private:
	CSoundMgr();
	~CSoundMgr();

private:
	virtual void Free() override;

public:
	HRESULT Ready_Sound();

public:
	// 사운드 재생
	void Play_Sound(_tchar* pCategoryKey, _tchar* pSoundKey, CHANNELID eID, float fVolume);
	// 브금 재생
	void Play_BGM(_tchar* pCategoryKey, _tchar* pSoundKey, float fVolume);
	// 사운드 정지
	void Stop_Sound(CHANNELID eID);
	// 모든 사운드 정지
	void StopAll();
	// 채널의 볼륨 설정
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	// 내부적으로 로드할 사운드 파일 폴더를 지정해 로드하는 함수
	void LoadSoundFile(_tchar* pCategoryKey, const char* pPath);
	// 사운드 파일을 그룹 단위로 비동기 로드할 때 쓰이는 함수
	void LoadSoundFile_GroupAsync(_tchar* pGroupKey, const char* pPath);
	// 비동기 처리를 기다리는 함수
	void Wait_GroupAsync();
	// 사운드 파일 단일로 비동기 로드할 때 쓰이는 함수ㅈ
	FMOD_RESULT LoadSoundFile_Async(const char* pPath, const char* pFileName, FMOD_RESULT& hResult, FMOD_SOUND** pSound);

	// 사운드 리소스 정보를 갖는 객체 
	_unmap<wstring, FSoundData*>	m_mapSound;
	mutex							m_mtxSound;
	vector<future<void>>			m_vecAsyncSoundGroup;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL*		m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM*		m_pSystem = nullptr;

	FMOD_CHANNELGROUP*	m_pChannelGroup[MAX_CHANNEL_GROUP];
	FMOD_CHANNELGROUP*	m_pMasterChanelGroup;
};

END