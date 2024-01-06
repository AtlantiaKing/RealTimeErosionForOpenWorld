#pragma once

#include <vector>
#include "../Data/Heightmap.h"

namespace Erosion
{
	class ITerrainGenerator
	{
	public:
		virtual ~ITerrainGenerator() = default;

		virtual void SetChunk(int x, int y) = 0;
		virtual void GetHeights(Heightmap& heights) = 0;
		virtual void OnGUI() = 0;
	};
}