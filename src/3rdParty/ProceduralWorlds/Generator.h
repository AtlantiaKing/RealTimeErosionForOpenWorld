#pragma once

#include "Heightmap/HeightMap.h"
#include "SuccessPredicate.h"
#include "WorldShape/WorldShape.h"

#include <memory>

namespace that
{
	class Generator final
	{
	public:
		Generator() = default;
		~Generator() = default;

		void SetSize(float size);
		void AddPredicate(const SuccessPredicate& predicate);
		void SetShape(std::unique_ptr<shape::WorldShape> pShape);

		HeightMap& GetHeightMap();

		// Returns the heightmap value [0,1] for this coordinate
		float GetHeight(float x, float y) const;
		bool TryPredicates(int step);

	private:
		HeightMap m_HeightMap{};

		std::vector<SuccessPredicate> m_SuccessPredicates{};

		std::unique_ptr<shape::WorldShape> m_pShape{};

		float m_Size{ 100.0f };
	};
}