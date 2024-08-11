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

// �̹��� �߰� �Լ�
void CImguiAnimationTool::AddImage(const std::string& imagePath)
{

    // imagePath�� Wide ���ڿ��� ��ȯ
    WCHAR wideImagePath[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, imagePath.c_str(), -1, wideImagePath, MAX_PATH);

    // �̹����� �ε��ϰ� �ؽ�ó�� ��ȯ
    LPDIRECT3DDEVICE9 pGraphicDev = CImguiMgr::GetInstance()->Get_GraphicDev();
    if (FAILED(D3DXCreateTextureFromFile(pGraphicDev, wideImagePath, &m_pTexture)))
    {
        // �̹��� �ε� ���� ó��
        MSG_BOX("Image Load Failed");
    }
}

HRESULT CImguiAnimationTool::Ready_ImguiWin()
{
    return S_OK;
}

#pragma region ���̾ƿ�
_int CImguiAnimationTool::Update_ImguiWin(const _float& fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    SUPER::Update_ImguiWin(fTimeDelta);

#pragma region ��ŷ �����̼�



    ImGuiWindowFlags iMain_Flags = ImGuiWindowFlags_NoMove;
    ImGuiDockNodeFlags iDockSpace_Flags = ImGuiDockNodeFlags_None;

    // [����] ��ŷ �����̼� ���ε�
    ImGui::Begin("Center", NULL, iMain_Flags);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        // �⺻ ��
        ImGuiID dockspace_id = ImGui::GetID("DockSpace_AnimationTool");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), iDockSpace_Flags);

        if (m_bFirstLoop)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, iDockSpace_Flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

            // �����ʿ� ���̰�, ���� �ʺ� ����
            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, NULL, &dockspace_id);
            // ������ ������ ���̰�, ���� �ʺ� ����
            ImGuiID dock_right_tap_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.3f, &dock_right_id, &dockspace_id);
            // ������ �Ʒ� ���̰�, �ʺ� ����
            ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.328f, NULL, &dock_right_id);
            // ������ �Ʒ� ������ ���̰�, �ʺ� ����
            ImGuiID dock_right_down_tap_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.328f, &dock_right_down_id, &dock_right_id);
            // �Ʒ��ʿ� ���̰�, �ʺ� ����
            ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.32f, NULL, &dockspace_id);


            ImGui::DockBuilderDockWindow(u8"�ؽ�ó", dock_right_id);
            ImGui::DockBuilderDockWindow(u8"Generate", dock_right_tap_id);
            ImGui::DockBuilderDockWindow(u8"Ű������ ����", dock_right_down_id);
            ImGui::DockBuilderDockWindow(u8"Ű������ ����", dock_right_down_tap_id);
            ImGui::DockBuilderDockWindow(u8"�ִϸ��̼� ��", dock_down_id);
            ImGui::DockBuilderDockWindow(u8"���", dockspace_id);

            ImGui::DockBuilderFinish(dockspace_id);
        }

        m_bFirstLoop = false;
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

#pragma endregion

#pragma region ���
    if (ImGui::Begin(u8"���", NULL, iMain_Flags))
    {
        CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 clipSize = ImVec2(contentSize.x / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferWidth,
            contentSize.y / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferHeight);

        // �̹��� ����
        ImGui::Image((void*)pImguiMgr->Get_VecRenderTargetTex(0), contentSize,
            ImVec2((1.f - clipSize.x) * 0.5f, (1.f - clipSize.y) * 0.5f),
            ImVec2(clipSize.x + (1.f - clipSize.x) * 0.5f, clipSize.y + (1.f - clipSize.y) * 0.5f));

    }   ImGui::End();
#pragma endregion

    // ImGui���� �ؽ�ó�� ǥ���� �̹��� ũ�� (��: 200x200 �ȼ�)
    const ImVec2 imageSize(200.0f, 200.0f);

#pragma region �ؽ�ó ��
    static int counter = 0;

    if (ImGui::Begin(u8"�ؽ�ó"))
    {
        // ������
        ImGui::Text(u8"������ %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        // ���ڿ� ���͸� const char* �迭�� ��ȯ
        std::vector<const char*> charImagePaths = ConvertStringVectorToCharArray(imagePaths);

        int selectedPathIndex = 0; // ���õ� �̹��� ��� �ε���

        if (ImGui::CollapsingHeader(u8"�̹��� �ҷ�����"))
        {
            if (ImGui::ListBox("Image Paths", &selectedPathIndex, charImagePaths.data(), (int)charImagePaths.size())) {

                // ������ �̹��� ��θ� ����Ͽ� �̹����� �ε��ϴ� �Լ� ȣ��
                const std::string& selectedImagePath = imagePaths[selectedPathIndex];
                //LoadImg(ConverCtoWC(selectedImagePath.c_str()));
                LoadImg(ConverCtoWC(ConverWStringtoC(ConvertToWideString(imagePaths[selectedPathIndex]))));
            }
        }

        ToggleButton(u8"���", &m_bTest);

        static const char* ini_to_load = NULL;
        if (ini_to_load)
        {
            ImGui::LoadIniSettingsFromDisk(ini_to_load);
            ini_to_load = NULL;
        }

        if (ImGui::Button(u8"���� ��ư"))
        {
            //SaveObjectInformationData();

        }

        ImGui::SameLine();

        if (ImGui::Button(u8"�ҷ����� ��ư"))
        {
            //LoadObjectInforamtionData();
        }

    }
    ImGui::End();
#pragma endregion

#pragma region �ִϸ��̼� ��
    // Ÿ�Ӷ��� ��
    ImGui::Begin(u8"�ִϸ��̼� ��");
    RenderTimeline();
    ImGui::End();
#pragma endregion

    // [����] Center End
    ImGui::End();

    return 0;
}
#pragma endregion

// �̹��� �ε�
void CImguiAnimationTool::LoadImg(const _tchar* folderPath)
{
    // ã�� �̹��� �����͸� ���� ����
    WIN32_FIND_DATA findData;

    // ���� ���
    wstring wfolderPath = (wstring)folderPath + L"/*.*";

    // ���� ��� ���� �� �ڵ� ��ȯ
    HANDLE hFind = FindFirstFile(wfolderPath.c_str(), &findData);

    // �ڵ��� ���� ���
    if (hFind != INVALID_HANDLE_VALUE)
    {
        // ���� ���丮
        vector<wstring> subDirectories;
        // �̹��� ����
        vector<wstring> imageFiles;
        do
        {
            //  ������ �Ӽ� ������ �Էµȴ�. (���丮���� �������� ��)
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (lstrcmp(findData.cFileName, L".") != 0 && lstrcmp(findData.cFileName, L"..") != 0)
                {
                    // ���� ��� = ��� + / + ���ϸ�
                    wstring subFolderPath = (wstring)folderPath + L"/" + findData.cFileName;
                    subDirectories.push_back(subFolderPath);
                    LoadImg(subFolderPath.c_str());
                }
            }
            else
            {
                // ������ ���, �̹��� �������� Ȯ���ϰ� �ε�
                wstring filePath = (wstring)folderPath + L"\\" + findData.cFileName;

                // ���� Ȯ���� Ȯ��
                if (wcsstr(findData.cFileName, L".png") || wcsstr(findData.cFileName, L".jpg") ||
                    wcsstr(findData.cFileName, L".bmp") || wcsstr(findData.cFileName, L".tga") ||
                    wcsstr(findData.cFileName, L".dds"))
                {
                    // IDirect3DBaseTexture9 �������̽��� �޼��带 ����Ͽ� ť�� �� ���� �ؽ�ó�� ������ �ؽ�ó ���ҽ��� ����.
                    IDirect3DBaseTexture9* pBaseTexture = nullptr;

                    // D3DXCreateTextureFromFile(����̽�, ���ϰ��, ���̽� �ؽ�ó) ���Ͽ��� �ؽ�ó�� ����� �Լ�.
                    // �̹��� ������ ���, �ؽ�ó �ε� �� ����
                    if (SUCCEEDED(D3DXCreateTextureFromFile(Engine::Get_GraphicDev(), filePath.c_str(), (LPDIRECT3DTEXTURE9*)&pBaseTexture)))
                    {
                        LPDIRECT3DTEXTURE9 pTexture = static_cast<LPDIRECT3DTEXTURE9>(pBaseTexture);
                        wstring filePathStr = filePath;

                        // ���� ��θ� ������(/)�� ����
                        Replace(filePathStr, L"\\", L"/");

                        // ���� ��ο��� ��ü Ű�� ���� Ű�� ����
                        size_t pos = filePathStr.find_last_of(L"/");
                        wstring ObjKey, StateKey;
                        if (pos != wstring::npos)
                        {
                            ObjKey = filePathStr.substr(filePathStr.find_last_of(L"/", pos - 1) + 1, pos - (filePathStr.find_last_of(L"/", pos - 1) + 1));
                            StateKey = filePathStr.substr(pos + 1);
                        }
                        // ��ü Ű�� uniqueObj ���Ϳ� ���� ��� �߰�
                        if (find(uniqueObj.begin(), uniqueObj.end(), ObjKey) == uniqueObj.end())
                        {
                            uniqueObj.push_back(ObjKey);
                        }
                        // ���� Ű�� uniqueState ���Ϳ� ���� ��� �߰�
                        if (find(uniqueState.begin(), uniqueState.end(), StateKey) == uniqueState.end())
                        {
                            uniqueState.push_back(StateKey);
                            m_MapTexture[make_pair(ObjKey, StateKey)] = pTexture;
                        }
                        // ��ü Ű�� �ش��ϴ� ���� ��Ͽ� ���� Ű �߰�
                        m_MapFile[ObjKey].push_back(StateKey);
                    }
                }
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }
}

// ã��
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

#pragma region ���ڿ� ��ȯ �Լ���
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

//// ������ ����
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
//        // (������ ����, ������ ���� ����, ������, ũ��, nullptr);
//        WriteFile(hFile, &timeline[m_iCurType], sizeof(KEYFRAME), &dwByte, nullptr);
//
//        CloseHandle(hFile);
//    }
//}

//// ������ �ε�
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

// ���� ��ο��� ���� �̸��� �����ϴ� �Լ�
void CImguiAnimationTool::PathRemoveFileSpec(TCHAR* path)
{
    if (path == nullptr || *path == '\0')
        return;

    size_t len = wcslen(path);
    if (len == 0)
        return;

    // ����� �޺κк��� �˻��Ͽ� '\' ���ڸ� ã�� �����մϴ�.
    for (size_t i = len - 1; i > 0; --i)
    {
        if (path[i] == '\\')
        {
            path[i] = '\0';
            break;
        }
    }
}

// ���ڿ� Ÿ�� ��ȯ
std::wstring CImguiAnimationTool::ConvertToWideString(const std::string& ansiString)
{
    int wideStrLen = MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, nullptr, 0);
    if (wideStrLen == 0)
    {
        // ��ȯ ���� ó��
        return L"Fail";
    }

    std::wstring wideString(wideStrLen, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, &wideString[0], wideStrLen);

    return wideString;
}

