#pragma once

#include "ITerrainGenerator.h"

namespace Erosion
{
	class RiverLand final : public ITerrainGenerator
	{
	public:
		virtual ~RiverLand() = default;

		virtual void GetHeights(std::vector<float>& heights) override;
		virtual void OnGUI() override;
	private:
		struct RiverLandCell final
		{
			int x{};
			int z{};
			float slope{};
			float distance{ FLT_MAX };
			bool isChecked{};
		};

		float m_RiverHeight{ 0.35f };
		float m_CliffThreshold{ 0.0035f };
	};
}