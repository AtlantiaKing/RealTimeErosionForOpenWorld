#pragma once

#include <vector>

namespace Erosion
{
	class ITerrainGenerator
	{
	public:
		virtual ~ITerrainGenerator() = default;

		virtual void GetHeights(std::vector<float>& heights) = 0;
		virtual void OnGUI() = 0;
	};
}