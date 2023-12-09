#pragma once

#include "WorldShape.h"

namespace that::shape
{
	class SquarePeak final : public WorldShape
	{
	public:
		/// <summary>
		/// Create a world shape that is a rounded square. The heightmap values usually higher at the center.
		/// </summary>
		/// <param name="angularity">Roundedness of the square. Higher value is less rounded. This number needs to be in steps of 2.</param>
		/// <param name="smoothPower">How quickly the heightmap values fade to 1 when nearing the center of the square.</param>
		SquarePeak(float angularity, float smoothPower);
		virtual ~SquarePeak() = default;

		virtual float Transform(float size, float x, float y, float height) const override;

	private:
		float m_SmoothPower{};
		float m_Angularity{};
	};
}
