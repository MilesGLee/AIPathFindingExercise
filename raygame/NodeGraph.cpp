#include "NodeGraph.h"
#include <raylib.h>
#include <xlocinfo>

DynamicArray<NodeGraph::Node*> reconstructPath(NodeGraph::Node* start, NodeGraph::Node* end)
{
	DynamicArray<NodeGraph::Node*> path;
	NodeGraph::Node* currentNode = end;

	while (currentNode != start->previous)
	{
		currentNode->color = 0xFFFF00FF;
		path.insert(currentNode, 0);
		currentNode = currentNode->previous;
	}

	return path;
}

float diagonalDistance(NodeGraph::Node* node, NodeGraph::Node* goal, float cardinalCost, float diagonalCost)
{
	float displacementX = abs(node->position.x - goal->position.x);
	float displacementY = abs(node->position.y - goal->position.y);

	return cardinalCost * (displacementX + displacementY) + (diagonalCost - 2 * cardinalCost) * fmin(displacementX, displacementY);
}

void sortFScore(DynamicArray<NodeGraph::Node*>& nodes)
{
	NodeGraph::Node* key = nullptr;
	int j = 0;

	for (int i = 1; i < nodes.getLength(); i++) {
		key = nodes[i];
		j = i - 1;
		while (j >= 0 && nodes[j]->fScore > key->fScore) {
			nodes[j + 1] = nodes[j];
			j--;
		}

		nodes[j + 1] = key;
	}
}

void sortGScore(DynamicArray<NodeGraph::Node*>& nodes)
{
	NodeGraph::Node* key = nullptr;
	int j = 0;

	for (int i = 1; i < nodes.getLength(); i++) {
		key = nodes[i];
		j = i - 1;
		while (j >= 0 && nodes[j]->gScore > key->gScore) {
			nodes[j + 1] = nodes[j];
			j--;
		}

		nodes[j + 1] = key;
	}
}

void sortHScore(DynamicArray<NodeGraph::Node*>& nodes)
{
	NodeGraph::Node* key = nullptr;
	int j = 0;

	for (int i = 1; i < nodes.getLength(); i++) {
		key = nodes[i];
		j = i - 1;
		while (j >= 0 && nodes[j]->hScore > key->hScore) {
			nodes[j + 1] = nodes[j];
			j--;
		}

		nodes[j + 1] = key;
	}
}

DynamicArray<NodeGraph::Node*> NodeGraph::findPath(Node* start, Node* goal)
{
	resetGraphScore(start); //Reset graphs to 0
	NodeGraph::Node* currentNode;
	float gScore = 0;
	float hScore = 0;
	DynamicArray<NodeGraph::Node*> openList, closedList = DynamicArray<NodeGraph::Node*>(); //The two lists of nodes the function will use to sort and find the goal.
	openList.addItem(start);
	currentNode = start;
	while (openList.getLength() != 0) //While the open list is not empty.
	{
		sortFScore(openList); //Sort the F score of the open lists items.
		currentNode = openList[0]; //Set the current node to the first item in the open list.

		if (currentNode == goal) //If the current node is our goal, end this function and return the path.
			return reconstructPath(start, currentNode);

		closedList.addItem(currentNode); //Remove the current node from the open list and add it to the closed list.
		openList.remove(currentNode);

		for (int n = 0; n < currentNode->edges.getLength(); n++) //Loop through each edge of the current node.
		{
			NodeGraph::Node* targetNode = currentNode->edges[0].target; //The target node will be the node tested for its scores.
			if (currentNode->walkable == false) //If the current node is not a wall.
				continue;
			if (!closedList.contains(currentNode->edges[n].target)) //If the target node is not in the closed list.
			{
				gScore = currentNode->edges[n].cost + currentNode->gScore; //Set the G score of the target node to the current node's g score + its edge cost.
				hScore = manhattanDistance(currentNode->edges[n].target, goal); //Find the manhattan distance and set it to the target nodes H score.
			}
			else
				continue;
			//If the f score of the current nodes target is more than the target nodes g score.
			if (currentNode->edges[n].target->fScore > (gScore + hScore))
			{
				currentNode->edges[n].target->color = 0x0FFFFF;//changes color
				currentNode->edges[n].target->gScore = gScore; //sets the current nodes edge targets g score to the target nodes g score.
				currentNode->edges[n].target->hScore = hScore; //sets the current nodes edge targets h score to the target nodes h score.
				currentNode->edges[n].target->fScore = gScore + hScore; //Sets the current nodes edge target to the sum of the target nodes g and h scores.
				currentNode->edges[n].target->previous = currentNode; //sets the current nodes edge targets previous node to the current node.
			}
			if (!openList.contains(targetNode->edges[n].target)) //If the target nodes edge target is not in the open list.
			{
				openList.addItem(currentNode->edges[n].target); //Add the current nodes edge target to the open list.
				currentNode->edges[n].target->color = 0x0FFFFF;//changes color
				currentNode->edges[n].target->gScore = gScore; //sets the current nodes edge targets g score to the target nodes g score.
				currentNode->edges[n].target->hScore = hScore; //sets the current nodes edge targets h score to the target nodes h score.
				currentNode->edges[n].target->fScore = gScore + hScore; //Sets the current nodes edge target to the sum of the target nodes g and h scores.
				currentNode->edges[n].target->previous = currentNode; //sets the current nodes edge targets previous node to the current node.
			}
		}
	}
	return reconstructPath(start, goal); //Construct and return the new path using A*.
}

