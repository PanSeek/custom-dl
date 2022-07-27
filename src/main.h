#pragma once
#include <windows.h>

#include <filesystem>
#include <format>
#include <string>
#include <fstream>
#include <istream>
#include <unordered_map>

#include "../include/memwrapper/memwrapper.h"
#include "nlohmann/json.hpp"

#include "stConfig.h"
#include "D3DHook.h"
#include "details.h"
#include "samp.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using CTimer__Update = void(__cdecl*)();
std::unique_ptr<memwrapper::memhook<CTimer__Update>> hook_CTimer__Update;

using sprintf_t = int(__cdecl*)(char*, const char*, int, int, int, double, uint32_t, double, uint32_t, double, double, double, double, double, double);
std::unique_ptr<memwrapper::memhook<sprintf_t>> hook_sprintf;

using sub_t = void(__thiscall*)(void*, float*, int, int, int, char);
std::unique_ptr<memwrapper::memhook<sub_t>> hook_sub;

using ProcessInput_t = void(__thiscall*)(CInput*);
std::unique_ptr<memwrapper::memhook<ProcessInput_t>> hook_ProcessInput;

using keyHandler_t = BOOL(__cdecl*)(uint32_t);
std::unique_ptr<memwrapper::memhook<keyHandler_t>> hook_keyHandler;

// helpful for hk_sprintf
auto if_without(__int8 type = -1) {
	switch (type) {
		case 0: return !pConfig.bDisable[eDis::id]; break;
		case 1: return !pConfig.bDisable[eDis::type]; break;
		case 2: return !pConfig.bDisable[eDis::subtype]; break;
		case 3: return !pConfig.bDisable[eDis::health]; break;
		case 4: return !pConfig.bDisable[eDis::preloaded]; break;
		default: return !(pConfig.bDisable[eDis::id] && pConfig.bDisable[eDis::type] && pConfig.bDisable[eDis::subtype] && pConfig.bDisable[eDis::health] && pConfig.bDisable[eDis::preloaded]); break;
	}
}

std::string __cdecl format_not(int id, int type, int subtype, double health, uint32_t preloaded, double distance, uint32_t passengerSeats,
	double cPX, double cPY, double cPZ, double sPX, double sPY, double sPZ) {

	return std::format("{}{}{}{}{}", if_without() ? std::format("[{}{}{}{}{}]\n",
		!pConfig.bDisable[eDis::id] ?			std::format("id: {}{}", id, if_without(1) || if_without(2) || if_without(3) || if_without(4) ? ", " : "") : "",	// id
		!pConfig.bDisable[eDis::type] ?			std::format("type: {}{}", type, if_without(2) || if_without(3) || if_without(4) ? " " : "") : "",				// type
		!pConfig.bDisable[eDis::subtype] ?		std::format("subtype: {}{}", subtype, if_without(3) || if_without(4) ? " " : "") : "",							// subtype
		!pConfig.bDisable[eDis::health] ?		std::format("Health: {:.1f}{}", health, if_without(4) ? " " : "") : "",											// health
		!pConfig.bDisable[eDis::preloaded] ?	std::format("preloaded: {}", preloaded) : "") : "",																// preloaded

		!pConfig.bDisable[eDis::dist] ?			std::format("Distance: {:.2f}m\n", distance) : "",					// distance
		!pConfig.bDisable[eDis::pasSeats] ?		std::format("PassengerSeats: {}\n", passengerSeats) : "",			// PassengerSeats
		!pConfig.bDisable[eDis::cPos] ?			std::format("cPos: {:.3f},{:.3f},{:.3f}\n", cPX, cPY, cPZ) : "",	// cPos
		!pConfig.bDisable[eDis::sPos] ?			std::format("sPos: {:.3f},{:.3f},{:.3f}", sPX, sPY, sPZ) : ""		// sPos
	);
}

