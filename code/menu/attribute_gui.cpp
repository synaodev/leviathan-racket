#ifndef __EMSCRIPTEN__

#include "./attribute_gui.hpp"

#include "../imgui/imgui.h"
#include "../field/tileset_viewer.hpp"
#include "../utility/vfs.hpp"

static constexpr byte_t kTileKeyPath[]	= "./tilekey/";

attribute_gui_t::attribute_gui_t() :
	bitmask(0),
	index(0),
	files()
{
	bitmask[attrgui_bits_t::Enable] = true;
	bitmask[attrgui_bits_t::Load] = false;
	bitmask[attrgui_bits_t::Save] = false;
}

void attribute_gui_t::reset() {
	bitmask[attrgui_bits_t::Enable] = true;
	bitmask[attrgui_bits_t::Load] = false;
	bitmask[attrgui_bits_t::Save] = false;
	index = 0;
	files.clear();
}

void attribute_gui_t::handle(tileset_viewer_t& tileset_viewer, renderer_t& renderer) {
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Load", "Ctrl+L", nullptr, bitmask[attrgui_bits_t::Enable])) {
			bitmask[attrgui_bits_t::Load] = true;
			bitmask[attrgui_bits_t::Save] = false;
			bitmask[attrgui_bits_t::Enable] = false;
			index = 0;
		} else if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, bitmask[attrgui_bits_t::Enable])) {
			bitmask[attrgui_bits_t::Load] = false;
			bitmask[attrgui_bits_t::Save] = true;
			bitmask[attrgui_bits_t::Enable] = false;
			index = 0;
		} else if (ImGui::MenuItem("Clear", "Ctrl+Q", nullptr, bitmask[attrgui_bits_t::Enable])) {
			bitmask[attrgui_bits_t::Load] = false;
			bitmask[attrgui_bits_t::Save] = false;
			bitmask[attrgui_bits_t::Enable] = false;
			index = 0;
			tileset_viewer.reset();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
	if (bitmask[attrgui_bits_t::Load]) {
		ImGui::Begin("File Dialogue");
		if (files.empty()) {
			files = vfs::file_list(kTileKeyPath);
		}
		ImGui::ListBox(
			"Files", &index,
			[] (optr_t data, sint_t index, const byte_t** output) {
				if (data == nullptr or output == nullptr) {
					return false;
				}
				const std::vector<std::string>* files = reinterpret_cast<std::vector<std::string>*>(data);
				if (index >= static_cast<sint_t>(files->size())) {
					return false;
				}
				*output = files->at(index).c_str();
				return true;
			},
			&files, static_cast<sint_t>(files.size())
		);
		if (ImGui::Button("Load")) {
			tileset_viewer.load(files[index], renderer);
			this->reset();
		} else if (ImGui::Button("Cancel")) {
			this->reset();
		}
		ImGui::End();
	} else if (bitmask[attrgui_bits_t::Save]) {
		tileset_viewer.save();
		this->reset();
	}
}

bool attribute_gui_t::active() const {
	return bitmask[attrgui_bits_t::Load] or bitmask[attrgui_bits_t::Save];
}

#endif // __EMSCRIPTEN__