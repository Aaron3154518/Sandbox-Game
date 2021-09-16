#include "SparseArray.h"

bool CmpCoords::operator()(const std::pair<int, int>& lhs,
	const std::pair<int, int>& rhs) {
	return lhs.first < rhs.first
		|| (lhs.first == rhs.first && lhs.second < rhs.second);
}
