#include "NoiseMap.h"

float that::NoiseMap::GetNoise(float x, float y) const
{
	return m_Graph.GetValue(m_Perlin.GetNoise(x, y));
}

that::PerlinComposition& that::NoiseMap::GetPerlin()
{
	return m_Perlin;
}

that::Graph& that::NoiseMap::GetGraph()
{
	return m_Graph;
}