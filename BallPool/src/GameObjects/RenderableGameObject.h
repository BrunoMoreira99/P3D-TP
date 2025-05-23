#pragma once

#include <Roose/Core/Base.h>
#include <Roose/RenderableObject.h>

#include "GameObject.h"

class RenderableGameObject : public GameObject
{
public:
    RenderableGameObject(const std::string& objModelFilePath);

    void Render() const;

    void SetMaterial(const Roose::Ref<Roose::Material>& material) { m_Material = material; }

    [[nodiscard]] const Roose::Ref<Roose::RenderableObject>& GetRenderableObject() const { return m_RenderableObject; }
private:
    Roose::Ref<Roose::RenderableObject> m_RenderableObject = nullptr;
    Roose::Ref<Roose::Material> m_Material = nullptr;
};
