#ifndef SYNAO_RESOURCE_ID_HPP
#define SYNAO_RESOURCE_ID_HPP

#include "./entry.hpp"

#define SYNAO_RES_GFX_DEF(NAME, STR) constexpr byte_t NAME[] = STR
#define SYNAO_RES_SFX_DEF(TAG, FILE) constexpr resource_entry_t TAG(FILE);

namespace res {
	namespace anim {
		SYNAO_RES_GFX_DEF(Heads,		"heads");
		SYNAO_RES_GFX_DEF(Naomi,		"naomi");
		SYNAO_RES_GFX_DEF(Foxie,		"foxie");
		SYNAO_RES_GFX_DEF(GyoShin,		"gyoshin");
		SYNAO_RES_GFX_DEF(Kyoko,		"kyoko");
		SYNAO_RES_GFX_DEF(Nauzika,		"nauzika");
		SYNAO_RES_GFX_DEF(Senma,		"senma");
		SYNAO_RES_GFX_DEF(Shoshi,		"shoshi");
		SYNAO_RES_GFX_DEF(Barrier,		"barrier");
		SYNAO_RES_GFX_DEF(Blast,		"blast");
		SYNAO_RES_GFX_DEF(DashFlash,	"dashflash");
		SYNAO_RES_GFX_DEF(Droplet,		"droplet");
		SYNAO_RES_GFX_DEF(Shrapnel,		"shrapnel");
		SYNAO_RES_GFX_DEF(Smoke,		"smoke");
		SYNAO_RES_GFX_DEF(Splash,		"splash");
		SYNAO_RES_GFX_DEF(Bubble,		"bubble");
		SYNAO_RES_GFX_DEF(Frontier,		"frontier");
		SYNAO_RES_GFX_DEF(Hammer,		"hammer");
		SYNAO_RES_GFX_DEF(HolyLance,	"holylance");
		SYNAO_RES_GFX_DEF(Items,		"items");
		SYNAO_RES_GFX_DEF(Kannon,		"kannon");
		SYNAO_RES_GFX_DEF(NailRay,		"nailray");
		SYNAO_RES_GFX_DEF(WolfVulcan,	"wolfvulcan");
		SYNAO_RES_GFX_DEF(Austere,		"austere");
		SYNAO_RES_GFX_DEF(Autovator,	"autovtr");
		SYNAO_RES_GFX_DEF(Chest,		"chest");
		SYNAO_RES_GFX_DEF(Computer,		"computer");
		SYNAO_RES_GFX_DEF(Death,		"death");
		SYNAO_RES_GFX_DEF(Door,			"door");
		SYNAO_RES_GFX_DEF(Fireplace,	"fireplace");
		SYNAO_RES_GFX_DEF(Helpful,		"helpful");
		SYNAO_RES_GFX_DEF(Spring,		"spring");
		SYNAO_RES_GFX_DEF(Ghost,		"ghost");
	}
	namespace img {
		SYNAO_RES_GFX_DEF(Heads, "heads");
	}
	namespace pal {
		SYNAO_RES_GFX_DEF(Heads, "heads");
	}
	namespace sfx {
		SYNAO_RES_SFX_DEF(Spark,			"spark");
		SYNAO_RES_SFX_DEF(Bwall,			"bwall");
		SYNAO_RES_SFX_DEF(Fan,				"fan");
		SYNAO_RES_SFX_DEF(Blade,			"blade");
		SYNAO_RES_SFX_DEF(Kannon,			"kanon");
		SYNAO_RES_SFX_DEF(Explode1,			"explode_1");
		SYNAO_RES_SFX_DEF(Explode2,			"explode_2");
		SYNAO_RES_SFX_DEF(Beam,				"beam");
		SYNAO_RES_SFX_DEF(Drill,			"drill");
		SYNAO_RES_SFX_DEF(Spring,			"spring");
		SYNAO_RES_SFX_DEF(Splash,			"splash");
		SYNAO_RES_SFX_DEF(Text,				"text");
		SYNAO_RES_SFX_DEF(Select,			"select");
		SYNAO_RES_SFX_DEF(TitleBeg,			"titlebeg");
		SYNAO_RES_SFX_DEF(Inven,			"inven");
		SYNAO_RES_SFX_DEF(BrokenBarrier,	"brokebarr");
		SYNAO_RES_SFX_DEF(Damage,			"damage");
		SYNAO_RES_SFX_DEF(Walk,				"walk");
		SYNAO_RES_SFX_DEF(Landing,			"land");
		SYNAO_RES_SFX_DEF(Jump,				"jump");
		SYNAO_RES_SFX_DEF(Projectile0,		"projectile_0");
		SYNAO_RES_SFX_DEF(Razor,			"razor");
	}
}

#endif // SYNAO_RESOURCE_ID_HPP