#include "BallGameObject.h"

BallGameObject::BallGameObject(const std::string& objModelFilePath)
{
    m_RenderableObject = Roose::CreateRef<Roose::RenderableObject>();
    m_RenderableObject->Load(objModelFilePath);
    m_RenderableObject->Install();
    m_Material = m_RenderableObject->GetMeshesEntries()[0].Material;
}

void BallGameObject::Render() const
{
    m_RenderableObject->SetMaterial(m_Material);
    m_RenderableObject->Render(GetPosition(), GetRotation());
}
