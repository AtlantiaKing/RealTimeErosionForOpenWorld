#include "Sample.h"

#include <SceneGraph/Scene.h>
#include <Components/RenderComponents/CameraComponent.h>
#include <Camera.h>
#include <Components/RenderComponents/DirectionalLightComponent.h>
#include <Components/Transform/Transform.h>

#include "../Components/FreeCamMovement.h"

#include <Components/RenderComponents/TerrainComponent.h>
#include "../Components/TerrainGeneratorComponent.h"

#include <Graphics/Mesh.h>
#include <Graphics/Material.h>
#include <Shaders/Pos3D.h>
#include <Components/RenderComponents/MeshRenderer.h>

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
	pTerrain->AddComponent<leap::TerrainComponent>()->SetSize(256, 1);
	pTerrain->AddComponent<Erosion::TerrainGeneratorComponent>();

	const auto pPlane{ scene.CreateGameObject("Plane") };
	const leap::Mesh planeMesh{ "Data/Engine/Models/Plane.obj" };
	const leap::Material planeMat{ leap::Material::Create<leap::graphics::shaders::Pos3D>("Plane") };
	planeMat.Set("gColor", glm::vec4{ 0.375f, 0.5468f, 0.6992f, 1.0f });
	const auto pPlaneRenderer{ pPlane->AddComponent<leap::MeshRenderer>() };
	pPlaneRenderer->SetMesh(planeMesh);
	pPlaneRenderer->SetMaterial(planeMat);
	pPlane->GetTransform()->Scale(1024.0f);
	pPlane->GetTransform()->Translate(512.0f, 50.0f, 512.0f);
}