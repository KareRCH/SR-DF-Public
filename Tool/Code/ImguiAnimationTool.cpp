#include "ImguiAnimationTool.h"
#include <Windows.h>
#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include <ImguiMgr.h>

#include "PlayerAnimation.h"

CImguiAnimationTool::CImguiAnimationTool()
{
}

CImguiAnimationTool::~CImguiAnimationTool()
{
}

CImguiAnimationTool* CImguiAnimationTool::Create()
{
    ThisClass* pInstance = new ThisClass;

    if (FAILED(pInstance->Ready_ImguiWin()))
    {
        Safe_Release(pInstance);

        MSG_BOX("BackGround Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CImguiAnimationTool::Free()
{
    SUPER::Free();

    if (m_pTexture != nullptr)
        m_pTexture->Release();
    if (m_pRenderTargetSurface != nullptr)
        m_pRenderTargetSurface->Release();
}

// 이미지 추가 함수
void CImguiAnimationTool::AddImage(const std::string& imagePath)
{

    // imagePath를 Wide 문자열로 변환
    WCHAR wideImagePath[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, imagePath.c_str(), -1, wideImagePath, MAX_PATH);

    // 이미지를 로드하고 텍스처로 변환
    LPDIRECT3DDEVICE9 pGraphicDev = CImguiMgr::GetInstance()->Get_GraphicDev();
    if (FAILED(D3DXCreateTextureFromFile(pGraphicDev, wideImagePath, &m_pTexture)))
    {
        // 이미지 로드 실패 처리
        MSG_BOX("Image Load Failed");
    }
}

HRESULT CImguiAnimationTool::Ready_ImguiWin()
{
    return S_OK;
}

#pragma region 레이아웃
_int CImguiAnimationTool::Update_ImguiWin(const _float& fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    SUPER::Update_ImguiWin(fTimeDelta);

#pragma region 도킹 스테이션



    ImGuiWindowFlags iMain_Flags = ImGuiWindowFlags_NoMove;
    ImGuiDockNodeFlags iDockSpace_Flags = ImGuiDockNodeFlags_None;

    // [학준] 도킹 스테이션 바인딩
    ImGui::Begin("Center", NULL, iMain_Flags);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        // 기본 독
        ImGuiID dockspace_id = ImGui::GetID("DockSpace_AnimationTool");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), iDockSpace_Flags);

        if (m_bFirstLoop)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, iDockSpace_Flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

            // 오른쪽에 붙이고, 가로 너비 조정
            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, NULL, &dockspace_id);
            // 오른쪽 탭으로 붙이고, 가로 너비를 조절
            ImGuiID dock_right_tap_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.3f, &dock_right_id, &dockspace_id);
            // 오른쪽 아래 붙이고, 너비 조정
            ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.328f, NULL, &dock_right_id);
            // 오른쪽 아래 탭으로 붙이고, 너비 조정
            ImGuiID dock_right_down_tap_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.328f, &dock_right_down_id, &dock_right_id);
            // 아래쪽에 붙이고, 너비 조정
            ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.32f, NULL, &dockspace_id);


            ImGui::DockBuilderDockWindow(u8"텍스처", dock_right_id);
            ImGui::DockBuilderDockWindow(u8"Generate", dock_right_tap_id);
            ImGui::DockBuilderDockWindow(u8"키프레임 수정", dock_right_down_id);
            ImGui::DockBuilderDockWindow(u8"키프레임 정보", dock_right_down_tap_id);
            ImGui::DockBuilderDockWindow(u8"애니메이션 툴", dock_down_id);
            ImGui::DockBuilderDockWindow(u8"뷰어", dockspace_id);

            ImGui::DockBuilderFinish(dockspace_id);
        }

        m_bFirstLoop = false;
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

#pragma endregion

#pragma region 뷰어
    if (ImGui::Begin(u8"뷰어", NULL, iMain_Flags))
    {
        CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 clipSize = ImVec2(contentSize.x / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferWidth,
            contentSize.y / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferHeight);

        // 이미지 조정
        ImGui::Image((void*)pImguiMgr->Get_VecRenderTargetTex(0), contentSize,
            ImVec2((1.f - clipSize.x) * 0.5f, (1.f - clipSize.y) * 0.5f),
            ImVec2(clipSize.x + (1.f - clipSize.x) * 0.5f, clipSize.y + (1.f - clipSize.y) * 0.5f));

    }   ImGui::End();
#pragma endregion

    // ImGui에서 텍스처를 표시할 이미지 크기 (예: 200x200 픽셀)
    const ImVec2 imageSize(200.0f, 200.0f);

#pragma region 텍스처 툴
    static int counter = 0;

    if (ImGui::Begin(u8"텍스처"))
    {
        // 프레임
        ImGui::Text(u8"프레임 %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        // 문자열 벡터를 const char* 배열로 변환
        std::vector<const char*> charImagePaths = ConvertStringVectorToCharArray(imagePaths);

        int selectedPathIndex = 0; // 선택된 이미지 경로 인덱스

        if (ImGui::CollapsingHeader(u8"이미지 불러오기"))
        {
            if (ImGui::ListBox("Image Paths", &selectedPathIndex, charImagePaths.data(), (int)charImagePaths.size())) {

                // 선택한 이미지 경로를 사용하여 이미지를 로드하는 함수 호출
                const std::string& selectedImagePath = imagePaths[selectedPathIndex];
                //LoadImg(ConverCtoWC(selectedImagePath.c_str()));
                LoadImg(ConverCtoWC(ConverWStringtoC(ConvertToWideString(imagePaths[selectedPathIndex]))));
            }
        }

        ToggleButton(u8"토글", &m_bTest);

        static const char* ini_to_load = NULL;
        if (ini_to_load)
        {
            ImGui::LoadIniSettingsFromDisk(ini_to_load);
            ini_to_load = NULL;
        }

        if (ImGui::Button(u8"저장 버튼"))
        {
            //SaveObjectInformationData();

        }

        ImGui::SameLine();

        if (ImGui::Button(u8"불러오기 버튼"))
        {
            //LoadObjectInforamtionData();
        }

    }
    ImGui::End();
#pragma endregion

#pragma region 애니메이션 툴
    // 타임라인 툴
    ImGui::Begin(u8"애니메이션 툴");
    RenderTimeline();
    ImGui::End();
#pragma endregion

    // [학준] Center End
    ImGui::End();

    return 0;
}
#pragma endregion

// 이미지 로드
void CImguiAnimationTool::LoadImg(const _tchar* folderPath)
{
    // 찾은 이미지 데이터를 받을 변수
    WIN32_FIND_DATA findData;

    // 폴더 경로
    wstring wfolderPath = (wstring)folderPath + L"/*.*";

    // 폴더 경로 저장 및 핸들 반환
    HANDLE hFind = FindFirstFile(wfolderPath.c_str(), &findData);

    // 핸들이 있을 경우
    if (hFind != INVALID_HANDLE_VALUE)
    {
        // 하위 디렉토리
        vector<wstring> subDirectories;
        // 이미지 파일
        vector<wstring> imageFiles;
        do
        {
            //  파일의 속성 정보가 입력된다. (디렉토리인지 파일인지 등)
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (lstrcmp(findData.cFileName, L".") != 0 && lstrcmp(findData.cFileName, L"..") != 0)
                {
                    // 폴더 경로 = 경로 + / + 파일명
                    wstring subFolderPath = (wstring)folderPath + L"/" + findData.cFileName;
                    subDirectories.push_back(subFolderPath);
                    LoadImg(subFolderPath.c_str());
                }
            }
            else
            {
                // 파일인 경우, 이미지 파일인지 확인하고 로드
                wstring filePath = (wstring)folderPath + L"\\" + findData.cFileName;

                // 파일 확장자 확인
                if (wcsstr(findData.cFileName, L".png") || wcsstr(findData.cFileName, L".jpg") ||
                    wcsstr(findData.cFileName, L".bmp") || wcsstr(findData.cFileName, L".tga") ||
                    wcsstr(findData.cFileName, L".dds"))
                {
                    // IDirect3DBaseTexture9 인터페이스의 메서드를 사용하여 큐브 및 볼륨 텍스처를 포함한 텍스처 리소스를 조작.
                    IDirect3DBaseTexture9* pBaseTexture = nullptr;

                    // D3DXCreateTextureFromFile(디바이스, 파일경로, 베이스 텍스처) 파일에서 텍스처를 만드는 함수.
                    // 이미지 파일인 경우, 텍스처 로드 및 관리
                    if (SUCCEEDED(D3DXCreateTextureFromFile(Engine::Get_GraphicDev(), filePath.c_str(), (LPDIRECT3DTEXTURE9*)&pBaseTexture)))
                    {
                        LPDIRECT3DTEXTURE9 pTexture = static_cast<LPDIRECT3DTEXTURE9>(pBaseTexture);
                        wstring filePathStr = filePath;

                        // 파일 경로를 슬래시(/)로 통일
                        Replace(filePathStr, L"\\", L"/");

                        // 파일 경로에서 객체 키와 상태 키를 추출
                        size_t pos = filePathStr.find_last_of(L"/");
                        wstring ObjKey, StateKey;
                        if (pos != wstring::npos)
                        {
                            ObjKey = filePathStr.substr(filePathStr.find_last_of(L"/", pos - 1) + 1, pos - (filePathStr.find_last_of(L"/", pos - 1) + 1));
                            StateKey = filePathStr.substr(pos + 1);
                        }
                        // 객체 키가 uniqueObj 벡터에 없을 경우 추가
                        if (find(uniqueObj.begin(), uniqueObj.end(), ObjKey) == uniqueObj.end())
                        {
                            uniqueObj.push_back(ObjKey);
                        }
                        // 상태 키가 uniqueState 벡터에 없을 경우 추가
                        if (find(uniqueState.begin(), uniqueState.end(), StateKey) == uniqueState.end())
                        {
                            uniqueState.push_back(StateKey);
                            m_MapTexture[make_pair(ObjKey, StateKey)] = pTexture;
                        }
                        // 객체 키에 해당하는 파일 목록에 상태 키 추가
                        m_MapFile[ObjKey].push_back(StateKey);
                    }
                }
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }
}

// 찾기
void CImguiAnimationTool::Replace(wstring& strCurrentDirectory, wstring strSearch, wstring strReplace)
{
    wstring temp = strCurrentDirectory;

    size_t start_pos = 0;
    while ((start_pos = temp.find(strSearch, start_pos)) != wstring::npos)
    {
        temp.replace(start_pos, strSearch.length(), strReplace);
        start_pos += strReplace.length();
    }
    strCurrentDirectory = temp;
}

#pragma region 문자열 변환 함수들
char* CImguiAnimationTool::ConverWStringtoC(const wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    char* result = new char[size_needed];
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result, size_needed, NULL, NULL);
    return result;
}

wchar_t* CImguiAnimationTool::ConverCtoWC(char* str)
{
    _tchar* pStr;
    int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
    pStr = new WCHAR[strSize];
    MultiByteToWideChar(CP_ACP, 0, str, (_int)strlen(str) + (size_t)1, pStr, strSize);

    return pStr;
}

char* CImguiAnimationTool::ConverWCtoC(wchar_t* str)
{
    char* pStr;
    int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
    pStr = new char[strSize];
    WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
    return pStr;
}

string CImguiAnimationTool::WstringToUTF8(const wstring& wstr)
{
    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), - 1, nullptr, 0, nullptr, nullptr);
    if (utf8Length == 0)
    {

        return string();
    }

    string utf8Str(utf8Length + 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], utf8Length + 1, nullptr, nullptr);

    return utf8Str;
}
#pragma endregion

