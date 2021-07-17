#pragma once

#include "../types.hpp"

#include <string>
#include <nlohmann/json.hpp>

struct config_t : public not_copyable_t {
public:
	config_t() = default;
	config_t(config_t&&) noexcept = default;
	config_t& operator=(config_t&&) noexcept = default;
	~config_t() = default;
public:
	void generate(const std::string& path);
	bool load(const std::string& path);
	bool save();
public:
	bool get_meta_menu() const {
		if (
			valid and
			data.contains("Setup") and
			data["Setup"].contains("MetaMenu") and
			data["Setup"]["MetaMenu"].is_boolean()
		) {
			return data["Setup"]["MetaMenu"].get<bool>();
		}
		return false;
	}
	void set_meta_menu(bool value) {
		if (valid) {
			data["Setup"]["MetaMenu"] = value;
		}
	}
	bool get_legacy_gl() const {
		if (
			valid and
			data.contains("Setup") and
			data["Setup"].contains("LegacyGL") and
			data["Setup"]["LegacyGL"].is_boolean()
		) {
			return data["Setup"]["LegacyGL"].get<bool>();
		}
		return false;
	}
	void set_legacy_gl(bool value) {
		if (valid) {
			data["Setup"]["LegacyGL"] = value;
		}
	}
	std::string get_language() const {
		if (
			valid and
			data.contains("Setup") and
			data["Setup"].contains("Language") and
			data["Setup"]["Language"].is_string()
		) {
			return data["Setup"]["Language"].get<std::string>();
		}
		return "english";
	}
	void set_language(const std::string& value) {
		if (valid) {
			data["Setup"]["Language"] = value;
		}
	}
	bool get_vertical_sync() const {
		if (
			valid and
			data.contains("Video") and
			data["Video"].contains("VerticalSync") and
			data["Video"]["VerticalSync"].is_boolean()
		) {
			return data["Video"]["VerticalSync"].get<bool>();
		}
		return false;
	}
	void set_vertical_sync(bool value) {
		if (valid) {
			data["Video"]["VerticalSync"] = value;
		}
	}
	bool get_fullscreen() const {
		if (
			valid and
			data.contains("Video") and
			data["Video"].contains("Fullscreen") and
			data["Video"]["Fullscreen"].is_boolean()
		) {
			return data["Video"]["Fullscreen"].get<bool>();
		}
		return false;
	}
	void set_fullscreen(bool value) {
		if (valid) {
			data["Video"]["Fullscreen"] = value;
		}
	}
	sint_t get_scaling() const {
		if (
			valid and
			data.contains("Video") and
			data["Video"].contains("Scaling") and
			data["Video"]["Scaling"].is_number_unsigned()
		) {
			return data["Video"]["Scaling"].get<sint_t>();
		}
		return 2;
	}
	void set_scaling(sint_t value) {
		if (valid) {
			data["Video"]["Scaling"] = value;
		}
	}
	real64_t get_framerate() const {
		if (
			valid and
			data.contains("Video") and
			data["Video"].contains("Framerate") and
			data["Video"]["Framerate"].is_number_float()
		) {
			return data["Video"]["Framerate"].get<real64_t>();
		}
		return 60.0;
	}
	void set_framerate(real64_t value) {
		if (valid) {
			data["Video"]["Framerate"] = value;
		}
	}
	real_t get_audio_volume() const {
		if (
			valid and
			data.contains("Audio") and
			data["Audio"].contains("Volume") and
			data["Audio"]["Volume"].is_number_float()
		) {
			return data["Audio"]["Volume"].get<real_t>();
		}
		return 1.0f;
	}
	void set_audio_volume(real_t value) {
		if (valid) {
			data["Audio"]["Volume"] = value;
		}
	}
	real_t get_music_volume() const {
		if (
			valid and
			data.contains("Music") and
			data["Music"].contains("Volume") and
			data["Music"]["Volume"].is_number_float()
		) {
			return data["Music"]["Volume"].get<real_t>();
		}
		return 0.35f;
	}
	void set_music_volume(real_t value) {
		if (valid) {
			data["Music"]["Volume"] = value;
		}
	}
	sint_t get_channels() const {
		if (
			valid and
			data.contains("Music") and
			data["Music"].contains("Channels") and
			data["Music"]["Channels"].is_number_unsigned()
		) {
			return data["Music"]["Channels"].get<sint_t>();
		}
		return 2;
	}
	void set_channels(sint_t value) {
		if (valid) {
			data["Music"]["Channels"] = value;
		}
	}
	sint_t get_sampling_rate() const {
		if (
			valid and
			data.contains("Music") and
			data["Music"].contains("SamplingRate") and
			data["Music"]["SamplingRate"].is_number_unsigned()
		) {
			return data["Music"]["SamplingRate"].get<sint_t>();
		}
		return 44100;
	}
	void set_sampling_rate(sint_t value) {
		if (valid) {
			data["Music"]["SamplingRate"] = value;
		}
	}
	real_t get_buffered_time() const {
		if (
			valid and
			data.contains("Music") and
			data["Music"].contains("BufferedTime") and
			data["Music"]["BufferedTime"].is_number_float()
		) {
			return data["Music"]["BufferedTime"].get<real_t>();
		}
		return 0.1f;
	}
	void set_buffered_time(real_t value) {
		if (valid) {
			data["Music"]["BufferedTime"] = value;
		}
	}
	sint_t get_keybinding(arch_t button) {
		const std::string name = get_keyboard_name(button);
		if (
			!name.empty() and
			valid and
			data.contains("Input") and
			data["Input"].contains(name) and
			data["Input"][name].is_number_integer()
		) {
			return data["Input"][name].get<sint_t>();
		}
		return -1;
	}
	void set_keybinding(arch_t button, sint_t value) {
		const std::string name = get_keyboard_name(button);
		if (!name.empty() and valid) {
			data["Input"][name] = value;
		}
	}
	sint_t get_joybinding(arch_t button) const {
		const std::string name = get_joystick_name(button);
		if (
			!name.empty() and
			valid and
			data.contains("Input") and
			data["Input"].contains(name) and
			data["Input"][name].is_number_integer()
		) {
			return data["Input"][name].get<sint_t>();
		}
		return -1;
	}
	void set_joybinding(arch_t button, sint_t value) {
		const std::string name = get_joystick_name(button);
		if (!name.empty() and valid) {
			data["Input"][name] = value;
		}
	}
	std::string get_macro_file() const {
		if (
			valid and
			data.contains("Input") and
			data["Input"].contains("Macro") and
			data["Input"]["Macro"].is_string()
		) {
			return data["Input"]["Macro"].get<std::string>();
		}
		return {};
	}
	void set_macro_file(const std::string& value) {
		if (valid) {
			data["Input"]["Macro"] = value;
		}
	}
	bool get_playback() const {
		if (
			valid and
			data.contains("Input") and
			data["Input"].contains("Playback") and
			data["Input"]["Playback"].is_boolean()
		) {
			return data["Input"]["Playback"].get<bool>();
		}
		return false;
	}
	void set_playback(bool value) {
		if (valid) {
			data["Input"]["Playback"] = value;
		}
	}
private:
	static std::string get_keyboard_name(arch_t index);
	static std::string get_joystick_name(arch_t index);
private:
	bool_t valid { false };
	nlohmann::json data {};
	std::string path {};
};
