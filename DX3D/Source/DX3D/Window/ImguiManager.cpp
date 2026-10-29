#include <DX3D/Core/WinSwitches.hpp>
#include <DX3D/Window/ImguiManager.hpp>
#include <DX3D/Window/ImGUI/imgui.h>

ImguiManager::ImguiManager() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
}

ImguiManager::~ImguiManager() {
    ImGui::DestroyContext();
}
