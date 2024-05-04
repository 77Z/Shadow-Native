#include "CommandCenterBackend.hpp"
#include "Editor/EditorParts/EditorParts.hpp"
#include "imgui.h"
#include "imgui/TextEditor.h"

namespace Shadow::Editor::EditorParts {

static bool shouldShowRawSceneEditor = false;
static TextEditor rawSceneTextEditor;

void showRawSceneEditor() {
	shouldShowRawSceneEditor = true;
}
void hideRawSceneEditor() {
	shouldShowRawSceneEditor = false;
}

void rawSceneEditorInit() {
	CMD_CTR_NEWCMD("Scene: Edit Raw Scene Data", "Deserializes the current open scene into text and displays an editor for you to modify the raw data", []() {
		showRawSceneEditor();
	});

	rawSceneTextEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());

	TextEditor::Breakpoints bpts;
	bpts.insert(5);
	rawSceneTextEditor.SetBreakpoints(bpts);
	
}

void rawSceneEditorUpdate() {
	if (!shouldShowRawSceneEditor) return;

	ImGui::Begin("Raw Scene Editor", &shouldShowRawSceneEditor);

	ImGui::Button("Load Scene");

	ImGui::SameLine();

	ImGui::Button("Push Data to Scene");

	rawSceneTextEditor.Render("Raw Scene Editor");

	ImGui::End();
}

}