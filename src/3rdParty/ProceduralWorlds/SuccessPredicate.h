#pragma once

#include <functional>

namespace that
{
	class SuccessPredicate final
	{
	public:
		SuccessPredicate(std::function<bool(float, float)> predicate);
		~SuccessPredicate() = default;

		bool IsTrue(float height0, float height1) const;

	private:
		std::function<bool(float, float)> m_Predicate;
	};
}
