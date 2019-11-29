#include "./gfx.hpp"
#include "./glcheck.hpp"
#include "./program.hpp"
#include "./texture.hpp"
#include "./palette.hpp"
#include "./depth_buffer.hpp"
#include "./const_buffer.hpp"

gfx_t::gfx_t() :
	depth_func(gfx_cmp_func_t::Disable),
	blend_mode(blend_mode_t::Disable),
	program(nullptr),
	samplers{},
	buffers{}
{
	std::fill(samplers, samplers + kSmpUnits, nullptr);
	std::fill(buffers, buffers + kBufUnits, nullptr);
}

void gfx_t::set_depth_func(gfx_cmp_func_t depth_func) {
	if (this->depth_func != depth_func) {
		if (depth_func == gfx_cmp_func_t::Disable) {
			glCheck(glDisable(GL_DEPTH_TEST));
		} else if (this->depth_func == gfx_cmp_func_t::Disable) {
			glCheck(glEnable(GL_DEPTH_TEST));
		}
		this->depth_func = depth_func;
		if (this->depth_func != gfx_cmp_func_t::Disable) {
			glCheck(glDepthFunc(depth_func));
		}
	}
}

void gfx_t::set_blend_mode(blend_mode_t blend_mode) {
	if (this->blend_mode != blend_mode) {
		if (blend_mode == blend_mode_t::Disable) {
			glCheck(glDisable(GL_BLEND));
		} else if (this->blend_mode == blend_mode_t::Disable) {
			glCheck(glEnable(GL_BLEND));
		}
		this->blend_mode = blend_mode;
		switch (blend_mode) {
		case blend_mode_t::None:
			glCheck(glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO));
			glCheck(glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD));
			break;
		case blend_mode_t::Alpha:
			glCheck(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
			glCheck(glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD));
			break;
		case blend_mode_t::Add:
			glCheck(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE));
			glCheck(glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD));
			break;
		case blend_mode_t::Subtract:
			glCheck(glBlendFuncSeparate(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA));
			glCheck(glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD));
			break;
		case blend_mode_t::Multiply:
			glCheck(glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_DST_COLOR, GL_ZERO));
			glCheck(glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD));
			break;
		default:
			break;
		}
	}
}

void gfx_t::set_program(const program_t* program) {
	if (this->program != program) {
		this->program = program;
		if (program != nullptr) {
			if (program_t::is_version_420()) {
				glCheck(glBindProgramPipeline(program->handle));
			} else {
				glCheck(glUseProgram(program->handle));
			}			
		}
	}
}

void gfx_t::set_sampler(const texture_t* texture, arch_t index) {
	assert(index < kSmpUnits);
	if (this->samplers[index] != texture) {
		this->samplers[index] = texture;
		glCheck(glActiveTexture(GL_TEXTURE0 + static_cast<uint_t>(index)));
		if (texture != nullptr) {
			texture->assure();
			glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, texture->handle));
		}
	}
}

void gfx_t::set_sampler(const palette_t* palette, arch_t index) {
	assert(index < kSmpUnits);
	if (this->samplers[index] != palette) {
		this->samplers[index] = palette;
		glCheck(glActiveTexture(GL_TEXTURE0 + static_cast<uint_t>(index)));
		if (palette != nullptr) {
			palette->assure();
			glCheck(glBindTexture(GL_TEXTURE_1D_ARRAY, palette->handle));
		}
	}
}

void gfx_t::set_sampler(const depth_buffer_t* depth_buffer, arch_t index) {
	assert(index < kSmpUnits);
	if (this->samplers[index] != depth_buffer) {
		this->samplers[index] = depth_buffer;
		glCheck(glActiveTexture(GL_TEXTURE0 + static_cast<uint_t>(index)));
		if (depth_buffer != nullptr and !depth_buffer->compress) {
			glCheck(glBindTexture(GL_TEXTURE_2D, depth_buffer->handle));
		}
	}
}

void gfx_t::set_sampler(std::nullptr_t, arch_t index) {
	assert(index < kSmpUnits);
	if (this->samplers[index] != nullptr) {
		this->samplers[index] = nullptr;
		glCheck(glActiveTexture(GL_TEXTURE0 + static_cast<uint_t>(index)));
		glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	}
}

void gfx_t::set_buffer(const const_buffer_t* buffer, arch_t index) {
	assert(index < kBufUnits);
	if (this->buffers[index] != buffer) {
		this->buffers[index] = buffer;
		if (buffer != nullptr) {
			glCheck(glBindBufferBase(
				GL_UNIFORM_BUFFER,
				static_cast<uint_t>(index),
				buffer->handle
			));
		}
	}
}