// ��� ��ư
void CImguiAnimationTool::ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos(); // ���� Ŀ����ġ�� ȭ�� ��ǥ�� ��ȯ���ִ� �Լ� (���콺 ��ġ�� ��ŷ� Ȱ�� �����ҵ� �ϴ�)
    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // ���� â�� �׸��� �׸��� ���� ������ �ڵ�
    // ImGui::GetWindowDrawList()�� ���� â������ �����͸� ��´�.

    float height = ImGui::GetFrameHeight(); // �⺻ ũ�� �� ����
    float width = height * 1.55f;   // ��۹�ư ��� ���� ����
    float radius = height * 0.50f;  // ��۹�ư ���� ���׶�� ũ��(��)

    // ����ڿ��� ������ ������, Ŭ�� �̺�Ʈ�� �����ϰų� ���¸� �����ϱ� ���� ����ϴ� �Լ�
    //              (�����ĺ���(str_id), ��ư�� ũ��)
    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked()) // ������ Ŭ���Ǿ��� �� '*v'��� �Ҹ��� ������ ������ ����Ѵ�. �� ������ ��� ��ư�� ���¸� ��Ÿ����.
        *v = !*v;

    float t = *v ? 1.0f : 0.0f; // '*v' ������ ���� ���� 't' ���� �����Ѵ�. 
    // ex : '*v'�� true�� 't'�� 1.0, false�� 0.0

// ���콺 ���������� ���� ����
    ImU32 col_bg;
    if (ImGui::IsItemHovered()) // ���콺�� ��ư���� �ö󰬴��� �Ǵ� 
        // ImLerp : ���� ���� �Լ� (���콺 ������ ����)
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
    else
        // ImLerp : ���� ���� �Լ� (���콺 Ŭ���� ����)
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

    // draw_list�� ����Ͽ� ��ư�� ����� �׸���.
    // AddRectFilled �Լ��� ����Ͽ� �簢���� �׸���.
    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);

    // ��ư ���ο� ���׶�̸� �׸���. AddCircleFilled �Լ��� ����Ͽ� ���� �׸���.
    draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}




ImVec2 timelinePos(0.f, 0.f);
ImVec2 timelineSize(800.f, 85.f); // �ִϸ��̼� Ÿ�� ���� ũ��

//==============================================[�ִϸ��̼� ��]==============================================
void CImguiAnimationTool::RenderTimeline()
{

#pragma region �ð� Ÿ�Ӷ���

    // �ð�
    ImGui::Text(u8"�ð� : %.2f", currentTime);
    ImGui::SameLine(80.f);

    ImGui::PushItemWidth(150);
    // �ð� �׷���
    float fFrame[100];
    for (int n = 0; n < 100; n++)
        fFrame[n] = sinf(n * 0.2f + (_float)ImGui::GetTime() * playbackSpeed);
    ImGui::PlotLines(u8"�ð� �׷���", fFrame, 100);
    ImGui::PopItemWidth();
    ImGui::SameLine();

    if (ImGui::Button(u8"�ð� �ʱ�ȭ"))
    {
        currentTime = 0.f;
    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(20, 0)); // ���� ������ �߰�
    ImGui::SameLine();

#pragma endregion

#pragma region �ִϸ��̼� ���/����

    // �ִϸ��̼� ���/�Ͻ� ���� ��ư
    if (isPlaying) {
        if (ImGui::Button(u8"����"))
        {
            isPlaying = false;
        }
    }
    else {
        if (ImGui::Button(u8"���"))
        {
            isPlaying = true;
            playbackTime = 0.0f;
        }
    }

    ImGui::SameLine();
    //ImGui::Spacing(); // ���� �߰�
    //ImGui::Dummy(ImVec2(0, 20)); // ���� ������ �߰�
    //ImGui::SameLine();

    ImGui::PushItemWidth(150); // ���� ������
    // �ִϸ��̼� ��� �ӵ� ����
    ImGui::SliderFloat(u8"��� �ӵ�", &playbackSpeed, 0.1f, 3.0f);
    ImGui::SameLine();

    // �ӵ� ���� ��ư
    if (ImGui::Button(u8"�ӵ� �ʱ�ȭ")) // �ӵ� �⺻������ �ʱ�ȭ
    {
        playbackSpeed = 1.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("-")) // �ӵ� -
    {
        playbackSpeed -= 1.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("+")) // �ӵ� +
    {
        playbackSpeed += 1.f;
    }
    ImGui::PopItemWidth();



    ImVec4 textColor = { 1.0f, 0.0f, 0.0f, 1.0f }; // �ؽ�Ʈ �÷� (RGBA)
    ImGui::SameLine(820.f); // ��ġ

    // ���� ���� ��� ǥ��
    switch (eEditMode)
    {
    case EDITMODE_NONE:
    {
        textColor = { 1.f, 1.f, 1.f, 1.f };
        ImGui::TextColored(textColor, u8"����[OFF]");
        break;
    }
    case EDITMODE_SCALE:
    {
        ImGui::TextColored(textColor, u8"ũ�� ����[ON]");
        break;
    }
    case EDITMODE_ROTATION:
    {
        ImGui::TextColored(textColor, u8"ȸ�� ����[ON]");
        break;
    }
    case EDITMODE_TRANSLATION:
    {
        ImGui::TextColored(textColor, u8"�̵� ����[ON]");
        break;
    }
    case EDITMODE_TIME_VALUE:
    {
        ImGui::TextColored(textColor, u8"�ð� �� ���� ����[ON]");
        break;
    }
    case EDITMODE_TEXTURE:
    {
        ImGui::TextColored(textColor, u8"�ؽ�ó ����[ON]");
        break;
    }
    default:
        break;
    }

    ImVec4 textColor2 = { 1.0f, 1.0f, 1.0f, 1.0f }; // �ؽ�Ʈ �÷� (RGBA)
    ImGui::SameLine(920.f); // ��ġ
    ImGui::TextColored(textColor2, u8"���� ���� �� : %f", vValueSize);

#pragma endregion

    ImGui::Separator(); // �� �߰�

#pragma region �ִϸ��̼� Ÿ�� ����
    // Ÿ�� ���� ������

    //ImDrawList* draw_list = ImGui::GetWindowDrawList(); // �׸���
    timelinePos = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // �׸���
    // �ִϸ��̼� Ÿ�� ���� ��ġ ����
    timelinePos.x += 0.f;
    timelinePos.y += 30.f;

    // �ִϸ��̼� Ÿ�� ���� �׸���
    draw_list->AddRectFilled(
        timelinePos,
        ImVec2(timelinePos.x + timelineSize.x, timelinePos.y + timelineSize.y),
        IM_COL32(50, 50, 50, 255) // Ÿ�Ӷ��� ��� ����
    );

    // ���� �ð��� �ּҿ� �ִ� �ð� ������ ��ȯ
    float minTime = 0.0f; // �ּ� �ð�
    //float maxTime = 100.0f; // �ִ� �ð�

    // �ִϸ��̼� Ÿ�� ���� ���� �׸��� (1���� ǥ�� ���� : �ִ� �ð� �� ��ŭ)
    for (int i = 0; i <= MaxTime; ++i)
    {
        // ���� �ð��� ����                                �ִ� �ð������� ���� ���� ����
        float xPos = timelinePos.x + i * (timelineSize.x / MaxTime);

        float relativeStartTimeXPos = timelinePos.x;    //�ִϸ��̼� Ÿ�� ���� �׷����� ����
        float relativeEndTimeXPos = timelinePos.x + timelineSize.x; // �ִϸ��̼� Ÿ�� ���� �׷����� ��

        // �׷��� ������ �迭�� ���� �ð��� �°� ������Ʈ
        for (int n = 0; n < 100; n++) {
            float time = n / 100.0f * MaxTime;
            fFrame[n] = EvaluateAnimationAtTime(time);
        }

        // ���� �ð��� �ִ� �ð� ���� ���� Ŭ����
        currentTime = ImClamp(currentTime, 0.0f, MaxTime);

        // ���� �ð��� �ּҿ� �ִ� �ð� ������ ��ȯ
        float relativeCurrentTime = (currentTime - minTime) / (MaxTime - minTime);

        // ���� �ð� ������ �ִϸ��̼� Ÿ�� ���ο� �´� ��ġ���� ���
        float relativeCurrentTimeXPos = ImLerp(relativeStartTimeXPos, relativeEndTimeXPos, relativeCurrentTime);

        // ���� ��(���� �ð� ��ġ) �׸���
        draw_list->AddLine(
            ImVec2(relativeCurrentTimeXPos, timelinePos.y), // ���� ������
            ImVec2(relativeCurrentTimeXPos, timelinePos.y + timelineSize.y), // ���� ����
            IM_COL32(255, 0, 0, 255) // ���� ������ ���� �ð� ǥ��
        );

        // ���� �Ʒ��� ���ڷ� �ð� ǥ��
        float timeLabel = i * fDisplayTime * (MaxTime / MaxTime); // 0.1 �������� 0���� MaxTime���� ǥ��
        char timeLabelStr[16];
        snprintf(timeLabelStr, sizeof(timeLabelStr), "%.1f", timeLabel);
        ImVec2 textPos(xPos - 10, timelinePos.y + timelineSize.y + 5.f);
        draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), timeLabelStr);
    }

    // graphPos : �ִϸ��̼� Ÿ�� ���� �׷����� �׸��� ���� ���� ��ġ�� ��Ÿ��
    //  graphPos = ImVec2(���� �� �𼭸� ȭ����� ��ġx, y, Ÿ�Ӷ����� ũ��)
    ImVec2 graphPos = ImVec2(timelinePos.x, timelinePos.y + timelineSize.y + 10);


#pragma endregion

