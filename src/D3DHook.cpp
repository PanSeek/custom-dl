#include "D3DHook.h"
#include "stConfig.h"
#include "details.h"
#include "samp.h"

const char* bOriginal_items[] = { "Distance", "Color", "Settings", "Text" };
const char* bDisable_items[] = { "ID", "Type", "Subtype", "Health", "Preloaded", "Distance", "PassengerSeats", "cPos", "sPos" };

// helpful
static auto MultiCombo(const char* label, bool combos[], const char* items[], int items_count) {
	std::vector<std::string> vec;
	static std::string preview;
	for (int i = 0, j = 0; i < items_count; i++) {
		if (combos[i]) {
			vec.push_back(items[i]);
			if (j > 2)      preview = vec.at(0) + ", " + vec.at(1) + ", " + vec.at(2) + ", ...";
			else if (j)     preview += ", " + (std::string)items[i];
			else            preview = items[i];
			j++;
		}
	}
	if (ImGui::BeginCombo(label, preview.c_str())) {
		for (int i = 0; i < items_count; i++) {
			ImGui::Selectable(items[i], &combos[i], ImGuiSelectableFlags_DontClosePopups);
		} ImGui::EndCombo();
	} preview = "None";
}

static auto Hint(const char* cmd) {
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::Text("If this checkbox true -> default /cdl and /dlc will be disabled.\nUse your command: %s\n\nIf this checbox false -> default /cdl and /dlc will be enabled.\nCustom command %s disabled", cmd, cmd);
		ImGui::EndTooltip();
	}
}

static auto Copy(const char* text, const char* copy) {
	ImGui::Text("%s - %s", text, copy);
	if (ImGui::IsItemClicked()) {
		ImGui::LogToClipboard();
		ImGui::LogText(copy);
		ImGui::LogFinish();
	}
}

// main
void __cdecl RenderMenu(void) {
	char buf_text[200];
	strcpy_s(buf_text, pConfig.buf_text.c_str());

	ImGui::GetIO().MouseDrawCursor = pConfig.bOpen;
	if (pConfig.bOpen) ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	if (!(*(BYTE*)(0xBA67A4) == 1) && pConfig.bOpen) {
		ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f }, ImGuiCond_Once, { 0.5f, 0.5f });
		ImGui::Begin("##begin", &pConfig.bOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PushItemWidth(150.f);
		ImGui::InputText("Command", &pConfig.cmd); ImGui::SameLine();
		ImGui::Checkbox("##use_cmd", &pConfig.bOnlyCmd); Hint(pConfig.cmd.c_str());
		MultiCombo("Original", pConfig.bOriginal, bOriginal_items, IM_ARRAYSIZE(bOriginal_items));
		if (!pConfig.bOriginal[eOrig::text]) ImGui::Checkbox("Format text", &pConfig.bFormatText);
		ImGui::Checkbox("Show info /dl when enter in the game", &pConfig.bShowInfo);
		ImGui::Separator();
		if (!pConfig.bOriginal[eOrig::distance]) {
			if (ImGui::SliderFloat("Distance", &pConfig.fDistance, 0.f, 201.f, details::formatSliderFloat(pConfig.fDistance))) {
				if (pConfig.fDistance >= 201.f) details::setValue(SAMP::Library() + pConfig.pAddr.distance, &pConfig.fDistMax);
				else details::setValue(SAMP::Library() + pConfig.pAddr.distance, pConfig.fDistance > 0.01f ? &pConfig.fDistance : &pConfig.fDistDefault);
			}
		}
		else {
			details::setValue(SAMP::Library() + pConfig.pAddr.distance, &pConfig.fDistDefault);
			pConfig.bReturn[eOrig::distance] = true;
		}
		if (!pConfig.bOriginal[eOrig::settings]) {
			ImGui::SliderFloat("Position Y for text", &pConfig.posY, -10.f, 10.f, "%.1f", 1.f);
			ImGui::Checkbox("Shadow", &pConfig.bShadow); ImGui::SameLine();
			ImGui::Checkbox("Obstacles", &pConfig.bObstacles);
		}
		if (!pConfig.bOriginal[eOrig::color]) {
			if (!pConfig.bOriginal[eOrig::settings]) ImGui::SameLine();
			ImGui::ColorEdit4("Color", pConfig.Col, ImGuiColorEditFlags_NoInputs);
		}
		if (!pConfig.bOriginal[eOrig::text]) {
			if (!pConfig.bFormatText) MultiCombo("Remove", pConfig.bDisable, bDisable_items, IM_ARRAYSIZE(bDisable_items));
			else {
				ImGui::PushItemWidth(370.f);
				Copy("ID", "@id");				Copy("Type", "@type");						Copy("Model", "@model");
				Copy("Subtype", "@subtype");	Copy("Health", "@health");					Copy("Preloaded", "@preloaded");
				Copy("Distance", "@distance");	Copy("PassengerSeats", "@passengerseats");	Copy("cPos", "@cpos");
				Copy("sPos", "@spos");
				ImGui::Separator();
				ImGui::Text("* If you click on the tag text (above), it will be copied.\n* Can't use duplicate tag.\nWARNING: Can't use \"%%\".");
				if (ImGui::InputTextMultiline("##fmt", buf_text, sizeof(buf_text)))
					pConfig.buf_text = buf_text;
				ImGui::Text("%d/%d", strlen(buf_text), sizeof(buf_text));
				ImGui::PopItemWidth();
			}
		}
		ImGui::PopItemWidth();
		ImGui::Separator();
		ImGui::TextDisabled("Custom /dl by PanSeek [SA-MP %s]", pConfig.cVer);
		ImGui::End();
	} return;
}

// hooks
HRESULT __stdcall CD3DHook::hk_Present(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
	static bool init{ false };
	if (!init) {
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(**reinterpret_cast<HWND**>(0xC17054));
		ImGui_ImplDX9_Init(pDevice);
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 15.0f);
		ImGui::StyleColorsLight();
		init = true;
	}
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	RenderMenu();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	return pD3DHook->hook_present->call(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

HRESULT __stdcall CD3DHook::hk_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams) {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	return pD3DHook->hook_reset->call(pDevice, pPresentParams);
}