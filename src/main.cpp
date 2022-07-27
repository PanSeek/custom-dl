#include "main.h"

#pragma warning (disable: 4996)

static WNDPROC OrigWnd;
static DWORD dwStart;
static int nowTime{ 0 };

// win proc
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (pConfig.bOpen && uMsg == WM_KEYDOWN && wParam == VK_ESCAPE) {
		pConfig.bOpen = false;
		details::pauseScreen(false);
		return true;
	} ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
	return CallWindowProcA(OrigWnd, hwnd, uMsg, wParam, lParam);
}

// hooks
int __cdecl hk_sprintf(char* buffer, const char* format, int id, int type, int subtype, double health, uint32_t preloaded, double distance,
	uint32_t passengerSeats, double cPX, double cPY, double cPZ, double sPX, double sPY, double sPZ) {

	return pConfig.bOriginal[eOrig::text] ?
		hook_sprintf->call(buffer, format, id, type, subtype, health, preloaded, distance, passengerSeats, cPX, cPY, cPZ, sPX, sPY, sPZ) : (pConfig.bFormatText ? // orig
		hook_sprintf->call(buffer, format_yes(id, type, subtype, health, preloaded, distance, passengerSeats, cPX, cPY, cPZ, sPX, sPY, sPZ).c_str()):	// FormatText is true
		hook_sprintf->call(buffer, format_not(id, type, subtype, health, preloaded, distance, passengerSeats, cPX, cPY, cPZ, sPX, sPY, sPZ).c_str()));	// FormatText is false
}

void __fastcall hk_sub(void* this_, void* EDX, float* a2, int a3, int color, int shadow, char obstacles) {
	// color
	if (pConfig.bOriginal[eOrig::color])	color = 0xFF358BD4;
	else									color = D3DCOLOR_ARGB((int)(pConfig.Col[3] * 255.f), (int)(pConfig.Col[0] * 255.f), (int)(pConfig.Col[1] * 255.f), (int)(pConfig.Col[2] * 255.f));

	// shadow, obstacles, posY
	if (!pConfig.bOriginal[eOrig::settings]) {
		shadow = pConfig.bShadow;
		obstacles = !pConfig.bObstacles;
		a2[2] = a2[2] + pConfig.posY;
	}

	return hook_sub->call(this_, a2, a3, color, shadow, obstacles);
}

void __fastcall hk_ProcessInput(CInput* this_, DWORD EDX) {
	std::wstring wMsg{ this_->m_pEditbox->m_pwszBuffer };
	std::string str(wMsg.begin(), wMsg.end());
	if ((!pConfig.bOnlyCmd && str == "/cdl" || str == "/dlc") || (pConfig.bOnlyCmd && str == pConfig.cmd)) {
		pConfig.bOpen = !pConfig.bOpen;
		details::pauseScreen(pConfig.bOpen);
	} return hook_ProcessInput->call(this_);
}

BOOL __cdecl hk_keyHandler(uint32_t nChar) {
	if (!pConfig.bOpen)
		return hook_keyHandler->call(nChar);
}

