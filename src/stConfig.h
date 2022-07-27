#pragma once
#include "nlohmann/json.hpp"

struct stConfig {
	bool bOpen{ false };
	const char* cVer{ "Unknown" };
	bool bFormatText{ false };

	const float fDistMax{ 300.f }, fDistDefault{ 20.f };

	float posY{ 0 };
	bool bOnlyCmd{ false };
	std::string cmd{ "/customdl" };
	float fDistance{ 0.f };
	float Col[4]{ 0.21f, 0.545f, 0.83f, 1.f }; // DEFAULT COLOR: 0xFF358BD4 (ARGB)
	bool bShadow{ true }, bObstacles{ true }, bShowInfo{ false };
	std::string buf_text{ "[id: @id, type: @type subtype: @subtype Health: @health preloaded: @preloaded]\n\
Distance: @distance\n\
PassengerSeats: @passengerseats\n\
cPos: @cpos\n\
sPos: @spos" }; // 268 orig char
	bool bOriginal[4] {
		false,	// distance
		false,	// color
		false,	// settings
		false	// text
	};
	bool bDisable[9] {
		false,	// id
		false,	// type
		false,	// subtype
		false,	// health
		false,	// preloaded
		false,	// distance
		false,	// passengerSeats
		false,	// cPos
		false	// sPos
	};

	struct stAddr {
		uint32_t hk_sub{ 0x0 };
		uint32_t hk_sprintf{ 0x0 };
		uint32_t hk_processInput{ 0x0 };
		uint32_t hk_drawLittleText{ 0x0 };
		uint32_t hk_keyHandler{ 0x0 };
		uint32_t distance{ 0x0 };
		uint32_t bShowInfo{ 0x0 };
	} pAddr;

	bool bReturn[4] {
		false,	// distance
		false,	// color
		false,	// settings [NOT USED]
		false	// text
	};
};
inline stConfig pConfig;

enum eDis : uint16_t {
	id = 0,
	type,
	subtype,
	health,
	preloaded,
	dist,
	pasSeats,
	cPos,
	sPos
};

enum eOrig : uint16_t {
	distance = 0,
	color,
	settings,
	text
};