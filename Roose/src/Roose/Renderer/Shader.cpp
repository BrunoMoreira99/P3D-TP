#include "rspch.h"
#include "Roose/Renderer/Shader.h"

#include "Roose/Utils/FileSystemUtils.h"

namespace Roose {

    #pragma region Shader
    Shader::~Shader()
    {
        glDeleteProgram(m_RendererID);
    }

    GLuint Shader::CompileShader(const GLenum type, const std::string& source)
    {
        GLuint shader = glCreateShader(type);

        const GLchar* sourceCStr = source.c_str();
        glShaderSource(shader, 1, &sourceCStr, nullptr);

        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());

            glDeleteShader(shader);

            RS_ERROR("Failed to compile shader: %s", infoLog.data());
        }

        return shader;
    }

    Ref<Shader> Shader::FromGLSLTextFile(const std::string& shaderPath)
    {
        Ref<Shader> shader = CreateRef<Shader>();
        shader->LoadFromGLSLTextFile(shaderPath);
        return shader;
    }

    Ref<Shader> Shader::FromGLSLTextFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
    {
        Ref<Shader> shader = CreateRef<Shader>();
        shader->LoadFromGLSLTextFile(vertexShaderPath, fragmentShaderPath);
        return shader;
    }

    Ref<Shader> Shader::FromGLSLString(const std::string& name, const std::string& source)
    {
        Ref<Shader> shader = CreateRef<Shader>();
        shader->m_Name = name;
        shader->CreateFromGLSL(source);
        return shader;
    }

    Ref<Shader> Shader::FromGLSLString(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    {
        Ref<Shader> shader = CreateRef<Shader>();
        shader->m_Name = name;
        shader->CreateFromGLSL(vertexSource, fragmentSource);
        return shader;
    }

    void Shader::LoadFromGLSLTextFile(const std::string& shaderPath)
    {
        std::string source = FileSystemUtils::ReadFileAsString(shaderPath);
        m_Name = FileSystemUtils::GetFileNameWithoutExtension(shaderPath);
        CreateFromGLSL(source);
    }

    void Shader::LoadFromGLSLTextFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
    {
        const std::string vertexSource = FileSystemUtils::ReadFileAsString(vertexShaderPath);
        const std::string fragmentSource = FileSystemUtils::ReadFileAsString(fragmentShaderPath);

        m_Name = FileSystemUtils::GetFileNameWithoutExtension(vertexShaderPath);
        CreateFromGLSL(vertexSource, fragmentSource);
    }

    void Shader::CreateFromGLSL(const std::string& source)
    {
        enum class ShaderType : int8_t
        {
            None = -1, Vertex, Fragment
        };

        ShaderType currentType = ShaderType::None;
        std::string vertexSource, fragmentSource;

        size_t pos = 0;
        while ((pos = source.find("#type", pos)) != std::string::npos)
        {
            size_t eol = source.find('\n', pos);
            std::string typeLine = source.substr(pos, eol - pos);

            if (typeLine.find("vertex") != std::string::npos)
                currentType = ShaderType::Vertex;
            else if (typeLine.find("fragment") != std::string::npos)
                currentType = ShaderType::Fragment;
            else
            {
                RS_ERROR("Unknown shader type: %s", typeLine.c_str());
                return;
            }

            size_t nextTypePos = source.find("#type", eol);
            std::string shaderCode = source.substr(eol + 1, nextTypePos - (eol + 1));

            if (currentType == ShaderType::Vertex)
                vertexSource = shaderCode;
            else if (currentType == ShaderType::Fragment)
                fragmentSource = shaderCode;

            pos = nextTypePos;
        }

        if (vertexSource.empty())
        {
            RS_ERROR("Shader '%s' is missing vertex shader code", m_Name.c_str());
            return;
        }
        if (fragmentSource.empty())
        {
            RS_ERROR("Shader '%s' is missing fragment shader code", m_Name.c_str());
            return;
        }

        CreateFromGLSL(vertexSource, fragmentSource);
    }

    void Shader::CreateFromGLSL(const std::string& vertexSource, const std::string& fragmentSource)
    {
        const GLuint program = glCreateProgram();

        const GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
        glAttachShader(program, vertexShader);
        const GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);

        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

            glDeleteProgram(program);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            RS_ERROR("Failed to link shader program: %s", infoLog.data());
        }

        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        m_RendererID = program;
    }

    void Shader::Bind() const
    {
        glUseProgram(m_RendererID);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    #pragma region Uniforms
    void Shader::SetBool(const std::string& name, const bool value)
    {
        glProgramUniform1i(m_RendererID, GetUniformLocation(name), static_cast<int>(value));
    }

    void Shader::SetUInt(const std::string& name, const uint32_t value)
    {
        glProgramUniform1ui(m_RendererID, GetUniformLocation(name), value);
    }

    void Shader::SetInt(const std::string& name, const int value)
    {
        glProgramUniform1i(m_RendererID, GetUniformLocation(name), value);
    }

    void Shader::SetInt2(const std::string& name, const glm::ivec2& value)
    {
        glProgramUniform2i(m_RendererID, GetUniformLocation(name), value.x, value.y);
    }

    void Shader::SetInt3(const std::string& name, const glm::ivec3& value)
    {
        glProgramUniform3i(m_RendererID, GetUniformLocation(name), value.x, value.y, value.z);
    }

    void Shader::SetInt4(const std::string& name, const glm::ivec4& value)
    {
        glProgramUniform4i(m_RendererID, GetUniformLocation(name), value.x, value.y, value.z, value.w);
    }

    void Shader::SetFloat(const std::string& name, const float value)
    {
        glProgramUniform1f(m_RendererID, GetUniformLocation(name), value);
    }

    void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
    {
        glProgramUniform2f(m_RendererID, GetUniformLocation(name), value.x, value.y);
    }

    void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
    {
        glProgramUniform3f(m_RendererID, GetUniformLocation(name), value.x, value.y, value.z);
    }

    void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
    {
        glProgramUniform4f(m_RendererID, GetUniformLocation(name), value.x, value.y, value.z, value.w);
    }

    void Shader::SetMat3(const std::string& name, const glm::mat3& matrix)
    {
        glProgramUniformMatrix3fv(m_RendererID, GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& matrix)
    {
        glProgramUniformMatrix4fv(m_RendererID, GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
    }

    GLint Shader::GetUniformLocation(const std::string& name)
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        const GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1)
            RS_INFO("Warning: uniform '%s' doesn't exist!", name.c_str());

        m_UniformLocationCache[name] = location;
        return location;
    }
    #pragma endregion
    #pragma endregion

    #pragma region ShaderLibrary
    std::unordered_map<std::string, Ref<Shader>> ShaderLibrary::m_Shaders;

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
    {
        RS_ASSERT(!Exists(name), "Shader with this name already exists! Shader will be replaced.")
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        const std::string& name = shader->GetName();
        Add(name, shader);
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
    {
        if (const std::string name = FileSystemUtils::GetFileNameWithoutExtension(filepath); Exists(name))
            return Get(name);
        const Ref<Shader> shader = Shader::FromGLSLTextFile(filepath);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
    {
        const Ref<Shader> shader = Shader::FromGLSLTextFile(filepath);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name)
    {
        RS_ASSERT(Exists(name), "Shader not found!")
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name)
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }
    #pragma endregion

}