//// 데이터 저장
//void CImguiAnimationTool::SaveObjectInformationData()
//{
//    OPENFILENAME    open;
//    TCHAR   lpstrFile[MAX_PATH] = L"";
//    static TCHAR filter[] = L"*.dat";
//
//    ZeroMemory(&open, sizeof(OPENFILENAME));
//    open.lStructSize = sizeof(OPENFILENAME);
//    open.lpstrFilter = filter;
//    open.lpstrFile = lpstrFile;
//    open.nMaxFile = 100;
//    open.lpstrInitialDir = L"";
//
//    wcscat_s(lpstrFile, L"PlayerData");
//    wcscat_s(lpstrFile, L".dat");
//
//    if (GetSaveFileName(&open) != 0) {
//
//        HANDLE hFile = CreateFile(lpstrFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
//
//        if (INVALID_HANDLE_VALUE == hFile)
//            return;
//
//        DWORD   dwByte = 0;
//
//        for (const KEYFRAME& keyframe : timeline[m_iCurType]) {
//            WriteFile(hFile, &keyframe, sizeof(KEYFRAME), &dwByte, nullptr);
//        }
//
//        // (생성된 파일, 저장할 정보 변수, 사이즈, 크기, nullptr);
//        WriteFile(hFile, &timeline[m_iCurType], sizeof(KEYFRAME), &dwByte, nullptr);
//
//        CloseHandle(hFile);
//    }
//}

//// 데이터 로드
//void CImguiAnimationTool::LoadObjectInforamtionData()
//{
//    OPENFILENAME    open;
//    TCHAR   lpstrFile[MAX_PATH] = L"";
//    static TCHAR filter[] = L"*.*\*.dat";
//
//    ZeroMemory(&open, sizeof(OPENFILENAME));
//    open.lStructSize = sizeof(OPENFILENAME);
//    open.lpstrFilter = filter;
//    open.lpstrFile = lpstrFile;
//    open.nMaxFile = 100;
//    open.lpstrInitialDir = L"";
//
//    wcscat_s(lpstrFile, L"PlayerData");
//    wcscat_s(lpstrFile, L".dat");
//
//    if (GetOpenFileName(&open) != 0) {
//
//        HANDLE hFile = CreateFile(lpstrFile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//
//        if (INVALID_HANDLE_VALUE == hFile)
//            return;
//
//        DWORD   dwByte = 0;
//        DWORD   dwStrByte = 0;
//
//        //TOTALITEMINFO* pItemPartsInfo = new TOTALITEMINFO;
//
//        ReadFile(hFile, &timeline[m_iCurType], sizeof(MYANIMATIONINFO), &dwByte, nullptr);
//
//        CloseHandle(hFile);
//    }
//}

// 파일 경로에서 파일 이름을 제거하는 함수
void CImguiAnimationTool::PathRemoveFileSpec(TCHAR* path)
{
    if (path == nullptr || *path == '\0')
        return;

    size_t len = wcslen(path);
    if (len == 0)
        return;

    // 경로의 뒷부분부터 검사하여 '\' 문자를 찾아 제거합니다.
    for (size_t i = len - 1; i > 0; --i)
    {
        if (path[i] == '\\')
        {
            path[i] = '\0';
            break;
        }
    }
}

// 문자열 타입 변환
std::wstring CImguiAnimationTool::ConvertToWideString(const std::string& ansiString)
{
    int wideStrLen = MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, nullptr, 0);
    if (wideStrLen == 0)
    {
        // 변환 실패 처리
        return L"Fail";
    }

    std::wstring wideString(wideStrLen, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, &wideString[0], wideStrLen);

    return wideString;
}

// 토글 버튼
void CImguiAnimationTool::ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos(); // 현재 커서위치를 화면 좌표로 반환해주는 함수 (마우스 배치나 피킹등에 활용 가능할듯 하다)
    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // 현재 창에 그림을 그리기 위한 포인터 코드
    // ImGui::GetWindowDrawList()는 현재 창에대한 포인터를 얻는다.

    float height = ImGui::GetFrameHeight(); // 기본 크기 값 설정
    float width = height * 1.55f;   // 토글버튼 배경 가로 길이
    float radius = height * 0.50f;  // 토글버튼 안쪽 동그라미 크기(원)

    // 사용자에겐 보이지 않지만, 클릭 이벤트를 감지하거나 상태를 변경하귀 위해 사용하는 함수
    //              (고유식별자(str_id), 버튼의 크기)
    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked()) // 위젯이 클릭되었을 때 '*v'라는 불리언 포인터 변수를 토글한다. 이 변수는 토글 버튼의 상태를 나타낸다.
        *v = !*v;

    float t = *v ? 1.0f : 0.0f; // '*v' 변수의 값에 따라 't' 값을 설정한다. 
    // ex : '*v'가 true면 't'는 1.0, false면 0.0

// 마우스 오버에따른 색상 설정
    ImU32 col_bg;
    if (ImGui::IsItemHovered()) // 마우스가 버튼위에 올라갔는지 판단 
        // ImLerp : 선형 보간 함수 (마우스 오버시 색상)
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
    else
        // ImLerp : 선형 보간 함수 (마우스 클릭시 색상)
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

    // draw_list를 사용하여 버튼의 배경을 그린다.
    // AddRectFilled 함수를 사용하여 사각형을 그린다.
    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);

    // 버튼 내부에 동그라미를 그린다. AddCircleFilled 함수를 사용하여 원을 그린다.
    draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}




ImVec2 timelinePos(0.f, 0.f);
ImVec2 timelineSize(800.f, 85.f); // 애니메이션 타임 라인 크기

//==============================================[애니메이션 툴]==============================================
void CImguiAnimationTool::RenderTimeline()
{

#pragma region 시간 타임라인

    // 시간
    ImGui::Text(u8"시간 : %.2f", currentTime);
    ImGui::SameLine(80.f);

    ImGui::PushItemWidth(150);
    // 시간 그래프
    float fFrame[100];
    for (int n = 0; n < 100; n++)
        fFrame[n] = sinf(n * 0.2f + (_float)ImGui::GetTime() * playbackSpeed);
    ImGui::PlotLines(u8"시간 그래프", fFrame, 100);
    ImGui::PopItemWidth();
    ImGui::SameLine();

    if (ImGui::Button(u8"시간 초기화"))
    {
        currentTime = 0.f;
    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(20, 0)); // 가로 간격을 추가
    ImGui::SameLine();

#pragma endregion

#pragma region 애니메이션 재생/정지

    // 애니메이션 재생/일시 정지 버튼
    if (isPlaying) {
        if (ImGui::Button(u8"정지"))
        {
            isPlaying = false;
        }
    }
    else {
        if (ImGui::Button(u8"재생"))
        {
            isPlaying = true;
            playbackTime = 0.0f;
        }
    }

    ImGui::SameLine();
    //ImGui::Spacing(); // 간격 추가
    //ImGui::Dummy(ImVec2(0, 20)); // 세로 간격을 추가
    //ImGui::SameLine();

    ImGui::PushItemWidth(150); // 가로 사이즈
    // 애니메이션 재생 속도 조절
    ImGui::SliderFloat(u8"재생 속도", &playbackSpeed, 0.1f, 3.0f);
    ImGui::SameLine();

    // 속도 조절 버튼
    if (ImGui::Button(u8"속도 초기화")) // 속도 기본값으로 초기화
    {
        playbackSpeed = 1.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("-")) // 속도 -
    {
        playbackSpeed -= 1.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("+")) // 속도 +
    {
        playbackSpeed += 1.f;
    }
    ImGui::PopItemWidth();



    ImVec4 textColor = { 1.0f, 0.0f, 0.0f, 1.0f }; // 텍스트 컬러 (RGBA)
    ImGui::SameLine(820.f); // 위치

    // 현재 변경 모드 표시
    switch (eEditMode)
    {
    case EDITMODE_NONE:
    {
        textColor = { 1.f, 1.f, 1.f, 1.f };
        ImGui::TextColored(textColor, u8"변경[OFF]");
        break;
    }
    case EDITMODE_SCALE:
    {
        ImGui::TextColored(textColor, u8"크기 변경[ON]");
        break;
    }
    case EDITMODE_ROTATION:
    {
        ImGui::TextColored(textColor, u8"회전 변경[ON]");
        break;
    }
    case EDITMODE_TRANSLATION:
    {
        ImGui::TextColored(textColor, u8"이동 변경[ON]");
        break;
    }
    case EDITMODE_TIME_VALUE:
    {
        ImGui::TextColored(textColor, u8"시간 및 벨류 변경[ON]");
        break;
    }
    case EDITMODE_TEXTURE:
    {
        ImGui::TextColored(textColor, u8"텍스처 변경[ON]");
        break;
    }
    default:
        break;
    }

    ImVec4 textColor2 = { 1.0f, 1.0f, 1.0f, 1.0f }; // 텍스트 컬러 (RGBA)
    ImGui::SameLine(920.f); // 위치
    ImGui::TextColored(textColor2, u8"현재 변경 값 : %f", vValueSize);

#pragma endregion

    ImGui::Separator(); // 선 추가

#pragma region 애니메이션 타임 라인
    // 타임 라인 렌더링

    //ImDrawList* draw_list = ImGui::GetWindowDrawList(); // 그리기
    timelinePos = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // 그리기
    // 애니메이션 타임 라인 위치 설정
    timelinePos.x += 0.f;
    timelinePos.y += 30.f;

    // 애니메이션 타임 라인 그리기
    draw_list->AddRectFilled(
        timelinePos,
        ImVec2(timelinePos.x + timelineSize.x, timelinePos.y + timelineSize.y),
        IM_COL32(50, 50, 50, 255) // 타임라인 배경 색상
    );

    // 현재 시간을 최소와 최대 시간 비율로 변환
    float minTime = 0.0f; // 최소 시간
    //float maxTime = 100.0f; // 최대 시간

    // 애니메이션 타임 라인 눈금 그리기 (1단위 표시 기준 : 최대 시간 값 만큼)
    for (int i = 0; i <= MaxTime; ++i)
    {
        // 눈금 시간의 간격                                최대 시간값으로 나눠 간격 설정
        float xPos = timelinePos.x + i * (timelineSize.x / MaxTime);

        float relativeStartTimeXPos = timelinePos.x;    //애니메이션 타임 라인 그래프의 시작
        float relativeEndTimeXPos = timelinePos.x + timelineSize.x; // 애니메이션 타임 라인 그래프의 끝

        // 그래프 데이터 배열을 현재 시간에 맞게 업데이트
        for (int n = 0; n < 100; n++) {
            float time = n / 100.0f * MaxTime;
            fFrame[n] = EvaluateAnimationAtTime(time);
        }

        // 현재 시간을 최대 시간 범위 내로 클램핑
        currentTime = ImClamp(currentTime, 0.0f, MaxTime);

        // 현재 시간을 최소와 최대 시간 비율로 변환
        float relativeCurrentTime = (currentTime - minTime) / (MaxTime - minTime);

        // 현재 시간 값으로 애니메이션 타임 라인에 맞는 위치값을 계산
        float relativeCurrentTimeXPos = ImLerp(relativeStartTimeXPos, relativeEndTimeXPos, relativeCurrentTime);

        // 빨간 선(현재 시간 위치) 그리기
        draw_list->AddLine(
            ImVec2(relativeCurrentTimeXPos, timelinePos.y), // 라인 시작점
            ImVec2(relativeCurrentTimeXPos, timelinePos.y + timelineSize.y), // 라인 끝점
            IM_COL32(255, 0, 0, 255) // 빨간 선으로 현재 시간 표시
        );

        // 눈금 아래에 숫자로 시간 표시
        float timeLabel = i * fDisplayTime * (MaxTime / MaxTime); // 0.1 간격으로 0부터 MaxTime까지 표시
        char timeLabelStr[16];
        snprintf(timeLabelStr, sizeof(timeLabelStr), "%.1f", timeLabel);
        ImVec2 textPos(xPos - 10, timelinePos.y + timelineSize.y + 5.f);
        draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), timeLabelStr);
    }

    // graphPos : 애니메이션 타임 라인 그래프를 그리기 위해 계산된 위치를 나타냄
    //  graphPos = ImVec2(왼쪽 위 모서리 화면상의 위치x, y, 타임라인의 크기)
    ImVec2 graphPos = ImVec2(timelinePos.x, timelinePos.y + timelineSize.y + 10);


#pragma endregion

