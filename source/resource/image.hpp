#pragma once

#include <string>
#include <vector>
#include <glm/vec2.hpp>

#include "../types.hpp"

struct image_t : public not_copyable_t {
public:
	image_t() = default;
	image_t(image_t&& that) noexcept = default;
	image_t& operator=(image_t&& that) noexcept = default;
	~image_t() = default;
public:
	void clear();
	byte_t& operator[](arch_t index);
	const byte_t& operator[](arch_t index) const;
	const glm::ivec2& get_dimensions() const;
	arch_t size() const;
	bool empty() const;
public:
	static image_t generate(const std::string& full_path);
	static std::vector<image_t> generate(const std::vector<std::string>& full_paths);
private:
	glm::ivec2 dimensions {};
	std::vector<byte_t> pixels {};
};
