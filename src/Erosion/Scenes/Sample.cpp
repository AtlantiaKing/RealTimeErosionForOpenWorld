#include "Sample.h"

#include <SceneGraph/Scene.h>
#include <Components/RenderComponents/CameraComponent.h>
#include <Camera.h>
#include <Components/RenderComponents/DirectionalLightComponent.h>
#include <Components/Transform/Transform.h>

#include "../Components/FreeCamMovement.h"

#include <Components/RenderComponents/TerrainComponent.h>
#include "../Components/TerrainGenerator.h"

void Erosion::SampleScene::Load(leap::Scene& scene)
{
	const auto pCamera{ scene.CreateGameObject("Main camera") };
	const auto pCameraComponent{ pCamera->AddComponent<leap::CameraComponent>() };
	pCameraComponent->SetAsActiveCamera(true);
	pCameraComponent->GetData()->SetFarPlane(1000.0f);
	pCamera->GetTransform()->SetWorldPosition(0, 0, -5);
	pCamera->AddComponent<Erosion::FreeCamMovement>();

	const auto pDirLight{ scene.CreateGameObject("Directional light") };
	pDirLight->AddComponent<leap::DirectionalLightComponent>()->GetTransform()->SetWorldRotation(60, 45, 0, true);

	const auto pTerrain{ scene.CreateGameObject("Terrain") };
	pTerrain->AddComponent<leap::TerrainComponent>()->SetSize(256);
	pTerrain->AddComponent<Erosion::TerrainGenerator>();
}