#pragma region 타임라인 키프레임 조절

    // 타임라인에서 키프레임을 드래그하여 순서 변경
    for (int i = 0; i < timeline[m_iCurType].size(); ++i)
    {
        KEYFRAME& keyframe = timeline[m_iCurType][i];
        //float xPos = timelinePos.x + timelineSize.x * ImClamp(keyframe.time, 0.0f, MaxTime);

        // 타임 라인 상의 위치를 계산
        float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f), // 좌표 설정
            ImVec2(xPos + 5.0f, yPos + 5.0f)) // 좌표 설정
            && ImGui::IsMouseClicked(0))
        {
            isDraggingKeyframe = true;
            draggingKeyframeIndex = i;
            initialKeyframeX = xPos; // 드래그를 시작하는 위치 저장
        }
    }

    if (isDraggingKeyframe)
    {
        // 키프레임의 시간을 드래그한 위치로 설정
        KEYFRAME& keyframe = timeline[m_iCurType][draggingKeyframeIndex];
        float xPos = ImGui::GetMousePos().x - timelinePos.x;

        // 마우스 드래그로 키프레임의 시간을 조정
        float newXPos = ImGui::GetMousePos().x;
        float deltaTime = (newXPos - initialKeyframeX) / timelineSize.x * MaxTime;

        // 클릭한 위치에 해당하는 value 값
        keyframe.value = 1.0f - (ImGui::GetMousePos().y - timelinePos.y) / timelineSize.y;

        // 시간 값
        timeline[m_iCurType][draggingKeyframeIndex].time += deltaTime;

        // 시간과 값을 적절하게 클램핑할 수도 있음.
        // 드래그 했을 때 시간과 벨류 값 조정 (키프레임 time과 value, 최소 값, 최대 값);
        keyframe.time = ImClamp(keyframe.time, 0.0f, MaxTime); // 최대 값을 MaxTime으로 지정
        keyframe.value = ImClamp(keyframe.value, 0.0f, MaxTime); // 최대 값을 MaxTime으로 지정

        // 현재 위치를 갱신
        initialKeyframeX = newXPos;

        // 키프레임 순서 변경
        if (draggingKeyframeIndex > 0 && keyframe.time < timeline[m_iCurType][draggingKeyframeIndex - 1].time) {
            std::swap(timeline[m_iCurType][draggingKeyframeIndex], timeline[m_iCurType][draggingKeyframeIndex - 1]);
            draggingKeyframeIndex--;
        }
        else if (draggingKeyframeIndex < timeline[m_iCurType].size() - 1 && keyframe.time > timeline[m_iCurType][draggingKeyframeIndex + 1].time) {
            std::swap(timeline[m_iCurType][draggingKeyframeIndex], timeline[m_iCurType][draggingKeyframeIndex + 1]);
            draggingKeyframeIndex++;
        }
    }

#pragma endregion

    // 마우스 릴리즈 시 드래그 종료
    if (ImGui::IsMouseReleased(0))
    {
        isDraggingTimeline = false;
        isDraggingKeyframe = false;
    }

#pragma region 마우스 클릭으로 키프레임 선택

    SelectKeyframeMouseL();

#pragma endregion

#pragma region 버튼 (추가, 삭제, 저장, 로드)

    // 키프레임 추가 버튼
    if (ImGui::Button(u8"추가", ImVec2(30, 0))) {
        // 새로운 키프레임을 추가할 때 현재 시간과 가장 가까운 키프레임을 찾습니다.
        float minDist = FLT_MAX;
        int insertIndex = 0;
        for (int i = 0; i < timeline[m_iCurType].size(); ++i) {
            float dist = fabsf(currentTime - timeline[m_iCurType][i].time);
            if (dist < minDist) {
                minDist = dist;
                insertIndex = i;
            }
        }

        // 새로운 키프레임을 생성하고 삽입합니다.
        KEYFRAME newKeyframe = { currentTime, 0.5f, 0, false, false };
        newKeyframe.time = currentTime;
        timeline[m_iCurType].insert(timeline[m_iCurType].begin() + insertIndex, newKeyframe);
    }

#pragma region 단축키

    KeyInput();

#pragma endregion

#pragma region 키프레임 목록 / 삭제, 저장 버튼

    ImGui::SameLine();

    KeyframeList(); // 키프레임 목록

    ImGui::SameLine();

    // 키프레임 삭제 버튼
    if (ImGui::Button(u8"삭제", ImVec2(30, 0))) {
        if (!timeline[m_iCurType].empty()) {
            timeline[m_iCurType].pop_back();
        }
    }

    ImGui::SameLine();

    // 애니메이션 저장
    if (ImGui::Button(u8"저장", ImVec2(30, 0))) {
        SaveAnimationToFile(cFileSaveName);
    }

#pragma endregion

    ImGui::SameLine();

#pragma region 타임라인 그래프

    // 타임라인 그래프
    float* values = new float[100];
    for (int i = 0; i < 100; i++) {
        float time = i / 100.0f;
        values[i] = EvaluateAnimationAtTime(time);
    }
    ImGui::PlotLines("Value Curve", values, 100, 0, nullptr, FLT_MIN, FLT_MAX, ImVec2(300, 0));

    delete[] values;

#pragma endregion

#pragma region 애니메이션 속성 편집

    KeyframeChangeWindow();

    //KeyframeAniV();

    ImGui::Dummy(ImVec2(0.f, 110.f));


    ImGui::PushItemWidth(100); // 입력 필드의 길이를 원하는 픽셀로 지정
    // 애니메이션 프레임 숫자 입력
    ImGui::InputInt(u8"애니메이션 프레임", &animationFrame);
    ImGui::PopItemWidth(); // 길이 설정을 되돌림

    ImGui::SameLine();

    // 애니메이션 프레임 변경 버튼
    if (ImGui::Button(u8"프레임 변경")) {
        // 키프레임을 animationFrame 값에 따라 생성하고 편집
        EditKeyframesForAnimation(animationFrame);
        currentTime = playbackTime;
    }

    ImGui::SameLine(320);

    ImGui::Text(u8"애니메이션 반복 : ");

    ImGui::SameLine();

    ImGui::RadioButton(u8"On", &bRepetition, false); // 반복 On
    ImGui::SameLine();
    ImGui::RadioButton(u8"Off", &bRepetition, true); // 반복 Off

    ImGui::SameLine(690.f);

    // 애니메이션 불러오기
    if (ImGui::Button(u8"애니메이션 불러오기", ImVec2(120, 0))) {
        LoadAnimationFromFile(cFileLoadName);
    }

    // 애니메이션 시간 설정
    if (isPlaying)
    {
        currentTime += playbackSpeed * ImGui::GetIO().DeltaTime;
        if (currentTime > MaxTime) // 현재 시간이 최대 시간 값 보다 클 경우
        {
            currentTime = MaxTime; // 현재 시간을 최대 시간 값으로 고정
            isPlaying = false;  // 플레이 중지
        }
    }

    ImGui::PushItemWidth(100); // 입력 필드의 길이를 원하는 픽셀로 지정
    ImGui::InputText(u8"저장할 파일 이름", cFileSaveName, sizeof(cFileSaveName));
    ImGui::SameLine();
    ImGui::InputText(u8"불러올 파일 이름", cFileLoadName, sizeof(cFileLoadName));
    ImGui::PopItemWidth(); // 길이 설정을 되돌림

#pragma endregion

#pragma region 키프레임 자동 생성

    KeyframeAutomaticGeneration();

#pragma region 키프레임 랜더링 및 편집 (마지막 순서 유지)

    // # 마우스 오버시 나오는 툴팁은 모든 UI위에 그려져야 하기 때문에 순서가 가장 마지막이여야 한다.
    // 키프레임을 드래그로 이동시 위치 값이 바뀌기 때문에 선택 키프레임 값 변경보단 위에 있어야 한다. (보류)
    //KeyframeRender_ValueChange(); // draw 때문에 임시로 밖에 꺼내둠

       // 키프레임 렌더링 및 편집
    for (int i = 0; i < timeline[m_iCurType].size(); ++i)
    {
        KEYFRAME& keyframe = timeline[m_iCurType][i];

        // 타임 라인 상의 위치를 계산
        float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        // 키프레임 위치 값이 0(초기값)일 경우
        if (keyframe.vKeyFramePos.x <= 1 &&
            keyframe.vKeyFramePos.y <= 1)
        {
            // 현재 선택된 키프레임 위치를 넣어준다.
            keyframe.vKeyFramePos.x = xPos;
            keyframe.vKeyFramePos.y = yPos;
        }

        // 현재 시간에 해당하는 키프레임 표시 (빨간색 원)
        if (keyframe.time >= currentTime - 0.1 &&
            keyframe.time <= currentTime + 0.1) {
            draw_list->AddCircleFilled(
                ImVec2(xPos, yPos),
                5.0f,
                IM_COL32(255, 0, 0, 255) // 빨간색으로 표시
            );
        }
        else {
            // 다른 시간의 키프레임 표시 (노란색 원)
            draw_list->AddCircleFilled(
                ImVec2(xPos, yPos),
                5.0f,
                IM_COL32(255, 255, 0, 255) // 노란색으로 표시
            );
        }

        // 마우스가 키프레임 위에 있는 경우에만 툴팁 표시 (마우스 오버)
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f)))
        {

            // 키프레임 정보 표시
            ImGui::SetCursorScreenPos(ImVec2(xPos + 10.0f, yPos - 20.0f));
            ImGui::BeginTooltip();
            ImGui::Text(u8"키프레임 %d", i);
            ImGui::Separator();
            ImGui::Text(u8"시간: %.2f", keyframe.time);
            ImGui::Text(u8"값: %.2f", keyframe.value);
            ImGui::Text(u8"크기: (%.2f, %.2f, %.2f)", keyframe.vScale.x, keyframe.vScale.y, keyframe.vScale.z);
            ImGui::Text(u8"회전:(%.2f, %.2f, %.2f)", keyframe.vRot.x, keyframe.vRot.y, keyframe.vRot.z);
            ImGui::Text(u8"이동: (%.2f, %.2f, %.2f)", keyframe.vPos.x, keyframe.vPos.y, keyframe.vPos.z);

            if (keyframe.isEaseIn)
                ImGui::Text(u8"이징 In: 켜짐");
            else
                ImGui::Text(u8"이징 In: 꺼짐");
            if (keyframe.isEaseOut)
                ImGui::Text(u8"이징 Out: 켜짐");
            else
                ImGui::Text(u8"이징 Out: 꺼짐");
            ImGui::EndTooltip();
        }

        // 마우스 릴리즈 시 드래그 종료
        if (ImGui::IsMouseReleased(0))
        {
            isDraggingTimeline = false;
        }

        //if (!isDraggingTimeline) // 드래그를 하지 않을 경우만 삭제 가능
        //{
        //    // 오른쪽 마우스 클릭으로 키프레임 삭제
        //    if (ImGui::IsMouseHoveringRect(
        //        ImVec2(xPos - 5.0f, yPos - 5.0f),
        //        ImVec2(xPos + 5.0f, yPos + 5.0f))
        //        && ImGui::IsMouseClicked(1))
        //    {
        //        timeline.erase(timeline.begin() + i);
        //        break; // 삭제한 후에 루프를 빠져나갑니다.
        //    }
        //}

    }

#pragma endregion

#pragma region 선택한 키프레임 값 변경 (단축키)

    SelectKeyframeValueChange();

#pragma endregion

#pragma region 선택한 키프레임 값 변경 (창)

    // 선택된 키프렘의 인덱스를 사용하여 키프레임을 편집. (선택한 인덱스가 범위 내에 있을 경우)
    if (!timeline[m_iCurType].empty() && closestKeyframeIndex >= 0 && closestKeyframeIndex < timeline[m_iCurType].size())
    {
        KEYFRAME& selectedKeyframe = timeline[m_iCurType][closestKeyframeIndex];

        // 선택된 키프렘의 애니메이션 속성 편집
        DrawSelectedKeyframeEditor(selectedKeyframe);
    }

#pragma endregion

    ImGui::End();

}
//==============================================[애니메이션 툴]==============================================

