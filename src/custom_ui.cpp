#include "custom_ui.h"

void CustomUI::AddTooltip(const char* label, const char* desc) {
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(label);
    if (desc) {
      ImGui::Separator();
      ImGui::TextDisabled("%s", desc);
    }
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}