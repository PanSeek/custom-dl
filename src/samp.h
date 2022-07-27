#pragma once
#include <windows.h>
#include <cstdint>
#include <filesystem>

#pragma pack(push, 1)
struct CDXUTEditBox {
	uint8_t		stDXUTControl[0x4D];

	wchar_t* m_pwszBuffer;
	int			m_nBufferSize;
};

struct CInput {
	typedef void(__cdecl* CMDPROC)(char*);

	struct IDirect3DDevice9* m_pDevice;
	class CDXUTDialog* m_pGameUi;
	CDXUTEditBox* m_pEditbox;
	CMDPROC					m_pCommandProc[144];
	char					m_szCommandName[144][33];
	int						m_nCommandCount;
	BOOL					m_bEnabled;
	char					m_szInput[129];
	char					m_szRecallBufffer[10][129];
	char					m_szCurrentBuffer[129];
	int						m_nCurrentRecall;
	int						m_nTotalRecall;
	CMDPROC					m_pDefaultCommand;

	typedef void(__thiscall* ProcessInput_t)(CInput*);
};
#pragma pack(pop)

namespace SAMP {
	enum class eVerCode : int {
		notLoaded = -1,
		unknown = 0,
		R1,
		R2,
		R3,
		R4,
		DL,
		E
	};

	inline unsigned long Library() {
		static DWORD samp = 0;
		if (!samp) {
			samp = (DWORD)GetModuleHandleA("samp");
			if (samp == 0xFFFFFFFF) samp = 0;
		} return samp;
	}

	inline eVerCode Version() {
		static eVerCode ver = eVerCode::unknown;
		if (ver != eVerCode::unknown) return ver;
		auto samp = Library();
		if (!samp)
			return eVerCode::notLoaded;
		else if (*(unsigned char*)(samp + 0x129) == 0xF4)
			ver = eVerCode::R1;
		else if (*(unsigned char*)(samp + 0x129) == 0x0C)
			ver = eVerCode::R2;
		else if (*(unsigned char*)(samp + 0x129) == 0x00) {
			if (*(unsigned char*)(samp + 0x140) == 0xD0)
				ver = eVerCode::R3;
			else if (*(unsigned char*)(samp + 0x140) == 0xB0)
				ver = eVerCode::R4;
			else if (*(unsigned char*)(samp + 0x140) == 0xB7)
				ver = eVerCode::E;
		}
		else if (*(unsigned char*)(samp + 0x129) == 0x31)
			ver = eVerCode::DL;
		return ver;
	}

	inline bool isR1() {
		return Version() == eVerCode::R1;
	}

	inline bool isR2() {
		return Version() == eVerCode::R2;
	}

	inline bool isR3() {
		return Version() == eVerCode::R3;
	}

	inline bool isR4() {
		return Version() == eVerCode::R4;
	}

	inline bool isDL() {
		return Version() == eVerCode::DL;
	}

	inline bool isE() {
		return Version() == eVerCode::E;
	}

	inline bool isLauncher() {
		if (std::filesystem::exists("arizona\\arizona.txd") || std::filesystem::exists("AZVoice.asi")) return true;
		return false;
	}
}