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
	// Create main perlin map
	GenerateNewPerlin();

	// Create a default erosion algorithm
	SetNewAlgorithm();

	// Get the terrain size from the terrain component
	m_pTerrain = GetGameObject()->GetComponent<leap::TerrainComponent>();
	m_TerrainSize = static_cast<unsigned int>(sqrt(m_pTerrain->GetHeights().size()));

	// Create optional second perlin map
	that::NoiseMap doublePerlin{};
	doublePerlin.GetGraph().AddNode(0.0f, 0.005f);
	doublePerlin.GetGraph().AddNode(0.5f, 0.25f);
	doublePerlin.GetGraph().AddNode(1.0f, 1.0f);
	doublePerlin.GetPerlin().AddOctave(1.0f, 10.0f * m_PerlinMultiplier);
	doublePerlin.GetPerlin().AddOctave(1.0f, 5.0f * m_PerlinMultiplier);
	doublePerlin.GetPerlin().AddOctave(1.0f, 2.0f * m_PerlinMultiplier);
	m_AfterGen.GetHeightMap().AddNoiseMap(doublePerlin);

	m_AfterGen.SetSize(static_cast<float>(m_TerrainSize));
}

void Erosion::TerrainGeneratorComponent::GenerateNewPerlin()
{
	// Create random seed
	srand(seed);

	m_pGen = std::make_unique<that::Generator>();

	// Create generator
	m_pGen->SetSize(static_cast<float>(m_TerrainSize));

	// Create a terrain generator
	if (m_UsePerlinPreset)
	{
		that::preset::Presets::CreateDefaultTerrain(*m_pGen, seed, m_PerlinMultiplier);
	}
	else
	{
		that::NoiseMap continentalNess{};
		continentalNess.GetGraph().AddNode(0.0f, 0.1f);
		continentalNess.GetGraph().AddNode(0.4f, 0.1f);
		continentalNess.GetGraph().AddNode(0.6f, 0.7f);
		continentalNess.GetGraph().AddNode(0.75f, 0.8f);
		continentalNess.GetGraph().AddNode(0.8f, 0.74f);
		continentalNess.GetGraph().AddNode(1.0f, 1.0f);
		continentalNess.GetPerlin().AddOctave(1.0f, 100.0f * m_PerlinMultiplier);
		m_pGen->GetHeightMap().AddNoiseMap(continentalNess);

		if (m_DetailedPerlin)
		{
			that::NoiseMap noise{};
			noise.GetGraph().AddNode(0.0f, m_DetailedPerlinMultiplier * 0.01f);
			noise.GetGraph().AddNode(0.5f, m_DetailedPerlinMultiplier * 0.25f);
			noise.GetGraph().AddNode(1.0f, m_DetailedPerlinMultiplier);
			noise.GetPerlin().AddOctave(1.0f, 10.0f * m_PerlinMultiplier);
			noise.GetPerlin().AddOctave(1.0f, 5.0f * m_PerlinMultiplier);
			noise.GetPerlin().AddOctave(1.0f, 2.0f * m_PerlinMultiplier);
			m_pGen->GetHeightMap().AddNoiseMap(noise);
		}
	}

	// Set the blendmode for the noisemaps
	m_pGen->GetHeightMap().SetBlendMode(m_UsePerlinPreset ? that::HeightMap::BlendMode::Multiply : that::HeightMap::BlendMode::Add);
}

void Erosion::TerrainGeneratorComponent::SetNewAlgorithm()
{
	switch (m_SelectedAlgorithmIdx)
	{
	case static_cast<int>(ErosionAlgorithm::VelocityField):
	{
		m_pErosion = std::make_unique<VelocityField>();
		break;
	}
	case static_cast<int>(ErosionAlgorithm::RiverLand):
	{
		m_pErosion = std::make_unique<RiverLand>();
		break;
	}
	case static_cast<int>(ErosionAlgorithm::HansBeyer):
	{
		m_pErosion = std::make_unique<HansBeyer>();
		break;
	}
	}
}

void Erosion::TerrainGeneratorComponent::OnGUI()
{
	ImGui::Begin("Terrain Generator");

	// Regenerate button
	if (ImGui::Button("Generate")) Generate();

	ImGui::Spacing();

	// Terrain Component Customization
	ImGui::DragScalar("Terrain Size", ImGuiDataType_::ImGuiDataType_U32, &m_TerrainSize);
	ImGui::InputScalar("Quads Per Meter", ImGuiDataType_::ImGuiDataType_U32, &m_QuadsPerMeter);
	if (ImGui::Button("Create new terrain"))
	{
		m_pTerrain->SetSize(m_TerrainSize, m_QuadsPerMeter);
	}

	ImGui::Spacing();

	// Perlin Customization
	if (ImGui::Checkbox("Use Library Preset", &m_UsePerlinPreset))
	{
		GenerateNewPerlin();
	}
	if (ImGui::Checkbox("Enable detailed perlin", &m_DetailedPerlin))
	{
		GenerateNewPerlin();
	}
	if (ImGui::SliderFloat("Detailed perlin multiplier", &m_DetailedPerlinMultiplier, 0.0f, 1.0f, "%.5f"))
	{
		GenerateNewPerlin();
	}
	if (ImGui::SliderFloat("Perlin Size", &m_PerlinMultiplier, 0.01f, 100.0f))
	{
		GenerateNewPerlin();
	}
	if (ImGui::Button("New seed"))
	{
		seed = static_cast<unsigned int>(time(nullptr));
		GenerateNewPerlin();
	}

	ImGui::Spacing();

	// Erosion algorithm drop down menu
	if (m_IsChoosingNewAlgorithm)
	{
		if (ImGui::ListBox("Erosion Algorithm", &m_SelectedAlgorithmIdx, m_ErosionAlgorithmStr, m_NrAlgorithms))
		{
			SetNewAlgorithm();
			m_IsChoosingNewAlgorithm = false;
		}
	}
	else
	{
		if(ImGui::Button(m_ErosionAlgorithmStr[m_SelectedAlgorithmIdx]))
		{
			m_IsChoosingNewAlgorithm = true;
		}
	}

	ImGui::Spacing();

	// Global erosion customization
	ImGui::Checkbox("Enable Erosion", &m_EnableErosion);
	ImGui::Checkbox("Enable Double Perlin", &m_EnableDoublePerlin);
	ImGui::DragScalar("Pos X", ImGuiDataType_::ImGuiDataType_U32, &m_PosX);
	ImGui::DragScalar("Pos Y", ImGuiDataType_::ImGuiDataType_U32, &m_PosY);

	// Implementation specific customization
	m_pErosion->OnGUI();

	ImGui::End();
}

void Erosion::TerrainGeneratorComponent::Generate() const
{
	// Get the size and height data from the terrain component
	auto heights{ m_pTerrain->GetHeights() };
	const unsigned int terrainSize{ static_cast<unsigned int>(sqrt(heights.size())) };

	// Generate a terrain by perlin noise
	const that::Generator& generator{ *m_pGen };
	for (unsigned int x{}; x < terrainSize; ++x)
	{
		for (unsigned int z{}; z < terrainSize; ++z)
		{
			float noiseValue{ generator.GetHeight(static_cast<float>(x + m_PosX), static_cast<float>(z + m_PosY)) };

			heights[x + z * terrainSize] = noiseValue;
		}
	}

	// Apply a erosion algorithm
	if (m_EnableErosion)
	{
		m_pErosion->GetHeights(heights);
	}


	// Apply the new heightmap to the terrain component
	m_pTerrain->SetHeights(heights);
}