// 애니메이션 저장
void CImguiAnimationTool::SaveAnimationToFile(const char* fileName)
{
    // .dat 파일 확장자를 추가한 파일 이름 생성
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // 파일을 UTF-8로 열기 (문자열 깨짐 방지)
    std::ofstream file(datFileName.c_str(), std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        // 파일을 열 수 없을 때의 오류 처리
        std::cerr << "Failed to open file: " << datFileName << std::endl;
        return;
    }

    for (const KEYFRAME& keyframe : timeline[m_iCurType]) {
        // Keyframe 구조체를 파일에 쓸 때 UTF-8로 인코딩된 문자열로 저장
        file << keyframe.time << " "
            << keyframe.value << " "
            << keyframe.type << " "
            << keyframe.isEaseIn << " "
            << keyframe.isEaseOut << " "
            << keyframe.bChargePossible << " "
            << keyframe.bShieldPossible << " "
            << keyframe.texureframe << " "
            << keyframe.bFullChargeKeyframe << " "
            << keyframe.bShieldKeyFrame << " "
            << keyframe.vScale.x << " "
            << keyframe.vScale.y << " "
            << keyframe.vScale.z << " "
            << keyframe.vRot.x << " "
            << keyframe.vRot.y << " "
            << keyframe.vRot.z << " "
            << keyframe.vPos.x << " "
            << keyframe.vPos.y << " "
            << keyframe.vPos.z << " "
            << keyframe.vKeyFramePos.x << " "
            << keyframe.vKeyFramePos.y << "\n";
    }

    file.close();
    std::cout << "Animation saved to file: " << datFileName << std::endl;
}

// 애니메이션 불러오기
void CImguiAnimationTool::LoadAnimationFromFile(const char* fileName)
{
    // .dat 파일 확장자를 추가한 파일 이름 생성
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // 파일을 UTF-8로 열기
    std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        // 파일을 열 수 없을 때 메세지
        return;
    }

    timeline[m_iCurType].clear(); // 클리어

    KEYFRAME keyframe; // 키프레임 선언

    while (file >>
        keyframe.time >>
        keyframe.value >>
        keyframe.type >>
        keyframe.isEaseIn >>
        keyframe.isEaseOut >>
        keyframe.bChargePossible >>
        keyframe.bShieldPossible >>
        keyframe.texureframe >>
        keyframe.bFullChargeKeyframe >>
        keyframe.bShieldKeyFrame >>
        keyframe.vScale.x >>
        keyframe.vScale.y >>
        keyframe.vScale.z >>
        keyframe.vRot.x >>
        keyframe.vRot.y >>
        keyframe.vRot.z >>
        keyframe.vPos.x >>
        keyframe.vPos.y >>
        keyframe.vPos.z >>
        keyframe.vKeyFramePos.x >>
        keyframe.vKeyFramePos.y)
    {
        timeline[m_iCurType].push_back(keyframe); // 불러온 애니메이션 값 담아주기
    }

    file.close(); // 파일 닫기
}

// 애니메이션을 계산하여 현재 시간 값을 반환하는 함수
float CImguiAnimationTool::EvaluateAnimationAtTime(float time)
{
    if (timeline[m_iCurType].empty()) {
        // 만약 키프레임이 없다면 기본값 0.0을 반환하거나,
        // 다른 처리를 수행.
        return 0.0f;
    }

    // 키프레임을 시간에 따라 정렬.
    std::sort(timeline[m_iCurType].begin(), timeline[m_iCurType].end(), [](const KEYFRAME& a, const KEYFRAME& b) {
        return a.time < b.time;
        });

    // 현재 시간과 가장 가까운 이전과 이후의 키프레임을 찾기
    KEYFRAME* prevKeyframe = nullptr;
    KEYFRAME* nextKeyframe = nullptr;

    for (int i = 0; i < timeline[m_iCurType].size(); ++i) {
        if (timeline[m_iCurType][i].time <= time) {
            prevKeyframe = &timeline[m_iCurType][i];
        }
        if (timeline[m_iCurType][i].time >= time) {
            nextKeyframe = &timeline[m_iCurType][i];
            break;
        }
    }

    // 이전과 다음 키프레임을 찾지 못하면 첫 번째 키프레임을 사용.
    if (!prevKeyframe) {
        prevKeyframe = &timeline[m_iCurType][0];
    }

    // 다음 키프레임을 찾지 못하면 마지막 키프레임을 사용.
    if (!nextKeyframe) {
        nextKeyframe = &timeline[m_iCurType].back();
    }

    // 이전과 다음 키프레임 사이에서 시간을 보간하여 값을 계산.
    float t = (time - prevKeyframe->time) / (nextKeyframe->time - prevKeyframe->time);
    float value = ImLerp(prevKeyframe->value, nextKeyframe->value, t);

    // 이징(Easing)을 적용합니다.
    if (t < 0.5f) {
        if (prevKeyframe->isEaseIn) {
            float easedT = t / 0.5f;
            value = ImEaseInQuad(prevKeyframe->value, nextKeyframe->value, easedT);
        }
    }
    else {
        if (nextKeyframe->isEaseOut) {
            float easedT = (t - 0.5f) / 0.5f;
            value = ImEaseOutQuad(prevKeyframe->value, nextKeyframe->value, easedT);
        }
    }

    return value;
}

// 쿼드라틱 이징(InQuad) 함수 (시작)
float CImguiAnimationTool::ImEaseInQuad(float start, float end, float t)
{
    return start + (end - start) * t * t;
}

// 쿼드라틱 이징(OutQuad) 함수 (끝)
float CImguiAnimationTool::ImEaseOutQuad(float start, float end, float t)
{
    return start + (end - start) * (-t * (t - 2));
}

// 미리보기 애니메이션 렌더링 함수
void CImguiAnimationTool::RenderPreviewAnimation(float value)
{
    // 이 함수에서 미리보기 애니메이션 렌더링
    // value를 기반으로 크기, 회전, 이동 등을 조절하여 렌더링

    ImGui::SameLine(670.f);
    ImVec4 textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 빨간색
    ImGui::TextColored(textColor, "Preview Animation On");

    // 예제: 원 그리기
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // 텍스트 색상 설정

    // 반지름은 value에 따라 조절합니다.
    float radius = 20.0f + value * 30.0f;

    // 회전은 value에 따라 조절합니다.
    float rotation_degrees = value * 360.0f;

    // 이동은 value에 따라 조절합니다.
    float x_position = 100.0f + value * 200.0f;
    float y_position = 100.0f + value * 50.0f;

    ImGui::GetWindowDrawList()->AddCircleFilled(
        ImVec2(x_position, y_position),
        radius,
        IM_COL32(255, 0, 0, 255), // 원의 색상
        64); // 원의 분할 수

    ImGui::PopStyleColor();
}

// 선형 보간 함수
float CImguiAnimationTool::Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

// std::string 벡터를 const char* 배열로 변환하는 함수
std::vector<const char*> CImguiAnimationTool::ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector)
{
    std::vector<const char*> charArray;
    charArray.reserve(stringVector.size());
    for (const std::string& str : stringVector) {
        charArray.push_back(str.c_str());
    }
    return charArray;
}

// 선택한 키프레임 값을 변경하는 함수
void CImguiAnimationTool::DrawSelectedKeyframeEditor(KEYFRAME& selectedKeyframe)
{

    //if (iOldIndex != closestKeyframeIndex)
    //{
    //    m_bSheild = false;
    //    m_bSheildFrame = false;
    //    m_bCharging = false;
    //    m_bChargingFrame = false;
    //}

    // 타임라인 툴
    ImGui::Begin(u8"선택한 키프레임 속성 편집");

    //Keyframe& selectedKeyframe = timeline.back();
    ImGui::Text(u8"현재 선택 : 키프레임%d", closestKeyframeIndex);

    ImGuiStyle& style = ImGui::GetStyle(); // 스타일 불러오기
    float originalItemWidth = style.ItemInnerSpacing.x; // 원래의 ItemInnerSpacing 값을 저장

    // 원하는 크기로 ItemInnerSpacing을 설정
    style.ItemInnerSpacing.x = 10.0f; // 수평 간격을 10.0f로 조절

    ImGui::SeparatorText(u8"시간 및 벨류 설정");

    // 시간 입력 필드의 가로 길이를 조절 (ex : 40 픽셀)
    ImGui::PushItemWidth(40);

    // 시간 입력 필드
    if (ImGui::InputFloat(u8"시간", &selectedKeyframe.time, fMin_Time, fMax_Time))
    {
        // 원래 키프레임의 인덱스 찾기
        for (int i = 0; i < timeline[m_iCurType].size(); ++i) {
            if (&timeline[m_iCurType][i] == &selectedKeyframe) {
                originalIndex = i;
                break;
            }
        }

        // 키프레임을 시간에 따라 정렬
        std::sort(timeline[m_iCurType].begin(), timeline[m_iCurType].end(), [](const KEYFRAME& a, const KEYFRAME& b) {
            return a.time < b.time;
            });

        // 원래 키프레임의 인덱스 찾기
        for (int i = 0; i < timeline[m_iCurType].size(); ++i) {
            if (&timeline[m_iCurType][i] == &selectedKeyframe) {
                closestKeyframeIndex = i;
                break;
            }
        }

        // 키프레임 순서 변경
        if (originalIndex != closestKeyframeIndex) {
            // 키프레임이 원래 위치에서 이동한 경우
            // 이동한 위치에 따라 인덱스 업데이트
            draggingKeyframeIndex = closestKeyframeIndex;
        }
    }

    ImGui::PopItemWidth();

    //// 시간 값 저장
    //oldTime = selectedKeyframe.time;

    //// 이제 다른 키프렘들의 시간 값을 변경하지 않도록 조치한다
    //// 시간 값을 변경한 키프렘보다 높은 인덱스의 키프렘들은 이동하지 않아야 한다
    //for (int i = selectedKeyframeIndex + 1; i < timeline.size(); ++i)
    //{
    //    Keyframe& currentKeyframe = timeline[i];

    //    // 이동하지 않도록 기존 시간과의 차이를 더합니다.
    //    float timeDifference = currentKeyframe.time - oldTime;
    //    currentKeyframe.time = newTime + timeDifference;
    //}

    ImGui::SameLine(); // 같은 라인

    // 벨류 입력 필드의 가로 길이를 조절 (ex : 40 픽셀)
    ImGui::PushItemWidth(40);
    // 벨류 입력 필드
    ImGui::InputFloat(u8"벨류", &selectedKeyframe.value, fMin_Value, fMax_Value);
    ImGui::PopItemWidth();

    //ImGui::SameLine(); // 같은 라인
    ImGui::Dummy(ImVec2(0, 5)); // 공백

    // 텍스처 입력 필드의 가로 길이를 조절 (ex : 40 픽셀)
    ImGui::PushItemWidth(80);
    //ImGui::Text("%d", selectedKeyframe.texureframe);
    ImGui::InputInt(u8"텍스처 번호", &selectedKeyframe.texureframe);
    ImGui::PopItemWidth();

    // 원래의 ItemInnerSpacing 값으로 복원
    style.ItemInnerSpacing.x = originalItemWidth;

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    // 애니메이션 타입 설정
    ImGui::SeparatorText(u8"애니메이션 타입 설정");
    ImGui::RadioButton(u8"크기", &selectedKeyframe.type, 0); // 크기 애니메이션
    ImGui::SameLine();
    ImGui::RadioButton(u8"회전", &selectedKeyframe.type, 1); // 회전 애니메이션
    ImGui::SameLine();
    ImGui::RadioButton(u8"이동", &selectedKeyframe.type, 2); // 이동 애니메이션

    ImGui::Dummy(ImVec2(0, 5)); // 공백

    style.ItemInnerSpacing.x = 10.0f; // 수평 간격을 10.0f로 조절
    // 키프레임 타입에 따른 추가 설정 (슬라이더)
    if (selectedKeyframe.type == 0) { // 크기 애니메이션
        ImGui::SliderFloat3(u8"크기(슬라이더)", selectedKeyframe.vScale, fMin_Scale, fMax_Scale);
    }
    else if (selectedKeyframe.type == 1) { // 회전 애니메이션
        ImGui::SliderFloat3(u8"회전(슬라이더)", selectedKeyframe.vRot, fMin_Rot, fMax_Rot);
    }
    else if (selectedKeyframe.type == 2) { // 이동 애니메이션
        ImGui::SliderFloat3(u8"이동(슬라이더)", selectedKeyframe.vPos, fMin_Pos, fMax_Pos);
    }
    style.ItemInnerSpacing.x = originalItemWidth;// 원래의 ItemInnerSpacing 값으로 복원

    //ImGui::Dummy(ImVec2(0, 10));
    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 5)); // 공백

    style.ItemInnerSpacing.x = 10.0f; // 수평 간격을 10.0f로 조절
    // 키프레임 타입에 따른 추가 설정 (입력)
    if (selectedKeyframe.type == 0) // 크기 애니메이션
    {
        // 크기 값 입력
        ImGui::InputFloat3(u8"크기(입력)", selectedKeyframe.vScale);
        // 입력된 값의 범위를 제한(수동)
        selectedKeyframe.vScale.x = (selectedKeyframe.vScale.x < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.x > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.x);
        selectedKeyframe.vScale.y = (selectedKeyframe.vScale.y < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.y > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.y);
        selectedKeyframe.vScale.z = (selectedKeyframe.vScale.z < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.z > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.z);
    }
    else if (selectedKeyframe.type == 1) // 회전 애니메이션
    {
        // 회전 값 입력
        ImGui::InputFloat3(u8"회전(입력)", selectedKeyframe.vRot);

        // 입력된 값의 범위를 제한(함수)
        selectedKeyframe.vRot.x = ImClamp(selectedKeyframe.vRot.x, fMin_Rot, fMax_Rot); // X 값의 범위 제한
        selectedKeyframe.vRot.y = ImClamp(selectedKeyframe.vRot.y, fMin_Rot, fMax_Rot); // Y 값의 범위 제한
        selectedKeyframe.vRot.z = ImClamp(selectedKeyframe.vRot.z, fMin_Rot, fMax_Rot); // Z 값의 범위 제한
    }
    else if (selectedKeyframe.type == 2) // 이동 애니메이션
    {
        // 이동 값 입력
        ImGui::InputFloat3(u8"이동(입력)", selectedKeyframe.vPos);

        // 입력된 값의 범위를 제한(함수)
        selectedKeyframe.vPos.x = ImClamp(selectedKeyframe.vPos.x, fMin_Pos, fMax_Pos); // X 값의 범위 제한
        selectedKeyframe.vPos.y = ImClamp(selectedKeyframe.vPos.y, fMin_Pos, fMax_Pos); // Y 값의 범위 제한
        selectedKeyframe.vPos.z = ImClamp(selectedKeyframe.vPos.z, fMin_Pos, fMax_Pos); // Z 값의 범위 제한
    }
    style.ItemInnerSpacing.x = originalItemWidth;// 원래의 ItemInnerSpacing 값으로 복원

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"무기 설정");

    //// OBJ_TYPE 선택 목록
    //const char* objTypeItems[] = { u8"없음", u8"주먹", u8"양손 무기", u8"한손 무기" };
    //int selectedObjType = static_cast<int>(selectedKeyframe.m_eObjectType);
    //ImGui::Combo(u8"타입", &selectedObjType, objTypeItems, IM_ARRAYSIZE(objTypeItems));
    //selectedKeyframe.m_eObjectType = static_cast<OBJ_TYPE>(selectedObjType);

    //// OBJ_NAME 선택 목록
    //const char* objNameItems[] = { u8"없음", u8"권총", u8"톰슨 기관총", u8"쇠파이프", u8"맥주병", u8"프라이팬" };
    //int selectedObjName = static_cast<int>(selectedKeyframe.m_eObjectName);
    //ImGui::Combo(u8"무기", &selectedObjName, objNameItems, IM_ARRAYSIZE(objNameItems));
    //selectedKeyframe.m_eObjectName = static_cast<OBJ_NAME>(selectedObjName);

