#include "./attribute-viewer.hpp"

#ifdef LEVIATHAN_USES_META

#include "./tileset-viewer.hpp"
#include "../resource/vfs.hpp"

#if defined(LEVIATHAN_TOOLCHAIN_MSVC) && !defined(_CRT_SECURE_NO_WARNINGS)
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <imgui/imgui.h>

attribute_viewer_t::attribute_viewer_t() {
	bitmask[flags_t::Enabled] = true;
	bitmask[flags_t::Loading] = false;
	bitmask[flags_t::Saving] = false;
}

void attribute_viewer_t::reset() {
	bitmask[flags_t::Enabled] = true;
	bitmask[flags_t::Loading] = false;
	bitmask[flags_t::Saving] = false;
	index = 0;
	files.clear();
}

void attribute_viewer_t::handle(tileset_viewer_t& tileset_viewer, renderer_t& renderer) {
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Load", "Ctrl+L", nullptr, bitmask[flags_t::Enabled])) {
			bitmask[flags_t::Loading] = true;
			bitmask[flags_t::Saving] = false;
			bitmask[flags_t::Enabled] = false;
			index = 0;
		} else if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, bitmask[flags_t::Enabled])) {
			bitmask[flags_t::Loading] = false;
			bitmask[flags_t::Saving] = true;
			bitmask[flags_t::Enabled] = false;
			index = 0;
		} else if (ImGui::MenuItem("Clear", "Ctrl+Q", nullptr, bitmask[flags_t::Enabled])) {
			bitmask[flags_t::Loading] = false;
			bitmask[flags_t::Saving] = false;
			bitmask[flags_t::Enabled] = false;
			index = 0;
			tileset_viewer.reset();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	if (bitmask[flags_t::Loading]) {
		ImGui::Begin("File Dialogue");
		if (files.empty()) {
			const std::string tilekey_path = vfs_t::resource_path(vfs_resource_path_t::TileKey);
			files = vfs_t::file_list(tilekey_path);
		}
		ImGui::ListBox(
			"Files", &index,
			[] (void_t data, sint_t index, const byte_t** output) {
				if (!data or !output) {
					return false;
				}
				const auto files = reinterpret_cast<std::vector<std::string>*>(data);
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
	} else if (bitmask[flags_t::Saving]) {
		tileset_viewer.save();
		this->reset();
	}
}

bool attribute_viewer_t::active() const {
	return bitmask[flags_t::Loading] or bitmask[flags_t::Saving];
}

#endif
