#ifndef LEVIATHAN_INCLUDED_UTILITY_SETUP_FILE_HPP
#define LEVIATHAN_INCLUDED_UTILITY_SETUP_FILE_HPP

#include <array>
#include <vector>
#include <string>
#include <iosfwd>
#include <sstream>
#include <locale>
#include <fmt/format.h>

#include "../types.hpp"

struct setup_chunk_t {
public:
	setup_chunk_t(const std::string& title);
	setup_chunk_t() = default;
	setup_chunk_t(const setup_chunk_t&) = default;
	setup_chunk_t& operator=(const setup_chunk_t&) = default;
	setup_chunk_t(setup_chunk_t&&) = default;
	setup_chunk_t& operator=(setup_chunk_t&&) = default;
	~setup_chunk_t() = default;
public:
	arch_t get_length() const;
	const std::string& get_title() const;
	std::string get(const std::string& key) const;
	std::string get(arch_t index) const;
	void set(const std::string& key, const std::string& value);
	void set(std::pair<std::string, std::string>& kvp);
	void write_to(std::string& buffer) const;
	bool swap(const std::string& lhk, const std::string& rhk);
private:
	std::string title;
	std::vector<std::pair<std::string, std::string> > data;
};

struct setup_file_t : public not_copyable_t {
public:
	setup_file_t();
	~setup_file_t() = default;
public:
	bool load(const std::string& full_path);
	bool save(const std::string& full_path);
	bool save();
	void clear(const std::string& full_path);
	void clear();
	bool exists(const std::string& title) const;
	arch_t size() const;
	bool swap(const std::string& title, const std::string& lhk, const std::string& rhk);
	bool read(std::ifstream& file);
	bool write(std::ofstream& file) const;
	std::pair<std::string, std::string> parse(const std::string& line) const;
	template<typename T> void get(const std::string& title, const std::string& key, T& value) const;
	template<typename T> void get(const std::string& title, const std::string& key, std::vector<T>& value) const;
	template<typename T, arch_t L> void get(const std::string& title, const std::string& key, std::array<T, L>& value) const;
	template<typename T, glm::length_t L, glm::qualifier Q> void get(const std::string& title, const std::string& key, glm::vec<L, T, Q>& value) const;
	template<typename T> void get(arch_t number, const std::string& key, T& value) const;
	template<typename T> void get(arch_t number, const std::string& key, std::vector<T>& value) const;
	template<typename T, arch_t L> void get(arch_t number, const std::string& key, std::array<T, L>& value) const;
	template<typename T, glm::length_t L, glm::qualifier Q> void get(arch_t number, const std::string& key, glm::vec<L, T, Q>& value) const;
	template<typename T> void set(const std::string& title, const std::string& key, const T& value);
	template<typename T> void set(const std::string& title, const std::string& key, const std::vector<T>& value);
	template<typename T, glm::length_t L, glm::qualifier Q> void set(const std::string& title, const std::string& key, const glm::vec<L, T, Q>& value);
	template<typename T> T convert_to(const std::string& input) const;
private:
	std::string origin;
	std::vector<setup_chunk_t> data;
	std::locale locale;
};

template<typename T>
inline void setup_file_t::get(const std::string& title, const std::string& key, T& value) const {
	for (auto&& chunk : data) {
		if (chunk.get_title() == title) {
			const std::string str = chunk.get(key);
			if (!str.empty()) {
				value = convert_to<T>(str);
			}
			break;
		}
	}
}

template<typename T>
inline void setup_file_t::get(const std::string& title, const std::string& key, std::vector<T>& value) const {
	for (auto&& chunk : data) {
		if (chunk.get_title() == title) {
			const std::string str = chunk.get(key);
			if (!str.empty()) {
				std::string output;
				std::istringstream parser(str);
				while (std::getline(parser, output, ',')) {
					value.push_back(convert_to<T>(output));
				}
			}
			break;
		}
	}
}

template<typename T, arch_t L>
inline void setup_file_t::get(const std::string& title, const std::string& key, std::array<T, L>& value) const {
	for (auto&& chunk : data) {
		if (chunk.get_title() == title) {
			const std::string str = chunk.get(key);
			if (!str.empty()) {
				std::string output;
				std::istringstream parser(str);
				arch_t n = 0;
				while (std::getline(parser, output, ',') and n < L) {
					value[n++] = convert_to<T>(output);
				}
			}
			break;
		}
	}
}

template<typename T, glm::length_t L, glm::qualifier Q>
inline void setup_file_t::get(const std::string& title, const std::string& key, glm::vec<L, T, Q>& value) const {
	for (auto&& chunk : data) {
		if (chunk.get_title() == title) {
			const std::string str = chunk.get(key);
			if (!str.empty()) {
				std::string output;
				std::istringstream parser(str);
				glm::length_t n = 0;
				while (std::getline(parser, output, ',') and n < L) {
					value[n++] = convert_to<T>(output);
				}
			}
			break;
		}
	}
}

template<typename T>
inline void setup_file_t::get(arch_t index, const std::string& key, T& value) const {
	const std::string str = data[index].get(key);
	if (!str.empty()) {
		value = convert_to<T>(str);
	}
}

