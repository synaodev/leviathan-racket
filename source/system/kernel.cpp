#include "./kernel.hpp"
#include "./input.hpp"

#include <angelscript.h>
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <nlohmann/json.hpp>

#include "../system/receiver.hpp"
#include "../utility/logger.hpp"

namespace {
	constexpr byte_t kTimerEntry[] 		= "Timer";
	constexpr byte_t kCursorEntry[] 	= "Cursor";
	constexpr byte_t kItemPtrEntry[] 	= "ItemPtr";
	constexpr byte_t kItemListEntry[] 	= "ItemList";
	constexpr byte_t kFlagListEntry[] 	= "FlagList";
	constexpr arch_t kMaxItemList 		= 30;
	constexpr arch_t kMaxFlagList 		= 64;
	constexpr arch_t kMaxFlagBits 		= sizeof(uint64_t) * 8;
}

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

void kernel_t::read(const nlohmann::json& file) {
	if (
		file.contains(kTimerEntry) and
		file[kTimerEntry].is_number_float()
	) {
		timer = file[kTimerEntry].get<real64_t>();
	} else {
		timer = 0.0;
	}

	if (
		file.contains(kCursorEntry) and
		file[kCursorEntry].is_array() and
		file[kCursorEntry].size() == 2 and
		file[kCursorEntry][0].is_number_unsigned() and
		file[kCursorEntry][1].is_number_unsigned()
	) {
		cursor = {
			file[kCursorEntry][0].get<sint_t>(),
			file[kCursorEntry][1].get<sint_t>()
		};
	} else {
		cursor = glm::zero<glm::ivec2>();
	}

	if (
		file.contains(kItemPtrEntry) and
		file[kItemPtrEntry].is_number_unsigned()
	) {
		arch_t literal = file[kItemPtrEntry].get<arch_t>();
		if (literal < items.size()) {
			item_ptr = &items[literal];
		} else {
			item_ptr = nullptr;
		}
	} else {
		item_ptr = nullptr;
	}

	if (
		file.contains(kItemListEntry) and
		file[kItemListEntry].is_array() and
		file[kItemListEntry].size() == items.size()
	) {
		auto& list = file[kItemListEntry];
		for (arch_t x = 0; x < items.size(); ++x) {
			if (list[x].is_array()) {
				glm::length_t total = glm::min(
					items[x].length(),
					(glm::length_t)list[x].size()
				);
				for (glm::length_t y = 0; y < total; ++y) {
					if (list[x][y].is_number_unsigned()) {
						items[x][y] = list[x][static_cast<arch_t>(y)].get<sint_t>();
					} else {
						items[x][y] = 0;
					}
				}
			} else {
				items[x] = glm::zero<glm::ivec4>();
			}
		}
	} else {
		std::fill(items.begin(), items.end(), glm::zero<glm::ivec4>());
	}

	if (
		file.contains(kFlagListEntry) and
		file[kFlagListEntry].is_array() and
		file[kFlagListEntry].size() == flags.size()
	) {
		auto& list = file[kFlagListEntry];
		for (arch_t it = 0; it < flags.size(); ++it) {
			if (list[it].is_number_unsigned()) {
				flags[it] = list[it].get<uint64_t>();
			} else {
				flags[it] = 0;
			}
		}
	} else {
		std::fill(flags.begin(), flags.end(), 0);
	}
}

void kernel_t::write(nlohmann::json& file) const {
	file[kTimerEntry] = timer;
	file[kCursorEntry] = nlohmann::json::array({ cursor.x, cursor.y });
	if (item_ptr) {
		auto literal = std::distance(items.data(), (const glm::ivec4*)item_ptr);
		file[kItemPtrEntry] = static_cast<sint_t>(literal);
	} else {
		file[kItemListEntry] = -1;
	}
	{
		auto list = nlohmann::json::array();
		for (auto&& item : items) {
			auto values = nlohmann::json::array({
				item.x,
				item.y,
				item.z,
				item.w
			});
			list.push_back(values);
		}
		file[kItemListEntry] = list;
	}
	{
		auto list = nlohmann::json::array();
		for (auto&& flag : flags) {
			list.push_back(flag);
		}
		file[kFlagListEntry] = list;
	}
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
