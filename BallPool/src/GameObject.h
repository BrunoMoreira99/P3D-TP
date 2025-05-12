#pragma once

#include <glm/glm.hpp>

class GameObject
{
public:
    GameObject() : m_Position(0.0f), m_Rotation(0.0f) {}

    void SetPosition(const glm::vec3& position) { m_Position = position; }
    void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }

    [[nodiscard]] const glm::vec3& GetPosition() const { return m_Position; }
    [[nodiscard]] const glm::vec3& GetRotation() const { return m_Rotation; }
    [[nodiscard]] const glm::vec3& GetScale() const { return m_Scale; }
protected:
    glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };
};
