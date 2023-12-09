#pragma once

#include "WorldShape.h"

namespace that::shape
{
	class CirclePeak final : public WorldShape
	{
	public:
		/// <summary>
		/// Create a world shape that is a circle. The heightmap values usually higher at the center.
		/// </summary>
		/// <param name="smoothPower">How quickly the heightmap values fade to 1 when nearing the center of the circle.</param>
		CirclePeak(float smoothPower);
		virtual ~CirclePeak() = default;

		virtual float Transform(float size, float x, float y, float height) const override;

	private:
		float m_SmoothPower{};
	};
}
