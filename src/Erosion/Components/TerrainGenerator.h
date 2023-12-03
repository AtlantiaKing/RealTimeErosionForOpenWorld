#pragma once

#include <Components/Component.h>

namespace Erosion
{
	class TerrainGenerator final : public leap::Component
	{
	public:
		TerrainGenerator() = default;
		virtual ~TerrainGenerator() = default;

		TerrainGenerator& operator=(const TerrainGenerator& other) = delete;
		TerrainGenerator& operator=(TerrainGenerator&& other) = delete;
		TerrainGenerator(const TerrainGenerator& other) = delete;
		TerrainGenerator(TerrainGenerator&& other) = delete;

	private:
		virtual void Awake() override;
	};
}
