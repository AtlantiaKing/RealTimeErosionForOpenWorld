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
		virtual void Awake() override;
		virtual void OnGUI() override;

		void Generate() const;

		that::Generator m_Gen{};
		that::Generator m_AfterGen{};
		std::unique_ptr<ITerrainGenerator> m_pErosion{};

		unsigned int m_PosX{};
		unsigned int m_PosY{};
		bool m_EnableErosion{ false };
		bool m_EnableDoublePerlin{ false };
	};
}
