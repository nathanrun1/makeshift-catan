#pragma once

class Node;

class Occupation {
public:
	Node& node;

	Occupation(Node& node) : node(node) {}
};