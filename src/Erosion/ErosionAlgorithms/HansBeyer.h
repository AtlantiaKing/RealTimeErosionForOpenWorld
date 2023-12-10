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
	};
}