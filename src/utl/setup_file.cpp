#include "./setup_file.hpp"

#include <fstream>

setup_chunk_t::setup_chunk_t(const std::string& title) : 
	title(title),
	data() 
{

}

std::string setup_chunk_t::get(const std::string& key) const {
	for (auto&& pair : data) {
		if (pair.first == key) {
			return pair.second;
		}
	}
	return std::string();
}

std::string setup_chunk_t::get(arch_t index) const {
	if (index < data.size()) {
		return data.at(index).second;
	}
	return std::string();
}

arch_t setup_chunk_t::get_length() const {
	return data.size();
}

const std::string& setup_chunk_t::get_title() const {
	return title;
}

void setup_chunk_t::set(const std::string& key, const std::string& value) {
	for (auto&& pair : data) {
		if (pair.first == key) {
			pair.second = value;
			return;
		}
	}
	data.emplace_back(key, value);
}

void setup_chunk_t::set(std::pair<std::string, std::string>& kvp) {
	for (auto&& pair : data) {
		if (pair.first == kvp.first) {
			pair.second = kvp.second;
			return;
		}
	}
	data.push_back(kvp);
}

bool setup_chunk_t::swap(const std::string& lhk, const std::string& rhk) {
	std::pair<std::string, std::string>* lhp = nullptr;
	std::pair<std::string, std::string>* rhp = nullptr;
	for (auto&& pair : data) {
		if (pair.first == lhk) {
			lhp = &pair;
		} else if (pair.first == rhk) {
			rhp = &pair;
		}
	}
	if (lhp != nullptr and rhp != nullptr) {
		std::swap(lhp->second, rhp->second);
		return true;
	}
	return false;
}

void setup_chunk_t::write_to(std::string& buffer) const {
	for (auto&& pair : data) {
		buffer.append(pair.first + " = " + pair.second + '\n');
	}
}

setup_file_t::setup_file_t() :
	origin(),
	data(),
	locale() 
{
	
}

bool setup_file_t::load(const std::string& full_path) {
	data.clear();
	origin = full_path;
	std::ifstream file(full_path, std::ios::binary);
	if (file.is_open()) {
		return this->read(file);
	}
	return false;
}

bool setup_file_t::save() {
	return this->save(origin);
}

bool setup_file_t::save(const std::string& full_path) {
	std::ofstream file(full_path, std::ios::binary);
	if (file.is_open()) {
		return this->write(file);
	}
	return false;
}

void setup_file_t::clear(const std::string& full_path) {
	origin = full_path;
	data.clear();
}

void setup_file_t::clear() {
	this->clear("");
}

bool setup_file_t::exists(const std::string& title) const {
	for (auto&& chunk : data) {
		if (title == chunk.get_title()) {
			bool_t result = false;
			std::stringstream ss(chunk.get(0));
			ss >> result;
			return result;
		}
	}
	return false;
}

arch_t setup_file_t::size() const {
	return data.size();
}

bool setup_file_t::swap(const std::string& title, const std::string& lhk, const std::string& rhk) {
	for (auto&& chunk : data) {
		if (chunk.get_title() == title) {
			return chunk.swap(lhk, rhk);
		}
	}
	return false;
}

bool setup_file_t::read(std::ifstream& file) {
	std::string line;
	std::string title = "NONE";
	while (std::getline(file, line)) {
		std::pair<std::string, std::string> kvp = parse(line);
		if (!kvp.first.empty()) {
			if (kvp.first[0] != '!') {
				for (auto&& chunk : data) {
					if (chunk.get_title() == title) {
						chunk.set(kvp);
					}
				}
			} else {
				data.emplace_back(kvp.second);
				title = kvp.second;
			}
		}
	}
	return true;
}

bool setup_file_t::write(std::ofstream& file) const {
	for (auto&& chunk : data) {
		std::string buffer = "\n[" + chunk.get_title() + "]\n\n";
		chunk.write_to(buffer);
		file.write(buffer.c_str(), buffer.size());
	}
	return true;
}

std::pair<std::string, std::string> setup_file_t::parse(const std::string& line) const {
	if (line.size() > 0 and line[0] != '#' and line[0] != ';' and line[0] != '[' and line[0] != '\r' and line[0] != '\n') {
		arch_t index = 0;
		while (std::isspace(line[index], locale)) {
			index++;
		}
		const arch_t begin = index;
		while (!std::isspace(line[index], locale) and line[index] != '=') {
			index++;
		}
		const std::string key = line.substr(begin, index - begin);
		while (std::isspace(line[index], locale) or line[index] == '=') {
			index++;
		}
		const std::string value = line.substr(index, line.size() - index);
		return std::make_pair(key, value);

	} else if (line[0] == '[') {
		arch_t index = 1;
		const arch_t begin = index;
		while (!std::isspace(line[index], locale) and line[index] != ']') {
			index++;
		}
		const std::string title = line.substr(begin, index - begin);
		return std::make_pair("!", title);
	}

	return std::make_pair(std::string(), std::string());
}