#pragma region Ÿ�Ӷ��� Ű������ ����

    // Ÿ�Ӷ��ο��� Ű�������� �巡���Ͽ� ���� ����
    for (int i = 0; i < timeline[m_iCurType].size(); ++i)
    {
        KEYFRAME& keyframe = timeline[m_iCurType][i];
        //float xPos = timelinePos.x + timelineSize.x * ImClamp(keyframe.time, 0.0f, MaxTime);

        // Ÿ�� ���� ���� ��ġ�� ���
        float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f), // ��ǥ ����
            ImVec2(xPos + 5.0f, yPos + 5.0f)) // ��ǥ ����
            && ImGui::IsMouseClicked(0))
        {
            isDraggingKeyframe = true;
            draggingKeyframeIndex = i;
            initialKeyframeX = xPos; // �巡�׸� �����ϴ� ��ġ ����
        }
    }

    if (isDraggingKeyframe)
    {
        // Ű�������� �ð��� �巡���� ��ġ�� ����
        KEYFRAME& keyframe = timeline[m_iCurType][draggingKeyframeIndex];
        float xPos = ImGui::GetMousePos().x - timelinePos.x;

        // ���콺 �巡�׷� Ű�������� �ð��� ����
        float newXPos = ImGui::GetMousePos().x;
        float deltaTime = (newXPos - initialKeyframeX) / timelineSize.x * MaxTime;

        // Ŭ���� ��ġ�� �ش��ϴ� value ��
        keyframe.value = 1.0f - (ImGui::GetMousePos().y - timelinePos.y) / timelineSize.y;

        // �ð� ��
        timeline[m_iCurType][draggingKeyframeIndex].time += deltaTime;

        // �ð��� ���� �����ϰ� Ŭ������ ���� ����.
        // �巡�� ���� �� �ð��� ���� �� ���� (Ű������ time�� value, �ּ� ��, �ִ� ��);
        keyframe.time = ImClamp(keyframe.time, 0.0f, MaxTime); // �ִ� ���� MaxTime���� ����
        keyframe.value = ImClamp(keyframe.value, 0.0f, MaxTime); // �ִ� ���� MaxTime���� ����

        // ���� ��ġ�� ����
        initialKeyframeX = newXPos;

        // Ű������ ���� ����
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

    // ���콺 ������ �� �巡�� ����
    if (ImGui::IsMouseReleased(0))
    {
        isDraggingTimeline = false;
        isDraggingKeyframe = false;
    }

#pragma region ���콺 Ŭ������ Ű������ ����

    SelectKeyframeMouseL();

#pragma endregion

#pragma region ��ư (�߰�, ����, ����, �ε�)

    // Ű������ �߰� ��ư
    if (ImGui::Button(u8"�߰�", ImVec2(30, 0))) {
        // ���ο� Ű�������� �߰��� �� ���� �ð��� ���� ����� Ű�������� ã���ϴ�.
        float minDist = FLT_MAX;
        int insertIndex = 0;
        for (int i = 0; i < timeline[m_iCurType].size(); ++i) {
            float dist = fabsf(currentTime - timeline[m_iCurType][i].time);
            if (dist < minDist) {
                minDist = dist;
                insertIndex = i;
            }
        }

        // ���ο� Ű�������� �����ϰ� �����մϴ�.
        KEYFRAME newKeyframe = { currentTime, 0.5f, 0, false, false };
        newKeyframe.time = currentTime;
        timeline[m_iCurType].insert(timeline[m_iCurType].begin() + insertIndex, newKeyframe);
    }

#pragma region ����Ű

    KeyInput();

#pragma endregion

#pragma region Ű������ ��� / ����, ���� ��ư

    ImGui::SameLine();

    KeyframeList(); // Ű������ ���

    ImGui::SameLine();

    // Ű������ ���� ��ư
    if (ImGui::Button(u8"����", ImVec2(30, 0))) {
        if (!timeline[m_iCurType].empty()) {
            timeline[m_iCurType].pop_back();
        }
    }

    ImGui::SameLine();

    // �ִϸ��̼� ����
    if (ImGui::Button(u8"����", ImVec2(30, 0))) {
        SaveAnimationToFile(cFileSaveName);
    }

#pragma endregion

    ImGui::SameLine();

#pragma region Ÿ�Ӷ��� �׷���

    // Ÿ�Ӷ��� �׷���
    float* values = new float[100];
    for (int i = 0; i < 100; i++) {
        float time = i / 100.0f;
        values[i] = EvaluateAnimationAtTime(time);
    }
    ImGui::PlotLines("Value Curve", values, 100, 0, nullptr, FLT_MIN, FLT_MAX, ImVec2(300, 0));

    delete[] values;

#pragma endregion

#pragma region �ִϸ��̼� �Ӽ� ����

    KeyframeChangeWindow();

    //KeyframeAniV();

    ImGui::Dummy(ImVec2(0.f, 110.f));


    ImGui::PushItemWidth(100); // �Է� �ʵ��� ���̸� ���ϴ� �ȼ��� ����
    // �ִϸ��̼� ������ ���� �Է�
    ImGui::InputInt(u8"�ִϸ��̼� ������", &animationFrame);
    ImGui::PopItemWidth(); // ���� ������ �ǵ���

    ImGui::SameLine();

    // �ִϸ��̼� ������ ���� ��ư
    if (ImGui::Button(u8"������ ����")) {
        // Ű�������� animationFrame ���� ���� �����ϰ� ����
        EditKeyframesForAnimation(animationFrame);
        currentTime = playbackTime;
    }

    ImGui::SameLine(320);

    ImGui::Text(u8"�ִϸ��̼� �ݺ� : ");

    ImGui::SameLine();

    ImGui::RadioButton(u8"On", &bRepetition, false); // �ݺ� On
    ImGui::SameLine();
    ImGui::RadioButton(u8"Off", &bRepetition, true); // �ݺ� Off

    ImGui::SameLine(690.f);

    // �ִϸ��̼� �ҷ�����
    if (ImGui::Button(u8"�ִϸ��̼� �ҷ�����", ImVec2(120, 0))) {
        LoadAnimationFromFile(cFileLoadName);
    }

    // �ִϸ��̼� �ð� ����
    if (isPlaying)
    {
        currentTime += playbackSpeed * ImGui::GetIO().DeltaTime;
        if (currentTime > MaxTime) // ���� �ð��� �ִ� �ð� �� ���� Ŭ ���
        {
            currentTime = MaxTime; // ���� �ð��� �ִ� �ð� ������ ����
            isPlaying = false;  // �÷��� ����
        }
    }

    ImGui::PushItemWidth(100); // �Է� �ʵ��� ���̸� ���ϴ� �ȼ��� ����
    ImGui::InputText(u8"������ ���� �̸�", cFileSaveName, sizeof(cFileSaveName));
    ImGui::SameLine();
    ImGui::InputText(u8"�ҷ��� ���� �̸�", cFileLoadName, sizeof(cFileLoadName));
    ImGui::PopItemWidth(); // ���� ������ �ǵ���

#pragma endregion

#pragma region Ű������ �ڵ� ����

    KeyframeAutomaticGeneration();

#pragma region Ű������ ������ �� ���� (������ ���� ����)

    // # ���콺 ������ ������ ������ ��� UI���� �׷����� �ϱ� ������ ������ ���� �������̿��� �Ѵ�.
    // Ű�������� �巡�׷� �̵��� ��ġ ���� �ٲ�� ������ ���� Ű������ �� ���溸�� ���� �־�� �Ѵ�. (����)
    //KeyframeRender_ValueChange(); // draw ������ �ӽ÷� �ۿ� ������

       // Ű������ ������ �� ����
    for (int i = 0; i < timeline[m_iCurType].size(); ++i)
    {
        KEYFRAME& keyframe = timeline[m_iCurType][i];

        // Ÿ�� ���� ���� ��ġ�� ���
        float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        // Ű������ ��ġ ���� 0(�ʱⰪ)�� ���
        if (keyframe.vKeyFramePos.x <= 1 &&
            keyframe.vKeyFramePos.y <= 1)
        {
            // ���� ���õ� Ű������ ��ġ�� �־��ش�.
            keyframe.vKeyFramePos.x = xPos;
            keyframe.vKeyFramePos.y = yPos;
        }

        // ���� �ð��� �ش��ϴ� Ű������ ǥ�� (������ ��)
        if (keyframe.time >= currentTime - 0.1 &&
            keyframe.time <= currentTime + 0.1) {
            draw_list->AddCircleFilled(
                ImVec2(xPos, yPos),
                5.0f,
                IM_COL32(255, 0, 0, 255) // ���������� ǥ��
            );
        }
        else {
            // �ٸ� �ð��� Ű������ ǥ�� (����� ��)
            draw_list->AddCircleFilled(
                ImVec2(xPos, yPos),
                5.0f,
                IM_COL32(255, 255, 0, 255) // ��������� ǥ��
            );
        }

        // ���콺�� Ű������ ���� �ִ� ��쿡�� ���� ǥ�� (���콺 ����)
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f)))
        {

            // Ű������ ���� ǥ��
            ImGui::SetCursorScreenPos(ImVec2(xPos + 10.0f, yPos - 20.0f));
            ImGui::BeginTooltip();
            ImGui::Text(u8"Ű������ %d", i);
            ImGui::Separator();
            ImGui::Text(u8"�ð�: %.2f", keyframe.time);
            ImGui::Text(u8"��: %.2f", keyframe.value);
            ImGui::Text(u8"ũ��: (%.2f, %.2f, %.2f)", keyframe.vScale.x, keyframe.vScale.y, keyframe.vScale.z);
            ImGui::Text(u8"ȸ��:(%.2f, %.2f, %.2f)", keyframe.vRot.x, keyframe.vRot.y, keyframe.vRot.z);
            ImGui::Text(u8"�̵�: (%.2f, %.2f, %.2f)", keyframe.vPos.x, keyframe.vPos.y, keyframe.vPos.z);

            if (keyframe.isEaseIn)
                ImGui::Text(u8"��¡ In: ����");
            else
                ImGui::Text(u8"��¡ In: ����");
            if (keyframe.isEaseOut)
                ImGui::Text(u8"��¡ Out: ����");
            else
                ImGui::Text(u8"��¡ Out: ����");
            ImGui::EndTooltip();
        }

        // ���콺 ������ �� �巡�� ����
        if (ImGui::IsMouseReleased(0))
        {
            isDraggingTimeline = false;
        }

        //if (!isDraggingTimeline) // �巡�׸� ���� ���� ��츸 ���� ����
        //{
        //    // ������ ���콺 Ŭ������ Ű������ ����
        //    if (ImGui::IsMouseHoveringRect(
        //        ImVec2(xPos - 5.0f, yPos - 5.0f),
        //        ImVec2(xPos + 5.0f, yPos + 5.0f))
        //        && ImGui::IsMouseClicked(1))
        //    {
        //        timeline.erase(timeline.begin() + i);
        //        break; // ������ �Ŀ� ������ ���������ϴ�.
        //    }
        //}

    }

#pragma endregion

#pragma region ������ Ű������ �� ���� (����Ű)

    SelectKeyframeValueChange();

#pragma endregion

#pragma region ������ Ű������ �� ���� (â)

    // ���õ� Ű������ �ε����� ����Ͽ� Ű�������� ����. (������ �ε����� ���� ���� ���� ���)
    if (!timeline[m_iCurType].empty() && closestKeyframeIndex >= 0 && closestKeyframeIndex < timeline[m_iCurType].size())
    {
        KEYFRAME& selectedKeyframe = timeline[m_iCurType][closestKeyframeIndex];

        // ���õ� Ű������ �ִϸ��̼� �Ӽ� ����
        DrawSelectedKeyframeEditor(selectedKeyframe);
    }

#pragma endregion

    ImGui::End();

}
//==============================================[�ִϸ��̼� ��]==============================================

// �ִϸ��̼� ����
void CImguiAnimationTool::SaveAnimationToFile(const char* fileName)
{
    // .dat ���� Ȯ���ڸ� �߰��� ���� �̸� ����
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // ������ UTF-8�� ���� (���ڿ� ���� ����)
    std::ofstream file(datFileName.c_str(), std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        // ������ �� �� ���� ���� ���� ó��
        std::cerr << "Failed to open file: " << datFileName << std::endl;
        return;
    }

    for (const KEYFRAME& keyframe : timeline[m_iCurType]) {
        // Keyframe ����ü�� ���Ͽ� �� �� UTF-8�� ���ڵ��� ���ڿ��� ����
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

// �ִϸ��̼� �ҷ�����
void CImguiAnimationTool::LoadAnimationFromFile(const char* fileName)
{
    // .dat ���� Ȯ���ڸ� �߰��� ���� �̸� ����
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // ������ UTF-8�� ����
    std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        // ������ �� �� ���� �� �޼���
        return;
    }

    timeline[m_iCurType].clear(); // Ŭ����

    KEYFRAME keyframe; // Ű������ ����

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
        timeline[m_iCurType].push_back(keyframe); // �ҷ��� �ִϸ��̼� �� ����ֱ�
    }

    file.close(); // ���� �ݱ�
}

