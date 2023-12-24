#pragma once

#include "ITerrainGenerator.h"

namespace Erosion
{
	class VelocityField final : public ITerrainGenerator
	{
	public:
		struct Cell final
		{
			float* height{};
			float water{};
			float leftFlux{};
			float rightFlux{};
			float topFlux{};
			float bottomFlux{};
			float velocityX{};
			float velocityY{};
			float sediment{};
			float incomingSediment{};
		};

		virtual ~VelocityField() = default;

		virtual void GetHeights(std::vector<float>& heights) override;
		virtual void OnGUI() override;

	private:
		int m_Cycles{ 248 };
		int m_DropletsPerCycle{ 100 };
		float m_WaterIncrement{ 0.0316f };
		float m_PipeArea{ 1.0f };
		float m_Gravity{ -9.81f };
		float m_PipeLength{ 1.0f };
		float m_Capacity{ 0.003934f };
		float m_Erosion{ 0.084f };
		float m_Deposition{ 0.0f };
		float m_Evaporation{ 0.090f };
		float m_FluxEpsilon{ 0.000001f };
		float m_TimeStep{ 1.0f };
		int m_ErosionRadius{ 5 };
	};
}