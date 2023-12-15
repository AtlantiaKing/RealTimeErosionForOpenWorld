#pragma once

#include "ITerrainGenerator.h"

namespace Erosion
{
	class HansBeyer final : public ITerrainGenerator
	{
	public:
		virtual ~HansBeyer() = default;

		virtual void GetHeights(std::vector<float>& heights) override;
		inline static float Random01() { return static_cast<float>(rand()) / RAND_MAX; }

		virtual void OnGUI() override;

	private:
		// Simulation data
		int m_Cycles{ 1000000 };

		// Erosion radius data
		int m_ErosionRadius{ 9 };

		// Droplet simulation data
		int m_MaxPathLength{ 60 };
		float m_Inertia{ 0.1f };
		float m_MinSlope{ 0.05f };
		float m_Capacity{ 16.0f };
		float m_Gravity{ -9.81f };
		float m_Evaporation{ 0.0125f };
		float m_Deposition{ 1.0f };
		float m_Erosion{ 0.9f };
	};
}