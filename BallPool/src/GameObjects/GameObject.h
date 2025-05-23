#pragma once

#include <Roose/Core/Base.h>
#include <Roose/Core/Time.h>
#include "Components.h"

class GameObject
{
public:
    GameObject() = default;
    GameObject(const GameObject&) = default;
    virtual ~GameObject() = default;

    virtual void Update(const Roose::Timestep deltaTime) {
        for (const auto& child : m_Children) {
            child->Update(deltaTime);
        }
    }

    virtual void FixedUpdate(const Roose::Timestep fixedDeltaTime) {
        for (const auto& child : m_Children) {
            child->FixedUpdate(fixedDeltaTime);
        }
    }

    virtual void Render() const {
        for (const auto& child : m_Children) {
            child->Render();
        }
    }

    virtual void Reset()
    {
        for (const auto& child : m_Children) {
            child->Reset();
        }
    }

    void AddChild(const Roose::Ref<GameObject>& child) {
        m_Children.push_back(child);
        child->m_Parent = this;
    }

    void RemoveChild(const Roose::Ref<GameObject>& child) {
        auto it = std::find(m_Children.begin(), m_Children.end(), child);
        if (it != m_Children.end()) {
            (*it)->m_Parent = nullptr;
            m_Children.erase(it);
        }
    }

    [[nodiscard]] const std::vector<Roose::Ref<GameObject>>& GetChildren() const {
        return m_Children;
    }

    [[nodiscard]] TransformComponent GetWorldTransform() const {
        if (m_Parent) {
            return Transform.Combine(m_Parent->GetWorldTransform());
        }
        return Transform;
    }

    TransformComponent Transform;
protected:
    std::vector<Roose::Ref<GameObject>> m_Children;
    GameObject* m_Parent = nullptr;
};