// �ִϸ��̼��� ����Ͽ� ���� �ð� ���� ��ȯ�ϴ� �Լ�
float CImguiAnimationTool::EvaluateAnimationAtTime(float time)
{
    if (timeline[m_iCurType].empty()) {
        // ���� Ű�������� ���ٸ� �⺻�� 0.0�� ��ȯ�ϰų�,
        // �ٸ� ó���� ����.
        return 0.0f;
    }

    // Ű�������� �ð��� ���� ����.
    std::sort(timeline[m_iCurType].begin(), timeline[m_iCurType].end(), [](const KEYFRAME& a, const KEYFRAME& b) {
        return a.time < b.time;
        });

    // ���� �ð��� ���� ����� ������ ������ Ű�������� ã��
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

    // ������ ���� Ű�������� ã�� ���ϸ� ù ��° Ű�������� ���.
    if (!prevKeyframe) {
        prevKeyframe = &timeline[m_iCurType][0];
    }

    // ���� Ű�������� ã�� ���ϸ� ������ Ű�������� ���.
    if (!nextKeyframe) {
        nextKeyframe = &timeline[m_iCurType].back();
    }

    // ������ ���� Ű������ ���̿��� �ð��� �����Ͽ� ���� ���.
    float t = (time - prevKeyframe->time) / (nextKeyframe->time - prevKeyframe->time);
    float value = ImLerp(prevKeyframe->value, nextKeyframe->value, t);

    // ��¡(Easing)�� �����մϴ�.
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

// �����ƽ ��¡(InQuad) �Լ� (����)
float CImguiAnimationTool::ImEaseInQuad(float start, float end, float t)
{
    return start + (end - start) * t * t;
}

// �����ƽ ��¡(OutQuad) �Լ� (��)
float CImguiAnimationTool::ImEaseOutQuad(float start, float end, float t)
{
    return start + (end - start) * (-t * (t - 2));
}

// �̸����� �ִϸ��̼� ������ �Լ�
void CImguiAnimationTool::RenderPreviewAnimation(float value)
{
    // �� �Լ����� �̸����� �ִϸ��̼� ������
    // value�� ������� ũ��, ȸ��, �̵� ���� �����Ͽ� ������

    ImGui::SameLine(670.f);
    ImVec4 textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // ������
    ImGui::TextColored(textColor, "Preview Animation On");

    // ����: �� �׸���
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // �ؽ�Ʈ ���� ����

    // �������� value�� ���� �����մϴ�.
    float radius = 20.0f + value * 30.0f;

    // ȸ���� value�� ���� �����մϴ�.
    float rotation_degrees = value * 360.0f;

    // �̵��� value�� ���� �����մϴ�.
    float x_position = 100.0f + value * 200.0f;
    float y_position = 100.0f + value * 50.0f;

    ImGui::GetWindowDrawList()->AddCircleFilled(
        ImVec2(x_position, y_position),
        radius,
        IM_COL32(255, 0, 0, 255), // ���� ����
        64); // ���� ���� ��

    ImGui::PopStyleColor();
}

// ���� ���� �Լ�
float CImguiAnimationTool::Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

// std::string ���͸� const char* �迭�� ��ȯ�ϴ� �Լ�
std::vector<const char*> CImguiAnimationTool::ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector)
{
    std::vector<const char*> charArray;
    charArray.reserve(stringVector.size());
    for (const std::string& str : stringVector) {
        charArray.push_back(str.c_str());
    }
    return charArray;
}

// ������ Ű������ ���� �����ϴ� �Լ�
void CImguiAnimationTool::DrawSelectedKeyframeEditor(KEYFRAME& selectedKeyframe)
{

    //if (iOldIndex != closestKeyframeIndex)
    //{
    //    m_bSheild = false;
    //    m_bSheildFrame = false;
    //    m_bCharging = false;
    //    m_bChargingFrame = false;
    //}

    // Ÿ�Ӷ��� ��
    ImGui::Begin(u8"������ Ű������ �Ӽ� ����");

    //Keyframe& selectedKeyframe = timeline.back();
    ImGui::Text(u8"���� ���� : Ű������%d", closestKeyframeIndex);

    ImGuiStyle& style = ImGui::GetStyle(); // ��Ÿ�� �ҷ�����
    float originalItemWidth = style.ItemInnerSpacing.x; // ������ ItemInnerSpacing ���� ����

    // ���ϴ� ũ��� ItemInnerSpacing�� ����
    style.ItemInnerSpacing.x = 10.0f; // ���� ������ 10.0f�� ����

    ImGui::SeparatorText(u8"�ð� �� ���� ����");

    // �ð� �Է� �ʵ��� ���� ���̸� ���� (ex : 40 �ȼ�)
    ImGui::PushItemWidth(40);

    // �ð� �Է� �ʵ�
    if (ImGui::InputFloat(u8"�ð�", &selectedKeyframe.time, fMin_Time, fMax_Time))
    {
        // ���� Ű�������� �ε��� ã��
        for (int i = 0; i < timeline[m_iCurType].size(); ++i) {
            if (&timeline[m_iCurType][i] == &selectedKeyframe) {
                originalIndex = i;
                break;
            }
        }

        // Ű�������� �ð��� ���� ����
        std::sort(timeline[m_iCurType].begin(), timeline[m_iCurType].end(), [](const KEYFRAME& a, const KEYFRAME& b) {
            return a.time < b.time;
            });

        // ���� Ű�������� �ε��� ã��
        for (int i = 0; i < timeline[m_iCurType].size(); ++i) {
            if (&timeline[m_iCurType][i] == &selectedKeyframe) {
                closestKeyframeIndex = i;
                break;
            }
        }

        // Ű������ ���� ����
        if (originalIndex != closestKeyframeIndex) {
            // Ű�������� ���� ��ġ���� �̵��� ���
            // �̵��� ��ġ�� ���� �ε��� ������Ʈ
            draggingKeyframeIndex = closestKeyframeIndex;
        }
    }

    ImGui::PopItemWidth();

    //// �ð� �� ����
    //oldTime = selectedKeyframe.time;

    //// ���� �ٸ� Ű�������� �ð� ���� �������� �ʵ��� ��ġ�Ѵ�
    //// �ð� ���� ������ Ű�������� ���� �ε����� Ű�������� �̵����� �ʾƾ� �Ѵ�
    //for (int i = selectedKeyframeIndex + 1; i < timeline.size(); ++i)
    //{
    //    Keyframe& currentKeyframe = timeline[i];

    //    // �̵����� �ʵ��� ���� �ð����� ���̸� ���մϴ�.
    //    float timeDifference = currentKeyframe.time - oldTime;
    //    currentKeyframe.time = newTime + timeDifference;
    //}

    ImGui::SameLine(); // ���� ����

    // ���� �Է� �ʵ��� ���� ���̸� ���� (ex : 40 �ȼ�)
    ImGui::PushItemWidth(40);
    // ���� �Է� �ʵ�
    ImGui::InputFloat(u8"����", &selectedKeyframe.value, fMin_Value, fMax_Value);
    ImGui::PopItemWidth();

    //ImGui::SameLine(); // ���� ����
    ImGui::Dummy(ImVec2(0, 5)); // ����

    // �ؽ�ó �Է� �ʵ��� ���� ���̸� ���� (ex : 40 �ȼ�)
    ImGui::PushItemWidth(80);
    //ImGui::Text("%d", selectedKeyframe.texureframe);
    ImGui::InputInt(u8"�ؽ�ó ��ȣ", &selectedKeyframe.texureframe);
    ImGui::PopItemWidth();

    // ������ ItemInnerSpacing ������ ����
    style.ItemInnerSpacing.x = originalItemWidth;

    ImGui::Dummy(ImVec2(0, 5)); // ����
    // �ִϸ��̼� Ÿ�� ����
    ImGui::SeparatorText(u8"�ִϸ��̼� Ÿ�� ����");
    ImGui::RadioButton(u8"ũ��", &selectedKeyframe.type, 0); // ũ�� �ִϸ��̼�
    ImGui::SameLine();
    ImGui::RadioButton(u8"ȸ��", &selectedKeyframe.type, 1); // ȸ�� �ִϸ��̼�
    ImGui::SameLine();
    ImGui::RadioButton(u8"�̵�", &selectedKeyframe.type, 2); // �̵� �ִϸ��̼�

    ImGui::Dummy(ImVec2(0, 5)); // ����

    style.ItemInnerSpacing.x = 10.0f; // ���� ������ 10.0f�� ����
    // Ű������ Ÿ�Կ� ���� �߰� ���� (�����̴�)
    if (selectedKeyframe.type == 0) { // ũ�� �ִϸ��̼�
        ImGui::SliderFloat3(u8"ũ��(�����̴�)", selectedKeyframe.vScale, fMin_Scale, fMax_Scale);
    }
    else if (selectedKeyframe.type == 1) { // ȸ�� �ִϸ��̼�
        ImGui::SliderFloat3(u8"ȸ��(�����̴�)", selectedKeyframe.vRot, fMin_Rot, fMax_Rot);
    }
    else if (selectedKeyframe.type == 2) { // �̵� �ִϸ��̼�
        ImGui::SliderFloat3(u8"�̵�(�����̴�)", selectedKeyframe.vPos, fMin_Pos, fMax_Pos);
    }
    style.ItemInnerSpacing.x = originalItemWidth;// ������ ItemInnerSpacing ������ ����

    //ImGui::Dummy(ImVec2(0, 10));
    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 5)); // ����

    style.ItemInnerSpacing.x = 10.0f; // ���� ������ 10.0f�� ����
    // Ű������ Ÿ�Կ� ���� �߰� ���� (�Է�)
    if (selectedKeyframe.type == 0) // ũ�� �ִϸ��̼�
    {
        // ũ�� �� �Է�
        ImGui::InputFloat3(u8"ũ��(�Է�)", selectedKeyframe.vScale);
        // �Էµ� ���� ������ ����(����)
        selectedKeyframe.vScale.x = (selectedKeyframe.vScale.x < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.x > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.x);
        selectedKeyframe.vScale.y = (selectedKeyframe.vScale.y < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.y > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.y);
        selectedKeyframe.vScale.z = (selectedKeyframe.vScale.z < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.z > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.z);
    }
    else if (selectedKeyframe.type == 1) // ȸ�� �ִϸ��̼�
    {
        // ȸ�� �� �Է�
        ImGui::InputFloat3(u8"ȸ��(�Է�)", selectedKeyframe.vRot);

        // �Էµ� ���� ������ ����(�Լ�)
        selectedKeyframe.vRot.x = ImClamp(selectedKeyframe.vRot.x, fMin_Rot, fMax_Rot); // X ���� ���� ����
        selectedKeyframe.vRot.y = ImClamp(selectedKeyframe.vRot.y, fMin_Rot, fMax_Rot); // Y ���� ���� ����
        selectedKeyframe.vRot.z = ImClamp(selectedKeyframe.vRot.z, fMin_Rot, fMax_Rot); // Z ���� ���� ����
    }
    else if (selectedKeyframe.type == 2) // �̵� �ִϸ��̼�
    {
        // �̵� �� �Է�
        ImGui::InputFloat3(u8"�̵�(�Է�)", selectedKeyframe.vPos);

        // �Էµ� ���� ������ ����(�Լ�)
        selectedKeyframe.vPos.x = ImClamp(selectedKeyframe.vPos.x, fMin_Pos, fMax_Pos); // X ���� ���� ����
        selectedKeyframe.vPos.y = ImClamp(selectedKeyframe.vPos.y, fMin_Pos, fMax_Pos); // Y ���� ���� ����
        selectedKeyframe.vPos.z = ImClamp(selectedKeyframe.vPos.z, fMin_Pos, fMax_Pos); // Z ���� ���� ����
    }
    style.ItemInnerSpacing.x = originalItemWidth;// ������ ItemInnerSpacing ������ ����

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"���� ����");

    //// OBJ_TYPE ���� ���
    //const char* objTypeItems[] = { u8"����", u8"�ָ�", u8"��� ����", u8"�Ѽ� ����" };
    //int selectedObjType = static_cast<int>(selectedKeyframe.m_eObjectType);
    //ImGui::Combo(u8"Ÿ��", &selectedObjType, objTypeItems, IM_ARRAYSIZE(objTypeItems));
    //selectedKeyframe.m_eObjectType = static_cast<OBJ_TYPE>(selectedObjType);

    //// OBJ_NAME ���� ���
    //const char* objNameItems[] = { u8"����", u8"����", u8"�轼 �����", u8"��������", u8"���ֺ�", u8"��������" };
    //int selectedObjName = static_cast<int>(selectedKeyframe.m_eObjectName);
    //ImGui::Combo(u8"����", &selectedObjName, objNameItems, IM_ARRAYSIZE(objNameItems));
    //selectedKeyframe.m_eObjectName = static_cast<OBJ_NAME>(selectedObjName);

