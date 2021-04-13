#include "./pipeline.hpp"
#include "./gl-check.hpp"

#include "../resource/vfs.hpp"
#include "../utility/logger.hpp"

/*
	When separable programs are available:
	- shader_t's handle is an OGL program object.
	- pipeline_t's handle is an OGL program pipeline object.
	If separable programs are not available:
	- shader_t's handle is an OGL shader object.
	- pipeline_t's handle is an OGL program object.
*/

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

		if (pipeline_t::has_separable()) {
			uint_t object = 0;
			glCheck(object = glCreateShader(gl_enum));
			if (!object) {
				synao_log("Failed to create shader for unknown reason!\n");
				return false;
			}
			glCheck(glShaderSource(object, 1, &source_pointer, NULL));
			glCheck(glCompileShader(object));

			sint_t success = 0;
			glCheck(glGetShaderiv(object, GL_COMPILE_STATUS, &success));
			if (!success) {
				byte_t log[1024];
				glCheck(glGetShaderInfoLog(object, sizeof(log), 0, log));
				synao_log("Failed to compile shader: {}\n", log);
				glCheck(glDeleteShader(object));
				return false;
			}

			glCheck(handle = glCreateProgram());
			if (!handle) {
				synao_log("Failed to create program for unknown reason!\n");
				glCheck(glDeleteShader(object));
				return false;
			}
			glCheck(glProgramParameteri(handle, GL_PROGRAM_SEPARABLE, true));
			glCheck(glAttachShader(handle, object));
			glCheck(glLinkProgram(handle));

			sint_t linking = 0;
			glCheck(glGetProgramiv(handle, GL_LINK_STATUS, &linking));
			if (!linking) {
				byte_t log[1024];
				glCheck(glGetProgramInfoLog(handle, sizeof(log), 0, log));
				synao_log("Failed to link program! Error: {}\n", log);
				glCheck(glDetachShader(handle, object));
				glCheck(glDeleteShader(object));
				return false;
			}

			glCheck(glDetachShader(handle, object));
			glCheck(glDeleteShader(object));
		} else {
			glCheck(handle = glCreateShader(gl_enum));
			glCheck(glShaderSource(handle, 1, &source_pointer, NULL));
			glCheck(glCompileShader(handle));

			sint_t success = 0;
			glCheck(glGetShaderiv(handle, GL_COMPILE_STATUS, &success));
			if (!success) {
				byte_t log[1024];
				glCheck(glGetShaderInfoLog(handle, sizeof(log), 0, log));
				synao_log("Failed to compile shader: {}\n", log);
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
		if (pipeline_t::has_separable()) {
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
		sint_t unused_0 = 0;
		sint_t unused_1 = 0;

		glCheck(glGetProgramiv(program_handle, GL_ACTIVE_ATTRIBUTES, &active));
		std::vector<uint_t> typeslist = std::vector<uint_t>(static_cast<arch_t>(active) + 1, 0);

		assert(glGetActiveAttrib != nullptr);

		for (sint_t it = 0; it < active; ++it) {
			glCheck(glGetActiveAttrib(
				program_handle, it, sizeof(buffer),
				&unused_0, &unused_1, &vltype, buffer
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

pipeline_t::pipeline_t(pipeline_t&& that) noexcept : pipeline_t() {
	if (this != &that) {
		std::swap(handle, that.handle);
		std::swap(specify, that.specify);
	}
}

pipeline_t& pipeline_t::operator=(pipeline_t&& that) noexcept {
	if (this != &that) {
		std::swap(handle, that.handle);
		std::swap(specify, that.specify);
	}
	return *this;
}

pipeline_t::~pipeline_t() {
	this->destroy();
}

bool pipeline_t::create(const shader_t* vert, const shader_t* frag, const shader_t* geom) {
	if (!handle) {
		if (pipeline_t::has_separable()) {
			sint_t length = 0;
			glCheck(glGenProgramPipelines(1, &handle));

			// NVIDIA doesn't like when you do this
			// glCheck(glBindProgramPipeline(handle));

			if (vert and vert->stage == shader_stage_t::Vertex) {
				glCheck(glUseProgramStages(handle, GL_VERTEX_SHADER_BIT, vert->handle));
				specify = shader_t::attributes(vert->handle);
			}

			if (frag and frag->stage == shader_stage_t::Fragment) {
				glCheck(glUseProgramStages(handle, GL_FRAGMENT_SHADER_BIT, frag->handle));
			}

			if (geom and geom->stage == shader_stage_t::Geometry) {
				glCheck(glUseProgramStages(handle, GL_GEOMETRY_SHADER_BIT, geom->handle));
			}

			glCheck(glValidateProgramPipeline(handle));
			glCheck(glGetProgramPipelineiv(handle, GL_INFO_LOG_LENGTH, &length));

			if (length > 0) {
				byte_t log[1024];
				glCheck(glGetProgramPipelineInfoLog(handle, sizeof(log), 0, log));
				synao_log("Failed to create program pipeline! Error: {}\n", log);
				this->destroy();
				return false;
			}
		} else {
			sint_t success = 0;
			glCheck(handle = glCreateProgram());

			if (vert and vert->stage == shader_stage_t::Vertex) {
				glCheck(glAttachShader(handle, vert->handle));
			}
			if (frag and frag->stage == shader_stage_t::Fragment) {
				glCheck(glAttachShader(handle, frag->handle));
			}
			if (geom and geom->stage == shader_stage_t::Geometry) {
				glCheck(glAttachShader(handle, geom->handle));
			}

			glCheck(glLinkProgram(handle));
			glCheck(glGetProgramiv(handle, GL_LINK_STATUS, &success));

			if (!success) {
				byte_t log[1024];
				glCheck(glGetProgramInfoLog(handle, sizeof(log), 0, log));
				synao_log("Failed to link program: {}\n", log);
				this->destroy();
			} else {
				specify = shader_t::attributes(handle);
			}

			if (vert) {
				glCheck(glDetachShader(handle, vert->handle));
			}
			if (frag) {
				glCheck(glDetachShader(handle, frag->handle));
			}
			if (geom) {
				glCheck(glDetachShader(handle, geom->handle));
			}
		}
	}
	return specify.length != 0;
}

bool pipeline_t::create(const shader_t* vert, const shader_t* frag) {
	return this->create(vert, frag, nullptr);
}

void pipeline_t::destroy() {
	if (handle != 0) {
		if (pipeline_t::has_separable()) {
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

void pipeline_t::set_block(const byte_t* name, arch_t binding) const {
	if (pipeline_t::has_separable()) {
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

void pipeline_t::set_sampler(const byte_t* name, arch_t sampler) const {
	if (pipeline_t::has_separable()) {
		synao_log("Error! OpenGL version is 4.2^! You shouldn't manually set sampler bindings!\n");
	} else if (handle != 0) {
		sint_t index = GL_INVALID_INDEX;
		glCheck(index = glGetUniformLocation(handle, name));
		if (index != GL_INVALID_INDEX) {
			if (pipeline_t::has_uniform_azdo()) {
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

const vertex_spec_t& pipeline_t::get_specify() const {
	return specify;
}

bool pipeline_t::has_separable() {
	return opengl_version[0] == 4 and opengl_version[1] >= 2;
}

bool pipeline_t::has_uniform_azdo() {
	return opengl_version[0] == 4 and opengl_version[1] >= 1;
}
