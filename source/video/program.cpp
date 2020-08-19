#include "./program.hpp"
#include "./glcheck.hpp"

#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"

/*
	When separable programs are available:
	- shader_t's handle is an OGL program object.
	- program_t's handle is an OGL program pipeline object.
	If separable programs are not available:
	- shader_t's handle is an OGL shader object.
	- program_t's handle is an OGL program object.
*/

shader_t::shader_t() :
	handle(0),
	stage(shader_stage_t::Vertex)
{

}

shader_t::shader_t(shader_t&& that) noexcept : shader_t() {
	if (this != &that) {
		std::swap(handle, that.handle);
		std::swap(stage, that.stage);
	}
}

shader_t& shader_t::operator=(shader_t&& that) noexcept {
	if (this != &that) {
		std::swap(handle, that.handle);
		std::swap(stage, that.stage);
	}
	return *this;
}

shader_t::~shader_t() {
	this->destroy();
}

bool shader_t::from(const std::string& source, shader_stage_t stage) {
	if (!handle) {
		this->stage = stage;
		const byte_t* source_pointer = source.c_str();

		uint_t gl_enum = gfx_t::get_shader_stage_gl_enum(stage);

		if (program_t::has_separable()) {
			glCheck(handle = glCreateShaderProgramv(gl_enum, 1, &source_pointer));
			glCheck(glValidateProgram(handle));
			sint_t length = 0;
			glCheck(glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length));
			if (length > 0) {
				byte_t log[1024];
				glCheck(glGetProgramInfoLog(handle, sizeof(log), 0, log));
				synao_log("Failed to create separable program! Error: %s", log);
				this->destroy();
				return false;
			}
		} else {
			glCheck(handle = glCreateShader(gl_enum));
			glCheck(glShaderSource(handle, 1, &source_pointer, NULL));
			glCheck(glCompileShader(handle));

			sint_t success = 0;
			glCheck(glGetShaderiv(handle, GL_COMPILE_STATUS, &success));
			if (!success) {
				byte_t log[1024];
				glCheck(glGetShaderInfoLog(handle, sizeof(log), 0, log));
				synao_log("Failed to compile vertex shader: %s\n", log);
				this->destroy();
				return false;
			}
		}
	}
	return true;
}

bool shader_t::load(const std::string& full_path, shader_stage_t stage) {
	if (!handle) {
		const std::string source = vfs::string_buffer(full_path);
		return this->from(source, stage);
	}
	return true;
}

void shader_t::destroy() {
	if (handle != 0) {
		if (program_t::has_separable()) {
			glCheck(glUseProgram(0));
			glCheck(glDeleteProgram(handle));
		} else {
			glCheck(glDeleteShader(handle));
		}
		handle = 0;
		stage = shader_stage_t::Vertex;
	}
}

bool shader_t::matches(shader_stage_t stage) const {
	return this->stage == stage;
}

vertex_spec_t shader_t::attributes(uint_t program_handle) {
	vertex_spec_t desc;
	if (program_handle != 0) {
		byte_t buffer [128];
		uint_t vltype = 0;
		sint_t active = 0;
		sint_t placmt = GL_INVALID_INDEX;

		glCheck(glGetProgramiv(program_handle, GL_ACTIVE_ATTRIBUTES, &active));
		std::vector<uint_t> typeslist = std::vector<uint_t>(static_cast<arch_t>(active) + 1, 0);

		for (sint_t it = 0; it < active; ++it) {
			glCheck(glGetActiveAttrib(
				program_handle, it, sizeof(buffer),
				nullptr, nullptr, &vltype, buffer
			));

			glCheck(placmt = glGetAttribLocation(program_handle, buffer));
			if (placmt == GL_INVALID_INDEX) {
				synao_log("Warning! Shader attribute location is invalid!\n");
			}

			typeslist[placmt] = vltype;
		}
		desc = vertex_spec_t::from(typeslist.data());
	}
	return desc;
}

program_t::program_t() :
	handle(0),
	specify()
{

}

program_t::program_t(program_t&& that) noexcept : program_t() {
	if (this != &that) {
		std::swap(handle, that.handle);
		std::swap(specify, that.specify);
	}
}

program_t& program_t::operator=(program_t&& that) noexcept {
	if (this != &that) {
		std::swap(handle, that.handle);
		std::swap(specify, that.specify);
	}
	return *this;
}

program_t::~program_t() {
	this->destroy();
}

