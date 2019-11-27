// Scripting Index

typedef int8_t   sint8;
typedef int16_t  sint16;
typedef int32_t  sint32;
typedef int64_t  sint64;
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef float	 real32;
typedef double	 real64;

namespace std
{
	class string;
	typedef void(*event)();
}

namespace sys
{
	void print(const std::string& statement);
	void wait(real32 seconds);
	void wait();
	void suspend();

	void setFlag(size_t index, bool value);
	void setFlag(size_t index);
	bool getFlag(size_t index);

	void setItem(sint32 type, sint32 count, sint32 limit, sint32 optor);
	void setItemLimit(sint32 type, sint32 limit);
	void setItemOptor(sint32 type, sint32 optor);
	void addItem(sint32 type, sint32 count, sint32 limit);
	void subItem(sint32 type, sint32 count);
	void ridItem(sint32 type);
	sint32 getItemCount(sint32 type);

	void setItemPtrIndex(size_t index);
	void setItemPtrIndex();
	size_t getItemPtrIndex();
	size_t getMaxItems();

	void lock();
	void freeze();
	void unlock();
	void reset();
	void quit();
	void setField(const std::string& room, sint32 id);
	void setField(const std::string& room, sint32 id, std::event event);
	void load();
	void save();
	void setFileIndex(size_t index);
	void pushWidget(std::event event, sint32 flags);
	void popWidget();
	size_t getFileIndex();
	sint32 getRand(sint32 low, sint32 high);
	real32 getRand(real32 low, real32 high);

	bool getKeyPress(arch_t action);
	bool getKeyHeld(arch_t action);

	std::string locale(const std::string& key, size_t index);
	std::string locale(const std::string& key, size_t first, size_t last);
	size_t lenloc(const std::string& key);
}

namespace msg
{
	void fadeIn();
	void fadeOut();
	void setFieldText(const std::string& text);
	void setFieldText();
	void setHighlight(real32 r, real32 g, real32 b);
	void setCard();
	void setCard(const std::string& text, size_t font, bool cenx, bool ceny, real32 x, real32 y);

	void setFace();
	void setFace(sint32 index, sint32 type);
	void setDelay(float seconds);
	void topBox();
	void lowBox();
	void say(const std::string& words);
	void clear();
	void close();
	void ask(const std::string& first, const std::string& second);
	void ask(const std::string& first, const std::string& second, const std::string& third);
	void ask(const std::string& first, const std::string& second, const std::string& third, const std::string& fourth);
	size_t getAnswer();
}

namespace aud
{
	void play(const std::string& id, size_t channel);
	void play(const std::string& id);
	void pause(size_t channel);
	void pause();
	void resume(size_t channel);
	void resume();
	void stop(size_t channel);
	void stop();
	void setVolume(real32 volume);
	real32 getVolume();
}

namespace pxt
{
	bool load(const std::string& tune);
	bool load(const std::string& tune, real32 start, real32 fade);
	void play(real32 start, real32 fade);
	void stop();
	void exit();
	void fade(real32 seconds);
	void redo(real32 seconds);
	void loop(bool state);
	bool isPlaying();
	void setVolume(real32 volume);
	real32 getVolume();
}

namespace ctx
{
	void animate(sint32 id, sint32 anim, sint32 direction);
	void spawn(const std::string& name, real32 x, real32 y, std::event event, sint32 flags, sint32 id, sint32 dir);
	void destroy(sint32 id);
	void kill(sint32 id);
	void setFlag(sint32 id, sint32 flag, bool value);
	void setState(sint32 id, sint32 state);
	void setEvent(sint32 id, std::event event);
	bool getStill(sint32 id);
	void smoke(real32 x, real32 y, size_t count);
	void shrapnel(real32 x, real32 y, size_t count);
}

namespace nao
{
	void show(bool state);
	void animate(sint32 state, sint32 direction);
	void teleport(real32 x, real32 y);
	void stop();
	void bump(sint32 direction);
	void lifeUp(sint32 amount);
	void lifeBoost(sint32 amount);
	void angelicUp(sint32 amount);
	void setEquip(sint32 flag, bool value);
}

namespace cam
{
	void setFocus();
	void setFocus(sint32 id);
	void setAngle();
	void setAngle(real32 angle);
	void setQuake(real32 factor);
	void setQuake(real32 factor, real32 seconds);
	void setZoom(real32 factor);
	void setZoom();
}