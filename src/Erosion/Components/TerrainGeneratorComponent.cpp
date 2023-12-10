#include "TerrainGeneratorComponent.h"

#include <Components/RenderComponents/TerrainComponent.h>
#include <SceneGraph/GameObject.h>

#include <Generator.h>
#include <Presets/Presets.h>

#include <WorldShape/CirclePeak.h>
#include <WorldShape/SquarePeak.h>

#include "../ErosionAlgorithms/HansBeyer.h"

void Erosion::TerrainGeneratorComponent::Awake()
{
	leap::TerrainComponent* pTerrain{ GetGameObject()->GetComponent<leap::TerrainComponent>() };

	auto heights{ pTerrain->GetHeights() };

	const unsigned int terrainSize{ static_cast<unsigned int>(sqrt(heights.size())) };

	// Create generator
	that::Generator gen{};
	gen.SetSize(static_cast<float>(terrainSize));

	// Create noise map
	that::NoiseMap noise{};
	noise.GetGraph().AddNode(0.0f, 0.0f);
	noise.GetGraph().AddNode(1.0f, 1.0f);
	noise.GetPerlin().AddOctave(1.0f, 40.0f);
	noise.GetPerlin().AddOctave(0.5f, 30.0f);
	noise.GetPerlin().AddOctave(0.2f, 20.0f);
	noise.GetPerlin().AddOctave(0.1f, 10.0f);
	gen.GetHeightMap().AddNoiseMap(noise);

	// Generate a terrain by perlin noise
	for (unsigned int x{}; x < terrainSize; ++x)
	{
		for (unsigned int z{}; z < terrainSize; ++z)
		{
			float noiseValue{ gen.GetHeight(static_cast<float>(x) / 5.0f, static_cast<float>(z) / 5.0f) };

			if (noiseValue < 0.25f) noiseValue = 0.25f;

			heights[x + z * terrainSize] = noiseValue;
		}
	}

	// Apply a erosion algorithm
	auto pErosion{ std::make_unique<HansBeyer>() };
	pErosion->GetHeights(heights);

	// Apply the new heightmap to the terrain component
	pTerrain->SetHeights(heights);
}
