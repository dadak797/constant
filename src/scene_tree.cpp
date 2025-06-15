#include "scene_tree.h"

#include "lcrs_tree.h"
#include "font_manager.h"
#include "custom_ui.h"
#include "config/size_config.h"

// ImGui
#include <imgui.h>

int32_t SceneTree::s_DeleteMeshId = -1;
int32_t SceneTree::s_SelectedMeshId = -1;

SceneTree::SceneTree() {}

SceneTree::~SceneTree() {}

void SceneTree::Render(bool* openWindow) {
  ImGui::Begin(ICON_FA6_FOLDER_TREE "  Scene Tree", openWindow,
               ImGuiWindowFlags_NoScrollbar);

  static ImGuiTableFlags tableFlags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize |
      ImGuiTableFlags_Reorderable |  // Reorder columns by dragging
      ImGuiTableFlags_PadOuterX | ImGuiTableFlags_BordersOuter;

  renderMeshTable(tableFlags);

  ImGui::End();
}

void SceneTree::renderMeshTable(ImGuiTableFlags tableFlags) {
  // Constants for table header
  constexpr int32_t TABLE_COLUMN_COUNT = 4;
  static const char* tableHeaders[TABLE_COLUMN_COUNT] = {
      "Name", "Id", ICON_FA6_EYE, ICON_FA6_TRASH_CAN};
  static ImGuiTableColumnFlags tableColumnFlags[TABLE_COLUMN_COUNT] = {
      ImGuiTableColumnFlags_WidthStretch, ImGuiTableColumnFlags_WidthFixed,
      ImGuiTableColumnFlags_WidthFixed, ImGuiTableColumnFlags_WidthFixed};
  static float tableColumnSizes[TABLE_COLUMN_COUNT] = {30.0f, 3.0f, 2.5f, 2.5f};

  bool isOpen =
      ImGui::CollapsingHeader("Scene Tree", ImGuiTreeNodeFlags_DefaultOpen);
  if (isOpen &&
      ImGui::BeginTable("SceneTree_Table", TABLE_COLUMN_COUNT, tableFlags)) {
    // Column headers
    for (int iCol = 0; iCol < TABLE_COLUMN_COUNT; iCol++) {
      ImGui::TableSetupColumn(tableHeaders[iCol], tableColumnFlags[iCol],
                              SizeConfig::TextBaseWidth() * tableColumnSizes[iCol]);
    }

    // Render the headers with the custom style
    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
    for (int iCol = 0; iCol < TABLE_COLUMN_COUNT; iCol++) {
      ImGui::TableSetColumnIndex(iCol);
      ImGui::PushID(iCol);
      ImGui::TableHeader(tableHeaders[iCol]);
      if (iCol == 2) {
        CustomUI::AddTooltip("Show/Hide Mesh");
      } else if (iCol == 3) {
        CustomUI::AddTooltip("Delete Mesh", "Double-click to delete the mesh");
      }
      ImGui::PopID();
    }

    // MeshManager& meshManager = MeshManager::Instance();
    // TreeNode* rootNode = meshManager.GetMeshTree()->GetRootMutable();
    // renderMeshTree(rootNode);

    ImGui::EndTable();
  }

  // TODO: Handle deletion of meshes
}