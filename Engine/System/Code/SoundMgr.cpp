#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
{
}


CSoundMgr::~CSoundMgr()
{
	Free();
}

void CSoundMgr::Free()
{
	for (auto& pairSoundCtn : m_mapSound)
	{
		for (auto& pairSound : pairSoundCtn.second->Get_MapSound())
		{
			FMOD_Sound_Release(pairSound.second);
		}
		pairSoundCtn.second->Free();
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

HRESULT CSoundMgr::Ready_Sound()
{
	FMOD_RESULT	result;

	// 사운드를 담당하는 대표객체를 생성하는 함수
	result = FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	result = FMOD_System_SetOutput(m_pSystem, FMOD_OUTPUTTYPE_ALSA);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	result = FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	result = FMOD_System_CreateChannelGroup(m_pSystem, "BGM_Group", &m_pChannelGroup[BGM_GROUP]);
	result = FMOD_System_CreateChannelGroup(m_pSystem, "SND1_Group", &m_pChannelGroup[SND1_GROUP]);
	result = FMOD_System_CreateChannelGroup(m_pSystem, "SND2_Group", &m_pChannelGroup[SND2_GROUP]);

	result = FMOD_System_GetMasterChannelGroup(m_pSystem, &m_pMasterChanelGroup);

	// 실제 사운드 폴더 찾아 로드
	
	// 스테이지별 브금리스트 
	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/bgm/");
	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/sfx/etc/");

	// 플레이어 
	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/Player/");
	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/Player/Attack/");
	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/Player/Effect/");

	// Food 사운드
	LoadSoundFile_GroupAsync(L"Food", "./Resource/Sound/FallenAces/sfx/Food/");

	//몬스터 사운드 
	LoadSoundFile_GroupAsync(L"Enemy", "./Resource/Sound/FallenAces/Monster/BossMonster/");
	LoadSoundFile_GroupAsync(L"Enemy", "./Resource/Sound/FallenAces/Monster/Brown/");
	LoadSoundFile_GroupAsync(L"Enemy", "./Resource/Sound/FallenAces/Monster/Gray/");
	LoadSoundFile_GroupAsync(L"Enemy", "./Resource/Sound/FallenAces/Monster/SkillEffect/");

	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/extra/");

	
	Wait_GroupAsync();

	return S_OK;
}

void CSoundMgr::Play_Sound(_tchar* pCategoryKey, _tchar* pSoundKey, CHANNELID eID, float fVolume)
{
	// 카테고리 키
	auto iterCate = m_mapSound.find(pCategoryKey);
	if (iterCate == m_mapSound.end())
		return;

	// 사운드 키
	auto iter = iterCate->second->Get_MapSound().find(pSoundKey);


	// 여기부터 사운드 플레이
	FMOD_BOOL bPlay = FALSE;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[SND1_GROUP], FALSE, &m_pChannelArr[eID]);
	}
	else
	{
		FMOD_Channel_Stop(m_pChannelArr[eID]);
		FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[SND1_GROUP], FALSE, &m_pChannelArr[eID]);
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], 0.1f);//fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Play_BGM(_tchar* pCategoryKey, _tchar* pSoundKey, float fVolume)
{
	// 카테고리 키
	auto iterCate = m_mapSound.find(pCategoryKey);
	if (iterCate == m_mapSound.end())
		return;

	// 사운드 키
	auto iter = iterCate->second->Get_MapSound().find(pSoundKey);


	// 여기부터 사운드 플레이
	CSoundMgr::GetInstance()->Stop_Sound(SOUND_BGM);
	FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[BGM_GROUP], FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], 0.1f);//fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Stop_Sound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::LoadSoundFile(_tchar* pCategoryKey, const char* pPath)
{
	// m_mapSound 보호
	m_mtxSound.lock();
	// 여기는 카테고리 키 만들기, 없으면 키를 만들고 컨테이너를 만들어 준다.
	auto iter = m_mapSound.find(pCategoryKey);
	if (iter == m_mapSound.end())
	{
		m_mapSound.emplace(pCategoryKey, FSoundData::Create());
	}
	m_mtxSound.unlock();

	// 여기부터 파일 로드부
	char sText[128] = "";
	strcpy_s(sText, pPath);
	strcat_s(sText, "*.*");

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	auto handle = _findfirst(sText, &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "";
	char szFullPath[128] = "";

	strcpy_s(szCurPath, pPath);

	// 이게 뭐게 ㅋ
	using sound_tuple = tuple<FMOD_SOUND*, FMOD_RESULT, string, string>;
	enum ETMP_SOUND : int {
		TMP_SOUND,
		TMP_RESULT,
		TMP_PATH,
		TMP_FILE_NAME
	};

	// 비동기 처리는 쓰레드를 사용하기 때문에 모든 데이터를 별도의 컨테이너에 저장하고 처리해줍니다.
	vector<future<FMOD_RESULT>> vecAsync;
	vector<sound_tuple> vecSoundData;
	
	// 데이터 입력부
	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/Success.wav"
		strcat_s(szFullPath, fd.name);
		sound_tuple tpSound;

		// 쓰레드에 넘겨줄 데이터를 입력
		get<TMP_PATH>(tpSound) = string(szFullPath);
		get<TMP_FILE_NAME>(tpSound) = string(fd.name);
		get<TMP_RESULT>(tpSound) = FMOD_OK;
		get<TMP_SOUND>(tpSound) = nullptr;

		vecSoundData.push_back(tpSound);

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	// 쓰레드 실행
	for (size_t i = 0; i < vecSoundData.size(); i++)
	{
		vecAsync.push_back(async(launch::async, &CSoundMgr::LoadSoundFile_Async, this
									, get<TMP_PATH>(vecSoundData[i]).c_str(), get<TMP_FILE_NAME>(vecSoundData[i]).c_str()
									, ref(get<TMP_RESULT>(vecSoundData[i])), &get<TMP_SOUND>(vecSoundData[i])));
	}

	// 결과 받아오기
	for (size_t i = 0; i < vecSoundData.size(); i++)
	{
		vecAsync[i].get();

		m_mtxSound.lock();
		if (get<TMP_RESULT>(vecSoundData[i]) == FMOD_OK)
		{
			int iLength = (int)strlen(get<TMP_FILE_NAME>(vecSoundData[i]).c_str()) + 1;

			_tchar* pSoundKey = new _tchar[iLength];
			ZeroMemory(pSoundKey, sizeof(_tchar) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, get<TMP_FILE_NAME>(vecSoundData[i]).c_str(), iLength, pSoundKey, iLength);

			m_mapSound[pCategoryKey]->Get_MapSound().emplace(pSoundKey, get<TMP_SOUND>(vecSoundData[i]));

			Safe_Delete_Array(pSoundKey);
		}
		m_mtxSound.unlock();
	}

	// FMOD 업데이트
	FMOD_System_Update(m_pSystem);

	_findclose(handle);
}

void CSoundMgr::LoadSoundFile_GroupAsync(_tchar* pGroupKey, const char* pPath)
{
	m_vecAsyncSoundGroup.push_back(async(launch::async, &CSoundMgr::LoadSoundFile, this, pGroupKey, pPath));
}

void CSoundMgr::Wait_GroupAsync()
{
	for (_uint i = 0; i < m_vecAsyncSoundGroup.size(); i++)
	{
		m_vecAsyncSoundGroup[i].get();
	}
	m_vecAsyncSoundGroup.clear();
}

FMOD_RESULT CSoundMgr::LoadSoundFile_Async(const char* pPath, const char* pFileName, FMOD_RESULT& hResult, FMOD_SOUND** pSound)
{
	hResult =  FMOD_System_CreateSound(m_pSystem, pPath, FMOD_DEFAULT, NULL, pSound);

	return hResult;
}

