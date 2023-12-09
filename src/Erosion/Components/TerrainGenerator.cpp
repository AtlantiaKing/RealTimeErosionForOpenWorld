#include "TerrainGenerator.h"

#include <Components/RenderComponents/TerrainComponent.h>
#include <SceneGraph/GameObject.h>

#include <Generator.h>
#include <Presets/Presets.h>

#include <WorldShape/CirclePeak.h>
#include <WorldShape/SquarePeak.h>

void Erosion::TerrainGenerator::Awake()
{
	leap::TerrainComponent* pTerrain{ GetGameObject()->GetComponent<leap::TerrainComponent>() };

	auto heights{ pTerrain->GetHeights() };

	const unsigned int terrainSize{ static_cast<unsigned int>(sqrt(heights.size())) };

	// Create generator
	constexpr float mapZoom{ 3.5f };
	that::Generator gen{};
	gen.SetSize(static_cast<float>(terrainSize));

	// Calculate a new seed
	const unsigned int seed{ static_cast<unsigned int>(time(nullptr)) };

	// Create a terrain generator
	that::preset::Presets::CreateDefaultTerrain(gen, seed, mapZoom);

	for (unsigned int x{}; x < terrainSize; ++x)
	{
		for (unsigned int z{}; z < terrainSize; ++z)
		{
			float noiseValue{ gen.GetHeight(static_cast<float>(x), static_cast<float>(z)) };

			if (noiseValue < 0.25f) noiseValue = 0.25f;

			heights[x + z * terrainSize] = noiseValue;
		}
	}

	pTerrain->SetHeights(heights);
}
