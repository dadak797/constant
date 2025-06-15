#pragma once

#include "macro/singleton_macro.h"

// ImGui
#include <imgui.h>

// Standard library
#include <cstdint>

class TreeNode;

class SceneTree {
  DECLARE_SINGLETON(SceneTree)

 public:
  void Render(bool* openWindow = nullptr);

  static int32_t GetSelectedMeshId() { return s_SelectedMeshId; }

 private:
  static int32_t s_DeleteMeshId;
  static int32_t s_SelectedMeshId;

  // Mesh table rendering
  void renderMeshTable(ImGuiTableFlags tableFlags);
  // static void renderMeshTree(TreeNode* node);
};