std::string __cdecl getCarsName(int type) {
	std::unordered_map<int, std::string> uVehNames = {
		{400, "Landstalker"}, {401, "Bravura"}, {402, "Buffalo"}, {403, "Linerunner"}, {404, "Perenniel"},
		{405, "Sentinel"}, {406, "Dumper"}, {407, "Firetruck"}, {408, "Trashmaster"}, {409, "Stretch"},
		{410, "Manana"}, {411, "Infernus"}, {412, "Voodoo"}, {413, "Pony"}, {414, "Mule"},
		{415, "Cheetah"}, {416, "Ambulance"}, {417, "Leviathan"}, {418, "Moonbeam"}, {419, "Esperanto"},
		{420, "Taxi"}, {421, "Washington"}, {422, "Bobcat"}, {423, "Mr Whoopee"}, {424, "BF Injection"},
		{425, "Hunter"}, {426, "Premier"}, {427, "Enforcer"}, {428, "Securicar"}, {429, "Banshee"},
		{430, "Predator"}, {431, "Bus"}, {432, "Rhino"}, {433, "Barracks"}, {434, "Hotknife"},
		{435, "Article Trailer"}, {436, "Previon"}, {437, "Coach"}, {438, "Cabbie"}, {439, "Stallion"},
		{440, "Rumpo"}, {441, "RC Bandit"}, {442, "Romero"}, {443, "Packer"}, {444, "Monster"},
		{445, "Admiral"}, {446, "Squallo"}, {447, "Seasparrow"}, {448, "Pizzaboy"}, {449, "Tram"},
		{450, "Article Trailer 2"}, {451, "Turismo"}, {452, "Speeder"}, {453, "Reefer"}, {454, "Tropic"},
		{455, "Flatbed"}, {456, "Yankee"}, {457, "Caddy"}, {458, "Solair"}, {459, "Berkley's RC"},
		{460, "Skimmer"}, {461, "PCJ-600"}, {462, "Faggio"}, {463, "Freeway"}, {464, "RC Baron"},
		{465, "RC Raider"}, {466, "Glendale"}, {467, "Oceanic"}, {468, "Sanchez"}, {469, "Sparrow"},
		{470, "Patriot"}, {471, "Quad"}, {472, "Coastguard"}, {473, "Dinghy"}, {474, "Hermes"},
		{475, "Sabre"}, {476, "Rustler"}, {477, "ZR-350"}, {478, "Walton"}, {479, "Regina"},
		{480, "Comet"}, {481, "BMX"}, {482, "Burrito"}, {483, "Camper"}, {484, "Marquis"},
		{485, "Baggage"}, {486, "Dozer"}, {487, "Maverick"}, {488, "SAN News Maverick"}, {489, "Rancher"},
		{490, "FBI Rancher"}, {491, "Virgo"}, {492, "Greenwood"}, {493, "Jetmax"}, {494, "Hotring Racer"},
		{495, "Sandking"}, {496, "Blista Compact"}, {497, "Police Maverick"}, {498, "Boxville"}, {499, "Benson"},
		{500, "Mesa"}, {501, "RC Goblin"}, {502, "Hotring Racer A"}, {503, "Hotring Racer B"}, {504, "Bloodring Banger"},
		{505, "Rancher"}, {506, "Super GT"}, {507, "Elegant"}, {508, "Journey"}, {509, "Bike"},
		{510, "Mountain Bike"}, {511, "Beagle"}, {512, "Cropduster"}, {513, "Stuntplane"}, {514, "Tanker"},
		{515, "Roadtrain"}, {516, "Nebula"}, {517, "Majestic"}, {518, "Buccaneer"}, {519, "Shamal"},
		{520, "Hydra"}, {521, "FCR-900"}, {522, "NRG-500"}, {523, "HPV1000"}, {524, "Cement Truck"},
		{525, "Towtruck"}, {526, "Fortune"}, {527, "Cadrona"}, {528, "FBI Truck"}, {529, "Willard"},
		{530, "Forklift"}, {531, "Tractor"}, {532, "Combine Harvester"}, {533, "Feltzer"}, {534, "Remington"},
		{535, "Slamvan"}, {536, "Blade"}, {537, "Freight (Train)"}, {538, "Brownstreak (Train)"}, {539, "Vortex"},
		{540, "Vincent"}, {541, "Bullet"}, {542, "Clover"}, {543, "Sadler"}, {544, "Firetruck LA"},
		{545, "Hustler"}, {546, "Intruder"}, {547, "Primo"}, {548, "Cargobob"}, {549, "Tampa"},
		{550, "Sunrise"}, {551, "Merit"}, {552, "Utility Van"}, {553, "Nevada"}, {554, "Yosemite"},
		{555, "Windsor"}, {556, "Monster A"}, {557, "Monster B"}, {558, "Uranus"}, {559, "Jester"},
		{560, "Sultan"}, {561, "Stratum"}, {562, "Elegy"}, {563, "Raindance"}, {564, "RC Tiger"},
		{565, "Flash"}, {566, "Tahoma"}, {567, "Savanna"}, {568, "Bandito"}, {569, "Freight Flat Trailer"},
		{570, "Streak Trailer"}, {571, "Kart"}, {572, "Mower"}, {573, "Dune"}, {574, "Sweeper"},
		{575, "Broadway"}, {576, "Tornado"}, {577, "AT400"}, {578, "DFT-30"}, {579, "Huntley"},
		{580, "Stafford"}, {581, "BF-400"}, {582, "Newsvan"}, {583, "Tug"}, {584, "Petrol Trailer"},
		{585, "Emperor"}, {586, "Wayfarer"}, {587, "Euros"}, {588, "Hotdog"}, {589, "Club"},
		{590, "Freight Box Trailer"}, {591, "Article Trailer 3"}, {592, "Andromada"}, {593, "Dodo"}, {594, "RC Cam"},
		{595, "Launch"}, {596, "Police Car (LSPD)"}, {597, "Police Car (SFPD)"}, {598, "Police Car (LVPD)"}, {599, "Police Ranger"},
		{600, "Picador"}, {601, "S.W.A.T."}, {602, "Alpha"}, {603, "Phoenix"}, {604, "Glendale Shit"},
		{605, "Sadler Shit"}, {606, "Baggage Trailer A"}, {607, "Baggage Trailer B"}, {608, "Tug Stairs Trailer"}, {609, "Boxville"},
		{610, "Farm Trailer"}, {611, "Utility Trailer"}, /* ARZ VEH */ {612, "Mercedes-Benz GT63 AMG"}, {613, "Mercedes-Benz G63 AMG"}, {614, "Audi RS6"},
		{662, "BMW X5"}, {663, "Chevrolet Corvette"}, {665, "Chevrolet Cruze"}, {666, "Lexus LX"}, {667, "Porsche 911"},
		{668, "Porsche Cayenne"}, {699, "Bentley"}, {793, "BMW M8"}, {794, "Mercedes-Benz E63"}, {909, "Mercedes-Benz S63"},
		{965, "Volksvagen Touareg"}, {1194, "Lamborghini Urus"}, {1195, "Audi Q8"}, {1196, "Dodge Challenger SRT"}, {1197, "Acura NSX"},
		{1198, "Volvo V60"}, {1199, "Range Rover Evoque"}, {1200, "Honda Civic Type-R"}, {1201, "Lexus Sport-S"}, {1202, "Ford Mustang GT"},
		{1203, "Volvo XC90"}, {1204, "Jaguar F-PACE"}, {1205, "Kia Optima"}, {3155, "BMW Z4 40i"}, {3156, "Mercedes-Benz S600 W140"},
		{3157, "BMW X5 e53"}, {3158, "Nissan GTR R34"}, {3194, "Ducatti Diavel"}, {3195, "Ducatti Panigale"}, {3196, "Ducatti Ducnacker"},
		{3197, "Kawasaki Ninja ZX-10RR"}, {3198, "Western"}, {3199, "Rolls Royce Cullinan"}, {3200, "Volkswagen Beetle"}, {3201, "Buggati Divo"},
		{3202, "Buggati Chiron"}, {3203, "Fiat 500"}, {3204, "Mercedes-Benz GLS 2020"}, {3205, "Mercedes-AMG G65 AMG"}, {3206, "Lamborghini SVJ"},
		{3207, "Range Rover SVA"}, {3208, "BMW 530i e39"}, {3209, "Mercedes-Benz S600 W220"}, {3210, "Tesla Model X"}, {3211, "Nissan Leaf"},
		{3212, "Nissan Silvia S15"}, {3213, "Subary Forester XT"}, {3215, "Subary Legasy 1989"}, {3216, "Hyundai Sonata"}, {3217, "BMW 750i e38"},
		{3218, "Mercedes-Benz E55 AMG"}, {3219, "Mercedes-Benz E500"}, {3220, "Jackson Storm"}, {3222, "Lightning McQueen"}, {3223, "Mater"},
		{3224, "Buckingham"}, {3232, "Infinity FX50"}, {3233, "Lexus RX 450H"}, {3234, "Kia Sportage"}, {3235, "Volkswagen Golf R"},
		{3236, "Audi R8"}, {3237, "Tayota Camry XV40"}, {3238, "Tayota Camry XV70"}, {3239, "BMW M5 e60"}, {3240, "BMW M5 F90"},
		{3245, "Mercedes Maybach S650"}, {3247, "Mercedes-Benz AMG GT"}, {3248, "Porsche Panamera"}, {3251, "Volkswagen Passat"}, {3254, "Chevrolet Corvette 1980"},
		{3266, "Dodge RAM"}, {3348, "Ford Mustang GT500"}, {3974, "Aston Martin BD5"}, {4542, "BMW M3 GTR"}, {4543, "Chevroler Camaro SS"},
		{4544, "Mazda RX7"}, {4545, "Mazda RX8"}, {4546, "Mitsubishi Eclipse"}, {4547, "Ford Mustang 289"}, {4548, "Nissan 350Z"},
		{4774, "BMW 760Li"}, {4775, "Aston Martin One-77"}, {4776, "Bentley Bacalar"}, {4777, "Bentley Bentayga"}, {4778, "BMW M4 Competition"},
		{4779, "BMW i8"}, {4780, "Genesis G90"}, {4781, "Honda Integra Type-R"}, {4782, "BMW M3 G20"}, {4783, "Mercedes-Benz S500 W223"},
		{4784, "Ford Raptor"}, {4785, "Ferrari J50"}, {4786, "Mercedes-Benz SLR McLaren"}, {4787, "Subary BRZ"}, {4788, "LADA Vesta SW Cross"},
		{4789, "Porsche Taycan"}, {4790, "Ferrari Enzo"}, {4791, "UAZ Patriot"}, {4792, "Volga"}, {4793, "Mercedes-Benz X-Class"},
		{4794, "Jaguar XF"}, {4795, "rcshutle"}, {4796, "Dodge Grand Caravan"}, {4797, "Dodge Charger"}, {4798, "Ford Explorer"},
		{4799, "Ford F-150"}, {4800, "DeltaPlan"}, {4801, "Seashark"}, {4802, "Lambarghini Cantenario"}, {4803, "Ferrari 812 SuperFast"},
		{6604, "Audi A6"}, {6605, "Audi Q7"}, {6606, "BMW M6 2020"}, {6607, "BMW M6 1990"}, {6608, "Mercedes-Benz CLA 45 AMG"},
		{6609, "Mercedes-Benz CLS 63 AMG"}, {6610, "Haval H6"}, {6611, "Toyota Land Cruiser 200"}, {6612, "Lincoln Continental"}, {6613, "Porsche Macan"},
		{6614, "Daewoo Matiz"}, {6615, "Mercedes-Benz 6x6"}, {6616, "Mercedes-Benz E63"}, {6617, "Monster Mutt"}, {6618, "Monster Indonesia"},
		{6619, "Monster El Toro"}, {6620, "Monster Grave Digger"}, {6621, "Toyota Land Cruiser Prado"}, {6611, "Toyota Rav4"}, {6623, "Toyota Supra A90"},
		{6624, "UAZ"}, {6625, "Volvo XC90 2012"}, {12713, "Mercedes-Benz GLE 63"}, {12714, "Renault Laguna"}, {12715, "Mercedes-Benz CLS 53"},
		{12716, "Audi RS5"}, {12717, "Cadillac Escalade 2020"}, {12718, "Tesla CyberTrack"}, {12719, "Tesla Model S"}, {12720, "Ford GT"},
		{12721, "Dodge Viper"}, {12722, "Volkswagen Polo"}, {12723, "Mitsibishi Evolution IX"}, {12724, "Audi TT RS"}, {12725, "Mercedes-Benz Actros"},
		{12726, "Audi S4"}, {12727, "BMW 4-series"}, {12728, "Cadillac Escalade 2007"}, {12729, "Tayota Chaser"}, {12730, "Dacia 1300"},
		{12731, "Mitsibishi Evolution X"}, {12732, "Chevrolet Impala 64"}, {12733, "Chevrolet Impala 67"}, {12734, "Kenwooth Track"}, {12735, "Kenwooth Trailer"},
		{12736, "McLaren MP4"}, {12737, "Ford Mustang Mach 1"}, {12738, "Rolls-Royce Phantom"}, {12739, "Picup Track"}, {12740, "Volvo Track"},
		{12741, "Subary WRX STI"}, {12742, "Sherp"}, {12743, "Sanki"}, {14119, "Audi A6"}, {14120, "Toyota Camry"},
		{14121, "Kia"}, {14122, "Tesla Model X"}, {14123, "Toyota Rav4"}, {14124, "Nissan GTR 2017"}, {14767, "Mercedes-AMG Project One"},
		{14768, "Aston Martin Valkyrie"}, {14769, "Chevrolet Aveo"}, {14857, "Bugatti Bolide"}, {14884, "Yacota K5"}, {14899, "Renault Duster"},
		{14904, "Chevrolet Monza"}, {14905, "Mercedes-Benz G63 EQG"}, {14906, "HotWheels"}, {14907, "Hummer HX"}, {14908, "Ferrari LaFerrari"},
		{14909, "BMW M5 CS"}, {14910, "LADA Priora"}, {14911, "quadra"}, {14912, "Mercedes-Benz GLE"}, {14913, "Mercedes-Benz Vision AVTR"},
		{14914, "Specialize Stumpjumper"}, {14915, "Santa Cruz Tallboy"}, {14916, "Spooky Metalhead"}, {14917, "Turner Burner"}, {14918, "Holding Bus Company"},
		{14919, "Los-Santos Inter Bus C."}, {15085, "charger"}, {15098, "BMW M1 e26"}, {15099, "Lamborghini Countach"}, {15100, "Nagasaki"},
		{15101, "Koenigsegg Gemera"}, {15102, "Kia K7"}, {15103, "toro"}, {15104, "Lexus LX600"}, {15105, "Nissan Qashqai"},
		{15106, "predatorr"}, {15107, "Volkswagen Scirocco"}, {15108, "Longfin"}, {15109, "Toyota Land Cruser 300"}, {15110, "Wellcraft"},
		{15111, "Yacht"}, {15112, "Boates"}, {15113, "Mercedes-Benz A45"}, {15114, "Toyota AE86"}, {15115, "Land Rover Defender"},
		{15116, "mach"}, {15117, "Mazda 6"}, {15118, "Audi R8s"}, {15119, "Hyundai Santa Fe"}, {15295, "Range Rover Velar"}
	};

	auto it = uVehNames.find(type);

	if (SAMP::isLauncher())		return it != uVehNames.end() ? uVehNames[type] : "Unknown";
	else						return (type > 611 || type < 400) ? "Unknown" : uVehNames[type];
}

