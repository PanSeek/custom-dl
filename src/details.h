#pragma once
#include <windows.h>
#include <cstdint>

namespace details {
	template <typename T>
	inline auto setValue(uint32_t addr, T value, bool protect = true) {
		DWORD oldProt;
		if (protect) VirtualProtect(reinterpret_cast<LPVOID>(addr), sizeof(T), PAGE_READWRITE, &oldProt);
		*reinterpret_cast<T*>(addr) = value;
		if (protect) VirtualProtect(reinterpret_cast<LPVOID>(addr), sizeof(T), oldProt, &oldProt);
	}

	inline auto setValue(uint32_t addr, float value, bool protect = true) {
		DWORD oldProt;
		if (protect) VirtualProtect(reinterpret_cast<LPVOID>(addr), sizeof(0x4), PAGE_READWRITE, &oldProt);
		*reinterpret_cast<float**>(addr) = &value;
		if (protect) VirtualProtect(reinterpret_cast<LPVOID>(addr), sizeof(0x4), oldProt, &oldProt);
	}

	inline auto formatSliderFloat(float value, float start = 0.0f, float end = 201.f) {
		if (value <= start)		return "Default";
		else if (value >= end)	return "Stream zone";
		else					return "%.2f";
	}

	inline auto pauseScreen(bool state) {
		static DWORD
			updateMouseProtection,
			rsMouseSetPosProtFirst,
			rsMouseSetPosProtSecond;

		if (state) {
			::VirtualProtect(reinterpret_cast<void*>(0x53F3C6U), 5U, PAGE_EXECUTE_READWRITE, &updateMouseProtection);
			::VirtualProtect(reinterpret_cast<void*>(0x53E9F1U), 5U, PAGE_EXECUTE_READWRITE, &rsMouseSetPosProtFirst);
			::VirtualProtect(reinterpret_cast<void*>(0x748A1BU), 5U, PAGE_EXECUTE_READWRITE, &rsMouseSetPosProtSecond);

			*reinterpret_cast<uint8_t*>(0x53F3C6U) = 0xE9U;
			*reinterpret_cast<uint32_t*>(0x53F3C6U + 1U) = 0x15BU;

			memset(reinterpret_cast<void*>(0x53E9F1U), 0x90, 5U);
			memset(reinterpret_cast<void*>(0x748A1BU), 0x90, 5U);
		} else {
			memcpy(reinterpret_cast<void*>(0x53F3C6U), "\xE8\x95\x6C\x20\x00", 5U);
			memcpy(reinterpret_cast<void*>(0x53E9F1U), "\xE8\xAA\xAA\x0D\x00", 5U);
			memcpy(reinterpret_cast<void*>(0x748A1BU), "\xE8\x80\x0A\xED\xFF", 5U);

			using CPad_ClearMouseHistory_t = void(__cdecl*)();
			using CPad_UpdatePads_t = void(__cdecl*)();
			reinterpret_cast<CPad_ClearMouseHistory_t>(0x541BD0U)();
			reinterpret_cast<CPad_UpdatePads_t>(0x541DD0U)();

			::VirtualProtect(reinterpret_cast<void*>(0x53F3C6U), 5U, updateMouseProtection, &updateMouseProtection);
			::VirtualProtect(reinterpret_cast<void*>(0x53E9F1U), 5U, rsMouseSetPosProtFirst, &rsMouseSetPosProtFirst);
			::VirtualProtect(reinterpret_cast<void*>(0x748A1BU), 5U, rsMouseSetPosProtSecond, &rsMouseSetPosProtSecond);
		} return;
	}
}