#pragma region Ÿ�Կ� ���� ���ø�� ������Ʈ (���� �̻��)
    //// OBJ_TYPE ���� ���
    //const char* objTypeItems[] = { u8"����", u8"�ָ�", u8"��� ����", u8"�Ѽ� ����" };
    //int selectedObjType = static_cast<int>(selectedKeyframe.m_eObjectType);
    //if (ImGui::Combo(u8"Ÿ��", &selectedObjType, objTypeItems, IM_ARRAYSIZE(objTypeItems))) {
    //    selectedKeyframe.m_eObjectType = static_cast<OBJ_TYPE>(selectedObjType);
    //}

    //// OBJ_NAME ���� ��� (Ÿ�Կ� ���� �ٸ� ��� ���)
    //const char* objNameItems[] = { u8"����" };
    //int numObjNameItems = 1; // �⺻������ "����"�� ǥ��
    //int selectedObjName = 0;

    //// Ÿ�Կ� ���� ���� ��� ������Ʈ
    //if (selectedKeyframe.m_eObjectType == OBJ_TYPE::TWO_HAND) {
    //    const char* fistItems[] = { u8"����", u8"�ָ�" };
    //    numObjNameItems = IM_ARRAYSIZE(fistItems);
    //    ImGui::Combo(u8"����", &selectedObjName, fistItems, numObjNameItems);
    //}
    //else if (selectedKeyframe.m_eObjectType == OBJ_TYPE::TWO_OBJECT) {
    //    const char* twoHandedItems[] = { u8"����", u8"���_����1", u8"���_����2" };
    //    numObjNameItems = IM_ARRAYSIZE(twoHandedItems);
    //    ImGui::Combo(u8"����", &selectedObjName, twoHandedItems, numObjNameItems);
    //}
    //else if (selectedKeyframe.m_eObjectType == OBJ_TYPE::RIGHT_OBJECT) {
    //    const char* oneHandedItems[] = { u8"����", u8"�Ѽ�_����1", u8"�Ѽ�_����2" };
    //    numObjNameItems = IM_ARRAYSIZE(oneHandedItems);
    //    ImGui::Combo(u8"����", &selectedObjName, oneHandedItems, numObjNameItems);
    //}
    //else {
    //    ImGui::Combo(u8"����", &selectedObjName, objNameItems, numObjNameItems);
    //}

    //selectedKeyframe.m_eObjectName = static_cast<OBJ_NAME>(selectedObjName);
#pragma endregion

#pragma region ����

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"��¡ ����");
    // ��¡ ���� (Ease In, Ease Out)
    ImGui::Checkbox("Ease In", &selectedKeyframe.isEaseIn);
    ImGui::SameLine();
    ImGui::Checkbox("Ease Out", &selectedKeyframe.isEaseOut);
    //ImGui::SameLine();

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"���� ����");

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::Checkbox(u8"���尡�� ����", &selectedKeyframe.bShieldPossible);
    ImGui::Dummy(ImVec2(0, 5)); // ����

    // ��¡ �Է� �ʵ��� ���� ���̸� ����
    ImGui::PushItemWidth(80);
    ImGui::Checkbox(u8"����� Ű������", &selectedKeyframe.bShieldKeyFrame);
    ImGui::PopItemWidth();

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"��¡ ����");

    ImGui::Dummy(ImVec2(0, 5)); // ����
    
    ImGui::Checkbox(u8"��¡���� ����", &selectedKeyframe.bChargePossible);
    ImGui::Dummy(ImVec2(0, 5)); // ����

    // ��¡ �Է� �ʵ��� ���� ���̸� ����
    ImGui::PushItemWidth(80);
    ImGui::Checkbox(u8"Ǯ��¡�� Ű������", &selectedKeyframe.bFullChargeKeyframe);
    ImGui::PopItemWidth();

#pragma endregion

    ImGui::End(); // Ű������ ���� â ��

    // ���õ� Ű�����Ӹ� ������Ʈ
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


    iOldIndex = closestKeyframeIndex; // ���� �ε��� ��ȣ�� ����
}

// _vec3 ���� ���� �Լ�
_vec3 CImguiAnimationTool::Lerp(const _vec3& a, const _vec3& b, float t) {
    _vec3 result;
    result.x = a.x + t * (b.x - a.x);
    result.y = a.y + t * (b.y - a.y);
    result.z = a.z + t * (b.z - a.z);
    return result;
}

// _vec2 ���� ���� �Լ�
_float CImguiAnimationTool::Lerp2(const _float& a, const _float& b, float t) {
    _float result;
    result = a + t * (b - a);
    return result;
}

