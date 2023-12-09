#pragma once

#include "../Noise/NoiseMap.h"

#include <vector>

namespace that
{
	class HeightMap final
	{
	public:
		enum class BlendMode
		{
			Multiply,
			Add,
			Average
		};

		HeightMap() = default;
		~HeightMap() = default;

		void AddNoiseMap(const NoiseMap& noiseMap);
		void SetBlendMode(BlendMode blendMode);

		float GetHeight(float x, float y) const;

	private:
		std::vector<NoiseMap> m_NoiseMaps{};

		BlendMode m_BlendMode{ BlendMode::Average };
	};
}

