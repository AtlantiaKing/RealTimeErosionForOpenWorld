#pragma once

#include "ITerrainGenerator.h"

namespace Erosion
{
	class HansBeyer final : public ITerrainGenerator
	{
	public:
		virtual ~HansBeyer() = default;

		virtual void SetChunk(int x, int y) override { m_ChunkX = x; m_ChunkY = y; }
		virtual void GetHeights(Heightmap& heights) override;
		inline static float Random01() { return static_cast<float>(rand()) / RAND_MAX; }

		virtual void OnGUI() override;

	private:
		// Simulation data
		int m_Cycles{ 15106 };

		// Erosion radius data
		int m_ErosionRadius{ 5 };

		// Droplet simulation data
		int m_MaxPathLength{ 60 };
		float m_Inertia{ 0.1f };
		float m_MinSlope{ 0.05f };
		float m_Capacity{ 16.0f };
		float m_Gravity{ -9.81f };
		float m_Evaporation{ 0.0125f };
		float m_Deposition{ 0.095f };
		float m_Erosion{ 0.633f };

		// Chunk data
		int m_ChunkX{};
		int m_ChunkY{};
	};
}