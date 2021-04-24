#include "./image.hpp"
#include "../utility/logger.hpp"

#include <cstring>
#include <glm/gtc/constants.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

image_t image_t::generate(const std::string& full_path) {
	image_t image;

	sint_t width = 0;
	sint_t height = 0;
	sint_t channels = 0;

	stbi_uc* data = stbi_load(
		full_path.c_str(),
		&width, &height,
		&channels,
		STBI_rgb_alpha
	);
	if (data) {
		image.dimensions = glm::ivec2(width, height);
		image.pixels.resize(
			static_cast<arch_t>(width) *
			static_cast<arch_t>(height) *
			sizeof(uint_t)
		);
		std::copy(data, data + image.pixels.size(), image.pixels.begin());
		stbi_image_free(data);
	} else {
		synao_log("Failed to load image from {}!\n", full_path);
	}

	return image;
}

std::vector<image_t> image_t::generate(const std::vector<std::string>& full_paths) {
	std::vector<image_t> images;
	for (auto&& full_path : full_paths) {
		image_t image = image_t::generate(full_path);
		if (image.empty()) {
			break;
		}
		images.push_back(std::move(image));
	}
	return images;
}

void image_t::clear() {
	dimensions = glm::zero<glm::ivec2>();
	pixels.clear();
}

byte_t& image_t::operator[](arch_t index) {
	return pixels[index];
}

const byte_t& image_t::operator[](arch_t index) const {
	return pixels[index];
}

const glm::ivec2& image_t::get_dimensions() const {
	return dimensions;
}

arch_t image_t::size() const {
	return pixels.size();
}

bool image_t::empty() const {
	return pixels.empty();
}
