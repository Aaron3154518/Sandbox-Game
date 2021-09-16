#include "Tiles.h"

namespace tile {
	int getCraftOrder(Id id) {
		switch (id) {
			case Id::HAND_CRAFTING: return 0;
			case Id::WORK_TABLE: return 1;
		};
		return INT_MAX;
	}
}