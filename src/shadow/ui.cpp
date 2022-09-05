//
// Created by Vince on 4/5/22.
//

#include <vector>

enum anchorPos {
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

namespace ShadowUI {
	class FlowCoordinator {
	public:
		std::vector<int> pos = {0, 0};
		anchorPos anchor = TopLeft;
	};
}
