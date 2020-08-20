#include "./kernel.hpp"
#include "./input.hpp"

#include <fstream>
#include <angelscript.h>

#include "../utility/setup_file.hpp"

static const byte_t kFlagProgsName[] = "_prog.bin";
static const byte_t kFlagCheckName[] = "_check.bin";
static const byte_t kTheFirstField[] = "naomi";

static constexpr sint_t kTheFirstIDN = 9999;
static constexpr arch_t kMaxItemList = 30;
static constexpr arch_t kMaxFlagList = 128;
static constexpr arch_t kMaxFlagBits = sizeof(uint64_t) * 8;

kernel_t::kernel_t() :
	bitmask(0),
	file_index(0),
	timer(0.0),
	field(),
	identity(0),
	function(nullptr),
	cursor(0),
	item_ptr(nullptr),
	items(kMaxItemList, glm::zero<glm::ivec4>()),
	flags(kMaxFlagList, 0)
{
	
}

kernel_t::~kernel_t() {
	if (function != nullptr) {
		function->Release();
		function = nullptr;
	}
}

void kernel_t::reset() {
	bitmask.reset();
	bitmask[kernel_state_t::Zero] = true;
	bitmask[kernel_state_t::Field] = true;
	// bitmask[kernel_state_t::Lock] = true;
	// bitmask[kernel_state_t::Freeze] = true;
	timer = 0.0;
	cursor = glm::zero<glm::ivec2>();
	item_ptr = nullptr;
	field = kTheFirstField;
	identity = kTheFirstIDN;
	std::fill(items.begin(), items.end(), glm::zero<glm::ivec4>());
	std::fill(flags.begin(), flags.end(), 0);
}

void kernel_t::reset(const std::string& field) {
	bitmask.reset();
	bitmask[kernel_state_t::Field] = true;
	timer = 0.0;
	cursor = glm::zero<glm::ivec2>();
	item_ptr = nullptr;
	this->field = field;
	this->identity = 0;
	std::fill(items.begin(), items.end(), glm::zero<glm::ivec4>());
	std::fill(flags.begin(), flags.end(), 0);
}

void kernel_t::update(real64_t delta) {
	if (!bitmask.any()) {
		timer += delta;
	}
}

void kernel_t::read_data(const setup_file_t& file) {
	arch_t item_ptr_literal = (arch_t)-1;
	file.get("Status", "Timer", timer);
	file.get("Status", "Cursor", cursor);
	file.get("Status", "ItemPtr", item_ptr_literal);
	if (item_ptr_literal < items.size()) {
		item_ptr = &items[item_ptr_literal];
	} else {
		item_ptr = nullptr;
	}
	for (arch_t it = 0; it < items.size(); ++it) {
		file.get("Item", "Slot_" + std::to_string(it), items[it]);
	}
}

bool kernel_t::read_stream(const std::string& path) {
	const byte_t* name = bitmask[kernel_state_t::Check] ? kFlagCheckName : kFlagProgsName;
	std::ifstream ifs(path + std::to_string(file_index) + name, std::ifstream::binary);
	if (ifs.is_open()) {
		ifs.read(
			reinterpret_cast<byte_t*>(flags.data()),
			flags.size() * sizeof(decltype(flags)::value_type)
		);
		return true;
	}
	return false;
}

void kernel_t::write_data(setup_file_t& file) const {
	file.set("Status", "Timer", timer);
	file.set("Status", "Cursor", cursor);
	if (item_ptr != nullptr) {
		sint64_t item_ptr_literal = std::distance(&items[0], (const glm::ivec4*)item_ptr);
		file.set("Status", "ItemPtr", item_ptr_literal);
	} else {
		file.set("Status", "ItemPtr", -1);
	}
	for (arch_t it = 0; it < items.size(); ++it) {
		file.set("Item", "Slot_" + std::to_string(it), items[it]);
	}
}