bool program_t::create(const shader_t* vert, const shader_t* frag, const shader_t* geom) {
	if (!handle) {
		if (program_t::has_separable()) {
			sint_t length = 0;
			glCheck(glGenProgramPipelines(1, &handle));
			glCheck(glBindProgramPipeline(handle));

			if (vert != nullptr and vert->stage == shader_stage_t::Vertex) {
				glCheck(glUseProgramStages(handle, GL_VERTEX_SHADER_BIT, vert->handle));
				specify = shader_t::attributes(vert->handle);
			}

			if (frag != nullptr and frag->stage == shader_stage_t::Fragment) {
				glCheck(glUseProgramStages(handle, GL_FRAGMENT_SHADER_BIT, frag->handle));
			}

			if (geom != nullptr and geom->stage == shader_stage_t::Geometry) {
				glCheck(glUseProgramStages(handle, GL_GEOMETRY_SHADER_BIT, geom->handle));
			}

			glCheck(glValidateProgramPipeline(handle));
			glCheck(glGetProgramPipelineiv(handle, GL_INFO_LOG_LENGTH, &length));

			if (length > 0) {
				byte_t log[1024];
				glCheck(glGetProgramPipelineInfoLog(handle, sizeof(log), 0, log));
				synao_log("Failed to create program pipeline! Error: %s", log);
				this->destroy();
				return false;
			}
		} else {
			sint_t success = 0;
			glCheck(handle = glCreateProgram());

			if (vert != nullptr and vert->stage == shader_stage_t::Vertex) {
				glCheck(glAttachShader(handle, vert->handle));
			}

			if (frag != nullptr and frag->stage == shader_stage_t::Fragment) {
				glCheck(glAttachShader(handle, frag->handle));
			}

			if (geom != nullptr and geom->stage == shader_stage_t::Geometry) {
				glCheck(glAttachShader(handle, geom->handle));
			}

			glCheck(glLinkProgram(handle));
			glCheck(glGetProgramiv(handle, GL_LINK_STATUS, &success));

			if (!success) {
				byte_t log[1024];
				glCheck(glGetProgramInfoLog(handle, sizeof(log), 0, log));
				synao_log("Failed to link program: %s\n", log);
				this->destroy();
				return false;
			}
			specify = shader_t::attributes(handle);
		}
	}
	return specify.length != 0;
}

bool program_t::create(const shader_t* vert, const shader_t* frag) {
	return this->create(vert, frag, nullptr);
}

void program_t::destroy() {
	if (handle != 0) {
		if (program_t::has_separable()) {
			glCheck(glBindProgramPipeline(0));
			glCheck(glDeleteProgramPipelines(1, &handle));
		} else {
			glCheck(glUseProgram(0));
			glCheck(glDeleteProgram(handle));
		}
		handle = 0;
		specify = vertex_spec_t();
	}
}

void program_t::set_block(const byte_t* name, arch_t binding) const {
	if (program_t::has_separable()) {
		synao_log("Warning! OpenGL version is 4.2^! Don't manually set constant buffer bindings!\n");
	} else if (handle != 0) {
		uint_t index = GL_INVALID_INDEX;
		glCheck(index = glGetUniformBlockIndex(handle, name));
		if (index != GL_INVALID_INDEX) {
			glCheck(glUniformBlockBinding(
				handle,
				index,
				static_cast<uint_t>(binding)
			));
		}
	}
}

void program_t::set_sampler(const byte_t* name, arch_t sampler) const {
	if (program_t::has_separable()) {
		synao_log("Warning! OpenGL version is 4.2^! Don't manually set sampler bindings!\n");
	} else if (handle != 0) {
		sint_t index = GL_INVALID_INDEX;
		glCheck(index = glGetUniformLocation(handle, name));
		if (index != GL_INVALID_INDEX) {
			if (program_t::has_uniform_azdo()) {
				glCheck(glProgramUniform1i(
					handle,
					index,
					static_cast<sint_t>(sampler)
				));
			} else {
				glCheck(glUseProgram(handle));
				glCheck(glUniform1i(
					index,
					static_cast<sint_t>(sampler)
				));
				glCheck(glUseProgram(0));
			}
		}
	}
}

const vertex_spec_t& program_t::get_specify() const {
	return specify;
}

bool program_t::has_separable() {
#ifndef __APPLE__
	return glBindProgramPipeline != nullptr;
#else // __APPLE__
	return glTexStorage2D != nullptr;
#endif // __APPLE__
}

bool program_t::has_uniform_azdo() {
	return glProgramUniform1i != nullptr;
}