float NodeGraph::manhattanDistance(Node* left, Node* right)
{
	return abs(right->position.x - left->position.x) + abs(right->position.y - left->position.y); //Math equation used to find the distance in a way that creates a diagonal input on a 2D grid.
}

void NodeGraph::drawGraph(Node* start)
{
	DynamicArray<Node*> drawnList = DynamicArray<Node*>();
	drawConnectedNodes(start, drawnList);
}

void NodeGraph::drawNode(Node* node, float size)
{
	static char buffer[10];
	sprintf_s(buffer, "%.0f", node->fScore);

	//Draw the circle for the outline
	DrawCircle((int)node->position.x, (int)node->position.y, size + 1, GetColor(node->color));
	//Draw the inner circle
	DrawCircle((int)node->position.x, (int)node->position.y, size, GetColor(node->color));
	//Draw the text
	DrawText(buffer, (int)node->position.x, (int)node->position.y, .8f, BLACK);
}

void NodeGraph::drawConnectedNodes(Node* node, DynamicArray<Node*>& drawnList)
{
	drawnList.addItem(node);
	if (node->walkable)
		drawNode(node, 8);

	for (int i = 0; i < node->edges.getLength(); i++)
	{
		Edge e = node->edges[i];
		////Draw the Edge
		//DrawLine((int)node->position.x, (int)node->position.y, (int)e.target->position.x, (int)e.target->position.y, WHITE);
		////Draw the cost
		//MathLibrary::Vector2 costPos = { (node->position.x + e.target->position.x) / 2, (node->position.y + e.target->position.y) / 2 };
		//static char buffer[10];
		//sprintf_s(buffer, "%.0f", e.cost);
		//DrawText(buffer, (int)costPos.x, (int)costPos.y, 16, RAYWHITE);
		//Draw the target node
		if (!drawnList.contains(e.target)) {
			drawConnectedNodes(e.target, drawnList);
		}
	}
}

void NodeGraph::resetGraphScore(Node * start)
{
	DynamicArray<Node*> resetList = DynamicArray<Node*>();
	resetConnectedNodes(start, resetList);
}

void NodeGraph::resetConnectedNodes(Node* node, DynamicArray<Node*>& resetList)
{
	resetList.addItem(node);

	for (int i = 0; i < node->edges.getLength(); i++)
	{
		node->edges[i].target->gScore = 0;
		node->edges[i].target->hScore = 0;
		node->edges[i].target->fScore = 0;
		node->edges[i].target->color = 0xFFFFFFFF;

		//Draw the target node
		if (!resetList.contains(node->edges[i].target)) {
			resetConnectedNodes(node->edges[i].target, resetList);
		}
	}
}