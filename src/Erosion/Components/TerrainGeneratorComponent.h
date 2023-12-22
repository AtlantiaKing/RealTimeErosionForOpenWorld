#pragma once

#include <Components/Component.h>
#include <Generator.h>

#include "../ErosionAlgorithms/ITerrainGenerator.h"

#include <memory>

namespace Erosion
{
	class TerrainGeneratorComponent final : public leap::Component
	{
	public:
		TerrainGeneratorComponent() = default;
		virtual ~TerrainGeneratorComponent() = default;

		TerrainGeneratorComponent& operator=(const TerrainGeneratorComponent& other) = delete;
		TerrainGeneratorComponent& operator=(TerrainGeneratorComponent&& other) = delete;
		TerrainGeneratorComponent(const TerrainGeneratorComponent& other) = delete;
		TerrainGeneratorComponent(TerrainGeneratorComponent&& other) = delete;

	private:
		enum class ErosionAlgorithm
		{
			HansBeyer,
			VelocityField,
			RiverLand
		};
		inline const static int m_NrAlgorithms{ 3 };
		inline const static char* m_ErosionAlgorithmStr[m_NrAlgorithms] { "Hans Beyer", "Velocity Field", "River Land" };

		virtual void Awake() override;
		virtual void OnGUI() override;

		void GenerateNewPerlin();
		void SetNewAlgorithm();
		void Generate() const;

		std::unique_ptr<that::Generator> m_pGen{};
		that::Generator m_AfterGen{};
		std::unique_ptr<ITerrainGenerator> m_pErosion{};

		unsigned int seed{};

		float m_PerlinMultiplier{ 8.0f };
		int m_SelectedAlgorithmIdx{};
		unsigned int m_PosX{};
		unsigned int m_PosY{};
		bool m_DetailedPerlin{ false };
		bool m_EnableErosion{ false };
		bool m_EnableDoublePerlin{ false };

		bool m_IsChoosingNewAlgorithm{};
	};
}
