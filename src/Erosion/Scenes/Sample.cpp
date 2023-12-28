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
#include "../Components/RealtimeGenerator.h"
#include "../Components/PlaneFollow.h"

void Erosion::SampleScene::Load(leap::Scene& scene)
{
	const auto pCamera{ scene.CreateGameObject("Main camera") };
	const auto pCameraComponent{ pCamera->AddComponent<leap::CameraComponent>() };
	pCameraComponent->SetAsActiveCamera(true);
	pCameraComponent->GetData()->SetFarPlane(2000.0f);
	pCamera->GetTransform()->SetWorldPosition(0, 60.0f, 0);
	pCamera->GetTransform()->SetLocalRotation(90.0f, 0.0f, 0.0f);
	pCamera->AddComponent<Erosion::FreeCamMovement>();

	const auto pDirLight{ scene.CreateGameObject("Directional light") };
	pDirLight->AddComponent<leap::DirectionalLightComponent>()->GetTransform()->SetWorldRotation(60, 45+80, 0, true);

	const auto pTerrain{ scene.CreateGameObject("Terrain") };
	pTerrain->AddComponent<RealtimeGenerator>()->SetPlayerTransform(pCamera->GetTransform());

	const auto pPlane{ scene.CreateGameObject("Plane") };
	const leap::Mesh planeMesh{ "Data/Engine/Models/Plane.obj" };
	const leap::Material planeMat{ leap::Material::Create<leap::graphics::shaders::Pos3D>("Plane") };
	planeMat.Set("gColor", glm::vec4{ 0.375f, 0.5468f, 0.6992f, 1.0f });
	const auto pPlaneRenderer{ pPlane->AddComponent<leap::MeshRenderer>() };
	pPlaneRenderer->SetMesh(planeMesh);
	pPlaneRenderer->SetMaterial(planeMat);
	pPlane->AddComponent<PlaneFollow>()->SetPlayer(pCamera->GetTransform());
	pPlane->GetTransform()->Scale(1); 
	pPlane->GetTransform()->SetWorldPosition(0, 50.0f, 0);
}