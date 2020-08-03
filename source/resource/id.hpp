#ifndef SYNAO_RESOURCE_ID_HPP
#define SYNAO_RESOURCE_ID_HPP

#include "./tbl_entry.hpp"

#define SYNAO_RES_BYTES_DEF(NAME, STR) constexpr byte_t NAME[] = STR
#define SYNAO_RES_ENTRY_DEF(TAG, FILE) constexpr tbl_entry_t TAG(FILE);

namespace res {
	namespace img {
		SYNAO_RES_BYTES_DEF(BkgRough, 	"bkgrough");
		SYNAO_RES_BYTES_DEF(Common, 	"common");
		SYNAO_RES_BYTES_DEF(Heads, 		"heads");
		SYNAO_RES_BYTES_DEF(PrtCave, 	"prtcave");
		SYNAO_RES_BYTES_DEF(PrtGhou, 	"prtghou");
		SYNAO_RES_BYTES_DEF(PrtSilver, 	"prtsilver");
	}
	namespace pal {
		SYNAO_RES_BYTES_DEF(Bullets, 	"bullets");
		SYNAO_RES_BYTES_DEF(Friends, 	"friends");
		SYNAO_RES_BYTES_DEF(Heads, 		"heads");
		SYNAO_RES_BYTES_DEF(Naomi, 		"naomi");
		SYNAO_RES_BYTES_DEF(PrtSilver, 	"prtsilver");
	}
	namespace anim {
		SYNAO_RES_ENTRY_DEF(Heads,		"heads");
		SYNAO_RES_ENTRY_DEF(Naomi,		"naomi");
		SYNAO_RES_ENTRY_DEF(Foxie,		"foxie");
		SYNAO_RES_ENTRY_DEF(GyoShin,	"gyoshin");
		SYNAO_RES_ENTRY_DEF(Kyoko,		"kyoko");
		SYNAO_RES_ENTRY_DEF(Nauzika,	"nauzika");
		SYNAO_RES_ENTRY_DEF(Senma,		"senma");
		SYNAO_RES_ENTRY_DEF(Shoshi,		"shoshi");
		SYNAO_RES_ENTRY_DEF(Barrier,	"barrier");
		SYNAO_RES_ENTRY_DEF(Blast,		"blast");
		SYNAO_RES_ENTRY_DEF(DashFlash,	"dashflash");
		SYNAO_RES_ENTRY_DEF(Droplet,	"droplet");
		SYNAO_RES_ENTRY_DEF(Shrapnel,	"shrapnel");
		SYNAO_RES_ENTRY_DEF(Smoke,		"smoke");
		SYNAO_RES_ENTRY_DEF(Splash,		"splash");
		SYNAO_RES_ENTRY_DEF(Bubble,		"bubble");
		SYNAO_RES_ENTRY_DEF(Frontier,	"frontier");
		SYNAO_RES_ENTRY_DEF(Hammer,		"hammer");
		SYNAO_RES_ENTRY_DEF(HolyLance,	"holylance");
		SYNAO_RES_ENTRY_DEF(Items,		"items");
		SYNAO_RES_ENTRY_DEF(Kannon,		"kannon");
		SYNAO_RES_ENTRY_DEF(NailRay,	"nailray");
		SYNAO_RES_ENTRY_DEF(WolfVulcan,	"wolfvulcan");
		SYNAO_RES_ENTRY_DEF(Austere,	"austere");
		SYNAO_RES_ENTRY_DEF(Autovator,	"autovtr");
		SYNAO_RES_ENTRY_DEF(Chest,		"chest");
		SYNAO_RES_ENTRY_DEF(Computer,	"computer");
		SYNAO_RES_ENTRY_DEF(Death,		"death");
		SYNAO_RES_ENTRY_DEF(Door,		"door");
		SYNAO_RES_ENTRY_DEF(Fireplace,	"fireplace");
		SYNAO_RES_ENTRY_DEF(Helpful,	"helpful");
		SYNAO_RES_ENTRY_DEF(Spring,		"spring");
		SYNAO_RES_ENTRY_DEF(Ghost,		"ghost");
	}
	namespace sfx {
		SYNAO_RES_ENTRY_DEF(Spark,			"spark");
		SYNAO_RES_ENTRY_DEF(Bwall,			"bwall");
		SYNAO_RES_ENTRY_DEF(Fan,			"fan");
		SYNAO_RES_ENTRY_DEF(Blade,			"blade");
		SYNAO_RES_ENTRY_DEF(Kannon,			"kanon");
		SYNAO_RES_ENTRY_DEF(Explode0,		"explode_0");
		SYNAO_RES_ENTRY_DEF(Explode1,		"explode_1");
		SYNAO_RES_ENTRY_DEF(Explode2,		"explode_2");
		SYNAO_RES_ENTRY_DEF(Beam,			"beam");
		SYNAO_RES_ENTRY_DEF(Drill,			"drill");
		SYNAO_RES_ENTRY_DEF(Spring,			"spring");
		SYNAO_RES_ENTRY_DEF(Splash,			"splash");
		SYNAO_RES_ENTRY_DEF(Text,			"text");
		SYNAO_RES_ENTRY_DEF(Select,			"select");
		SYNAO_RES_ENTRY_DEF(TitleBeg,		"titlebeg");
		SYNAO_RES_ENTRY_DEF(Inven,			"inven");
		SYNAO_RES_ENTRY_DEF(BrokenBarrier,	"brokebarr");
		SYNAO_RES_ENTRY_DEF(Damage,			"damage");
		SYNAO_RES_ENTRY_DEF(Walk,			"walk");
		SYNAO_RES_ENTRY_DEF(Landing,		"land");
		SYNAO_RES_ENTRY_DEF(Jump,			"jump");
		SYNAO_RES_ENTRY_DEF(Projectile0,	"projectile_0");
		SYNAO_RES_ENTRY_DEF(Razor,			"razor");
		SYNAO_RES_ENTRY_DEF(NpcDeath0,		"npc_dth_0");
	}
}

#endif // SYNAO_RESOURCE_ID_HPP