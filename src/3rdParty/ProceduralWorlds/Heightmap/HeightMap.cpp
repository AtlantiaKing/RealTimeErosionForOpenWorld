#include "HeightMap.h"

void that::HeightMap::AddNoiseMap(const NoiseMap& noiseMap)
{
	m_NoiseMaps.push_back(noiseMap);
}

void that::HeightMap::SetBlendMode(BlendMode blendMode)
{
	m_BlendMode = blendMode;
}

float that::HeightMap::GetHeight(float x, float y) const
{
	// The default height should be 1 if the blendmode is set to Multiply, 
	//	otherwise all multiplications will be ignored
	float height{ m_BlendMode == BlendMode::Multiply ? 1.0f : 0.0f };

	// Generate the height for this coordinate using the appropriate blend mode
	for (const NoiseMap& noiseMap : m_NoiseMaps)
	{
		const float noise{ noiseMap.GetNoise(x, y) };

		switch (m_BlendMode)
		{
		case BlendMode::Multiply:
			height *= noise;
			break;
		case BlendMode::Add:
		case BlendMode::Average:
			height += noise;
			break;
		}
	}

	// Calculate the average if all the noise maps if the BlendMode is set to Average
	if (m_BlendMode == BlendMode::Average)
	{
		height /= static_cast<int>(m_NoiseMaps.size());
	}

	return height;
}