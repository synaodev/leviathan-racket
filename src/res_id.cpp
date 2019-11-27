#include "./res_id.hpp"

#define RES_ID_INTERN_DEF(NAME, STR)  extern const char NAME[] = STR

namespace res {
	namespace anim {
		RES_ID_INTERN_DEF(Heads,		"heads");
		RES_ID_INTERN_DEF(Naomi,		"naomi");

		RES_ID_INTERN_DEF(Foxie,		"foxie");
		RES_ID_INTERN_DEF(GyoShin,		"gyoshin");
		RES_ID_INTERN_DEF(Kyoko,		"kyoko");
		RES_ID_INTERN_DEF(Nauzika,		"nauzika");
		RES_ID_INTERN_DEF(Senma,		"senma");
		RES_ID_INTERN_DEF(Shoshi,		"shoshi");
		RES_ID_INTERN_DEF(Barrier,		"barrier");
		RES_ID_INTERN_DEF(Blast,		"blast");
		RES_ID_INTERN_DEF(DashFlash,	"dashflash");
		RES_ID_INTERN_DEF(Droplet,		"droplet");
		RES_ID_INTERN_DEF(Shrapnel,		"shrapnel");
		RES_ID_INTERN_DEF(Smoke,		"smoke");
		RES_ID_INTERN_DEF(Splash,		"splash");
		RES_ID_INTERN_DEF(Bubble,		"bubble");
		RES_ID_INTERN_DEF(Frontier,		"frontier");
		RES_ID_INTERN_DEF(Hammer,		"hammer");
		RES_ID_INTERN_DEF(HolyLance,	"holylance");
		RES_ID_INTERN_DEF(Kannon,		"kannon");
		RES_ID_INTERN_DEF(NailRay,		"nailray");
		RES_ID_INTERN_DEF(WolfVulcan,	"wolfvulcan");
		RES_ID_INTERN_DEF(Austere,		"austere");

		RES_ID_INTERN_DEF(Autovator,	"autovtr");
		RES_ID_INTERN_DEF(Chest,		"chest");
		RES_ID_INTERN_DEF(Computer,		"computer");
		RES_ID_INTERN_DEF(Death,		"death");
		RES_ID_INTERN_DEF(Door,			"door");
		RES_ID_INTERN_DEF(Fireplace,	"fireplace");
		RES_ID_INTERN_DEF(Helpful,		"helpful");

		RES_ID_INTERN_DEF(Spring,		"spring");
	}
	namespace img {
		RES_ID_INTERN_DEF(Heads,		"heads");
	}
	namespace sfx {
		RES_ID_INTERN_DEF(Spark,			"spark");
		RES_ID_INTERN_DEF(Bwall,			"bwall");
		RES_ID_INTERN_DEF(Fan,				"fan");
		RES_ID_INTERN_DEF(Blade,			"blade");
		RES_ID_INTERN_DEF(Kannon,			"kanon");
		RES_ID_INTERN_DEF(Explode1,			"explode_1");
		RES_ID_INTERN_DEF(Explode2,			"explode_2");
		RES_ID_INTERN_DEF(Beam,				"beam");
		RES_ID_INTERN_DEF(Drill,			"drill");
		RES_ID_INTERN_DEF(Spring,			"spring");
		RES_ID_INTERN_DEF(Splash,			"splash");
		RES_ID_INTERN_DEF(Text,				"text");
		RES_ID_INTERN_DEF(Select,			"select");
		RES_ID_INTERN_DEF(TitleBeg,			"titlebeg");
		RES_ID_INTERN_DEF(Inven,			"inven");
		RES_ID_INTERN_DEF(BrokenBarrier,	"brokebarr");
		RES_ID_INTERN_DEF(Damage,			"damage");
		RES_ID_INTERN_DEF(Walk,				"walk");
		RES_ID_INTERN_DEF(Landing,			"land");
		RES_ID_INTERN_DEF(Jump,				"jump");
		RES_ID_INTERN_DEF(Projectile0,		"projectile_0");
		RES_ID_INTERN_DEF(Razor,			"razor");
	}
}