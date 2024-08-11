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

	// ���带 ����ϴ� ��ǥ��ü�� �����ϴ� �Լ�
	result = FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	result = FMOD_System_SetOutput(m_pSystem, FMOD_OUTPUTTYPE_ALSA);

	// 1. �ý��� ������, 2. ����� ����ä�� �� , �ʱ�ȭ ���) 
	result = FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	result = FMOD_System_CreateChannelGroup(m_pSystem, "BGM_Group", &m_pChannelGroup[BGM_GROUP]);
	result = FMOD_System_CreateChannelGroup(m_pSystem, "SND1_Group", &m_pChannelGroup[SND1_GROUP]);
	result = FMOD_System_CreateChannelGroup(m_pSystem, "SND2_Group", &m_pChannelGroup[SND2_GROUP]);

	result = FMOD_System_GetMasterChannelGroup(m_pSystem, &m_pMasterChanelGroup);

	// ���� ���� ���� ã�� �ε�
	
	// ���������� ��ݸ���Ʈ 
	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/bgm/");
	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/sfx/etc/");

	// �÷��̾� 
	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/Player/");
	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/Player/Attack/");
	LoadSoundFile_GroupAsync(L"FallenAces", "./Resource/Sound/FallenAces/Player/Effect/");

	// Food ����
	LoadSoundFile_GroupAsync(L"Food", "./Resource/Sound/FallenAces/sfx/Food/");

	//���� ���� 
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
	// ī�װ� Ű
	auto iterCate = m_mapSound.find(pCategoryKey);
	if (iterCate == m_mapSound.end())
		return;

	// ���� Ű
	auto iter = iterCate->second->Get_MapSound().find(pSoundKey);


	// ������� ���� �÷���
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
	// ī�װ� Ű
	auto iterCate = m_mapSound.find(pCategoryKey);
	if (iterCate == m_mapSound.end())
		return;

	// ���� Ű
	auto iter = iterCate->second->Get_MapSound().find(pSoundKey);


	// ������� ���� �÷���
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
	// m_mapSound ��ȣ
	m_mtxSound.lock();
	// ����� ī�װ� Ű �����, ������ Ű�� ����� �����̳ʸ� ����� �ش�.
	auto iter = m_mapSound.find(pCategoryKey);
	if (iter == m_mapSound.end())
	{
		m_mapSound.emplace(pCategoryKey, FSoundData::Create());
	}
	m_mtxSound.unlock();

	// ������� ���� �ε��
	char sText[128] = "";
	strcpy_s(sText, pPath);
	strcat_s(sText, "*.*");

	// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	_finddata_t fd;

	// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
	auto handle = _findfirst(sText, &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "";
	char szFullPath[128] = "";

	strcpy_s(szCurPath, pPath);

	// �̰� ���� ��
	using sound_tuple = tuple<FMOD_SOUND*, FMOD_RESULT, string, string>;
	enum ETMP_SOUND : int {
		TMP_SOUND,
		TMP_RESULT,
		TMP_PATH,
		TMP_FILE_NAME
	};

	// �񵿱� ó���� �����带 ����ϱ� ������ ��� �����͸� ������ �����̳ʿ� �����ϰ� ó�����ݴϴ�.
	vector<future<FMOD_RESULT>> vecAsync;
	vector<sound_tuple> vecSoundData;
	
	// ������ �Էº�
	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/Success.wav"
		strcat_s(szFullPath, fd.name);
		sound_tuple tpSound;

		// �����忡 �Ѱ��� �����͸� �Է�
		get<TMP_PATH>(tpSound) = string(szFullPath);
		get<TMP_FILE_NAME>(tpSound) = string(fd.name);
		get<TMP_RESULT>(tpSound) = FMOD_OK;
		get<TMP_SOUND>(tpSound) = nullptr;

		vecSoundData.push_back(tpSound);

		//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
		iResult = _findnext(handle, &fd);
	}

	// ������ ����
	for (size_t i = 0; i < vecSoundData.size(); i++)
	{
		vecAsync.push_back(async(launch::async, &CSoundMgr::LoadSoundFile_Async, this
									, get<TMP_PATH>(vecSoundData[i]).c_str(), get<TMP_FILE_NAME>(vecSoundData[i]).c_str()
									, ref(get<TMP_RESULT>(vecSoundData[i])), &get<TMP_SOUND>(vecSoundData[i])));
	}

	// ��� �޾ƿ���
	for (size_t i = 0; i < vecSoundData.size(); i++)
	{
		vecAsync[i].get();

		m_mtxSound.lock();
		if (get<TMP_RESULT>(vecSoundData[i]) == FMOD_OK)
		{
			int iLength = (int)strlen(get<TMP_FILE_NAME>(vecSoundData[i]).c_str()) + 1;

			_tchar* pSoundKey = new _tchar[iLength];
			ZeroMemory(pSoundKey, sizeof(_tchar) * iLength);

			// �ƽ�Ű �ڵ� ���ڿ��� �����ڵ� ���ڿ��� ��ȯ�����ִ� �Լ�
			MultiByteToWideChar(CP_ACP, 0, get<TMP_FILE_NAME>(vecSoundData[i]).c_str(), iLength, pSoundKey, iLength);

			m_mapSound[pCategoryKey]->Get_MapSound().emplace(pSoundKey, get<TMP_SOUND>(vecSoundData[i]));

			Safe_Delete_Array(pSoundKey);
		}
		m_mtxSound.unlock();
	}

	// FMOD ������Ʈ
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