bool kernel_t::write_stream(const std::string& path) const {
	const byte_t* name = bitmask[kernel_state_t::Check] ? kFlagCheckName : kFlagProgsName;
	std::ofstream ofs(path + std::to_string(file_index) + name, std::ofstream::binary);
	if (ofs.is_open()) {
		ofs.write(
			reinterpret_cast<const byte_t*>(flags.data()),
			flags.size() * sizeof(decltype(flags)::value_type)
		);
		return true;
	}
	return false;
}

void kernel_t::boot() {
	bitmask[kernel_state_t::Boot] = true;
}

void kernel_t::quit() {
	bitmask[kernel_state_t::Quit] = true;
}

void kernel_t::lock() {
	bitmask[kernel_state_t::Lock] = true;
	bitmask[kernel_state_t::Freeze] = false;
}

void kernel_t::freeze() {
	bitmask[kernel_state_t::Lock] = true;
	bitmask[kernel_state_t::Freeze] = true;
}

void kernel_t::unlock() {
	if (!bitmask[kernel_state_t::Field] and !bitmask[kernel_state_t::Boot] and !bitmask[kernel_state_t::Load]) {
		bitmask[kernel_state_t::Lock] = false;
		bitmask[kernel_state_t::Freeze] = false;
	}
}

void kernel_t::load_progress() {
	bitmask[kernel_state_t::Load] = true;
	bitmask[kernel_state_t::Check] = false;
}

void kernel_t::save_progress() {
	bitmask[kernel_state_t::Save] = true;
	bitmask[kernel_state_t::Check] = false;
}

void kernel_t::load_checkpoint() {
	bitmask[kernel_state_t::Load] = true;
	bitmask[kernel_state_t::Check] = true;
}

void kernel_t::save_checkpoint() {
	bitmask[kernel_state_t::Save] = true;
	bitmask[kernel_state_t::Check] = true;
}

void kernel_t::finish_file_operation() {
	bitmask[kernel_state_t::Boot] = false;
	bitmask[kernel_state_t::Zero] = false;
	bitmask[kernel_state_t::Load] = false;
	bitmask[kernel_state_t::Save] = false;
	bitmask[kernel_state_t::Check] = false;
}

void kernel_t::buffer_field(const std::string& field, sint_t identity) {
	bitmask[kernel_state_t::Field] = true;
	this->field = field;
	this->identity = identity;
}

void kernel_t::buffer_field(const std::string& field, sint_t identity, asIScriptFunction* function) {
	bitmask[kernel_state_t::Field] = true;
	this->field = field;
	this->identity = identity;
	if (this->function != nullptr) {
		this->function->Release();
	}
	this->function = function;
}

void kernel_t::finish_field() {
	bitmask[kernel_state_t::Boot] = false;
	bitmask[kernel_state_t::Zero] = false;
	bitmask[kernel_state_t::Field] = false;
	if (function != nullptr) {
		function->Release();
		function = nullptr;
	}
}

bool kernel_t::has(kernel_state_t state) const {
	return bitmask[state];
}

void kernel_t::set_file_index(arch_t file_index) {
	this->file_index = file_index;
}

void kernel_t::set_cursor(glm::ivec2 cursor) {
	this->cursor = cursor;
}

void kernel_t::set_item(sint_t type, sint_t count, sint_t limit, sint_t optor) {
	for (auto&& item : items) {
		if (item.x == type) {
			item.y = glm::clamp(count, 0, limit);
			item.z = limit;
			item.w = optor;
			break;
		}
	}
}

void kernel_t::set_item_limit(sint_t type, sint_t limit) {
	for (auto&& item : items) {
		if (item.x == type) {
			item.y = glm::clamp(item.y, 0, limit);
			item.z = limit;
		}
	}
}

void kernel_t::set_item_optor(sint_t type, sint_t optor) {
	for (auto&& item : items) {
		if (item.x == type) {
			item.w = optor;
			break;
		}
	}
}

