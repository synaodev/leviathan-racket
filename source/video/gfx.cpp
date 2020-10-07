#include "./gfx.hpp"
#include "./gl-check.hpp"
#include "./pipeline.hpp"
#include "./texture.hpp"
#include "./const-buffer.hpp"
#include "./frame-buffer.hpp"

gfx_t::gfx_t() :
	depth_func(compare_func_t::Disable),
	blend_mode(blend_mode_t::Disable),
	pipeline(nullptr),
	sampler_allocator(nullptr),
	sampler_list{},
	buffer_list{}
{
	sampler_list.fill(nullptr);
	buffer_list.fill(nullptr);
}

void gfx_t::set_depth_func(compare_func_t depth_func) {
	if (this->depth_func != depth_func) {
		if (depth_func == compare_func_t::Disable) {
			glCheck(glDisable(GL_DEPTH_TEST));
		} else if (this->depth_func == compare_func_t::Disable) {
			glCheck(glEnable(GL_DEPTH_TEST));
		}
		this->depth_func = depth_func;
		if (this->depth_func != compare_func_t::Disable) {
			uint_t gl_enum = get_compare_func_gl_enum(depth_func);
			glCheck(glDepthFunc(gl_enum));
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

void gfx_t::set_pipeline(const pipeline_t* pipeline) {
	if (this->pipeline != pipeline) {
		this->pipeline = pipeline;
		if (pipeline != nullptr) {
			if (pipeline_t::has_separable()) {
				glCheck(glBindProgramPipeline(pipeline->handle));
			} else {
				glCheck(glUseProgram(pipeline->handle));
			}
		}
	}
}

void gfx_t::set_sampler_allocator(const sampler_allocator_t* sampler_allocator) {
	if (this->sampler_allocator != sampler_allocator) {
		this->sampler_allocator = sampler_allocator;
		if (sampler_allocator != nullptr) {
			auto& texture = sampler_allocator->texture();
			glCheck(glActiveTexture(GL_TEXTURE0));
			glCheck(glBindTexture(texture.type, texture.id));
			auto& palette = sampler_allocator->palette();
			glCheck(glActiveTexture(GL_TEXTURE1));
			glCheck(glBindTexture(palette.type, palette.id));
			sampler_list.fill(nullptr);
		}
	}
}

void gfx_t::set_sampler(const color_buffer_t* color_buffer, arch_t index) {
	if (index < sampler_list.size()) {
		if (sampler_list[index] != color_buffer) {
			sampler_list[index] = color_buffer;
			glCheck(glActiveTexture(GL_TEXTURE0 + static_cast<uint_t>(index)));
			if (color_buffer != nullptr) {
				if (color_buffer->layers > 1) {
					glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, color_buffer->handle));
				} else {
					glCheck(glBindTexture(GL_TEXTURE_2D, color_buffer->handle));
				}
			}
			sampler_allocator = nullptr;
		}
	}
}

void gfx_t::set_sampler(const depth_buffer_t* depth_buffer, arch_t index) {
	if (index < sampler_list.size()) {
		if (sampler_list[index] != depth_buffer) {
			sampler_list[index] = depth_buffer;
			glCheck(glActiveTexture(GL_TEXTURE0 + static_cast<uint_t>(index)));
			if (depth_buffer != nullptr and !depth_buffer->compress) {
				glCheck(glBindTexture(GL_TEXTURE_2D, depth_buffer->handle));
			}
			sampler_allocator = nullptr;
		}
	}
}

void gfx_t::set_sampler(std::nullptr_t, arch_t index) {
	if (index < sampler_list.size()) {
		if (sampler_list[index] != nullptr) {
			sampler_list[index] = nullptr;
			glCheck(glActiveTexture(GL_TEXTURE0 + static_cast<uint_t>(index)));
			glCheck(glBindTexture(GL_TEXTURE_2D, 0));
			sampler_allocator = nullptr;
		}
	}
}

void gfx_t::set_buffer(const const_buffer_t* buffer, arch_t index) {
	if (index < buffer_list.size()) {
		if (buffer_list[index] != buffer) {
			buffer_list[index] = buffer;
			if (buffer != nullptr) {
				glCheck(glBindBufferBase(
					GL_UNIFORM_BUFFER,
					static_cast<uint_t>(index),
					buffer->handle
				));
			}
		}
	}
}

uint_t gfx_t::get_compare_func_gl_enum(compare_func_t func) {
	switch (func) {
	case compare_func_t::Disable:
		break;
	case compare_func_t::Never:
		return GL_NEVER;
	case compare_func_t::Less:
		return GL_LESS;
	case compare_func_t::Equal:
		return GL_EQUAL;
	case compare_func_t::Lequal:
		return GL_LEQUAL;
	case compare_func_t::Greater:
		return GL_GREATER;
	case compare_func_t::Nequal:
		return GL_NOTEQUAL;
	case compare_func_t::Gequal:
		return GL_GEQUAL;
	case compare_func_t::Always:
		return GL_ALWAYS;
	}
	return GL_NONE;
}

uint_t gfx_t::get_buffer_usage_gl_enum(buffer_usage_t usage) {
	switch (usage) {
	case buffer_usage_t::Static:
		return GL_STATIC_DRAW;
	case buffer_usage_t::Dynamic:
		return GL_DYNAMIC_DRAW;
	case buffer_usage_t::Stream:
		return GL_STREAM_DRAW;
	}
	return GL_NONE;
}

uint_t gfx_t::get_primitive_gl_enum(primitive_t primitive) {
	switch (primitive) {
	case primitive_t::Points:
		return GL_POINTS;
	case primitive_t::Lines:
		return GL_LINES;
	case primitive_t::LineLoop:
		return GL_LINE_LOOP;
	case primitive_t::LineStrip:
		return GL_LINE_STRIP;
	case primitive_t::Triangles:
		return GL_TRIANGLES;
	case primitive_t::TriangleStrip:
		return GL_TRIANGLE_STRIP;
	case primitive_t::TriangleFan:
		return GL_TRIANGLE_FAN;
	}
	return GL_NONE;
}


uint_t gfx_t::get_pixel_format_gl_enum(pixel_format_t format) {
	switch (format) {
	case pixel_format_t::Invalid:
		break;
	case pixel_format_t::R3G3B2A0:
		return GL_R3_G3_B2;
	case pixel_format_t::R8G8B8A0:
		return GL_RGB8;
	case pixel_format_t::R2G2B2A2:
		return GL_RGBA2;
	case pixel_format_t::R4G4B4A4:
		return GL_RGBA4;
	case pixel_format_t::R8G8B8A8:
		return GL_RGBA8;
	}
	return GL_NONE;
}

uint_t gfx_t::get_shader_stage_gl_enum(shader_stage_t stage) {
	switch (stage) {
	case shader_stage_t::Vertex:
		return GL_VERTEX_SHADER;
	case shader_stage_t::Geometry:
		return GL_GEOMETRY_SHADER;
	case shader_stage_t::Fragment:
		return GL_FRAGMENT_SHADER;
	}
	return GL_NONE;
}
