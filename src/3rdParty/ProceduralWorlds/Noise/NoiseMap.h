#pragma once

#include "PerlinComposition.h"
#include "Graph.h"

namespace that
{
	class NoiseMap final
	{
	public:
		NoiseMap() = default;
		~NoiseMap() = default;

		float GetNoise(float x, float y) const;

		PerlinComposition& GetPerlin();
		Graph& GetGraph();

	private:
		PerlinComposition m_Perlin{};
		Graph m_Graph{};
	};
}