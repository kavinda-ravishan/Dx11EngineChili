#pragma once

namespace dx3d {
	class Node;

	class ModelProbe {
	public:
		virtual bool PushNode(Node& node) = 0;
		virtual void PopNode(Node& node) = 0;
	};
} // namespace dx3d
