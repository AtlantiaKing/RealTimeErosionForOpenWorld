#include "SuccessPredicate.h"

that::SuccessPredicate::SuccessPredicate(std::function<bool(float, float)> predicate)
	: m_Predicate{ predicate }
{
}

bool that::SuccessPredicate::IsTrue(float height0, float height1) const
{
	return m_Predicate(height0, height1);
}
