#ifndef LEVIATHAN_INCLUDED_RESOURCE_ID_HPP
#define LEVIATHAN_INCLUDED_RESOURCE_ID_HPP

#include "./table-entry.hpp"

#define LEVIATHAN_RES_BYTES_DEF(NAME, STR) constexpr byte_t NAME[] = STR
#define LEVIATHAN_RES_ENTRY_DEF(TAG, FILE) constexpr table_entry_t TAG(FILE)

namespace res {
	namespace img {
		LEVIATHAN_RES_BYTES_DEF(BkgRough, 	"bkgrough");
		LEVIATHAN_RES_BYTES_DEF(Common, 	"common");
		LEVIATHAN_RES_BYTES_DEF(Heads, 		"heads");
		LEVIATHAN_RES_BYTES_DEF(PrtCave, 	"prtcave");
		LEVIATHAN_RES_BYTES_DEF(PrtGhou, 	"prtghou");
		LEVIATHAN_RES_BYTES_DEF(PrtSilver, 	"prtsilver");
	}
	namespace pal {
		LEVIATHAN_RES_BYTES_DEF(Bullets, 	"bullets");
		LEVIATHAN_RES_BYTES_DEF(Friends, 	"friends");
		LEVIATHAN_RES_BYTES_DEF(Heads, 		"heads");
		LEVIATHAN_RES_BYTES_DEF(Naomi, 		"naomi");
		LEVIATHAN_RES_BYTES_DEF(PrtSilver, 	"prtsilver");
	}
	namespace anim {
		LEVIATHAN_RES_ENTRY_DEF(Heads,		"heads");
		LEVIATHAN_RES_ENTRY_DEF(Naomi,		"naomi");
		LEVIATHAN_RES_ENTRY_DEF(Foxie,		"foxie");
		LEVIATHAN_RES_ENTRY_DEF(GyoShin,	"gyoshin");
		LEVIATHAN_RES_ENTRY_DEF(Kyoko,		"kyoko");
		LEVIATHAN_RES_ENTRY_DEF(Nauzika,	"nauzika");
		LEVIATHAN_RES_ENTRY_DEF(Senma,		"senma");
		LEVIATHAN_RES_ENTRY_DEF(Shoshi,		"shoshi");
		LEVIATHAN_RES_ENTRY_DEF(Barrier,	"barrier");
		LEVIATHAN_RES_ENTRY_DEF(Blast,		"blast");
		LEVIATHAN_RES_ENTRY_DEF(DashFlash,	"dashflash");
		LEVIATHAN_RES_ENTRY_DEF(Droplet,	"droplet");
		LEVIATHAN_RES_ENTRY_DEF(Shrapnel,	"shrapnel");
		LEVIATHAN_RES_ENTRY_DEF(Smoke,		"smoke");
		LEVIATHAN_RES_ENTRY_DEF(Splash,		"splash");
		LEVIATHAN_RES_ENTRY_DEF(Bubble,		"bubble");
		LEVIATHAN_RES_ENTRY_DEF(Frontier,	"frontier");
		LEVIATHAN_RES_ENTRY_DEF(Hammer,		"hammer");
		LEVIATHAN_RES_ENTRY_DEF(HolyLance,	"holylance");
		LEVIATHAN_RES_ENTRY_DEF(Items,		"items");
		LEVIATHAN_RES_ENTRY_DEF(Kannon,		"kannon");
		LEVIATHAN_RES_ENTRY_DEF(NailRay,	"nailray");
		LEVIATHAN_RES_ENTRY_DEF(WolfVulcan,	"wolfvulcan");
		LEVIATHAN_RES_ENTRY_DEF(Austere,	"austere");
		LEVIATHAN_RES_ENTRY_DEF(Autovator,	"autovtr");
		LEVIATHAN_RES_ENTRY_DEF(Chest,		"chest");
		LEVIATHAN_RES_ENTRY_DEF(Computer,	"computer");
		LEVIATHAN_RES_ENTRY_DEF(Death,		"death");
		LEVIATHAN_RES_ENTRY_DEF(Door,		"door");
		LEVIATHAN_RES_ENTRY_DEF(Fireplace,	"fireplace");
		LEVIATHAN_RES_ENTRY_DEF(Helpful,	"helpful");
		LEVIATHAN_RES_ENTRY_DEF(Spring,		"spring");
		LEVIATHAN_RES_ENTRY_DEF(Ghost,		"ghost");
	}
	namespace sfx {
		LEVIATHAN_RES_ENTRY_DEF(Spark,			"spark");
		LEVIATHAN_RES_ENTRY_DEF(Bwall,			"bwall");
		LEVIATHAN_RES_ENTRY_DEF(Fan,			"fan");
		LEVIATHAN_RES_ENTRY_DEF(Blade,			"blade");
		LEVIATHAN_RES_ENTRY_DEF(Kannon,			"kanon");
		LEVIATHAN_RES_ENTRY_DEF(Explode0,		"explode_0");
		LEVIATHAN_RES_ENTRY_DEF(Explode1,		"explode_1");
		LEVIATHAN_RES_ENTRY_DEF(Explode2,		"explode_2");
		LEVIATHAN_RES_ENTRY_DEF(Beam,			"beam");
		LEVIATHAN_RES_ENTRY_DEF(Drill,			"drill");
		LEVIATHAN_RES_ENTRY_DEF(Spring,			"spring");
		LEVIATHAN_RES_ENTRY_DEF(Splash,			"splash");
		LEVIATHAN_RES_ENTRY_DEF(Text,			"text");
		LEVIATHAN_RES_ENTRY_DEF(Select,			"select");
		LEVIATHAN_RES_ENTRY_DEF(TitleBeg,		"titlebeg");
		LEVIATHAN_RES_ENTRY_DEF(Inven,			"inven");
		LEVIATHAN_RES_ENTRY_DEF(BrokenBarrier,	"brokebarr");
		LEVIATHAN_RES_ENTRY_DEF(Damage,			"damage");
		LEVIATHAN_RES_ENTRY_DEF(Walk,			"walk");
		LEVIATHAN_RES_ENTRY_DEF(Landing,		"land");
		LEVIATHAN_RES_ENTRY_DEF(Jump,			"jump");
		LEVIATHAN_RES_ENTRY_DEF(Projectile0,	"projectile_0");
		LEVIATHAN_RES_ENTRY_DEF(Razor,			"razor");
		LEVIATHAN_RES_ENTRY_DEF(NpcDeath0,		"npc_dth_0");
	}
}

#endif // LEVIATHAN_INCLUDED_RESOURCE_ID_HPP
