#include "ImguiWin_ProtoTool.h"

#include "Export_System.h"
#include "Export_Utility.h"


#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <ImguiMgr.h>
#include <imgui_internal.h>
#include <IconsFontAwesome5.h>

#include <codecvt>
#include <locale>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

CImguiWin_ProtoTool::CImguiWin_ProtoTool()
{
}

CImguiWin_ProtoTool::~CImguiWin_ProtoTool()
{
}

CImguiWin_ProtoTool* CImguiWin_ProtoTool::Create()
{
    ThisClass* pInstance = new ThisClass;

    if (FAILED(pInstance->Ready_ImguiWin()))
    {
        Safe_Release(pInstance);

        MSG_BOX("CImguiWin_TextureTool Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CImguiWin_ProtoTool::Free()
{
    SUPER::Free();

    for (auto iter = m_mapProtoData.begin(); iter != m_mapProtoData.end();)
    {
        if (iter != m_mapProtoData.end())
        {
            Safe_Delete((*iter).second);
            iter = m_mapProtoData.erase(iter);
        }
        else
            ++iter;
    }
}

HRESULT CImguiWin_ProtoTool::Ready_ImguiWin()
{

	return S_OK;
}

_int CImguiWin_ProtoTool::Update_ImguiWin(const _float& fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    SUPER::Update_ImguiWin(fTimeDelta);

    ImGuiWindowFlags iMain_Flags = ImGuiWindowFlags_NoMove;
    ImGuiDockNodeFlags iDockSpace_Flags = ImGuiDockNodeFlags_NoDockingInCentralNode;

    ImGui::Begin("Center", NULL, iMain_Flags);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("DockSpace_MapTool");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), iDockSpace_Flags);

        if (m_bFirstLoop)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, iDockSpace_Flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
            ImVec2 vDockSize = ImGui::GetWindowSize();

            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 250.f / vDockSize.x, NULL, &dockspace_id);
            ImGuiID dock_property_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.8f, NULL, &dock_right_id);

            ImGui::DockBuilderDockWindow(u8"뷰어", dockspace_id);
            ImGui::DockBuilderDockWindow(u8"컴포넌트", dock_right_id);
            ImGui::DockBuilderDockWindow(u8"속성", dock_property_id);


            ImGuiID dock_browser_scene = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 200.f / vDockSize.y, NULL, &dockspace_id);

            ImGui::DockBuilderDockWindow(u8"프로토", dock_browser_scene);

            ImGui::DockBuilderFinish(dockspace_id);

            m_bFirstLoop = false;

            
            // 프로토타입 자동 로드
            if (!m_bLoadProto_Init)
            {
                Load_Protos();
                m_bLoadProto_Init = !m_bLoadProto_Init;
            }
        }
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

    Layout_ObjectBrowser(iMain_Flags);
    Layout_Component(iMain_Flags);
    Layout_Property(iMain_Flags);
    Layout_Viewer(iMain_Flags);


    ImGui::End();

	return 0;
}

void CImguiWin_ProtoTool::Layout_ObjectBrowser(const ImGuiWindowFlags& iMain_Flags)
{
    // 리소스




    if (ImGui::Begin(u8"프로토", NULL, iMain_Flags))
    {
        _bool bAdd_Proto = false;
        if (ImGui::Button(u8"추가하기"))
        {
            bAdd_Proto = true;
        }

        ImGui::SameLine();
        char strAdd_Proto[30] = {};
        m_strAdd_Proto.reserve(30);
        strcpy_s(strAdd_Proto, m_strAdd_Proto.c_str());
        if (ImGui::InputTextEx(u8"##InputProtoName", u8"Proto Name",
            strAdd_Proto, IM_ARRAYSIZE(strAdd_Proto),
            ImVec2(200, 0), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            bAdd_Proto = true;
        }

        m_strAdd_Proto = strAdd_Proto;

        if (bAdd_Proto)
        {
            if (!m_strAdd_Proto.empty())
                m_mapProtoData.emplace(m_strAdd_Proto, new FProtoData());
            m_strAdd_Proto.clear();
        }


        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(100.f, 0.f));

        ImGui::SameLine();
        if (ImGui::Button(u8"프로토타입 전체 저장하기"))
        {
            Save_Protos();
        }

        ImGui::SameLine();
        if (ImGui::Button(u8"프로토타입 전체 로드하기"))
        {
            Load_Protos();
        }


        //----------------------------------------------------------------------------------
        ImGui::Separator();

        // 프로토 목록
        for (auto iter = m_mapProtoData.begin(); iter != m_mapProtoData.end(); ++iter)
        {
            _bool bSelected = (m_strCur_Proto == (*iter).first);

            // 존재하는 프로토 추가하기
            if (ImGui::Selectable((*iter).first.c_str(), 
                bSelected, ImGuiSelectableFlags_AllowDoubleClick))
            {
                m_eSelected_Type = ESELECTED_TYPE_PROTO;

                m_strCur_Proto = (*iter).first;
            }
        }

    }   ImGui::End();
}

