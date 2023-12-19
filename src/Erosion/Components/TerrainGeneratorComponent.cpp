#include "TerrainGeneratorComponent.h"

#include <Components/RenderComponents/TerrainComponent.h>
#include <SceneGraph/GameObject.h>

#include <Presets/Presets.h>

#include <WorldShape/CirclePeak.h>
#include <WorldShape/SquarePeak.h>

#include "../ErosionAlgorithms/HansBeyer.h"
#include "../ErosionAlgorithms/VelocityField.h"
#include "../ErosionAlgorithms/RiverLand.h"

#include <ImGui/imgui.h>

void Erosion::TerrainGeneratorComponent::Awake()
{
	// Get the terrain size from the terrain component
	leap::TerrainComponent* pTerrain{ GetGameObject()->GetComponent<leap::TerrainComponent>() };
	const unsigned int terrainSize{ static_cast<unsigned int>(sqrt(pTerrain->GetHeights().size())) };

	// Create random seed
	srand(static_cast<unsigned int>(time(nullptr)));

	// Create generator
	m_Gen.SetSize(static_cast<float>(terrainSize));

	// Create noise maps for the terrain generator
	const float multiplier{ 2.0f };

	that::NoiseMap noise{};
	noise.GetGraph().AddNode(0.0f, 0.001f);
	noise.GetGraph().AddNode(0.5f, 0.05f);
	noise.GetGraph().AddNode(1.0f, 0.2f);
	noise.GetPerlin().AddOctave(1.0f, 10.0f * multiplier);
	noise.GetPerlin().AddOctave(1.0f, 5.0f * multiplier);
	noise.GetPerlin().AddOctave(1.0f, 2.0f * multiplier);
	m_Gen.GetHeightMap().AddNoiseMap(noise);

	that::NoiseMap continentalNess{};
	continentalNess.GetGraph().AddNode(0.0f, 0.1f);
	continentalNess.GetGraph().AddNode(0.4f, 0.1f);
	continentalNess.GetGraph().AddNode(0.7f, 1.0f);
	continentalNess.GetGraph().AddNode(1.0f, 1.0f);
	continentalNess.GetPerlin().AddOctave(1.0f, 100.0f * multiplier);
	m_Gen.GetHeightMap().AddNoiseMap(continentalNess);

	// Set the blendmode for the noisemaps
	m_Gen.GetHeightMap().SetBlendMode(that::HeightMap::BlendMode::Add);

	that::NoiseMap doublePerlin{};
	doublePerlin.GetGraph().AddNode(0.0f, 0.005f);
	doublePerlin.GetGraph().AddNode(0.5f, 0.25f);
	doublePerlin.GetGraph().AddNode(1.0f, 1.0f);
	doublePerlin.GetPerlin().AddOctave(1.0f, 10.0f * multiplier);
	doublePerlin.GetPerlin().AddOctave(1.0f, 5.0f * multiplier);
	doublePerlin.GetPerlin().AddOctave(1.0f, 2.0f * multiplier);
	m_AfterGen.GetHeightMap().AddNoiseMap(doublePerlin);

	m_AfterGen.SetSize(static_cast<float>(terrainSize));


	// Create erosion algorithm
	m_pErosion = std::make_unique<RiverLand>();
}

void Erosion::TerrainGeneratorComponent::OnGUI()
{
	ImGui::Begin("Terrain Generator");

	if (ImGui::Button("Generate")) Generate();

	ImGui::Checkbox("Enable Erosion", &m_EnableErosion);
	ImGui::Checkbox("Enable Double Perlin", &m_EnableDoublePerlin);
	ImGui::DragScalar("Pos X", ImGuiDataType_::ImGuiDataType_U32, &m_PosX);
	ImGui::DragScalar("Pos Y", ImGuiDataType_::ImGuiDataType_U32, &m_PosY);

	m_pErosion->OnGUI();

	ImGui::End();
}

void Erosion::TerrainGeneratorComponent::Generate() const
{
	// Get the size and height data from the terrain component
	leap::TerrainComponent* pTerrain{ GetGameObject()->GetComponent<leap::TerrainComponent>() };
	auto heights{ pTerrain->GetHeights() };
	const unsigned int terrainSize{ static_cast<unsigned int>(sqrt(heights.size())) };

	// Generate a terrain by perlin noise
	for (unsigned int x{}; x < terrainSize; ++x)
	{
		for (unsigned int z{}; z < terrainSize; ++z)
		{
			float noiseValue{ m_Gen.GetHeight(static_cast<float>(x + m_PosX) / 4.0f, static_cast<float>(z + m_PosY) / 4.0f) };

			heights[x + z * terrainSize] = noiseValue;
		}
	}

	// Apply a erosion algorithm
	if (m_EnableErosion)
	{
		m_pErosion->GetHeights(heights);
	}

	if (m_EnableDoublePerlin)
	{
		// For each cell
		for (unsigned int x{}; x < terrainSize; ++x)
		{
			for (unsigned int z{}; z < terrainSize; ++z)
			{
				// Get the height in the current cell
				float& height{ heights[x + z * terrainSize] };

				// If the cell is water, discard it
				if (height < FLT_EPSILON) continue;

				// Calculate the perlin offset
				const float perlin{ m_AfterGen.GetHeight(static_cast<float>(x + m_PosX) / 4.0f, static_cast<float>(z + m_PosY) / 4.0f) / 10.0f };

				// Apply an offset at the beaches to avoid creating cliffs
				const float offset{ height < 0.1f ? height / 0.1f : 1.0f };

				// Add the perlin offset to the heightmap
				height += perlin * offset;
			}
		}
	}

	// Apply the new heightmap to the terrain component
	pTerrain->SetHeights(heights);
}
