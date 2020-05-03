#ifndef SYNAO_SYSTEM_KERNEL_HPP
#define SYNAO_SYSTEM_KERNEL_HPP

#include <vector>
#include <string>
#include <bitset>

#include "../utl/enums.hpp"

class asIScriptFunction;

namespace __enum_kernel_state {
	enum type : arch_t {
		Boot,
		Quit,
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

struct kernel_t : public not_copyable_t {
public:
	kernel_t();
	kernel_t(kernel_t&&) = default;
	kernel_t& operator=(kernel_t&&) = default;
	~kernel_t();
public:
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
	void buffer_field(const std::string& field, sint_t identity);
	void buffer_field(const std::string& field, sint_t identity, asIScriptFunction* function);
	void finish_field();
	bool has(kernel_state_t state) const;
	void set_file_index(arch_t file_index);
	void set_cursor(glm::ivec2 cursor);
	void set_item(sint_t type, sint_t count, sint_t limit, sint_t optor);
	void set_item_limit(sint_t type, sint_t limit);
	void set_item_optor(sint_t type, sint_t optor);
	void add_item(sint_t type, sint_t count, sint_t limit);
	void sub_item(sint_t type, sint_t count);
	void rid_item(sint_t type);
	void shift_item(arch_t deleted);
	void set_item_ptr_index();
	void set_item_ptr_index(arch_t index);
	void set_flag(arch_t index, bool value);
	arch_t get_file_index() const;
	const std::string& get_field() const;
	sint_t get_identity() const;
	asIScriptFunction* get_function() const;
	glm::ivec2 get_cursor() const;
	arch_t get_cursor_index() const;
	sint_t get_item_count(sint_t type) const;
	glm::ivec4 get_item_at(arch_t index) const;
	arch_t get_item_ptr_index() const;
	arch_t get_max_items() const;
	glm::ivec4* get_item_ptr();
	const glm::ivec4* get_item_ptr() const;
	bool get_flag(arch_t index) const;
private:
	std::bitset<kernel_state_t::Total> bitmask;
	arch_t file_index;
	real64_t timer;
	std::string field;
	sint_t identity;
	asIScriptFunction* function;
	glm::ivec2 cursor;
	glm::ivec4* item_ptr;
	std::vector<glm::ivec4> items;
	std::vector<uint64_t> flags;
};

#endif // SYNAO_SYSTEM_KERNEL_HPP