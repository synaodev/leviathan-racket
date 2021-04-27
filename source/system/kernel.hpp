#pragma once

#include <vector>
#include <string>
#include <bitset>
#include <functional>
#include <glm/vec2.hpp>

#include "../utility/enums.hpp"

class asIScriptFunction;

namespace __enum_kernel_state {
	enum type : arch_t {
		Boot,
		Zero,
		Quit,
		Language,
		Field,
		Menu,
		Load,
		Save,
		Check,
		Lock,
		Freeze,
		Total
	};
}

using kernel_state_t = __enum_kernel_state::type;

struct setup_file_t;
struct input_t;
struct audio_t;
struct music_t;
struct renderer_t;
struct receiver_t;

struct kernel_t : public not_copyable_t, public not_moveable_t {
public:
	kernel_t() = default;
	~kernel_t() = default;
public:
	bool init(const receiver_t& receiver);
	void reset();
	void reset(const std::string& field);
	void update(real64_t delta);
	void read_data(const setup_file_t& file);
	bool read_stream(const std::string& path);
	void write_data(setup_file_t& file) const;
	bool write_stream(const std::string& path) const;
	void boot();
	void quit();
	void lock();
	void freeze();
	void unlock();
	void load_progress();
	void save_progress();
	void load_checkpoint();
	void save_checkpoint();
	void finish_file_operation();
	void buffer_language(const std::string& language);
	void finish_language();
	void buffer_field(const std::string& field, sint_t identity);
	void buffer_field(asIScriptFunction* handle, sint_t identity);
	void finish_field();
	bool has(kernel_state_t state) const;
	void set_file_index(arch_t file_index);
	void set_cursor(glm::ivec2 cursor);
	void set_item(sint_t type, sint_t count, sint_t limit, sint_t option);
	void set_item_limit(sint_t type, sint_t limit);
	void set_item_option(sint_t type, sint_t option);
	void add_item(sint_t type, sint_t count, sint_t limit);
	void sub_item(sint_t type, sint_t count);
	void rid_item(sint_t type);
	void shift_item(arch_t deleted);
	void set_item_ptr_index();
	void set_item_ptr_index(arch_t index);
	void set_flag(arch_t index, bool value);
	void set_flag_range(arch_t from, arch_t to, bool value);
	arch_t get_file_index() const;
	const std::string& get_language() const;
	const std::string& get_field() const;
	sint_t get_identity() const;
	bool can_transfer() const;
	const std::string& get_function() const;
	glm::ivec2 get_cursor() const;
	arch_t get_cursor_index() const;
	sint_t get_item_count(sint_t type) const;
	glm::ivec4 get_item_at(arch_t index) const;
	arch_t get_item_ptr_index() const;
	arch_t get_max_items() const;
	glm::ivec4* get_item_ptr();
	const glm::ivec4* get_item_ptr() const;
	bool get_flag(arch_t index) const;
	bool get_flag_range(arch_t from, arch_t to) const;
private:
	std::function<std::string(asIScriptFunction*)> verify {};
	std::bitset<kernel_state_t::Total> bitmask { 0 };
	arch_t file_index { 0 };
	real64_t timer { 0.0 };
	std::string language {};
	std::string field {};
	sint_t identity { 0 };
	std::string function {};
	glm::ivec2 cursor {};
	glm::ivec4* item_ptr { nullptr };
	std::vector<glm::ivec4> items {};
	std::vector<uint64_t> flags {};
};