#pragma region 타입에 따른 선택목록 업데이트 (현재 미사용)
    //// OBJ_TYPE 선택 목록
    //const char* objTypeItems[] = { u8"없음", u8"주먹", u8"양손 무기", u8"한손 무기" };
    //int selectedObjType = static_cast<int>(selectedKeyframe.m_eObjectType);
    //if (ImGui::Combo(u8"타입", &selectedObjType, objTypeItems, IM_ARRAYSIZE(objTypeItems))) {
    //    selectedKeyframe.m_eObjectType = static_cast<OBJ_TYPE>(selectedObjType);
    //}

    //// OBJ_NAME 선택 목록 (타입에 따라 다른 목록 사용)
    //const char* objNameItems[] = { u8"없음" };
    //int numObjNameItems = 1; // 기본적으로 "없음"만 표시
    //int selectedObjName = 0;

    //// 타입에 따라 무기 목록 업데이트
    //if (selectedKeyframe.m_eObjectType == OBJ_TYPE::TWO_HAND) {
    //    const char* fistItems[] = { u8"없음", u8"주먹" };
    //    numObjNameItems = IM_ARRAYSIZE(fistItems);
    //    ImGui::Combo(u8"무기", &selectedObjName, fistItems, numObjNameItems);
    //}
    //else if (selectedKeyframe.m_eObjectType == OBJ_TYPE::TWO_OBJECT) {
    //    const char* twoHandedItems[] = { u8"없음", u8"양손_무기1", u8"양손_무기2" };
    //    numObjNameItems = IM_ARRAYSIZE(twoHandedItems);
    //    ImGui::Combo(u8"무기", &selectedObjName, twoHandedItems, numObjNameItems);
    //}
    //else if (selectedKeyframe.m_eObjectType == OBJ_TYPE::RIGHT_OBJECT) {
    //    const char* oneHandedItems[] = { u8"없음", u8"한손_무기1", u8"한손_무기2" };
    //    numObjNameItems = IM_ARRAYSIZE(oneHandedItems);
    //    ImGui::Combo(u8"무기", &selectedObjName, oneHandedItems, numObjNameItems);
    //}
    //else {
    //    ImGui::Combo(u8"무기", &selectedObjName, objNameItems, numObjNameItems);
    //}

    //selectedKeyframe.m_eObjectName = static_cast<OBJ_NAME>(selectedObjName);
#pragma endregion

#pragma region 설정

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"이징 설정");
    // 이징 설정 (Ease In, Ease Out)
    ImGui::Checkbox("Ease In", &selectedKeyframe.isEaseIn);
    ImGui::SameLine();
    ImGui::Checkbox("Ease Out", &selectedKeyframe.isEaseOut);
    //ImGui::SameLine();

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"쉴드 설정");

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::Checkbox(u8"쉴드가능 여부", &selectedKeyframe.bShieldPossible);
    ImGui::Dummy(ImVec2(0, 5)); // 공백

    // 차징 입력 필드의 가로 길이를 조절
    ImGui::PushItemWidth(80);
    ImGui::Checkbox(u8"쉴드시 키프레임", &selectedKeyframe.bShieldKeyFrame);
    ImGui::PopItemWidth();

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"차징 설정");

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    
    ImGui::Checkbox(u8"차징가능 여부", &selectedKeyframe.bChargePossible);
    ImGui::Dummy(ImVec2(0, 5)); // 공백

    // 차징 입력 필드의 가로 길이를 조절
    ImGui::PushItemWidth(80);
    ImGui::Checkbox(u8"풀차징시 키프레임", &selectedKeyframe.bFullChargeKeyframe);
    ImGui::PopItemWidth();

#pragma endregion

    ImGui::End(); // 키프레임 수정 창 끝

    // 선택된 키프레임만 업데이트
    timeline[m_iCurType][closestKeyframeIndex] = selectedKeyframe;

    //timeline[m_iCurType][closestKeyframeIndex].time = selectedKeyframe.time;
    //timeline[m_iCurType][closestKeyframeIndex].type = selectedKeyframe.type;
    //timeline[m_iCurType][closestKeyframeIndex].value = selectedKeyframe.value;
    //timeline[m_iCurType][closestKeyframeIndex].vPos = selectedKeyframe.vPos;
    //timeline[m_iCurType][closestKeyframeIndex].vRot = selectedKeyframe.vRot;
    //timeline[m_iCurType][closestKeyframeIndex].vScale = selectedKeyframe.vScale;

    //timeline[m_iCurType][closestKeyframeIndex].bChargePossible = selectedKeyframe.bChargePossible;
    //timeline[m_iCurType][closestKeyframeIndex].bFullChargeKeyframe = selectedKeyframe.bFullChargeKeyframe;
    //timeline[m_iCurType][closestKeyframeIndex].bShieldKeyFrame = selectedKeyframe.bShieldKeyFrame;
    //timeline[m_iCurType][closestKeyframeIndex].bShieldPossible = selectedKeyframe.bShieldPossible;
    //timeline[m_iCurType][closestKeyframeIndex].texureframe = selectedKeyframe.texureframe;


    iOldIndex = closestKeyframeIndex; // 현재 인덱스 번호를 저장
}

// _vec3 선형 보간 함수
_vec3 CImguiAnimationTool::Lerp(const _vec3& a, const _vec3& b, float t) {
    _vec3 result;
    result.x = a.x + t * (b.x - a.x);
    result.y = a.y + t * (b.y - a.y);
    result.z = a.z + t * (b.z - a.z);
    return result;
}

// _vec2 선형 보간 함수
_float CImguiAnimationTool::Lerp2(const _float& a, const _float& b, float t) {
    _float result;
    result = a + t * (b - a);
    return result;
}

// 최소, 최대 값 및 크기, 회전, 이동 값을 기반으로 선형 보간된 키프레임을 자동으로 생성하는 함수
void CImguiAnimationTool::CreateKeyframesWithLinearInterpolation(
    std::vector<KEYFRAME>& timeline, float minTime, float maxTime,
    _float minValue, _float maxValue,
    _vec3 minscaleValue, _vec3 maxscaleValue,
    _vec3 minrotationValue, _vec3 maxrotationValue,
    _vec3 mintranslationValue, _vec3 maxtranslationValue,
    _float _minTexture, _float _maxTexture,
    int numKeyframes)
{
    if (numKeyframes < 2) {
        // 최소 2개 이상의 키프레임이 필요합니다.
        return;
    }

    // 키프레임 간의 시간 간격을 계산
    float timeStep = 1.0f / static_cast<float>(numKeyframes - 1);

    // 첫 번째와 마지막 키프레임을 수동으로 추가
    KEYFRAME firstKeyframe;

    // 최소 값 보간
    firstKeyframe.time = minTime;
    firstKeyframe.value = minValue;
    firstKeyframe.vScale = minscaleValue;
    firstKeyframe.vRot = minrotationValue;
    firstKeyframe.vPos = mintranslationValue;
    firstKeyframe.texureframe = _minTexture;

    timeline.push_back(firstKeyframe);

    KEYFRAME lastKeyframe;

    // 최대 값 보간
    lastKeyframe.time = maxTime;
    lastKeyframe.value = maxValue;
    lastKeyframe.vScale = maxscaleValue;
    lastKeyframe.vRot = maxrotationValue;
    lastKeyframe.vPos = maxtranslationValue;
    lastKeyframe.texureframe = _maxTexture;

    timeline.push_back(lastKeyframe);


    int num_decimal_places = 2; // 소수점 단위 설정

    // 소수점 단위 표현
    int multiplier = static_cast<int>(pow(10, num_decimal_places));


    // 중간 키프레임 생성 및 보간
    for (int i = 1; i < numKeyframes - 1; ++i) 
    {
        float time = i * timeStep;
        float t = time;  // 시간을 보간 계수로 사용

        int itextureRange = _maxTexture - _minTexture + 1; // 텍스처 범위 계산
        int iTextureNum = _minTexture + (i % itextureRange); // 현재 인덱스에 따라 텍스처 값 설정

        // 매 키프레임마다 순차적으로 증가 및 감소한 값을 생성하여 추가
        float step = (maxValue - minValue) / (numKeyframes - 1); // 증가/감소 단계


        // 각각의 속성에 대해 보간값을 계산
        _vec3 scale = Lerp(minscaleValue, maxscaleValue, t);

        _vec3 rotation = Lerp(minrotationValue, maxrotationValue, t);

        _vec3 translation = Lerp(mintranslationValue, maxtranslationValue, t);

        _float ftime = Lerp2(minTime, maxTime, t);

        _float value = Lerp2(minValue, maxValue, t);

        _int iTexture = (_int)Lerp2(_minTexture, _maxTexture, t);

        KEYFRAME keyframe;

        // 값 보간
        keyframe.time = ftime;

        if (m_bRendomValue)
        {
            // 순차적으로 증가 및 감소한 소수점 값 생성
            if (i < numKeyframes / 2)
            {
                keyframe.value = minValue + i * step; // 최소값에서 증가
            }
            else
            {
                keyframe.value = maxValue - (i - numKeyframes / 2) * step; // 최대값에서 감소
            }
        }
        else
        {
            keyframe.value = value;
        }


        keyframe.vScale = scale;
        keyframe.vRot = rotation;
        keyframe.vPos = translation;

        if (m_bIndividualTexture)
        {
            keyframe.texureframe = iTextureNum;
        }
        else
        {
            keyframe.texureframe = iTexture;
        }

        // 기존 timeline 벡터에 키프레임을 이어서 추가
        timeline.emplace_back(keyframe);
    }

    // 시간에 따라 키프레임 정렬
    std::sort(timeline.begin(), timeline.end(), [](const KEYFRAME& a, const KEYFRAME& b) {
        return a.time < b.time;
        });
}

