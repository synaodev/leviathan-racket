#include "./kernel.hpp"
#include "./input.hpp"

#include <fstream>
#include <angelscript.h>

#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

#include "../system/receiver.hpp"
#include "../utility/logger.hpp"
#include "../utility/setup-file.hpp"

static const byte_t kFlagProgsName[] = "_prog.bin";
static const byte_t kFlagCheckName[] = "_check.bin";

static constexpr arch_t kMaxItemList = 30;
static constexpr arch_t kMaxFlagList = 128;
static constexpr arch_t kMaxFlagBits = sizeof(uint64_t) * 8;

bool kernel_t::init(const receiver_t& receiver) {
	if (verify) {
		synao_log("Error! Kernel is already initialized!\n");
		return false;
	}
	verify = [&receiver](asIScriptFunction* function) {
		return receiver.verify(function);
	};
	items = std::vector<glm::ivec4>(kMaxItemList, glm::zero<glm::ivec4>());
	flags = std::vector<uint64_t>(kMaxFlagList, 0);
	synao_log("Kernel is ready.\n");
	return true;
}

void kernel_t::reset() {
	bitmask.reset();
	bitmask[states_t::Zero] = true;
	bitmask[states_t::Lock] = true;
	bitmask[states_t::Freeze] = true;
	timer = 0.0;
	cursor = glm::zero<glm::ivec2>();
	item_ptr = nullptr;
	field.clear();
	identity = 0;
	function.clear();
	std::fill(items.begin(), items.end(), glm::zero<glm::ivec4>());
	std::fill(flags.begin(), flags.end(), 0);
}

void kernel_t::reset(const std::string& field) {
	bitmask.reset();
	bitmask[states_t::Field] = true;
	timer = 0.0;
	cursor = glm::zero<glm::ivec2>();
	item_ptr = nullptr;
	this->field = field;
	this->identity = 0;
	this->function.clear();
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
	const byte_t* name = bitmask[states_t::Check] ? kFlagCheckName : kFlagProgsName;
	std::ifstream ifs { path + std::to_string(file_index) + name, std::ios::binary };
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
	if (item_ptr) {
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
	const byte_t* name = bitmask[states_t::Check] ? kFlagCheckName : kFlagProgsName;
	std::ofstream ofs { path + std::to_string(file_index) + name, std::ios::binary };
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
	bitmask[states_t::Boot] = true;
}

void kernel_t::quit() {
	bitmask[states_t::Quit] = true;
}

void kernel_t::lock() {
	bitmask[states_t::Lock] = true;
	bitmask[states_t::Freeze] = false;
}

void kernel_t::freeze() {
	bitmask[states_t::Lock] = true;
	bitmask[states_t::Freeze] = true;
}

void kernel_t::unlock() {
	if (!bitmask[states_t::Field] and !bitmask[states_t::Boot] and !bitmask[states_t::Load]) {
		bitmask[states_t::Lock] = false;
		bitmask[states_t::Freeze] = false;
	}
}

void kernel_t::load_progress() {
	bitmask[states_t::Load] = true;
	bitmask[states_t::Check] = false;
}

void kernel_t::save_progress() {
	bitmask[states_t::Save] = true;
	bitmask[states_t::Check] = false;
}

void kernel_t::load_checkpoint() {
	bitmask[states_t::Load] = true;
	bitmask[states_t::Check] = true;
}

void kernel_t::save_checkpoint() {
	bitmask[states_t::Save] = true;
	bitmask[states_t::Check] = true;
}

void kernel_t::finish_file_operation() {
	bitmask[states_t::Boot] = false;
	bitmask[states_t::Zero] = false;
	bitmask[states_t::Load] = false;
	bitmask[states_t::Save] = false;
	bitmask[states_t::Check] = false;
}

void kernel_t::buffer_language(const std::string& language) {
	this->language = language;
	bitmask[states_t::Language] = true;
}

void kernel_t::finish_language() {
	language.clear();
	bitmask[states_t::Boot] = true;
	bitmask[states_t::Language] = false;
}

void kernel_t::buffer_field(const std::string& field, sint_t identity) {
	bitmask[states_t::Field] = true;
	this->field = field;
	this->identity = identity;
}

void kernel_t::buffer_field(asIScriptFunction* handle, sint_t identity) {
	if (handle) {
		const std::string location = std::invoke(verify, handle);
		if (!location.empty()) {
			bitmask[states_t::Field] = true;
			bitmask[states_t::Zero] = false;
			this->identity = identity;
			this->field = location;
			this->function = handle->GetDeclaration();
		} else {
			synao_log("Error! Passed transfer function is not imported!\n");
		}
		handle->Release();
	} else {
		synao_log("Error! Passed transfer function is null!\n");
	}
}

void kernel_t::finish_field() {
	bitmask[states_t::Boot] = false;
	bitmask[states_t::Zero] = false;
	bitmask[states_t::Field] = false;
	function.clear();
}

bool kernel_t::has(states_t state) const {
	return bitmask[state];
}

void kernel_t::set_file_index(arch_t file_index) {
	this->file_index = file_index;
}

void kernel_t::set_cursor(glm::ivec2 cursor) {
	this->cursor = cursor;
}

void kernel_t::set_item(sint_t type, sint_t count, sint_t limit, sint_t option) {
	for (auto&& item : items) {
		if (item.x == type) {
			item.y = glm::clamp(count, 0, limit);
			item.z = limit;
			item.w = option;
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

void kernel_t::set_item_option(sint_t type, sint_t option) {
	for (auto&& item : items) {
		if (item.x == type) {
			item.w = option;
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
	if (item_ptr) {
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
			items[index] = glm::zero<glm::ivec4>();
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

void kernel_t::set_flag_range(arch_t from, arch_t to, bool value) {
	for (arch_t it = from; it <= to; ++it) {
		this->set_flag(it, value);
	}
}

arch_t kernel_t::get_file_index() const {
	return file_index;
}

const std::string& kernel_t::get_language() const {
	return language;
}

const std::string& kernel_t::get_field() const {
	return field;
}

sint_t kernel_t::get_identity() const {
	return identity;
}

bool kernel_t::can_transfer() const {
	return !function.empty();
}

const std::string& kernel_t::get_function() const {
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
	if (item_ptr) {
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

bool kernel_t::get_flag_range(arch_t from, arch_t to) const {
	for (arch_t it = from; it <= to; ++it) {
		if (!this->get_flag(it)) {
			return false;
		}
	}
	return true;
}
