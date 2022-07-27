#pragma once
#include <windows.h>
#include <d3d9.h>

#include <vector>
#include <string>
#include <memory>

#include "../include/memwrapper/memwrapper.h"

#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_stdlib.h"
#include "../include/imgui/imgui_impl_dx9.h"
#include "../include/imgui/imgui_impl_win32.h"

class CD3DHook {
public:
    CD3DHook() {
        void* pPresent = reinterpret_cast<LPVOID*>(this->GetDeviceAddress(17));
        void* pReset = reinterpret_cast<LPVOID*>(this->GetDeviceAddress(16));
        hook_present = std::make_unique <memwrapper::memhook<present_t>>(pPresent, hk_Present); hook_present->install();
        hook_reset = std::make_unique <memwrapper::memhook<reset_t>>(pReset, hk_Reset); hook_reset->install();
    };
    ~CD3DHook() {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    };
private:
    static DWORD FindDevice(DWORD Len) {
        DWORD dwObjBase = 0;
        char infoBuf[MAX_PATH];
        GetSystemDirectoryA(infoBuf, MAX_PATH);
        strcat_s(infoBuf, MAX_PATH, "\\d3d9.dll");
        dwObjBase = (DWORD)LoadLibraryA(infoBuf);
        while (dwObjBase++ < dwObjBase + Len) {
            if ((*(WORD*)(dwObjBase + 0x00)) == 0x06C7 && (*(WORD*)(dwObjBase + 0x06)) == 0x8689 && (*(WORD*)(dwObjBase + 0x0C)) == 0x8689) {
                dwObjBase += 2;
                break;
            }
        } return(dwObjBase);
    };

    static DWORD GetDeviceAddress(int VTableIndex) {
        PDWORD VTable;
        *(DWORD*)&VTable = *(DWORD*)FindDevice(0x128000);
        return VTable[VTableIndex];
    };

    using present_t = int(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
    using reset_t   = int(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

    static HRESULT __stdcall hk_Present(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
    static HRESULT __stdcall hk_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams);

    std::unique_ptr<memwrapper::memhook<present_t>> hook_present;
    std::unique_ptr<memwrapper::memhook<reset_t>>   hook_reset;
};
inline std::unique_ptr<CD3DHook> pD3DHook;