void CImguiAnimationTool::EditKeyframesForAnimation(int numFrames)
{
    //timeline.clear(); // 기존의 키프레임을 모두 지웁니다.

    //float frameTime = MaxTime / static_cast<float>(numFrames);

    //for (int frame = 0; frame <= numFrames; ++frame) {
    //    float time = frame * frameTime;
    //    float value = EvaluateAnimationAtTime(time);

    //    // 키프레임을 생성하고 리스트에 추가합니다.
    //    Keyframe keyframe;
    //    keyframe.time = time;
    //    keyframe.value = value;

    //    // 나머지 키프레임 속성을 여기에 설정합니다.
    //    // 예: keyframe.vScale, keyframe.vPos, keyframe.vRot, keyframe.isEaseIn, keyframe.isEaseOut 등

    //    timeline.push_back(keyframe);
    //}
}

//// 애니메이션 타임라인을 생성하고 애니메이션 값을 계산하는 함수
//void GenerateAnimationTimeline(std::vector<Keyframe>& timeline, AnimationProperties properties, int numFrames) 
//{
//    // 키프레임 간의 시간 간격 계산
//    float timeStep = (properties.maxTime - properties.minTime) / (numFrames - 1);
//
//    // 키프레임을 생성하고 애니메이션 값을 계산하여 타임라인에 추가
//    timeline.clear();
//    for (int i = 0; i < numFrames; ++i) {
//        float time = properties.minTime + i * timeStep;
//        // 여기에서 애니메이션 값을 계산하십시오 (예: 랜덤한 값 또는 특정 함수를 사용하여)
//        float value = CalculateAnimationValue(time, properties);
//        Keyframe keyframe = { time, value, /* 다른 속성 값 */ };
//        timeline.push_back(keyframe);
//    }
//}
//
//// 애니메이션 값을 계산하는 함수 예제
//float CalculateAnimationValue(float time, AnimationProperties properties) {
//    // 예: 시간에 따라 선형으로 값 증가
//    float valueRange = properties.maxValue - properties.minValue;
//    float timeRange = properties.maxTime - properties.minTime;
//    float normalizedTime = (time - properties.minTime) / timeRange;
//    return properties.minValue + normalizedTime * valueRange;
//}
//
//// 사용자로부터 애니메이션 속성 및 키프렘 수를 입력 받는 함수
//void EditAnimationProperties(AnimationProperties& properties, int& numFrames) {
//    ImGui::Text(u8"애니메이션 속성 설정");
//    ImGui::InputFloat(u8"최소 시간", &properties.minTime);
//    ImGui::InputFloat(u8"최대 시간", &properties.maxTime);
//    ImGui::InputFloat(u8"최소 값", &properties.minValue);
//    ImGui::InputFloat(u8"최대 값", &properties.maxValue);
//    ImGui::InputFloat(u8"최소 크기", &properties.minSize);
//    ImGui::InputFloat(u8"최대 크기", &properties.maxSize);
//    ImGui::InputFloat(u8"최소 위치", &properties.minPosition);
//    ImGui::InputFloat(u8"최대 위치", &properties.maxPosition);
//    ImGui::InputFloat(u8"최소 회전", &properties.minRotation);
//    ImGui::InputFloat(u8"최대 회전", &properties.maxRotation);
//
//    ImGui::Text(u8"키프렘 수 입력");
//    ImGui::InputInt(u8"키프렘 수", &numFrames);
//
//    if (ImGui::Button(u8"애니메이션 생성")) {
//        GenerateAnimationTimeline(timeline, properties, numFrames);
//    }
//}

// 선형 보간 함수 ex1
//ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t)
//{
//    t = ImClamp(t, 0.0f, 1.0f); // t 값을 [0, 1] 범위로 클램프합니다.
//    return ImVec4(ImLerp(a.x, b.x, t), ImLerp(a.y, b.y, t), ImLerp(a.z, b.z, t), ImLerp(a.w, b.w, t));
//}

// 선형 보간 함수 ex2
//ImVec4 CImguiAnimationTool::ImLerp(const ImVec4& a, const ImVec4& b, float t)
//{
//    t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t); // t 값을 [0, 1] 범위로 클램프합니다.
//    return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
//}

//// 애니메이션 타임라인에 랜더링된 키프레임을 클릭했을 때 호출되는 함수
//void CImguiAnimationTool::HandleKeyframeClick()
//{
//    // 이 함수는 마우스 이벤트 처리 로직을 포함합니다.
//    // 여기서는 간단한 예제로 클릭 시 선택된 키프레임을 찾아 수정하는 로직을 추가합니다.
//
//    // 마우스 버튼 클릭 여부 확인
//    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
//    {
//        // 마우스 클릭 위치를 가져옵니다.
//        ImVec2 mousePos = ImGui::GetMousePos();
//
//        // 타임라인의 시작 위치를 가져옵니다.
//        ImVec2 timelineStartPos = ImGui::GetCursorScreenPos();
//
//        // 타임라인의 각 키프레임 버튼을 클릭 여부를 확인하고 선택합니다.
//        for (int i = 0; i < timeline.size(); ++i)
//        {
//            ImVec2 keyframeButtonPos = timelineStartPos + ImVec2(i * 30, 0); // 키프레임 버튼 간격은 30입니다.
//
//            // 클릭한 위치가 해당 키프레임 버튼의 영역에 포함되는지 확인합니다.
//            if (mousePos.x >= keyframeButtonPos.x && mousePos.x <= (keyframeButtonPos.x + 20)) // 버튼 너비는 20입니다.
//            {
//                // 클릭한 키프레임을 선택합니다.
//                selectedKeyframeIndex = i;
//                break; // 선택 후 루프 종료
//            }
//        }
//    }
//}

// 마우스 이벤트 처리 함수
void CImguiAnimationTool::HandleKeyframeClick()
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        ImVec2 mousePos = ImGui::GetMousePos();

        // 타임라인의 시작 위치와 끝 위치를 계산합니다.
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 timelineStartPos = windowPos + ImGui::GetCursorScreenPos();
        ImVec2 timelineEndPos = timelineStartPos + ImVec2((_float)timeline[m_iCurType].size() * 30.f, 20.f); // 키프레임 버튼 간격은 30, 높이는 20으로 가정합니다.

        // 마우스 클릭 위치가 타임라인 영역 내에 있는지 확인합니다.
       // if (mousePos.x >= timelineStartPos.x && mousePos.x <= timelineEndPos.x &&
       //     mousePos.y >= timelineStartPos.y && mousePos.y <= timelineEndPos.y)
        {
            // 각 키프레임 버튼을 클릭 여부를 확인하고 선택합니다.
            for (int i = 0; i < timeline[m_iCurType].size(); ++i)
            {
                ImVec2 keyframeButtonPos = timelineStartPos + ImVec2((_float)i * 30.f, 0); // 키프레임 버튼 간격은 30, 높이는 20으로 가정합니다.
                ImVec2 keyframeButtonEndPos = keyframeButtonPos + ImVec2(20, 20); // 버튼 너비와 높이는 20으로 가정합니다.

                // 클릭한 위치가 해당 키프레임 버튼의 영역에 포함되는지 확인합니다.
                if (mousePos.x >= keyframeButtonPos.x && mousePos.x <= keyframeButtonEndPos.x &&
                    mousePos.y >= keyframeButtonPos.y && mousePos.y <= keyframeButtonEndPos.y)
                {
                    // 클릭한 키프레임을 선택합니다.
                    closestKeyframeIndex = i;
                    break; // 선택 후 루프 종료
                }
            }
        }
    }

    // 선택된 키프레임의 인덱스를 사용하여 키프레임을 편집합니다.
    if (!timeline[m_iCurType].empty() && closestKeyframeIndex >= 0 && closestKeyframeIndex < timeline[m_iCurType].size())
    {
        KEYFRAME& selectedKeyframe = timeline[m_iCurType][closestKeyframeIndex];

        // 선택된 키프렘의 애니메이션 속성을 편집하는 함수 호출
        //DrawSelectedKeyframeEditor(selectedKeyframe);
    }
}

void CImguiAnimationTool::SelectKeyframeValueChange()
{
    /*

    키 입력을 모두 임시로 W, A, S, D, (Q, E), ( [, ] ), ( ;, '), (, . ), (m , /) 로 변경 "A"

    */


    // 키프레임을 찾아서 값이 있는 경우에만 수행
    if (closestKeyframeIndex != -1)
    {
        // 다른 키프레임을 선택했을 경우
        if (closestKeyframeIndex != oldKeyframeIndex)
        {

        }

        KEYFRAME& keyframe = timeline[m_iCurType][closestKeyframeIndex]; // 현재 선택한 키프레임

        switch (eEditMode)
        {
        case EDITMODE_NONE:
        {

            break;
        }
        case EDITMODE_SCALE: // 크기 (x, y)
        {   // 키 입력으로 크기 값을 변경

            keyframe.type = 0; // 타입 : 크기

            // "Up" 방향키 : 사이즈 Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W)))
            {
                keyframe.vScale.x += vValueSize;
                keyframe.vScale.y += vValueSize;
                //keyframe.vScale.z += vValueSize;
            }
            // "Down" 방향키 : 사이즈 Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)))
            {
                keyframe.vScale.x -= vValueSize;
                keyframe.vScale.y -= vValueSize;
                //keyframe.vScale.z -= vValueSize;
            }
            // "Left" 방향키 : 사이즈 Minus (큰 값)
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
            {
                keyframe.vScale.x -= vValueSize * 10.f;
                keyframe.vScale.y -= vValueSize * 10.f;
                //keyframe.vScale.z -= vValueSize * 10.f;
            }
            // "Right" 방향키 : 사이즈 Plus(큰 값)
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_D)))
            {
                keyframe.vScale.x += vValueSize * 10.f;
                keyframe.vScale.y += vValueSize * 10.f;
                //keyframe.vScale.z += vValueSize * 10.f;
            }

            //// 크기 값 클램핑 (원하는 범위로 설정)
            //keyframe.vScale.x = ImClamp(keyframe.vScale.x, minScale.x, maxScale.x);
            //keyframe.vScale.y = ImClamp(keyframe.vScale.y, minScale.y, maxScale.y);
            //keyframe.vScale.z = ImClamp(keyframe.vScale.z, minScale.z, maxScale.z);

            break;
        }
        case EDITMODE_ROTATION: // 회전 (x, y, z)
        {   // 키 입력으로 회전 값을 변경

            keyframe.type = 1; // 타입 : 회전

            // "Up" 방향키 : x회전 Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W)))
            {
                keyframe.vRot.x += vValueSize * fValueCurve;
            }
            // "Down" 방향키 : x회전 Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)))
            {
                keyframe.vRot.x -= vValueSize * fValueCurve;
            }
            // "Left" 방향키 : z회전 Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
            {
                keyframe.vRot.z -= vValueSize * fValueCurve;
            }
            // "Right" 방향키 : z회전 Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_D)))
            {
                keyframe.vRot.z += vValueSize * fValueCurve;
            }


            // ". >" : y회전 Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_E)))
            {
                keyframe.vRot.y += vValueSize * fValueCurve;
            }
            // ", <" : y회전 Minus ("Q")
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Q)))
            {
                keyframe.vRot.y -= vValueSize * fValueCurve;
            }

            break;
        }
        case EDITMODE_TRANSLATION: // 이동 (x, y)
        {   // 키 입력으로 이동 값을 변경

            keyframe.type = 2; // 타입 : 이동

            // "Up" 방향키 : y이동 Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W)))
            {
                keyframe.vPos.y += vValueSize;
                //keyframe.vPos.z += vValueSize;
            }
            // "Down" 방향키 : y이동 Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)))
            {
                keyframe.vPos.y -= vValueSize;
                //keyframe.vPos.z -= vValueSize;
            }
            // "Left" 방향키 : x이동 Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
            {
                keyframe.vPos.x -= vValueSize;
                //keyframe.vPos.z -= vValueSize;
            }
            // "Right" 방향키 : x이동 Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_D)))
            {
                keyframe.vPos.x += vValueSize;
                //keyframe.vPos.z += vValueSize;
            }

            break;
        }
        case EDITMODE_TIME_VALUE: // 시간
        {   // 키 입력으로 시간 값을 변경

            // "Up" 방향키 : 시간 Plus "]" 큰 값
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Slash)))
            {
                keyframe.time += vValueSize;
                //keyframe.vPos.z += vValueSize;
            }
            // "Down" 방향키 : 시간 Minus "[" 큰 값
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_M)))
            {
                keyframe.time -= vValueSize;
                //keyframe.vPos.z -= vValueSize;
            }
            // "Up" 방향키 : 시간 Plus "'" 작은 값
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Period)))
            {
                keyframe.time += vValueSize * 0.1f;
                //keyframe.vPos.z += vValueSize;
            }
            // "Down" 방향키 : 시간 Minus ";" 작은 값
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Comma)))
            {
                keyframe.time -= vValueSize * 0.1f;
                //keyframe.vPos.z -= vValueSize;
            }
            // "Left" 방향키 : 벨류 Minus ","
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Semicolon)))
            {
                keyframe.value -= vValueSize * 0.1f;
                //keyframe.vPos.z -= vValueSize;
            }
            // "Right" 방향키 : 벨류 Plus "."
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Apostrophe)))
            {
                keyframe.value += vValueSize * 0.1f;
                //keyframe.vPos.z += vValueSize;
            }

            break;
        }
        case EDITMODE_TEXTURE: // 텍스처
        {   // 키 입력으로 텍스처를 변경

            // "Up" 방향키 : 텍스처 Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightBracket)))
            {
                keyframe.texureframe += (_int)vValueSize;
                //keyframe.vPos.z += vValueSize;
            }
            // "Down" 방향키 : 텍스처 Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftBracket)))
            {
                keyframe.texureframe -= (_int)vValueSize;
                //keyframe.vPos.z -= vValueSize;
            }

            break;
        }
        default:
        {

            break;
        }
        }

        oldKeyframeIndex = closestKeyframeIndex; // 현재 키프레임을 저장
    }
}