// �ּ�, �ִ� �� �� ũ��, ȸ��, �̵� ���� ������� ���� ������ Ű�������� �ڵ����� �����ϴ� �Լ�
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
        // �ּ� 2�� �̻��� Ű�������� �ʿ��մϴ�.
        return;
    }

    // Ű������ ���� �ð� ������ ���
    float timeStep = 1.0f / static_cast<float>(numKeyframes - 1);

    // ù ��°�� ������ Ű�������� �������� �߰�
    KEYFRAME firstKeyframe;

    // �ּ� �� ����
    firstKeyframe.time = minTime;
    firstKeyframe.value = minValue;
    firstKeyframe.vScale = minscaleValue;
    firstKeyframe.vRot = minrotationValue;
    firstKeyframe.vPos = mintranslationValue;
    firstKeyframe.texureframe = _minTexture;

    timeline.push_back(firstKeyframe);

    KEYFRAME lastKeyframe;

    // �ִ� �� ����
    lastKeyframe.time = maxTime;
    lastKeyframe.value = maxValue;
    lastKeyframe.vScale = maxscaleValue;
    lastKeyframe.vRot = maxrotationValue;
    lastKeyframe.vPos = maxtranslationValue;
    lastKeyframe.texureframe = _maxTexture;

    timeline.push_back(lastKeyframe);


    int num_decimal_places = 2; // �Ҽ��� ���� ����

    // �Ҽ��� ���� ǥ��
    int multiplier = static_cast<int>(pow(10, num_decimal_places));


    // �߰� Ű������ ���� �� ����
    for (int i = 1; i < numKeyframes - 1; ++i) 
    {
        float time = i * timeStep;
        float t = time;  // �ð��� ���� ����� ���

        int itextureRange = _maxTexture - _minTexture + 1; // �ؽ�ó ���� ���
        int iTextureNum = _minTexture + (i % itextureRange); // ���� �ε����� ���� �ؽ�ó �� ����

        // �� Ű�����Ӹ��� ���������� ���� �� ������ ���� �����Ͽ� �߰�
        float step = (maxValue - minValue) / (numKeyframes - 1); // ����/���� �ܰ�


        // ������ �Ӽ��� ���� �������� ���
        _vec3 scale = Lerp(minscaleValue, maxscaleValue, t);

        _vec3 rotation = Lerp(minrotationValue, maxrotationValue, t);

        _vec3 translation = Lerp(mintranslationValue, maxtranslationValue, t);

        _float ftime = Lerp2(minTime, maxTime, t);

        _float value = Lerp2(minValue, maxValue, t);

        _int iTexture = (_int)Lerp2(_minTexture, _maxTexture, t);

        KEYFRAME keyframe;

        // �� ����
        keyframe.time = ftime;

        if (m_bRendomValue)
        {
            // ���������� ���� �� ������ �Ҽ��� �� ����
            if (i < numKeyframes / 2)
            {
                keyframe.value = minValue + i * step; // �ּҰ����� ����
            }
            else
            {
                keyframe.value = maxValue - (i - numKeyframes / 2) * step; // �ִ밪���� ����
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

        // ���� timeline ���Ϳ� Ű�������� �̾ �߰�
        timeline.emplace_back(keyframe);
    }

    // �ð��� ���� Ű������ ����
    std::sort(timeline.begin(), timeline.end(), [](const KEYFRAME& a, const KEYFRAME& b) {
        return a.time < b.time;
        });
}

void CImguiAnimationTool::EditKeyframesForAnimation(int numFrames)
{
    //timeline.clear(); // ������ Ű�������� ��� ����ϴ�.

    //float frameTime = MaxTime / static_cast<float>(numFrames);

    //for (int frame = 0; frame <= numFrames; ++frame) {
    //    float time = frame * frameTime;
    //    float value = EvaluateAnimationAtTime(time);

    //    // Ű�������� �����ϰ� ����Ʈ�� �߰��մϴ�.
    //    Keyframe keyframe;
    //    keyframe.time = time;
    //    keyframe.value = value;

    //    // ������ Ű������ �Ӽ��� ���⿡ �����մϴ�.
    //    // ��: keyframe.vScale, keyframe.vPos, keyframe.vRot, keyframe.isEaseIn, keyframe.isEaseOut ��

    //    timeline.push_back(keyframe);
    //}
}

//// �ִϸ��̼� Ÿ�Ӷ����� �����ϰ� �ִϸ��̼� ���� ����ϴ� �Լ�
//void GenerateAnimationTimeline(std::vector<Keyframe>& timeline, AnimationProperties properties, int numFrames) 
//{
//    // Ű������ ���� �ð� ���� ���
//    float timeStep = (properties.maxTime - properties.minTime) / (numFrames - 1);
//
//    // Ű�������� �����ϰ� �ִϸ��̼� ���� ����Ͽ� Ÿ�Ӷ��ο� �߰�
//    timeline.clear();
//    for (int i = 0; i < numFrames; ++i) {
//        float time = properties.minTime + i * timeStep;
//        // ���⿡�� �ִϸ��̼� ���� ����Ͻʽÿ� (��: ������ �� �Ǵ� Ư�� �Լ��� ����Ͽ�)
//        float value = CalculateAnimationValue(time, properties);
//        Keyframe keyframe = { time, value, /* �ٸ� �Ӽ� �� */ };
//        timeline.push_back(keyframe);
//    }
//}
//
//// �ִϸ��̼� ���� ����ϴ� �Լ� ����
//float CalculateAnimationValue(float time, AnimationProperties properties) {
//    // ��: �ð��� ���� �������� �� ����
//    float valueRange = properties.maxValue - properties.minValue;
//    float timeRange = properties.maxTime - properties.minTime;
//    float normalizedTime = (time - properties.minTime) / timeRange;
//    return properties.minValue + normalizedTime * valueRange;
//}
//
//// ����ڷκ��� �ִϸ��̼� �Ӽ� �� Ű���� ���� �Է� �޴� �Լ�
//void EditAnimationProperties(AnimationProperties& properties, int& numFrames) {
//    ImGui::Text(u8"�ִϸ��̼� �Ӽ� ����");
//    ImGui::InputFloat(u8"�ּ� �ð�", &properties.minTime);
//    ImGui::InputFloat(u8"�ִ� �ð�", &properties.maxTime);
//    ImGui::InputFloat(u8"�ּ� ��", &properties.minValue);
//    ImGui::InputFloat(u8"�ִ� ��", &properties.maxValue);
//    ImGui::InputFloat(u8"�ּ� ũ��", &properties.minSize);
//    ImGui::InputFloat(u8"�ִ� ũ��", &properties.maxSize);
//    ImGui::InputFloat(u8"�ּ� ��ġ", &properties.minPosition);
//    ImGui::InputFloat(u8"�ִ� ��ġ", &properties.maxPosition);
//    ImGui::InputFloat(u8"�ּ� ȸ��", &properties.minRotation);
//    ImGui::InputFloat(u8"�ִ� ȸ��", &properties.maxRotation);
//
//    ImGui::Text(u8"Ű���� �� �Է�");
//    ImGui::InputInt(u8"Ű���� ��", &numFrames);
//
//    if (ImGui::Button(u8"�ִϸ��̼� ����")) {
//        GenerateAnimationTimeline(timeline, properties, numFrames);
//    }
//}

// ���� ���� �Լ� ex1
//ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t)
//{
//    t = ImClamp(t, 0.0f, 1.0f); // t ���� [0, 1] ������ Ŭ�����մϴ�.
//    return ImVec4(ImLerp(a.x, b.x, t), ImLerp(a.y, b.y, t), ImLerp(a.z, b.z, t), ImLerp(a.w, b.w, t));
//}

// ���� ���� �Լ� ex2
//ImVec4 CImguiAnimationTool::ImLerp(const ImVec4& a, const ImVec4& b, float t)
//{
//    t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t); // t ���� [0, 1] ������ Ŭ�����մϴ�.
//    return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
//}

//// �ִϸ��̼� Ÿ�Ӷ��ο� �������� Ű�������� Ŭ������ �� ȣ��Ǵ� �Լ�
//void CImguiAnimationTool::HandleKeyframeClick()
//{
//    // �� �Լ��� ���콺 �̺�Ʈ ó�� ������ �����մϴ�.
//    // ���⼭�� ������ ������ Ŭ�� �� ���õ� Ű�������� ã�� �����ϴ� ������ �߰��մϴ�.
//
//    // ���콺 ��ư Ŭ�� ���� Ȯ��
//    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
//    {
//        // ���콺 Ŭ�� ��ġ�� �����ɴϴ�.
//        ImVec2 mousePos = ImGui::GetMousePos();
//
//        // Ÿ�Ӷ����� ���� ��ġ�� �����ɴϴ�.
//        ImVec2 timelineStartPos = ImGui::GetCursorScreenPos();
//
//        // Ÿ�Ӷ����� �� Ű������ ��ư�� Ŭ�� ���θ� Ȯ���ϰ� �����մϴ�.
//        for (int i = 0; i < timeline.size(); ++i)
//        {
//            ImVec2 keyframeButtonPos = timelineStartPos + ImVec2(i * 30, 0); // Ű������ ��ư ������ 30�Դϴ�.
//
//            // Ŭ���� ��ġ�� �ش� Ű������ ��ư�� ������ ���ԵǴ��� Ȯ���մϴ�.
//            if (mousePos.x >= keyframeButtonPos.x && mousePos.x <= (keyframeButtonPos.x + 20)) // ��ư �ʺ�� 20�Դϴ�.
//            {
//                // Ŭ���� Ű�������� �����մϴ�.
//                selectedKeyframeIndex = i;
//                break; // ���� �� ���� ����
//            }
//        }
//    }
//}

// ���콺 �̺�Ʈ ó�� �Լ�
void CImguiAnimationTool::HandleKeyframeClick()
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        ImVec2 mousePos = ImGui::GetMousePos();

        // Ÿ�Ӷ����� ���� ��ġ�� �� ��ġ�� ����մϴ�.
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 timelineStartPos = windowPos + ImGui::GetCursorScreenPos();
        ImVec2 timelineEndPos = timelineStartPos + ImVec2((_float)timeline[m_iCurType].size() * 30.f, 20.f); // Ű������ ��ư ������ 30, ���̴� 20���� �����մϴ�.

        // ���콺 Ŭ�� ��ġ�� Ÿ�Ӷ��� ���� ���� �ִ��� Ȯ���մϴ�.
       // if (mousePos.x >= timelineStartPos.x && mousePos.x <= timelineEndPos.x &&
       //     mousePos.y >= timelineStartPos.y && mousePos.y <= timelineEndPos.y)
        {
            // �� Ű������ ��ư�� Ŭ�� ���θ� Ȯ���ϰ� �����մϴ�.
            for (int i = 0; i < timeline[m_iCurType].size(); ++i)
            {
                ImVec2 keyframeButtonPos = timelineStartPos + ImVec2((_float)i * 30.f, 0); // Ű������ ��ư ������ 30, ���̴� 20���� �����մϴ�.
                ImVec2 keyframeButtonEndPos = keyframeButtonPos + ImVec2(20, 20); // ��ư �ʺ�� ���̴� 20���� �����մϴ�.

                // Ŭ���� ��ġ�� �ش� Ű������ ��ư�� ������ ���ԵǴ��� Ȯ���մϴ�.
                if (mousePos.x >= keyframeButtonPos.x && mousePos.x <= keyframeButtonEndPos.x &&
                    mousePos.y >= keyframeButtonPos.y && mousePos.y <= keyframeButtonEndPos.y)
                {
                    // Ŭ���� Ű�������� �����մϴ�.
                    closestKeyframeIndex = i;
                    break; // ���� �� ���� ����
                }
            }
        }
    }

    // ���õ� Ű�������� �ε����� ����Ͽ� Ű�������� �����մϴ�.
    if (!timeline[m_iCurType].empty() && closestKeyframeIndex >= 0 && closestKeyframeIndex < timeline[m_iCurType].size())
    {
        KEYFRAME& selectedKeyframe = timeline[m_iCurType][closestKeyframeIndex];

        // ���õ� Ű������ �ִϸ��̼� �Ӽ��� �����ϴ� �Լ� ȣ��
        //DrawSelectedKeyframeEditor(selectedKeyframe);
    }
}

void CImguiAnimationTool::SelectKeyframeValueChange()
{
    /*

    Ű �Է��� ��� �ӽ÷� W, A, S, D, (Q, E), ( [, ] ), ( ;, '), (, . ), (m , /) �� ���� "A"

    */


    // Ű�������� ã�Ƽ� ���� �ִ� ��쿡�� ����
    if (closestKeyframeIndex != -1)
    {
        // �ٸ� Ű�������� �������� ���
        if (closestKeyframeIndex != oldKeyframeIndex)
        {

        }

        KEYFRAME& keyframe = timeline[m_iCurType][closestKeyframeIndex]; // ���� ������ Ű������

        switch (eEditMode)
        {
        case EDITMODE_NONE:
        {

            break;
        }
        case EDITMODE_SCALE: // ũ�� (x, y)
        {   // Ű �Է����� ũ�� ���� ����

            keyframe.type = 0; // Ÿ�� : ũ��

            // "Up" ����Ű : ������ Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W)))
            {
                keyframe.vScale.x += vValueSize;
                keyframe.vScale.y += vValueSize;
                //keyframe.vScale.z += vValueSize;
            }
            // "Down" ����Ű : ������ Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)))
            {
                keyframe.vScale.x -= vValueSize;
                keyframe.vScale.y -= vValueSize;
                //keyframe.vScale.z -= vValueSize;
            }
            // "Left" ����Ű : ������ Minus (ū ��)
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
            {
                keyframe.vScale.x -= vValueSize * 10.f;
                keyframe.vScale.y -= vValueSize * 10.f;
                //keyframe.vScale.z -= vValueSize * 10.f;
            }
            // "Right" ����Ű : ������ Plus(ū ��)
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_D)))
            {
                keyframe.vScale.x += vValueSize * 10.f;
                keyframe.vScale.y += vValueSize * 10.f;
                //keyframe.vScale.z += vValueSize * 10.f;
            }

            //// ũ�� �� Ŭ���� (���ϴ� ������ ����)
            //keyframe.vScale.x = ImClamp(keyframe.vScale.x, minScale.x, maxScale.x);
            //keyframe.vScale.y = ImClamp(keyframe.vScale.y, minScale.y, maxScale.y);
            //keyframe.vScale.z = ImClamp(keyframe.vScale.z, minScale.z, maxScale.z);

            break;
        }
        case EDITMODE_ROTATION: // ȸ�� (x, y, z)
        {   // Ű �Է����� ȸ�� ���� ����

            keyframe.type = 1; // Ÿ�� : ȸ��

            // "Up" ����Ű : xȸ�� Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W)))
            {
                keyframe.vRot.x += vValueSize * fValueCurve;
            }
            // "Down" ����Ű : xȸ�� Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)))
            {
                keyframe.vRot.x -= vValueSize * fValueCurve;
            }
            // "Left" ����Ű : zȸ�� Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
            {
                keyframe.vRot.z -= vValueSize * fValueCurve;
            }
            // "Right" ����Ű : zȸ�� Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_D)))
            {
                keyframe.vRot.z += vValueSize * fValueCurve;
            }


            // ". >" : yȸ�� Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_E)))
            {
                keyframe.vRot.y += vValueSize * fValueCurve;
            }
            // ", <" : yȸ�� Minus ("Q")
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Q)))
            {
                keyframe.vRot.y -= vValueSize * fValueCurve;
            }

            break;
        }
        case EDITMODE_TRANSLATION: // �̵� (x, y)
        {   // Ű �Է����� �̵� ���� ����

            keyframe.type = 2; // Ÿ�� : �̵�

            // "Up" ����Ű : y�̵� Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W)))
            {
                keyframe.vPos.y += vValueSize;
                //keyframe.vPos.z += vValueSize;
            }
            // "Down" ����Ű : y�̵� Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)))
            {
                keyframe.vPos.y -= vValueSize;
                //keyframe.vPos.z -= vValueSize;
            }
            // "Left" ����Ű : x�̵� Minus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
            {
                keyframe.vPos.x -= vValueSize;
                //keyframe.vPos.z -= vValueSize;
            }
            // "Right" ����Ű : x�̵� Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_D)))
            {
                keyframe.vPos.x += vValueSize;
                //keyframe.vPos.z += vValueSize;
            }

            break;
        }
        case EDITMODE_TIME_VALUE: // �ð�
        {   // Ű �Է����� �ð� ���� ����

            // "Up" ����Ű : �ð� Plus "]" ū ��
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Slash)))
            {
                keyframe.time += vValueSize;
                //keyframe.vPos.z += vValueSize;
            }
            // "Down" ����Ű : �ð� Minus "[" ū ��
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_M)))
            {
                keyframe.time -= vValueSize;
                //keyframe.vPos.z -= vValueSize;
            }
            // "Up" ����Ű : �ð� Plus "'" ���� ��
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Period)))
            {
                keyframe.time += vValueSize * 0.1f;
                //keyframe.vPos.z += vValueSize;
            }
            // "Down" ����Ű : �ð� Minus ";" ���� ��
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Comma)))
            {
                keyframe.time -= vValueSize * 0.1f;
                //keyframe.vPos.z -= vValueSize;
            }
            // "Left" ����Ű : ���� Minus ","
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Semicolon)))
            {
                keyframe.value -= vValueSize * 0.1f;
                //keyframe.vPos.z -= vValueSize;
            }
            // "Right" ����Ű : ���� Plus "."
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Apostrophe)))
            {
                keyframe.value += vValueSize * 0.1f;
                //keyframe.vPos.z += vValueSize;
            }

            break;
        }
        case EDITMODE_TEXTURE: // �ؽ�ó
        {   // Ű �Է����� �ؽ�ó�� ����

            // "Up" ����Ű : �ؽ�ó Plus
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightBracket)))
            {
                keyframe.texureframe += (_int)vValueSize;
                //keyframe.vPos.z += vValueSize;
            }
            // "Down" ����Ű : �ؽ�ó Minus
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

        oldKeyframeIndex = closestKeyframeIndex; // ���� Ű�������� ����
    }
}