template<typename T>
inline void setup_file_t::get(arch_t index, const std::string& key, std::vector<T>& value) const {
	const std::string str = data[index].get(key);
	if (!str.empty()) {
		std::string output;
		std::istringstream parser(str);
		while (std::getline(parser, output, ',')) {
			value.push_back(convert_to<T>(output));
		}
	}
}

template<typename T, arch_t L>
inline void setup_file_t::get(arch_t index, const std::string& key, std::array<T, L>& value) const {
	const std::string str = data[index].get(key);
	if (!str.empty()) {
		std::string output;
		std::istringstream parser(str);
		arch_t n = 0;
		while (std::getline(parser, output, ',') and n < L) {
			value[n++] = convert_to<T>(output);
		}
	}
}

template<typename T, glm::length_t L, glm::qualifier Q>
inline void setup_file_t::get(arch_t index, const std::string& key, glm::vec<L, T, Q>& value) const {
	const std::string str = data[index].get(key);
	if (!str.empty()) {
		std::string output;
		std::istringstream parser(str);
		glm::length_t n = 0;
		while (std::getline(parser, output, ',') and n < L) {
			value[n++] = convert_to<T>(output);
		}
	}
}

template<typename T>
inline void setup_file_t::set(const std::string& title, const std::string& key, const T& value) {
	// Find and set chunk key-value-pair if it exists
	for (auto&& chunk : data) {
		if (chunk.get_title() == title) {
			chunk.set(key, fmt::format("{}", value));
			return;
		}
	}
	// If chunk doesn't exist, create it
	setup_chunk_t chunk(title);
	chunk.set(key, fmt::format("{}", value));
	data.push_back(chunk);
}

template<typename T>
inline void setup_file_t::set(const std::string& title, const std::string& key, const std::vector<T>& value) {
	// Find and set chunk key-value-pair if it exists
	for (auto&& chunk : data) {
		if (chunk.get_title() == title) {
			std::string buffer;
			for (arch_t i = 0; i < value.size() - 1; ++i) {
				buffer += fmt::format("{}", value.at(i)) + ", ";
			}
			buffer += fmt::format("{}", value.back());
			chunk.set(key, buffer);
			return;
		}
	}

	// If chunk doesn't exist, create it
	setup_chunk_t chunk(title);
	std::string buffer;
	for (arch_t i = 0; i < value.size() - 1; ++i) {
		buffer += fmt::format("{}", value.at(i)) + ", ";
	}
	buffer += fmt::format("{}", value.back());
	chunk.set(key, buffer);
	data.push_back(chunk);
}

template<typename T, glm::length_t L, glm::qualifier Q>
inline void setup_file_t::set(const std::string& title, const std::string& key, const glm::vec<L, T, Q>& value) {
	// Find and set chunk key-value-pair if it exists
	for (auto&& chunk : data) {
		if (chunk.get_title() == title) {
			std::string buffer;
			for (glm::length_t it = 0; it < L - 1; ++it) {
				buffer += fmt::format("{}", value[it]) + ", ";
			}
			buffer += fmt::format("{}", value[L - 1]);
			chunk.set(key, buffer);
			return;
		}
	}

	// If chunk doesn't exist, create it
	setup_chunk_t chunk(title);
	std::string buffer;
	for (glm::length_t it = 0; it < L - 1; ++it) {
		buffer += fmt::format("{}", value[it]) + ", ";
	}
	buffer += fmt::format("{}", value[L - 1]);
	chunk.set(key, buffer);
	data.push_back(chunk);
}

template<>
inline sint16_t setup_file_t::convert_to<sint16_t>(const std::string& input) const {
	sint16_t value;
	std::stringstream ss(input);
	ss >> value;
	return value;
}

template<>
inline uint16_t setup_file_t::convert_to<uint16_t>(const std::string& input) const {
	uint16_t value;
	std::stringstream ss(input);
	ss >> value;
	return value;
}

template<>
inline sint32_t setup_file_t::convert_to<sint32_t>(const std::string& input) const {
	sint32_t value;
	std::stringstream ss(input);
	ss >> value;
	return value;
}

template<>
inline uint32_t setup_file_t::convert_to<uint32_t>(const std::string& input) const {
	uint32_t value;
	std::stringstream ss(input);
	ss >> value;
	return value;
}

template<>
inline sint64_t setup_file_t::convert_to<sint64_t>(const std::string& input) const {
	sint64_t value;
	std::stringstream ss(input);
	ss >> value;
	return value;
}

template<>
inline uint64_t setup_file_t::convert_to<uint64_t>(const std::string& input) const {
	uint64_t value;
	std::stringstream ss(input);
	ss >> value;
	return value;
}

template<>
inline real32_t setup_file_t::convert_to<real32_t>(const std::string& input) const {
	real32_t value;
	std::stringstream ss(input);
	ss >> value;
	return value;
}

template<>
inline real64_t setup_file_t::convert_to<real64_t>(const std::string& input) const {
	real64_t value;
	std::stringstream ss(input);
	ss >> value;
	return value;
}

template<>
inline byte_t setup_file_t::convert_to<byte_t>(const std::string& input) const {
	return input[0];
}

template<>
inline std::string setup_file_t::convert_to<std::string>(const std::string& input) const {
	return input;
}

#endif // LEVIATHAN_INCLUDED_UTILITY_SETUP_FILE_HPP
