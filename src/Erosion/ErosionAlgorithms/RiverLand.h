#pragma once

#include "ITerrainGenerator.h"

#include <Generator.h>

namespace Erosion
{
	class RiverLand final : public ITerrainGenerator
	{
	public:
		RiverLand();
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
			//float minHeight{};
			//int ptrIdx{ -1 };
			bool isChecked{};
		};

		void NewMaps();

		float m_RiverHeight{ 0.32781f };
		float m_Multiplier{ 0.32781f };
		float m_Divider{ /*509.987f*/82.828f };
		float m_CliffThreshold{ 0.01132f };
		int m_BlurSize{ /*9*/28 };
		bool m_DoCliffDetection{false};

		that::NoiseMap m_SlopeMap{};
		std::vector<std::pair<that::NoiseMap, that::NoiseMap>> m_RiverMaps{};
	};
}