void CImguiAnimationTool::SelectKeyframeMouseL()
{
    ImGuiIO& io = ImGui::GetIO(); // ImGui�� �ý��� �⺻ ���� ���� (ex : �ð�, ���콺 ��)

    float mouseX = io.MousePos.x; // ���콺 ��ġx
    float mouseY = io.MousePos.y; // ���콺 ��ġy

    if (mouseX >= timelinePos.x && mouseX <= timelinePos.x + timelineSize.x &&
        mouseY >= timelinePos.y && mouseY <= timelinePos.y + timelineSize.y)
    {
        // ���콺 ��Ŭ�� ����
        if (io.MouseDown[0])
        {


            // �ε��� �� �ʱ�ȭ
            closestKeyframeIndex = -1;

            // ���� ����� Ű�������� ã�� �� �ʱ� �Ÿ��� �����ϰ�, ���߿� �� ���� �Ÿ��� �߰��ϸ� ������Ʈ.
            _float closestDistance = FLT_MAX; // ���� ū �ε� �Ҽ��� (�ʱ� �ּ� �Ÿ� �� ����)

            for (int i = 0; i < timeline[m_iCurType].size(); ++i)
            {
                KEYFRAME& keyframe = timeline[m_iCurType][i];

                // Ű�������� ��ġ
                float keyframeX = keyframe.vKeyFramePos.x;
                float keyframeY = keyframe.vKeyFramePos.y;

                // ���콺 ��ǥ�� Ű������ ��ġ ���� �Ÿ� ���
                float distance = std::sqrt((mouseX - keyframeX) * (mouseX - keyframeX) +
                    (mouseY - keyframeY) * (mouseY - keyframeY));

                // Ÿ�� ���� ���� Ű������ ��ġ�� ���
                float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
                float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

                // ���� ����� Ű������ ã��
                if (distance < closestDistance)
                {
                    // �ش� Ű�������� ��ġ �ȿ� ���콺�� �ִ��� �Ǵ�
                    if (mouseX >= xPos - 5.0f && mouseY >= yPos - 5.0f &&
                        mouseX <= xPos + 5.0f && mouseY <= yPos + 5.0f)
                    {
                        closestDistance = distance;
                        closestKeyframeIndex = i; // ���� ������ �ε����� ���콺�� Ŭ���� �ε����� ����
                    }
                }
            }

        }

        // ���콺 �̺�Ʈ���� ���� ���콺 ��ǥ
        float mouseRX = io.MousePos.x; // ���콺 ��ġx
        float mouseRY = io.MousePos.y; // ���콺 ��ġy

        // ���콺 ��Ŭ�� ����
        if (ImGui::IsMouseClicked(1))
        {
            for (int i = 0; i < timeline[m_iCurType].size(); ++i)
            {
                KEYFRAME& keyframe = timeline[m_iCurType][i];

                // Ű�������� ��ġ
                float keyframeX = keyframe.vKeyFramePos.x;
                float keyframeY = keyframe.vKeyFramePos.y;

                // ���콺 ��ǥ�� Ű������ ��ġ ���� �Ÿ� ���
                float distance = std::sqrt((mouseRX - keyframeX) * (mouseRX - keyframeX) +
                    (mouseRY - keyframeY) * (mouseRY - keyframeY));

                // Ÿ�� ���� ���� Ű������ ��ġ�� ���
                float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
                float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;


                if (!isDraggingTimeline) // �巡�׸� ���� ���� ��츸 ���� ����
                {
                    // ������ ���콺 Ŭ������ Ű������ ����
                    if (mouseRX >= xPos - 5.0f && mouseRY >= yPos - 5.0f &&
                        mouseRX <= xPos + 5.0f && mouseRY <= yPos + 5.0f &&
                        ImGui::IsMouseClicked(1))
                    {
                        // ���� �ε����� �ִ� ������ �� ���
                        if (i == timeline[m_iCurType].size() - 1)
                        {
                            timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i); // ���� Ŭ���� �ε��� ����
                            closestKeyframeIndex = i - 1;

                            break; // ������ �Ŀ� ������ ��������.
                        }
                        else // �ִ� ����� �ƴ� ���
                        {
                            // ���� �ε����� 0�̰ų� 0���� ũ�� ������� ���� ���
                            if (i < timeline[m_iCurType].size() - 2 && 0 <= i)
                            {
                                // 0���� �۰ų� ���� ���
                                if (i <= 0)
                                {
                                    timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i); // ���� Ŭ���� �ε��� ����
                                    closestKeyframeIndex = -1;

                                    break; // ������ �Ŀ� ������ ��������.
                                }
                                else
                                {
                                    timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i); // ���� Ŭ���� �ε��� ����
                                    closestKeyframeIndex = i;
                                }
                            }
                        }
                        break; // ������ �Ŀ� ������ ��������.
                    }
                }
            }
        }
    }
}

void CImguiAnimationTool::KeyInput()
{
    // "G" Ű : ���� ���콺 ��ġ�� Ű�������� ����
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_G)))
    {
        // ���ο� Ű�������� �߰��� �� ���� �ð��� ���� ����� Ű�������� ã�´�.
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

        // ���� ���콺 ��ġ
        ImVec2 mousePos = ImGui::GetMousePos();

        // �ִϸ��̼� Ÿ�� ���� ��ġ
        ImVec2 CurtimelinePos = ImGui::GetCursorScreenPos();
        CurtimelinePos.x += 0.f;
        CurtimelinePos.y += 30.f;

        // ���콺 ��ġX�� �ִϸ��̼� Ÿ�� ���� ���� ������� ��ġ�� ��ȯ
        float relativeMouseX = (mousePos.x - CurtimelinePos.x) / timelineSize.x;

        // ���콺 ��ġ�� �ִϸ��̼� Ÿ�� ���� ���� ������� ��ġ�� ��ȯ
        float relativeMouseY = (mousePos.y - CurtimelinePos.y) / timelineSize.y;

        // ���� ���� ����մϴ�. ���� ���, ������� ���콺 ��ġ�� ���� ������ ����� �� ����
        float newValue = 0.7f - ImClamp(relativeMouseY, 0.0f, 1.0f); // ���� �� ������ 0���� 1�� Ŭ����

        // �ð� ���� ����Ͽ� ���ο� Ű�������� ����
        float newTime = relativeMouseX * MaxTime; // MaxTime�� �ִ� �ð� ��

        // ���ο� Ű�������� �����ϰ� ����.
        KEYFRAME newKeyframe = { newTime, newValue, 0, false, false };
        newKeyframe.time = newTime;
        newKeyframe.value = newValue;
        newKeyframe.vKeyFramePos.x = relativeMouseX;
        newKeyframe.vKeyFramePos.y = relativeMouseY;
        timeline[m_iCurType].push_back(newKeyframe);

        // ���ο� Ű�������� �ð��� ���� ����� Ű�������� ã�´�. ( �� ��ȣ �ε����� ���õ� �ϴ� ����)
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

    // "B" Ű : ���� �ð� ��ġ�� Ű�������� ����
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_B)))
    {
        // �ִϸ��̼� Ÿ�� ���� ��ġ
        ImVec2 CurtimelinePos = ImGui::GetCursorScreenPos();
        CurtimelinePos.x += 0.f;
        CurtimelinePos.y += 30.f;

        // ���콺 ��ġX�� �ִϸ��̼� Ÿ�� ���� ���� ������� ��ġ�� ��ȯ
        float relativeMouseX = (CurtimelinePos.x) / timelineSize.x;

        // ���콺 ��ġ�� �ִϸ��̼� Ÿ�� ���� ���� ������� ��ġ�� ��ȯ
        float relativeMouseY = (CurtimelinePos.y) / timelineSize.y;

        // ���ο� Ű�������� �����ϰ� �����մϴ�.
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

    // "R" Ű : ���� �ð�(�ð���)�� 0���� �ʱ�ȭ [ó������ �̵�]
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_R)))
    {
        currentTime = 0.f;
    }

    // "T" Ű : ���� �ð�(�ð� ��)�� ���콺 ��ġ�� ����
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_T)))
    {
        // ���� ���콺 ��ġ
        ImVec2 mousePos = ImGui::GetMousePos();

        // ���콺 ��ġX�� �ִϸ��̼� Ÿ�� ���� ���� ������� ��ġ�� ��ȯ
        float relativeMouseX = (mousePos.x - timelinePos.x) / timelineSize.x;

        // �ð� ���� ����Ͽ� ���ο� Ű�������� ����
        float newTime = relativeMouseX * MaxTime; // MaxTime�� �ִ� �ð� ��

        // �ð��� ���� ���콺 ��ġ�� �°� �����մϴ�.
        currentTime = newTime;
    }

    // "Space" Ű : �ִϸ��̼� ���/����
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
    {
        isPlaying = !isPlaying;
    }

    // "0" Ű : ��� �ӵ� �ʱ�ȭ
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_0)))
    {
        playbackSpeed = 1.f;
    }

    // "-" Ű : ��� �ӵ� -
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Minus)))
    {
        playbackSpeed -= 1.f;
    }

    // "+" Ű : ��� �ӵ� +
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Equal)))
    {
        playbackSpeed += 1.f;
    }

    // " ` ~ " Ű : ���� ��� �ʱ�ȭ(����)
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GraveAccent)))
    {
        eEditMode = EDITMODE_NONE;
    }

    // "F1" Ű : ũ�� ���� ���
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F1)))
    {
        eEditMode = EDITMODE_SCALE;
    }

    // "F2" Ű : ȸ�� ���� ���
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F2)))
    {
        eEditMode = EDITMODE_ROTATION;
    }

    // "F3" Ű : �̵� ���� ���
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F3)))
    {
        eEditMode = EDITMODE_TRANSLATION;
    }

    // "F4" Ű : �ð� �� ���� ���� ���
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F4)))
    {
        eEditMode = EDITMODE_TIME_VALUE;
    }

    // "F5" Ű : Ű������ �ʱ�ȭ
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F5)))
    {
        timeline[m_iCurType].clear();
    }

    // "F6" Ű : �Ӽ� �� ���� ������ ���� �ʱ�ȭ
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F6)))
    {
        vValueSize = 1.f;
    }

    // "F7" Ű : �Ӽ� �� ���� ������ ���� Minus
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F7)))
    {
        vValueSize -= 1.f;
    }

    // "F8" Ű : �Ӽ� �� ���� ������ ���� Plus
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F8)))
    {
        vValueSize += 1.f;
    }

    // "P" Ű : ���� ��� �ʱ�ȭ(����)
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_P)))
    {
        eEditMode = EDITMODE_TEXTURE;
    }

    // "Z" Ű : ���� Ű������ ����
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
    {
        // ���� Ű�������� 0���� ���� ���� ��츸
        if (closestKeyframeIndex > 0)
        {
            closestKeyframeIndex -= (_int)1.f;
        }

        if (!timeline[m_iCurType].empty()) // �ִϸ��̼��� �ְ�
        {
            if (closestKeyframeIndex <= -1.f) // ���� �ƹ��͵� �������� �ʾ��� ���
            {
                closestKeyframeIndex = 0.f; // 0��° ����
            }
        }
    }

    // "X" Ű : ���� Ű������ ����
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
    {
        // ���� Ű�������� �ִ� Ű�������� ���� �ʾ��� ����
        if (closestKeyframeIndex < timeline[m_iCurType].size() - 1)
        {
            closestKeyframeIndex += 1.f;
        }

        if (!timeline[m_iCurType].empty()) // �ִϸ��̼��� �ְ�
        {
            if (closestKeyframeIndex <= -1.f) // ���� �ƹ��͵� �������� �ʾ��� ���
            {
                closestKeyframeIndex = 0.f; // 0��° ����
            }
        }
    }
}

