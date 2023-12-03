#include "TerrainGenerator.h"

#include <Components/RenderComponents/TerrainComponent.h>
#include <SceneGraph/GameObject.h>

void Erosion::TerrainGenerator::Awake()
{
	leap::TerrainComponent* pTerrain{ GetGameObject()->GetComponent<leap::TerrainComponent>() };

	const auto heights{ pTerrain->GetHeights() };

	// Where the magic happens

	pTerrain->SetHeights(heights);
}