void CImguiAnimationTool::SelectKeyframeMouseL()
{
    ImGuiIO& io = ImGui::GetIO(); // ImGui의 시스템 기본 제공 값들 (ex : 시간, 마우스 등)

    float mouseX = io.MousePos.x; // 마우스 위치x
    float mouseY = io.MousePos.y; // 마우스 위치y

    if (mouseX >= timelinePos.x && mouseX <= timelinePos.x + timelineSize.x &&
        mouseY >= timelinePos.y && mouseY <= timelinePos.y + timelineSize.y)
    {
        // 마우스 왼클릭 감지
        if (io.MouseDown[0])
        {


            // 인덱스 값 초기화
            closestKeyframeIndex = -1;

            // 가장 가까운 키프레임을 찾을 때 초기 거리를 설정하고, 나중에 더 작은 거리를 발견하면 업데이트.
            _float closestDistance = FLT_MAX; // 가장 큰 부동 소수점 (초기 최소 거리 값 성정)

            for (int i = 0; i < timeline[m_iCurType].size(); ++i)
            {
                KEYFRAME& keyframe = timeline[m_iCurType][i];

                // 키프레임의 위치
                float keyframeX = keyframe.vKeyFramePos.x;
                float keyframeY = keyframe.vKeyFramePos.y;

                // 마우스 좌표와 키프레임 위치 간의 거리 계산
                float distance = std::sqrt((mouseX - keyframeX) * (mouseX - keyframeX) +
                    (mouseY - keyframeY) * (mouseY - keyframeY));

                // 타임 라인 상의 키프레임 위치를 계산
                float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
                float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

                // 가장 가까운 키프레임 찾기
                if (distance < closestDistance)
                {
                    // 해당 키프레임의 위치 안에 마우스가 있는지 판단
                    if (mouseX >= xPos - 5.0f && mouseY >= yPos - 5.0f &&
                        mouseX <= xPos + 5.0f && mouseY <= yPos + 5.0f)
                    {
                        closestDistance = distance;
                        closestKeyframeIndex = i; // 현재 선택한 인덱스를 마우스가 클릭한 인덱스로 설정
                    }
                }
            }

        }

        // 마우스 이벤트에서 얻은 마우스 좌표
        float mouseRX = io.MousePos.x; // 마우스 위치x
        float mouseRY = io.MousePos.y; // 마우스 위치y

        // 마우스 우클릭 감지
        if (ImGui::IsMouseClicked(1))
        {
            for (int i = 0; i < timeline[m_iCurType].size(); ++i)
            {
                KEYFRAME& keyframe = timeline[m_iCurType][i];

                // 키프레임의 위치
                float keyframeX = keyframe.vKeyFramePos.x;
                float keyframeY = keyframe.vKeyFramePos.y;

                // 마우스 좌표와 키프레임 위치 간의 거리 계산
                float distance = std::sqrt((mouseRX - keyframeX) * (mouseRX - keyframeX) +
                    (mouseRY - keyframeY) * (mouseRY - keyframeY));

                // 타임 라인 상의 키프레임 위치를 계산
                float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
                float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;


                if (!isDraggingTimeline) // 드래그를 하지 않을 경우만 삭제 가능
                {
                    // 오른쪽 마우스 클릭으로 키프레임 삭제
                    if (mouseRX >= xPos - 5.0f && mouseRY >= yPos - 5.0f &&
                        mouseRX <= xPos + 5.0f && mouseRY <= yPos + 5.0f &&
                        ImGui::IsMouseClicked(1))
                    {
                        // 현재 인덱스가 최대 사이즈 일 경우
                        if (i == timeline[m_iCurType].size() - 1)
                        {
                            timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i); // 현재 클릭한 인덱스 삭제
                            closestKeyframeIndex = i - 1;

                            break; // 삭제한 후에 루프를 빠져나감.
                        }
                        else // 최대 사이즈가 아닐 경우
                        {
                            // 현재 인덱스가 0이거나 0보다 크고 사이즈보다 작을 경우
                            if (i < timeline[m_iCurType].size() - 2 && 0 <= i)
                            {
                                // 0보다 작거나 같을 경우
                                if (i <= 0)
                                {
                                    timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i); // 현재 클릭한 인덱스 삭제
                                    closestKeyframeIndex = -1;

                                    break; // 삭제한 후에 루프를 빠져나감.
                                }
                                else
                                {
                                    timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i); // 현재 클릭한 인덱스 삭제
                                    closestKeyframeIndex = i;
                                }
                            }
                        }
                        break; // 삭제한 후에 루프를 빠져나감.
                    }
                }
            }
        }
    }
}

void CImguiAnimationTool::KeyInput()
{
    // "G" 키 : 현재 마우스 위치에 키프레임을 생성
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_G)))
    {
        // 새로운 키프레임을 추가할 때 현재 시간과 가장 가까운 키프레임을 찾는다.
        float minDist = FLT_MAX;
        int insertIndex = 0;

        for (int i = 0; i < timeline[m_iCurType].size(); ++i)
        {
            float dist = fabsf(currentTime - timeline[m_iCurType][i].time);

            if (dist < minDist)
            {
                minDist = dist;
                insertIndex = i;
            }
        }

        // 현재 마우스 위치
        ImVec2 mousePos = ImGui::GetMousePos();

        // 애니메이션 타임 라인 위치
        ImVec2 CurtimelinePos = ImGui::GetCursorScreenPos();
        CurtimelinePos.x += 0.f;
        CurtimelinePos.y += 30.f;

        // 마우스 위치X를 애니메이션 타임 라인 상의 상대적인 위치로 변환
        float relativeMouseX = (mousePos.x - CurtimelinePos.x) / timelineSize.x;

        // 마우스 위치를 애니메이션 타임 라인 상의 상대적인 위치로 변환
        float relativeMouseY = (mousePos.y - CurtimelinePos.y) / timelineSize.y;

        // 벨류 값을 계산합니다. 예를 들어, 상대적인 마우스 위치를 벨류 값으로 사용할 수 있음
        float newValue = 0.7f - ImClamp(relativeMouseY, 0.0f, 1.0f); // 벨류 값 범위를 0에서 1로 클램핑

        // 시간 값을 계산하여 새로운 키프레임을 생성
        float newTime = relativeMouseX * MaxTime; // MaxTime은 최대 시간 값

        // 새로운 키프레임을 생성하고 삽입.
        KEYFRAME newKeyframe = { newTime, newValue, 0, false, false };
        newKeyframe.time = newTime;
        newKeyframe.value = newValue;
        newKeyframe.vKeyFramePos.x = relativeMouseX;
        newKeyframe.vKeyFramePos.y = relativeMouseY;
        timeline[m_iCurType].push_back(newKeyframe);

        // 새로운 키프레임의 시간과 가장 가까운 키프레임을 찾는다. ( 끝 번호 인덱스로 선택됨 일단 보류)
        float minDistNew = FLT_MAX;
        int insertIndexNew = 0;

        for (int i = 0; i < timeline[m_iCurType].size(); ++i)
        {
            float dist = fabsf(newTime - timeline[m_iCurType][i].time);

            if (dist < minDistNew)
            {
                minDistNew = dist;

                insertIndexNew = i;
                closestKeyframeIndex = insertIndexNew;
            }
        }

    }

    // "B" 키 : 현재 시간 위치에 키프레임을 생성
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_B)))
    {
        // 애니메이션 타임 라인 위치
        ImVec2 CurtimelinePos = ImGui::GetCursorScreenPos();
        CurtimelinePos.x += 0.f;
        CurtimelinePos.y += 30.f;

        // 마우스 위치X를 애니메이션 타임 라인 상의 상대적인 위치로 변환
        float relativeMouseX = (CurtimelinePos.x) / timelineSize.x;

        // 마우스 위치를 애니메이션 타임 라인 상의 상대적인 위치로 변환
        float relativeMouseY = (CurtimelinePos.y) / timelineSize.y;

        // 새로운 키프레임을 생성하고 삽입합니다.
        KEYFRAME newKeyframe = { currentTime, 0.f, 0, false, false };
        newKeyframe.time = currentTime;
        newKeyframe.value = 0.f;
        newKeyframe.vKeyFramePos.x = relativeMouseX;
        newKeyframe.vKeyFramePos.y = relativeMouseY;
        timeline[m_iCurType].push_back(newKeyframe);

        float minDist = FLT_MAX;
        int insertIndex = 0;

        for (int i = 0; i < timeline[m_iCurType].size(); ++i)
        {
            float dist = fabsf(currentTime - timeline[m_iCurType][i].time);

            if (dist < minDist)
            {
                minDist = dist;
                insertIndex = i;
                closestKeyframeIndex = insertIndex;
            }
        }
    }

    // "R" 키 : 현재 시간(시간선)을 0으로 초기화 [처음으로 이동]
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_R)))
    {
        currentTime = 0.f;
    }

    // "T" 키 : 현재 시간(시간 선)을 마우스 위치로 변경
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_T)))
    {
        // 현재 마우스 위치
        ImVec2 mousePos = ImGui::GetMousePos();

        // 마우스 위치X를 애니메이션 타임 라인 상의 상대적인 위치로 변환
        float relativeMouseX = (mousePos.x - timelinePos.x) / timelineSize.x;

        // 시간 값을 계산하여 새로운 키프레임을 생성
        float newTime = relativeMouseX * MaxTime; // MaxTime은 최대 시간 값

        // 시간을 현재 마우스 위치에 맞게 설정합니다.
        currentTime = newTime;
    }

    // "Space" 키 : 애니메이션 재생/정지
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
    {
        isPlaying = !isPlaying;
    }

    // "0" 키 : 재생 속도 초기화
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_0)))
    {
        playbackSpeed = 1.f;
    }

    // "-" 키 : 재생 속도 -
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Minus)))
    {
        playbackSpeed -= 1.f;
    }

    // "+" 키 : 재생 속도 +
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Equal)))
    {
        playbackSpeed += 1.f;
    }

    // " ` ~ " 키 : 변경 모드 초기화(없음)
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GraveAccent)))
    {
        eEditMode = EDITMODE_NONE;
    }

    // "F1" 키 : 크기 변경 모드
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F1)))
    {
        eEditMode = EDITMODE_SCALE;
    }

    // "F2" 키 : 회전 변경 모드
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F2)))
    {
        eEditMode = EDITMODE_ROTATION;
    }

    // "F3" 키 : 이동 변경 모드
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F3)))
    {
        eEditMode = EDITMODE_TRANSLATION;
    }

    // "F4" 키 : 시간 및 벨류 변경 모드
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F4)))
    {
        eEditMode = EDITMODE_TIME_VALUE;
    }

    // "F5" 키 : 키프레임 초기화
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F5)))
    {
        timeline[m_iCurType].clear();
    }

    // "F6" 키 : 속성 값 변경 사이즈 조절 초기화
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F6)))
    {
        vValueSize = 1.f;
    }

    // "F7" 키 : 속성 값 변경 사이즈 조절 Minus
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F7)))
    {
        vValueSize -= 1.f;
    }

    // "F8" 키 : 속성 값 변경 사이즈 조절 Plus
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F8)))
    {
        vValueSize += 1.f;
    }

    // "P" 키 : 변경 모드 초기화(없음)
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_P)))
    {
        eEditMode = EDITMODE_TEXTURE;
    }

    // "Z" 키 : 이전 키프레임 선택
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
    {
        // 현재 키프레임이 0보다 작지 않을 경우만
        if (closestKeyframeIndex > 0)
        {
            closestKeyframeIndex -= (_int)1.f;
        }

        if (!timeline[m_iCurType].empty()) // 애니메이션이 있고
        {
            if (closestKeyframeIndex <= -1.f) // 아직 아무것도 선택하지 않았을 경우
            {
                closestKeyframeIndex = 0.f; // 0번째 선택
            }
        }
    }

    // "X" 키 : 다음 키프레임 선택
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
    {
        // 현재 키프레임이 최대 키프레임을 넘지 않았을 때만
        if (closestKeyframeIndex < timeline[m_iCurType].size() - 1)
        {
            closestKeyframeIndex += 1.f;
        }

        if (!timeline[m_iCurType].empty()) // 애니메이션이 있고
        {
            if (closestKeyframeIndex <= -1.f) // 아직 아무것도 선택하지 않았을 경우
            {
                closestKeyframeIndex = 0.f; // 0번째 선택
            }
        }
    }
}

