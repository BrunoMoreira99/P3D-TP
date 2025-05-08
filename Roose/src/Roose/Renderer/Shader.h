#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Roose {

	class Shader
	{
	public:
		Shader() = default;
		~Shader();

		static Ref<Shader> FromGLSLTextFile(const std::string& shaderPath);
		static Ref<Shader> FromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

		void Bind() const;
		void Unbind() const;

		void SetBool(const std::string& name, bool value);
		void SetUInt(const std::string& name, uint32_t value);
		void SetInt(const std::string& name, int32_t value);
		void SetInt2(const std::string& name, const glm::ivec2& value);
		void SetInt3(const std::string& name, const glm::ivec3& value);
		void SetInt4(const std::string& name, const glm::ivec4& value);
		void SetFloat(const std::string& name, float value);
		void SetFloat2(const std::string& name, const glm::vec2& value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);
		void SetMat3(const std::string& name, const glm::mat3& matrix);
		void SetMat4(const std::string& name, const glm::mat4& matrix);
	private:
		void LoadFromSingleGLSLTextFile(const std::string& shaderPath);
		void LoadFromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		void CreateFromGLSL(const std::string& vertexSource, const std::string& fragmentSource);
		GLuint CompileShader(GLenum type, const std::string& source);

		GLint GetUniformLocation(const std::string& name);
	private:
		uint32_t m_RendererID;
		std::unordered_map<std::string, GLint> m_UniformLocationCache;
	};

}