std::string __cdecl format_yes(int id, int type, int subtype, double health, uint32_t preloaded, double distance, uint32_t passengerSeats,
	double cPX, double cPY, double cPZ, double sPX, double sPY, double sPZ) {

	std::string tokens[10]{
		"@id",
		"@type",
		"@subtype",
		"@health",
		"@preloaded",
		"@distance",
		"@passengerseats",
		"@cpos",
		"@spos",
		"@model"
	};
	std::string text{ pConfig.buf_text };

	static uintptr_t entry;
	if ((entry = text.find(tokens[0])) != std::string::npos) text.replace(entry, tokens[0].size(), std::to_string(id));
	if ((entry = text.find(tokens[1])) != std::string::npos) text.replace(entry, tokens[1].size(), std::to_string(type));
	if ((entry = text.find(tokens[2])) != std::string::npos) text.replace(entry, tokens[2].size(), std::to_string(subtype));
	if ((entry = text.find(tokens[3])) != std::string::npos) text.replace(entry, tokens[3].size(), std::format("{:.1f}", health));
	if ((entry = text.find(tokens[4])) != std::string::npos) text.replace(entry, tokens[4].size(), std::to_string(preloaded));
	if ((entry = text.find(tokens[5])) != std::string::npos) text.replace(entry, tokens[5].size(), std::format("{:.2f}m", distance));
	if ((entry = text.find(tokens[6])) != std::string::npos) text.replace(entry, tokens[6].size(), std::to_string(passengerSeats));
	if ((entry = text.find(tokens[7])) != std::string::npos) text.replace(entry, tokens[7].size(), std::format("{:.3f},{:.3f},{:.3f}", cPX, cPY, cPZ));
	if ((entry = text.find(tokens[8])) != std::string::npos) text.replace(entry, tokens[8].size(), std::format("{:.3f},{:.3f},{:.3f}", sPX, sPY, sPZ));
	if ((entry = text.find(tokens[9])) != std::string::npos) text.replace(entry, tokens[9].size(), getCarsName(type));

	return text;
}