void CImguiAnimationTool::KeyframeList()
{
    // 키프레임 목록 창
    ImGui::Begin(u8"키프레임 정보");

    for (int i = 0; i < timeline[m_iCurType].size(); ++i) {
        KEYFRAME& keyframe = timeline[m_iCurType][i];
        char keyframeName[64];
        snprintf(keyframeName, sizeof(keyframeName), u8"키프레임 %d", i);

        // 키프레임 정보를 목록으로 표시
        if (ImGui::TreeNode(keyframeName)) {
            ImGui::Text(u8"값: %.2f", keyframe.value);
            ImGui::Text(u8"시간: %.2f", keyframe.time);
            ImGui::Text(u8"크기: (%.2f, %.2f, %.2f)", keyframe.vScale.x, keyframe.vScale.y, keyframe.vScale.z);
            ImGui::Text(u8"회전:(%.2f, %.2f, %.2f)", keyframe.vRot.x, keyframe.vRot.y, keyframe.vRot.z);
            ImGui::Text(u8"이동: (%.2f, %.2f, %.2f)", keyframe.vPos.x, keyframe.vPos.y, keyframe.vPos.z);
            ImGui::Checkbox(u8"이징 In", &keyframe.isEaseIn);
            ImGui::Checkbox(u8"이징 Out", &keyframe.isEaseOut);

            // 키프레임 삭제 버튼
            if (ImGui::Button(u8"삭제")) {
                timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i);
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void CImguiAnimationTool::KeyframeChangeWindow()
{
    // 키프렘 목록을 보여주는 ImGui 윈도우를 만든다.
    if (ImGui::Begin(u8"키프레임 수정"))
    {
        // 키프렘 목록을 루프를 돌며 표시.
        for (int i = 0; i < timeline[m_iCurType].size(); ++i)
        {
            KEYFRAME& keyframe = timeline[m_iCurType][i];

            // 각 키프렘을 버튼 또는 다른 ImGui 위젯으로 표시.
            // 여기에서는 버튼을 사용.
            if (ImGui::Button((u8"키프레임" + std::to_string(i)).c_str()))
            {
                // 키프렘이 클릭되면 해당 인덱스를 선택된 키프렘 인덱스로 설정.
                closestKeyframeIndex = i;
            }
        }
    }
    ImGui::End();
}

void CImguiAnimationTool::KeyframeAniV()
{
    // 미리보기 키프레임 보간
    if (!timeline[m_iCurType].empty())
    {
        for (int i = 0; i < timeline[m_iCurType].size() - 1; ++i)
        {
            KEYFRAME& prevKeyframe = timeline[m_iCurType][i];
            KEYFRAME& nextKeyframe = timeline[m_iCurType][i + 1];

            if (currentTime >= prevKeyframe.time && currentTime <= nextKeyframe.time) {
                float alpha = (currentTime - prevKeyframe.time) / (nextKeyframe.time - prevKeyframe.time);

                // 보간된 애니메이션 미리보기 값 계산
                float interpolatedValue = Lerp(prevKeyframe.value, nextKeyframe.value, alpha);

                // 이 값을 사용하여 미리보기 애니메이션 렌더링
                RenderPreviewAnimation(interpolatedValue);

                break;
            }
        }
    }
}

void CImguiAnimationTool::KeyframeRender_ValueChange()
{

    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // 그리기

    // 키프레임 렌더링 및 편집
    for (int i = 0; i < timeline[m_iCurType].size(); ++i)
    {
        KEYFRAME& keyframe = timeline[m_iCurType][i];

        // 타임 라인 상의 위치를 계산
        float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        // 키프레임 위치 값이 0(초기값)일 경우
        if (keyframe.vKeyFramePos.x <= 1 &&
            keyframe.vKeyFramePos.y <= 1)
        {
            // 현재 선택된 키프레임 위치를 넣어준다.
            keyframe.vKeyFramePos.x = xPos;
            keyframe.vKeyFramePos.y = yPos;
        }

        // 현재 시간에 해당하는 키프레임 표시 (빨간색 원)
        if (keyframe.time >= currentTime - 0.1 &&
            keyframe.time <= currentTime + 0.1) {
            draw_list->AddCircleFilled(
                ImVec2(xPos, yPos),
                5.0f,
                IM_COL32(255, 0, 0, 255) // 빨간색으로 표시
            );
        }
        else {
            // 다른 시간의 키프레임 표시 (노란색 원)
            draw_list->AddCircleFilled(
                ImVec2(xPos, yPos),
                5.0f,
                IM_COL32(255, 255, 0, 255) // 노란색으로 표시
            );
        }

        // 마우스가 키프레임 위에 있는 경우에만 툴팁 표시 (마우스 오버)
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f)))
        {

            // 키프레임 정보 표시
            ImGui::SetCursorScreenPos(ImVec2(xPos + 10.0f, yPos - 20.0f));
            ImGui::BeginTooltip();
            ImGui::Text(u8"키프레임 %d", i);
            ImGui::Separator();
            ImGui::Text(u8"시간: %.2f", keyframe.time);
            ImGui::Text(u8"값: %.2f", keyframe.value);
            ImGui::Text(u8"크기: (%.2f, %.2f, %.2f)", keyframe.vScale.x, keyframe.vScale.y, keyframe.vScale.z);
            ImGui::Text(u8"회전:(%.2f, %.2f, %.2f)", keyframe.vRot.x, keyframe.vRot.y, keyframe.vRot.z);
            ImGui::Text(u8"이동: (%.2f, %.2f, %.2f)", keyframe.vPos.x, keyframe.vPos.y, keyframe.vPos.z);

            if (keyframe.isEaseIn)
                ImGui::Text(u8"이징 In: 켜짐");
            else
                ImGui::Text(u8"이징 In: 꺼짐");
            if (keyframe.isEaseOut)
                ImGui::Text(u8"이징 Out: 켜짐");
            else
                ImGui::Text(u8"이징 Out: 꺼짐");
            ImGui::EndTooltip();
        }

        // 마우스 릴리즈 시 드래그 종료
        if (ImGui::IsMouseReleased(0))
        {
            isDraggingTimeline = false;
        }

        if (!isDraggingTimeline) // 드래그를 하지 않을 경우만 삭제 가능
        {
            // 오른쪽 마우스 클릭으로 키프레임 삭제
            if (ImGui::IsMouseHoveringRect(
                ImVec2(xPos - 5.0f, yPos - 5.0f),
                ImVec2(xPos + 5.0f, yPos + 5.0f))
                && ImGui::IsMouseClicked(1))
            {
                timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i);
                break; // 삭제한 후에 루프를 빠져나갑니다.
            }
        }

    }
}

void CImguiAnimationTool::KeyframeAutomaticGeneration()
{
    ImGui::Begin(u8"키프레임 자동 생성");

    ImGui::Text(u8"키프레임 자동 생성 세팅");

    // 입력값을 받는 UI 요소 생성
    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"시간 설정");
    ImGui::InputFloat2(u8"시작/끝 시간 입력", _v2Time);
    ImGui::Separator();

    //ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"벨류 설정");
    ImGui::InputFloat2(u8"시작/끝 벨류 입력", _v2Value);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"크기 설정");
    ImGui::InputFloat3(u8"시작 프레임 크기 입력", minScale);

    //ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::InputFloat3(u8"끝 프레임 크기 입력", maxScale);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"회전 설정");
    ImGui::InputFloat3(u8"시작 프레임 회전 입력", minRot);

    //ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::InputFloat3(u8"끝 프레임 회전 입력", maxRot);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"이동 설정");
    ImGui::InputFloat3(u8"시작 프레임 이동 입력", minPos);

    //ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::InputFloat3(u8"끝 프레임 이동 입력", maxPos);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"텍스처 설정");
    ImGui::InputFloat(u8"시작 프레임 텍스처 입력", &fMin_Texture);

    //ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::InputFloat(u8"끝 프레임 텍스처 입력", &fMax_Texture);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"원하는 키프레임 수 설정");
    ImGui::InputFloat(u8"키프레임 수 입력", &numKeyframes);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"개별 텍스처 설정");
    ImGui::Checkbox(u8"개별 텍스처", &m_bIndividualTexture);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"랜덤 벨류 설정");
    ImGui::Checkbox(u8"랜덤 벨류", &m_bRendomValue);
    ImGui::Separator();

    if (ImGui::Button(u8"자동 생성")) {
        // 버튼이 클릭되면 timeline 벡터를 비우고 CreateKeyframesWithLinearInterpolation 함수 호출
        //timeline[m_iCurType].clear();

        CreateKeyframesWithLinearInterpolation(timeline[m_iCurType],
            _v2Time.x, _v2Time.y,
            _v2Value.x, _v2Value.y,
            minScale, maxScale,
            minRot, maxRot,
            minPos, maxPos,
            fMin_Texture, fMax_Texture,
            numKeyframes);
    }

    if (ImGui::Button("clear"))
    {
        // 버튼이 클릭되면 timeline 벡터를 비우고 CreateKeyframesWithLinearInterpolation 함수 호출
        timeline[m_iCurType].clear();
    }

    //ImGui::End(); // 이거 문제 있음
}

void CImguiAnimationTool::KeyframeDeleteMouseR()
{
    // 키프레임 렌더링 및 편집
    for (int i = 0; i < timeline[m_iCurType].size(); ++i)
    {
        KEYFRAME& keyframe = timeline[m_iCurType][i];

        // 타임 라인 상의 위치를 계산
        float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        if (!isDraggingTimeline) // 드래그를 하지 않을 경우만 삭제 가능
        {
            // 오른쪽 마우스 클릭으로 키프레임 삭제
            if (ImGui::IsMouseHoveringRect(
                ImVec2(xPos - 5.0f, yPos - 5.0f),
                ImVec2(xPos + 5.0f, yPos + 5.0f))
                && ImGui::IsMouseClicked(1))
            {
                timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i);
                break; // 삭제한 후에 루프를 빠져나갑니다.
            }
        }
    }
}