void CImguiAnimationTool::KeyframeList()
{
    // Ű������ ��� â
    ImGui::Begin(u8"Ű������ ����");

    for (int i = 0; i < timeline[m_iCurType].size(); ++i) {
        KEYFRAME& keyframe = timeline[m_iCurType][i];
        char keyframeName[64];
        snprintf(keyframeName, sizeof(keyframeName), u8"Ű������ %d", i);

        // Ű������ ������ ������� ǥ��
        if (ImGui::TreeNode(keyframeName)) {
            ImGui::Text(u8"��: %.2f", keyframe.value);
            ImGui::Text(u8"�ð�: %.2f", keyframe.time);
            ImGui::Text(u8"ũ��: (%.2f, %.2f, %.2f)", keyframe.vScale.x, keyframe.vScale.y, keyframe.vScale.z);
            ImGui::Text(u8"ȸ��:(%.2f, %.2f, %.2f)", keyframe.vRot.x, keyframe.vRot.y, keyframe.vRot.z);
            ImGui::Text(u8"�̵�: (%.2f, %.2f, %.2f)", keyframe.vPos.x, keyframe.vPos.y, keyframe.vPos.z);
            ImGui::Checkbox(u8"��¡ In", &keyframe.isEaseIn);
            ImGui::Checkbox(u8"��¡ Out", &keyframe.isEaseOut);

            // Ű������ ���� ��ư
            if (ImGui::Button(u8"����")) {
                timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i);
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void CImguiAnimationTool::KeyframeChangeWindow()
{
    // Ű���� ����� �����ִ� ImGui �����츦 �����.
    if (ImGui::Begin(u8"Ű������ ����"))
    {
        // Ű���� ����� ������ ���� ǥ��.
        for (int i = 0; i < timeline[m_iCurType].size(); ++i)
        {
            KEYFRAME& keyframe = timeline[m_iCurType][i];

            // �� Ű������ ��ư �Ǵ� �ٸ� ImGui �������� ǥ��.
            // ���⿡���� ��ư�� ���.
            if (ImGui::Button((u8"Ű������" + std::to_string(i)).c_str()))
            {
                // Ű������ Ŭ���Ǹ� �ش� �ε����� ���õ� Ű���� �ε����� ����.
                closestKeyframeIndex = i;
            }
        }
    }
    ImGui::End();
}

void CImguiAnimationTool::KeyframeAniV()
{
    // �̸����� Ű������ ����
    if (!timeline[m_iCurType].empty())
    {
        for (int i = 0; i < timeline[m_iCurType].size() - 1; ++i)
        {
            KEYFRAME& prevKeyframe = timeline[m_iCurType][i];
            KEYFRAME& nextKeyframe = timeline[m_iCurType][i + 1];

            if (currentTime >= prevKeyframe.time && currentTime <= nextKeyframe.time) {
                float alpha = (currentTime - prevKeyframe.time) / (nextKeyframe.time - prevKeyframe.time);

                // ������ �ִϸ��̼� �̸����� �� ���
                float interpolatedValue = Lerp(prevKeyframe.value, nextKeyframe.value, alpha);

                // �� ���� ����Ͽ� �̸����� �ִϸ��̼� ������
                RenderPreviewAnimation(interpolatedValue);

                break;
            }
        }
    }
}

void CImguiAnimationTool::KeyframeRender_ValueChange()
{

    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // �׸���

    // Ű������ ������ �� ����
    for (int i = 0; i < timeline[m_iCurType].size(); ++i)
    {
        KEYFRAME& keyframe = timeline[m_iCurType][i];

        // Ÿ�� ���� ���� ��ġ�� ���
        float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        // Ű������ ��ġ ���� 0(�ʱⰪ)�� ���
        if (keyframe.vKeyFramePos.x <= 1 &&
            keyframe.vKeyFramePos.y <= 1)
        {
            // ���� ���õ� Ű������ ��ġ�� �־��ش�.
            keyframe.vKeyFramePos.x = xPos;
            keyframe.vKeyFramePos.y = yPos;
        }

        // ���� �ð��� �ش��ϴ� Ű������ ǥ�� (������ ��)
        if (keyframe.time >= currentTime - 0.1 &&
            keyframe.time <= currentTime + 0.1) {
            draw_list->AddCircleFilled(
                ImVec2(xPos, yPos),
                5.0f,
                IM_COL32(255, 0, 0, 255) // ���������� ǥ��
            );
        }
        else {
            // �ٸ� �ð��� Ű������ ǥ�� (����� ��)
            draw_list->AddCircleFilled(
                ImVec2(xPos, yPos),
                5.0f,
                IM_COL32(255, 255, 0, 255) // ��������� ǥ��
            );
        }

        // ���콺�� Ű������ ���� �ִ� ��쿡�� ���� ǥ�� (���콺 ����)
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f)))
        {

            // Ű������ ���� ǥ��
            ImGui::SetCursorScreenPos(ImVec2(xPos + 10.0f, yPos - 20.0f));
            ImGui::BeginTooltip();
            ImGui::Text(u8"Ű������ %d", i);
            ImGui::Separator();
            ImGui::Text(u8"�ð�: %.2f", keyframe.time);
            ImGui::Text(u8"��: %.2f", keyframe.value);
            ImGui::Text(u8"ũ��: (%.2f, %.2f, %.2f)", keyframe.vScale.x, keyframe.vScale.y, keyframe.vScale.z);
            ImGui::Text(u8"ȸ��:(%.2f, %.2f, %.2f)", keyframe.vRot.x, keyframe.vRot.y, keyframe.vRot.z);
            ImGui::Text(u8"�̵�: (%.2f, %.2f, %.2f)", keyframe.vPos.x, keyframe.vPos.y, keyframe.vPos.z);

            if (keyframe.isEaseIn)
                ImGui::Text(u8"��¡ In: ����");
            else
                ImGui::Text(u8"��¡ In: ����");
            if (keyframe.isEaseOut)
                ImGui::Text(u8"��¡ Out: ����");
            else
                ImGui::Text(u8"��¡ Out: ����");
            ImGui::EndTooltip();
        }

        // ���콺 ������ �� �巡�� ����
        if (ImGui::IsMouseReleased(0))
        {
            isDraggingTimeline = false;
        }

        if (!isDraggingTimeline) // �巡�׸� ���� ���� ��츸 ���� ����
        {
            // ������ ���콺 Ŭ������ Ű������ ����
            if (ImGui::IsMouseHoveringRect(
                ImVec2(xPos - 5.0f, yPos - 5.0f),
                ImVec2(xPos + 5.0f, yPos + 5.0f))
                && ImGui::IsMouseClicked(1))
            {
                timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i);
                break; // ������ �Ŀ� ������ ���������ϴ�.
            }
        }

    }
}

void CImguiAnimationTool::KeyframeAutomaticGeneration()
{
    ImGui::Begin(u8"Ű������ �ڵ� ����");

    ImGui::Text(u8"Ű������ �ڵ� ���� ����");

    // �Է°��� �޴� UI ��� ����
    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"�ð� ����");
    ImGui::InputFloat2(u8"����/�� �ð� �Է�", _v2Time);
    ImGui::Separator();

    //ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"���� ����");
    ImGui::InputFloat2(u8"����/�� ���� �Է�", _v2Value);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"ũ�� ����");
    ImGui::InputFloat3(u8"���� ������ ũ�� �Է�", minScale);

    //ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::InputFloat3(u8"�� ������ ũ�� �Է�", maxScale);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"ȸ�� ����");
    ImGui::InputFloat3(u8"���� ������ ȸ�� �Է�", minRot);

    //ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::InputFloat3(u8"�� ������ ȸ�� �Է�", maxRot);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"�̵� ����");
    ImGui::InputFloat3(u8"���� ������ �̵� �Է�", minPos);

    //ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::InputFloat3(u8"�� ������ �̵� �Է�", maxPos);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"�ؽ�ó ����");
    ImGui::InputFloat(u8"���� ������ �ؽ�ó �Է�", &fMin_Texture);

    //ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::InputFloat(u8"�� ������ �ؽ�ó �Է�", &fMax_Texture);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"���ϴ� Ű������ �� ����");
    ImGui::InputFloat(u8"Ű������ �� �Է�", &numKeyframes);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"���� �ؽ�ó ����");
    ImGui::Checkbox(u8"���� �ؽ�ó", &m_bIndividualTexture);
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0, 5)); // ����
    ImGui::SeparatorText(u8"���� ���� ����");
    ImGui::Checkbox(u8"���� ����", &m_bRendomValue);
    ImGui::Separator();

    if (ImGui::Button(u8"�ڵ� ����")) {
        // ��ư�� Ŭ���Ǹ� timeline ���͸� ���� CreateKeyframesWithLinearInterpolation �Լ� ȣ��
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
        // ��ư�� Ŭ���Ǹ� timeline ���͸� ���� CreateKeyframesWithLinearInterpolation �Լ� ȣ��
        timeline[m_iCurType].clear();
    }

    //ImGui::End(); // �̰� ���� ����
}

void CImguiAnimationTool::KeyframeDeleteMouseR()
{
    // Ű������ ������ �� ����
    for (int i = 0; i < timeline[m_iCurType].size(); ++i)
    {
        KEYFRAME& keyframe = timeline[m_iCurType][i];

        // Ÿ�� ���� ���� ��ġ�� ���
        float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        if (!isDraggingTimeline) // �巡�׸� ���� ���� ��츸 ���� ����
        {
            // ������ ���콺 Ŭ������ Ű������ ����
            if (ImGui::IsMouseHoveringRect(
                ImVec2(xPos - 5.0f, yPos - 5.0f),
                ImVec2(xPos + 5.0f, yPos + 5.0f))
                && ImGui::IsMouseClicked(1))
            {
                timeline[m_iCurType].erase(timeline[m_iCurType].begin() + i);
                break; // ������ �Ŀ� ������ ���������ϴ�.
            }
        }
    }
}
