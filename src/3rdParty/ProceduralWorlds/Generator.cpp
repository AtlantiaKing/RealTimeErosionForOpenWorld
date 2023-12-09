#include "Generator.h"

#include "WorldShape/WorldShape.h"

void that::Generator::SetSize(float size)
{
	m_Size = size;
}

void that::Generator::AddPredicate(const SuccessPredicate& predicate)
{
	m_SuccessPredicates.emplace_back(predicate);
}

void that::Generator::SetShape(std::unique_ptr<shape::WorldShape> pShape)
{
	m_pShape = std::move(pShape);
}

that::HeightMap& that::Generator::GetHeightMap()
{
	return m_HeightMap;
}

float that::Generator::GetHeight(float x, float y) const
{
	// Retrieve the height from the heightmap
	float height{ m_HeightMap.GetHeight(x, y) };

	// If a shape is assigned, transform the heightmap to match the shape
	if (m_pShape) height = m_pShape->Transform(m_Size, x, y, height);

	return height;
}

bool that::Generator::TryPredicates(int step)
{
	if (m_SuccessPredicates.empty()) return true;

	std::vector<SuccessPredicate> predicatesCopy{ m_SuccessPredicates };

	for (int x{}; x < m_Size; x += step)
	{
		float prevHeight{ GetHeight(static_cast<float>(x), 0.0f)};

		for (int z{ 1 }; z < m_Size; z += step)
		{
			float height{ GetHeight(static_cast<float>(x), static_cast<float>(z)) };

			for (int i{ static_cast<int>(predicatesCopy.size()) - 1 }; i >= 0; --i)
			{
				const auto& predicate{ predicatesCopy[i] };

				if (predicate.IsTrue(height, prevHeight)) predicatesCopy.erase(begin(predicatesCopy) + i);
			}

			prevHeight = height;
		}
	}

	if (predicatesCopy.empty()) return true;

	for (int z{}; z < m_Size; z += step)
	{
		float prevHeight{ GetHeight(0.0f, static_cast<float>(z)) };

		for (int x{ 1 }; x < m_Size; x += step)
		{
			float height{ GetHeight(static_cast<float>(x), static_cast<float>(z)) };

			for (int i{ static_cast<int>(predicatesCopy.size()) - 1 }; i >= 0; --i)
			{
				const auto& predicate{ predicatesCopy[i] };

				if (predicate.IsTrue(height, prevHeight)) predicatesCopy.erase(begin(predicatesCopy) + i);
			}

			prevHeight = height;
		}
	}

	return predicatesCopy.empty();
}