void __cdecl hk_CTimer__Update(void) {
	static bool init{ false }, initSAMP{ false }, initHook{ false };
	if (!init) {
		OrigWnd = (WNDPROC)SetWindowLongA(**reinterpret_cast<HWND**>(0xC17054), GWL_WNDPROC, (LONG)WindowProc);
		init = true;
	}
	if (init && !initSAMP) {
		if (SAMP::isR1()) {
			pConfig.pAddr.hk_sub = 0x712F8;
			pConfig.pAddr.hk_sprintf = 0x712D4;
			pConfig.pAddr.hk_processInput = 0x65D30;
			pConfig.pAddr.hk_keyHandler = 0x5DA80;
			pConfig.pAddr.distance = 0x7120F + 0x2;
			pConfig.pAddr.bShowInfo = 0x21A118;
			pConfig.cVer = "0.3.7 R1";
			initSAMP = true;
		}
		else if (SAMP::isR2()) {
			pConfig.pAddr.hk_sub = 0x71398;
			pConfig.pAddr.hk_sprintf = 0x71374;
			pConfig.pAddr.hk_processInput = 0x65E00;
			pConfig.pAddr.hk_keyHandler = 0x5DB50;
			pConfig.pAddr.distance = 0x712AF + 0x2;
			pConfig.pAddr.bShowInfo = 0x21A120;
			pConfig.cVer = "0.3.7 R2";
			initSAMP = true;
		}
		else if (SAMP::isR3()) {
			pConfig.pAddr.hk_sub = 0x751E8;
			pConfig.pAddr.hk_sprintf = 0x751C4;
			pConfig.pAddr.hk_processInput = 0x69260;
			pConfig.pAddr.hk_keyHandler = 0x60E20;
			pConfig.pAddr.distance = 0x750FF + 0x2;
			pConfig.pAddr.bShowInfo = 0x26E900;
			pConfig.cVer = "0.3.7 R3";
			initSAMP = true;
		}
		else if (SAMP::isR4()) {
			pConfig.pAddr.hk_sub = 0x75928;
			pConfig.pAddr.hk_sprintf = 0x75904;
			pConfig.pAddr.hk_processInput = 0x69990;
			pConfig.pAddr.hk_keyHandler = 0x61550;
			pConfig.pAddr.distance = 0x7583F + 0x2;
			pConfig.pAddr.bShowInfo = 0x26EA30;
			pConfig.cVer = "0.3.7 R4";
			initSAMP = true;
		}
		else if (SAMP::isDL()) {
			pConfig.pAddr.hk_sub = 0x75378;
			pConfig.pAddr.hk_sprintf = 0x75354;
			pConfig.pAddr.hk_processInput = 0x69410;
			pConfig.pAddr.hk_keyHandler = 0x61010;
			pConfig.pAddr.distance = 0x7528F + 0x2;
			pConfig.pAddr.bShowInfo = 0x2ACA48;
			pConfig.cVer = "0.3DL";
			initSAMP = true;
		}
		else if (SAMP::isE()) {
			pConfig.pAddr.hk_sub = 0x6BDC0;
			pConfig.pAddr.hk_sprintf = 0x6BD9C;
			pConfig.pAddr.hk_processInput = 0x62140;
			pConfig.pAddr.hk_keyHandler = 0x5AC50;
			pConfig.pAddr.distance = 0x6BCD7 + 0x2;
			pConfig.pAddr.bShowInfo = 0x20E02C;
			pConfig.cVer = "0.3e";
			initSAMP = true;
		}
	}
	if (!initHook && initSAMP) {
		hook_sprintf = std::make_unique<memwrapper::memhook<sprintf_t>>(SAMP::Library() + pConfig.pAddr.hk_sprintf, &hk_sprintf);							hook_sprintf->install();
		hook_sub = std::make_unique<memwrapper::memhook<sub_t>>(SAMP::Library() + pConfig.pAddr.hk_sub, &hk_sub);											hook_sub->install();
		hook_ProcessInput = std::make_unique<memwrapper::memhook<ProcessInput_t>>(SAMP::Library() + pConfig.pAddr.hk_processInput, &hk_ProcessInput);		hook_ProcessInput->install();
		hook_keyHandler = std::make_unique<memwrapper::memhook<keyHandler_t>>(SAMP::Library() + pConfig.pAddr.hk_keyHandler, &hk_keyHandler);				hook_keyHandler->install();

		details::setValue(SAMP::Library() + pConfig.pAddr.distance, pConfig.fDistance > 0.1f ? &pConfig.fDistance : &pConfig.fDistDefault);
		if (pConfig.bShowInfo) details::setValue(SAMP::Library() + pConfig.pAddr.bShowInfo, TRUE);
		initHook = true;
	}
	if (initSAMP) {
		if (pConfig.bReturn[eOrig::distance] && !pConfig.bOriginal[eOrig::distance]) {
			details::setValue(SAMP::Library() + pConfig.pAddr.distance, &pConfig.fDistance);
			pConfig.bReturn[eOrig::distance] = false;
		}
	}

	/* CRINGE */

	time_t rawtime;
	struct tm* ptm;
	time(&rawtime);
	ptm = gmtime(&rawtime);

	if (GetTickCount() - dwStart) {
		if (nowTime > 600) {
			pJSON.save();
			nowTime = 0;
		} nowTime++;
	}
	return hook_CTimer__Update->call();
}

// main
CMain::CMain() {
	hook_CTimer__Update = std::make_unique <memwrapper::memhook<CTimer__Update>>(reinterpret_cast<void*>(0x561B10), hk_CTimer__Update);
	hook_CTimer__Update->install();
	pD3DHook = std::make_unique<CD3DHook>();
	if (std::filesystem::exists("CustomDL.json"))
		pJSON.load();
}