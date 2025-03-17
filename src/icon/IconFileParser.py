# This code is to generate C++ code for displaying icons and their names using ImGui.

fileNames = [('IconsFontAwesome6.h', 'ICON_FA'), ('IconsFontAwesome6Brands.h', 'ICON_FA')]

def extract_icon_macros(fileName, prefix):
    iconMacros = []
    with open(fileName, 'r', encoding='utf-8') as file:
        for line in file:
            if prefix in line:
                word = line.split()
                iconMacros.append(word[1])
    return iconMacros

def generate_imgui_code(fileName, iconMacros):
    iconfileName = fileName.split('.')[0]  # Remove extension
    
    code = f'ImGui::Begin("{iconfileName}", openWindow);\n'
    code += f'if (ImGui::BeginTable("Table-{iconfileName}", 5, flags)) {{\n'
 
    columeCount = 5
    for (index, macro) in enumerate(iconMacros):
        columeIndex = index % columeCount
        if columeIndex == 0: code += f'    ImGui::TableNextRow();\n'
        code += f'    ImGui::TableSetColumnIndex({columeIndex}); ImGui::Text({macro}"  {macro}");\n'
    
    code += '    ImGui::EndTable();\n'
    code += '}\nImGui::End();'

    return code

def main():
    for (fileName, prefix) in fileNames:
        iconMacros = extract_icon_macros(fileName, prefix)
        imguiCode = generate_imgui_code(fileName, iconMacros)
        
        iconfileName = fileName.split('.')[0]  # Remove extension
        with open(f'{iconfileName}.code', 'w', encoding='utf-8') as file:
            file.write(imguiCode)

if __name__ == "__main__":
    main()