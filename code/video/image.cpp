#include "./image.hpp"
#include "../utility/logger.hpp"

#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

image_t::image_t() :
	dimensions(0), 
	pixels()
{

}

image_t::image_t(image_t&& that) noexcept : image_t() {
	if (this != &that) {
		std::swap(dimensions, that.dimensions);
		std::swap(pixels, that.pixels);
	}
}

image_t& image_t::operator=(image_t&& that) noexcept {
	if (this != &that) {
		std::swap(dimensions, that.dimensions);
		std::swap(pixels, that.pixels);
	}
	return *this;
}

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
	if (data != nullptr) {
		image.dimensions = glm::ivec2(width, height);
		image.pixels.resize(
			static_cast<arch_t>(width) * 
			static_cast<arch_t>(height) * 
			sizeof(uint_t)
		);
		std::memcpy(&image.pixels[0], data, image.pixels.size());
		stbi_image_free(data);
	} else {
		SYNAO_LOG("Failed to load image from %s!\n", full_path.c_str());
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

glm::ivec2 image_t::get_dimensions() const {
	return dimensions;
}

arch_t image_t::size() const {
	return pixels.size();
}

bool image_t::empty() const {
	return pixels.empty();
}