void CImguiWin_ProtoTool::Layout_Component(const ImGuiWindowFlags& iMain_Flags)
{
    // 리소스
    if (ImGui::Begin(u8"컴포넌트", NULL, iMain_Flags))
    {
        // 이거 안씀
        ImGui::Text(u8"이거 안씀");

    }   ImGui::End();
}

void CImguiWin_ProtoTool::Layout_Property(const ImGuiWindowFlags& iMain_Flags)
{
    // 리소스
    if (ImGui::Begin(u8"속성", NULL, iMain_Flags))
    {
        // 속성 탭에서 공통적으로 쓰이는 속성을 설정해줄 수 있다.
        // 저장하면 Proto 폴더에 데이터를 저장한다.
        // 항상 이름을 기준으로 저장한다.
        // 나중에 씬에서 해당 씬을 로드할 때 오브젝트 정보와 이 오브젝트의 프로토 타입을 가져와서 객체를 생성하게 된다.
        if (!m_strCur_Proto.empty())
        {
            // Update
            Set_Button_NonActiveColor();
            ImGui::Button(u8"Transform");
            Set_Button_ReturnColor();

            FProtoData* refObject = m_mapProtoData[m_strCur_Proto];
            _bool bIsEdited = false;    // 에딧되었을 때 변화 이벤트


            // 오브젝트 타입
            if (ImGui::CollapsingHeader(u8"오브젝트 타입"))
            {
                if (ImGui::BeginCombo(u8"##ComboObjectType", m_vecObject_Type[refObject->eID].c_str()))
                {
                    for (size_t i = 0; i < m_vecObject_Type.size(); i++)
                    {
                        if (ImGui::Selectable(m_vecObject_Type[i].c_str(),
                            static_cast<EGO_CLASS>(i) == refObject->eID))
                        {
                            refObject->eID = static_cast<EGO_CLASS>(i);

                            switch (refObject->eID)
                            {
                            case Engine::ECLASS_NONE:
                                break;
                            case Engine::ECLASS_PLAYER:
                                break;
                            case Engine::ECLASS_BROWN:
                                break;
                            case Engine::ECLASS_GREY:
                                break;
                            case Engine::ECLASS_BOSS:
                                break;
                            case Engine::ECLASS_FOOD:
                                break;
                            case Engine::ECLASS_WEAPON:
                                break;
                            case Engine::ECLASS_THROW:
                                break;
                            case Engine::ECLASS_INTERACTION:
                                break;
                            case Engine::ECLASS_BUILDING:
                            {
                                /*FProtoCubeStructure* tProtoCube = new FProtoCubeStructure;
                                *tProtoCube = *refObject;
                                Safe_Delete(m_mapProtoData[m_strCur_Proto]);
                                m_mapProtoData[m_strCur_Proto] = tProtoCube;
                                refObject = tProtoCube;*/
                                break;
                            }
                            default:
                                break;
                            }
                        }
                    }
                    
                    ImGui::EndCombo();
                }
            }
            
            // 위치
            if (ImGui::CollapsingHeader(u8"이동"))
            {
                // X
                Set_Button_NonActiveColor();
                ImGui::Button(u8"X", ImVec2(60, 0));
                Set_Button_ReturnColor();

                // Y
                ImGui::SameLine();
                Set_Button_NonActiveColor();
                ImGui::Button(u8"Y", ImVec2(60, 0));
                Set_Button_ReturnColor();

                // Z
                ImGui::SameLine();
                Set_Button_NonActiveColor();
                ImGui::Button(u8"Y", ImVec2(60, 0));
                Set_Button_ReturnColor();


                ImGui::PushItemWidth((60.f + 6.f) * 3.f);
                if (ImGui::InputFloat3("##Translate",
                    refObject->vPos,
                    "%.3f",
                    ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    Clamp_Vec3Translate(refObject->vPos, 10000.f);
                    bIsEdited = true;
                }
                if (ImGui::SliderFloat3("##TranslateSlider",
                    refObject->vPos,
                    -10000.f, 10000.f))
                {
                    bIsEdited = true;
                }
                ImGui::PopItemWidth();
            }

            // 회전
            if (ImGui::CollapsingHeader(u8"회전"))
            {
                // X
                Set_Button_NonActiveColor();
                ImGui::Button(u8"X", ImVec2(60, 0));
                Set_Button_ReturnColor();

                // Y
                ImGui::SameLine();
                Set_Button_NonActiveColor();
                ImGui::Button(u8"Y", ImVec2(60, 0));
                Set_Button_ReturnColor();

                // Z
                ImGui::SameLine();
                Set_Button_NonActiveColor();
                ImGui::Button(u8"Y", ImVec2(60, 0));
                Set_Button_ReturnColor();

                ImGui::PushItemWidth((60.f + 6.f) * 3.f);
                if (ImGui::InputFloat3("##Rotate",
                    refObject->vRot,
                    "%.3f",
                    ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    Clamp_Vec3Rot(refObject->vRot, 360.f);
                    bIsEdited = true;
                }
                if (ImGui::SliderFloat3("##RoateSlider",
                    refObject->vRot,
                    -360.f, 360.f))
                {
                    bIsEdited = true;
                }
                ImGui::PopItemWidth();
            }

            // 크기
            if (ImGui::CollapsingHeader(u8"크기"))
            {
                // X
                Set_Button_NonActiveColor();
                ImGui::Button(u8"X", ImVec2(60, 0));
                Set_Button_ReturnColor();

                // Y
                ImGui::SameLine();
                Set_Button_NonActiveColor();
                ImGui::Button(u8"Y", ImVec2(60, 0));
                Set_Button_ReturnColor();

                // Z
                ImGui::SameLine();
                Set_Button_NonActiveColor();
                ImGui::Button(u8"Y", ImVec2(60, 0));
                Set_Button_ReturnColor();

                ImGui::PushItemWidth((60.f + 6.f) * 3.f);
                if (ImGui::InputFloat3("##Scale",
                    refObject->vScale,
                    "%.3f",
                    ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    Clamp_Vec3Scale(refObject->vScale, 1000.f);
                    bIsEdited = true;
                }
                if (ImGui::SliderFloat3("##ScaleSlider",
                    refObject->vScale,
                    -1000.f, 1000.f))
                {
                    bIsEdited = true;
                }
                ImGui::PopItemWidth();
            }

            if (ImGui::CollapsingHeader(u8"텍스처세팅"))
            {
                ImGui::Text(u8"그룹");
                char strEdit_GroupKey[30] = {};
                refObject->strGroupKey.reserve(30);
                strcpy_s(strEdit_GroupKey, refObject->strGroupKey.c_str());
                if (ImGui::InputTextEx(u8"##TextureGroupName", u8"",
                    strEdit_GroupKey, IM_ARRAYSIZE(strEdit_GroupKey),
                    ImVec2(200, 0), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    refObject->strGroupKey = strEdit_GroupKey;
                }

                ImGui::Text(u8"텍스처");
                char strEdit_TextureKey[30] = {};
                refObject->strTextureKey.reserve(30);
                strcpy_s(strEdit_TextureKey, refObject->strTextureKey.c_str());
                if (ImGui::InputTextEx(u8"##TexutureName", u8"",
                    strEdit_TextureKey, IM_ARRAYSIZE(strEdit_TextureKey),
                    ImVec2(200, 0), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    refObject->strTextureKey = strEdit_TextureKey;
                }
            }

            if (ImGui::CollapsingHeader(u8"사용자 정의 문자열"))
            {
                if (ImGui::Button(u8"문자열 추가"))
                {
                    refObject->vecUserString.push_back(string());
                    refObject->vecUserString.back().reserve(30);
                }
                for (size_t i = 0; i < refObject->vecUserString.size(); i++)
                {
                    char strEdit_UserString[30] = {};
                    refObject->vecUserString[i].reserve(30);
                    strcpy_s(strEdit_UserString, refObject->vecUserString[i].c_str());
                    if (ImGui::InputTextEx(u8"##UserString", u8"",
                        strEdit_UserString, IM_ARRAYSIZE(strEdit_UserString),
                        ImVec2(200, 0), ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        refObject->vecUserString[i] = strEdit_UserString;
                    }
                }
            }
        }

    }   ImGui::End();
}

void CImguiWin_ProtoTool::Layout_Viewer(const ImGuiWindowFlags& iMain_Flags)
{
    // 리소스
    if (ImGui::Begin(u8"뷰어", NULL, iMain_Flags | ImGuiWindowFlags_NoNavFocus))
    {
        ImGui::Text(u8"이거 안씀");

        CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 clipSize = ImVec2(contentSize.x / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferWidth,
            contentSize.y / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferHeight);

        ImGui::Image((void*)pImguiMgr->Get_VecRenderTargetTex(0), contentSize,
            ImVec2((1.f - clipSize.x) * 0.5f, (1.f - clipSize.y) * 0.5f),
            ImVec2(clipSize.x + (1.f - clipSize.x) * 0.5f, clipSize.y + (1.f - clipSize.y) * 0.5f));


    }   ImGui::End();
}

void CImguiWin_ProtoTool::Save_Protos()
{
    Document doc;
    StringBuffer buffer;

    // 씬 목록은 정해진 폴더에 파일로 저장한다.
    for (auto iter = m_mapProtoData.begin(); iter != m_mapProtoData.end(); ++iter)
    {
        // 직렬화
        FSerialize_Proto tProto;
        tProto.tHeader.eType = ESERIALIZE_PROTO;
        tProto.tHeader.strName = (*iter).first;
        tProto.eID = (*iter).second->eID;
        tProto.strClassName = (*iter).second->strClassName;
        tProto.strGroupKey = (*iter).second->strGroupKey;
        tProto.strTextureKey = (*iter).second->strTextureKey;
        tProto.vPos = (*iter).second->vPos;
        tProto.vRot = (*iter).second->vRot;
        tProto.vScale = (*iter).second->vScale;
        tProto.vTexPos = (*iter).second->vTexPos;
        tProto.vTexRot = (*iter).second->vTexRot;
        tProto.vTexScale = (*iter).second->vTexScale;
        tProto.vColPos = (*iter).second->vColPos;
        tProto.vColRot = (*iter).second->vColRot;
        tProto.vColScale = (*iter).second->vColScale;
        tProto.vecUserString = (*iter).second->vecUserString;

        // 프로토 저장
        Export_Proto(tProto);
    }
}

void CImguiWin_ProtoTool::Export_Proto(const FSerialize_Proto& tProto)
{
    // 생성한 JSON 문서를 가지고 writer에 적용
    Document doc;
    StringBuffer buffer;

    tProto.Parse_RapidJSON(doc, buffer, ESERIALIZE_PROCESS_IMMEDIATE, true);

    string strJson = buffer.GetString();
    ofstream outputFile(g_strProtoPath + tProto.tHeader.strName + g_strProtoExt);
    if (outputFile.is_open())
    {
        outputFile << strJson;
        outputFile.close();
        cout << "파일 저장함\n";
    }
    else
    {
        cerr << "파일을 저장할 수 없소!\n";
    }
}

void CImguiWin_ProtoTool::Load_Protos()
{
    // 폴더 순회로 데이터를 얻어냄
    _finddata_t fd;
    intptr_t handle;
    if ((handle = _findfirst((g_strProtoPath + "*" + g_strProtoExt).c_str(), &fd)) == -1L)
    {
        cout << "폴더에 파일 없음!!" << endl;
        m_mapProtoData.clear();
    }
    else
    {
        m_mapProtoData.clear();
        m_strCur_Proto = "";

        // 목록 로드
        do
        {
            string strName = fd.name;
            size_t extPos = strName.find_last_of('.');
            if (extPos == string::npos)
                break;

            // 확장자 체크 후 파일 로드
            if (strName.substr(extPos) == g_strProtoExt)
            {
                FSerialize_Proto tProtoSerial;
                FProtoData* tProtoData = new FProtoData();

                Import_Proto(strName, tProtoSerial, *tProtoData);

                m_mapProtoData.emplace(strName.substr((size_t)0, extPos), tProtoData);
            }
            
        } while (_findnext(handle, &fd) == S_OK);
    }

    _findclose(handle);
}

void CImguiWin_ProtoTool::Import_Proto(const string& strName, FSerialize_Proto& tProtoSerial, FProtoData& tProtoData)
{
    string strJson;
    ifstream inputFile(g_strProtoPath + strName);
    if (inputFile.is_open())
    {
        // 문자열 쉽게 읽어오는 반복자
        strJson = string(istreambuf_iterator<char>(inputFile), istreambuf_iterator<char>());
        inputFile.close();
        cout << "\n파일 불러옴!\n";

        // 파싱 성공시 툴전용 Data에 전달
        if (tProtoSerial.Receive_ByRapidJSON(strJson))
        {
            tProtoData.eID = tProtoSerial.eID;
            tProtoData.strClassName = tProtoSerial.strClassName;
            tProtoData.strGroupKey = tProtoSerial.strGroupKey;
            tProtoData.strTextureKey = tProtoSerial.strTextureKey;
            tProtoData.vPos = tProtoSerial.vPos;
            tProtoData.vRot = tProtoSerial.vRot;
            tProtoData.vScale = tProtoSerial.vScale;
            tProtoData.vTexPos = tProtoSerial.vTexPos;
            tProtoData.vTexRot = tProtoSerial.vTexRot;
            tProtoData.vTexScale = tProtoSerial.vTexScale;
            tProtoData.vColPos = tProtoSerial.vColPos;
            tProtoData.vColRot = tProtoSerial.vColRot;
            tProtoData.vColScale = tProtoSerial.vColScale;
            tProtoData.vecUserString = tProtoSerial.vecUserString;
        }
    }
    else
    {
        cerr << "파일을 불러들일 수 없소!\n";
    }
}

void CImguiWin_ProtoTool::Set_Button_ActiveColor()
{
    m_pStyle = ImGui::GetStyle();
    ImGuiStyle& style = ImGui::GetStyle();
    m_pStyle.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_Button];
    m_pStyle.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_ButtonHovered];
    m_pStyle.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_ButtonActive];

    style.Colors[ImGuiCol_Button] = ImVec4(0.12f, 0.56f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.70f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.40f, 0.90f, 1.00f);
}

void CImguiWin_ProtoTool::Set_Button_NonActiveColor()
{
    m_pStyle = ImGui::GetStyle();
    ImGuiStyle& style = ImGui::GetStyle();
    m_pStyle.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_Button];
    m_pStyle.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_ButtonHovered];
    m_pStyle.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_ButtonActive];

    style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_Button];
    style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_Button];
    style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_Button];
}

void CImguiWin_ProtoTool::Set_Button_ReturnColor()
{
    // 버튼 활성화 색상 복원
    ImGui::GetStyle().Colors[ImGuiCol_Button] = m_pStyle.Colors[ImGuiCol_Button];
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = m_pStyle.Colors[ImGuiCol_ButtonHovered];
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = m_pStyle.Colors[ImGuiCol_ButtonActive];
}
