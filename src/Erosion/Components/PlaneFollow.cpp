#include "PlaneFollow.h"

#include <Components/Transform/Transform.h>
#include <SceneGraph/GameObject.h>

void Erosion::PlaneFollow::SetPlayer(leap::Transform* pTransform)
{
	m_pPlayer = pTransform;
}

void Erosion::PlaneFollow::Start()
{
	m_pPlayer = m_pPlayer->GetGameObject()->GetParent()->GetTransform();
}

void Erosion::PlaneFollow::Update()
{
	auto pos = GetTransform()->GetLocalPosition();
	pos.x = m_pPlayer->GetLocalPosition().x;
	pos.z = m_pPlayer->GetLocalPosition().z;
	GetTransform()->SetLocalPosition(pos);
}
