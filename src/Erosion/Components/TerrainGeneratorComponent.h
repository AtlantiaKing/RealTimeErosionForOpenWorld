#pragma once

#include <Components/Component.h>
#include <Generator.h>

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
		unsigned int m_PosX{};
		unsigned int m_PosY{};
		bool m_EnableErosion{};
	};
}