void kernel_t::add_item(sint_t type, sint_t count, sint_t limit) {
	for (auto&& item : items) {
		if (!item.x) {
			item.x = type;
			item.y = glm::clamp(count, 0, limit);
			item.z = limit;
			break;
		} else if (item.x == type) {
			item.y = glm::clamp(item.y + count, 0, limit);
			break;
		}
	}
}

void kernel_t::sub_item(sint_t type, sint_t count) {
	for (auto&& item : items) {
		if (item.x == type) {
			item.y -= count;
			if (item.y <= 0 and !item.w) {
				arch_t it = std::distance(&items[0], &item);
				this->shift_item(it);
			}
			break;
		}
	}
}

void kernel_t::rid_item(sint_t type) {
	for (auto&& item : items) {
		if (item.x == type) {
			arch_t it = std::distance(&items[0], &item);
			item = glm::zero<glm::ivec4>();
			this->shift_item(it);
			break;
		}
	}
}

void kernel_t::shift_item(arch_t deleted) {
	if (item_ptr != nullptr) {
		arch_t item_ptr_literal = item_ptr - &items[0];
		if (item_ptr_literal == deleted) {
			item_ptr = nullptr;
		} else if (item_ptr_literal > deleted) {
			item_ptr = &items[item_ptr_literal - 1];
		}
	}
	arch_t total = items.size() - deleted;
	arch_t times = 0;
	while (times < total) {
		arch_t index = times + deleted;
		if (index < items.size() - 1) {
			items[index] = items[index + 1];
		} else if(index >= items.size() - 1) {
			items[index] = glm::ivec4(0);
		}
		++times;
	}
}

void kernel_t::set_item_ptr_index() {
	item_ptr = nullptr;
}

void kernel_t::set_item_ptr_index(arch_t index) {
	if (index < items.size()) {
		item_ptr = &items[index];
	} else {
		item_ptr = nullptr;
	}
}

void kernel_t::set_flag(arch_t index, bool value) {
	arch_t ondex = index / kMaxFlagBits;
	if (ondex < flags.size()) {
		uint64_t convert = 1ULL << (static_cast<uint64_t>(index) % (kMaxFlagBits - 1));
		if (value) {
			flags[ondex] |= convert;
		} else {
			flags[ondex] &= ~convert;
		}
	}
}

arch_t kernel_t::get_file_index() const {
	return file_index;
}

const std::string& kernel_t::get_field() const {
	return field;
}

sint_t kernel_t::get_identity() const {
	return identity;
}

asIScriptFunction* kernel_t::get_function() const {
	return function;
}

glm::ivec2 kernel_t::get_cursor() const {
	return cursor;
}

arch_t kernel_t::get_cursor_index() const {
	return static_cast<arch_t>(cursor.x) + static_cast<arch_t>(cursor.y) * 6;
}

sint_t kernel_t::get_item_count(sint_t type) const {
	for (auto&& item : items) {
		if (item.x == type) {
			return item.y;
		}
	}
	return -1;
}

glm::ivec4 kernel_t::get_item_at(arch_t index) const {
	if (index < items.size()) {
		return items[index];
	}
	return glm::zero<glm::ivec4>();
}

arch_t kernel_t::get_item_ptr_index() const {
	if (item_ptr != nullptr) {
		return std::distance(&items[0], (const glm::ivec4*)item_ptr);
	}
	return (arch_t)-1;
}

arch_t kernel_t::get_max_items() const {
	return items.size();
}

glm::ivec4* kernel_t::get_item_ptr() {
	return item_ptr;
}

const glm::ivec4* kernel_t::get_item_ptr() const {
	return item_ptr;
}

bool kernel_t::get_flag(arch_t index) const {
	arch_t ondex = index / kMaxFlagBits;
	if (ondex < flags.size()) {
		uint64_t convert = 1ULL << (static_cast<uint64_t>(index) % (kMaxFlagBits - 1));
		return flags[ondex] & convert;
	}
	return false;
}