// json
class cJSON {
public:
	auto save() {
		nlohmann::json jsonf;
		std::ofstream file("CustomDL.json");

		jsonf["settings"]["distance"] = pConfig.fDistance;
		jsonf["settings"]["shadow"] = pConfig.bShadow;
		jsonf["settings"]["obstacles"] = pConfig.bObstacles;
		jsonf["settings"]["command"] = pConfig.cmd;
		jsonf["settings"]["useCommand"] = pConfig.bOnlyCmd;
		jsonf["settings"]["useFormat"] = pConfig.bFormatText;
		jsonf["settings"]["posY"] = pConfig.posY;
		jsonf["settings"]["text"] = pConfig.buf_text;
		jsonf["settings"]["showInfo"] = pConfig.bShowInfo;

		jsonf["color"]["alpha"] = pConfig.Col[3];
		jsonf["color"]["red"] = pConfig.Col[0];
		jsonf["color"]["green"] = pConfig.Col[1];
		jsonf["color"]["blue"] = pConfig.Col[2];

		jsonf["disable"]["id"] = pConfig.bDisable[eDis::id];
		jsonf["disable"]["type"] = pConfig.bDisable[eDis::type];
		jsonf["disable"]["subtype"] = pConfig.bDisable[eDis::subtype];
		jsonf["disable"]["health"] = pConfig.bDisable[eDis::health];
		jsonf["disable"]["preloaded"] = pConfig.bDisable[eDis::preloaded];
		jsonf["disable"]["distance"] = pConfig.bDisable[eDis::dist];
		jsonf["disable"]["passengerSeats"] = pConfig.bDisable[eDis::pasSeats];
		jsonf["disable"]["cPos"] = pConfig.bDisable[eDis::cPos];
		jsonf["disable"]["sPos"] = pConfig.bDisable[eDis::sPos];

		jsonf["original"]["distance"] = pConfig.bOriginal[eOrig::distance];
		jsonf["original"]["settings"] = pConfig.bOriginal[eOrig::settings];
		jsonf["original"]["color"] = pConfig.bOriginal[eOrig::color];
		jsonf["original"]["text"] = pConfig.bOriginal[eOrig::text];

		file << std::setw(4) << jsonf;

		return;
	}
	auto load() {
		nlohmann::json jsonf;
		std::ifstream file("CustomDL.json");
		file >> jsonf;

		pConfig.fDistance = jsonf["settings"]["distance"].get<float>();
		pConfig.bShadow = jsonf["settings"]["shadow"].get<bool>();
		pConfig.bObstacles = jsonf["settings"]["obstacles"].get<bool>();
		pConfig.cmd = jsonf["settings"]["command"].get<std::string>();
		pConfig.bOnlyCmd = jsonf["settings"]["useCommand"].get<bool>();
		pConfig.bFormatText = jsonf["settings"]["useFormat"].get<bool>();
		pConfig.posY = jsonf["settings"]["posY"].get<long>();
		pConfig.buf_text = jsonf["settings"]["text"].get<std::string>();
		pConfig.bShowInfo = jsonf["settings"]["showInfo"].get<bool>();

		pConfig.Col[3] = jsonf["color"]["alpha"].get<float>();
		pConfig.Col[0] = jsonf["color"]["red"].get<float>();
		pConfig.Col[1] = jsonf["color"]["green"].get<float>();
		pConfig.Col[2] = jsonf["color"]["blue"].get<float>();

		pConfig.bDisable[eDis::id] = jsonf["disable"]["id"].get<bool>();
		pConfig.bDisable[eDis::type] = jsonf["disable"]["type"].get<bool>();
		pConfig.bDisable[eDis::subtype] = jsonf["disable"]["subtype"].get<bool>();
		pConfig.bDisable[eDis::health] = jsonf["disable"]["health"].get<bool>();
		pConfig.bDisable[eDis::preloaded] = jsonf["disable"]["preloaded"].get<bool>();
		pConfig.bDisable[eDis::dist] = jsonf["disable"]["distance"].get<bool>();
		pConfig.bDisable[eDis::pasSeats] = jsonf["disable"]["passengerSeats"].get<bool>();
		pConfig.bDisable[eDis::cPos] = jsonf["disable"]["cPos"].get<bool>();
		pConfig.bDisable[eDis::sPos] = jsonf["disable"]["sPos"].get<bool>();

		pConfig.bOriginal[eOrig::distance] = jsonf["original"]["distance"].get<bool>();
		pConfig.bOriginal[eOrig::settings] = jsonf["original"]["settings"].get<bool>();
		pConfig.bOriginal[eOrig::color] = jsonf["original"]["color"].get<bool>();
		pConfig.bOriginal[eOrig::text] = jsonf["original"]["text"].get<bool>();

		return;
	}

	~cJSON() {
		this->save();
	}
} pJSON;

// main
class CMain {
public:
	CMain();
} plugin;