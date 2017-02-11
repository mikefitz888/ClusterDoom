//Copyright (c) 2015, Antoine Vugliano
#include "../include/Dijkstra.h"

namespace paths
{
    const float DijkstraNode::infinity = std::numeric_limits<float>::infinity();

    DijkstraNode::DijkstraNode() : distance(infinity), closed(false) {}

    DijkstraNode::~DijkstraNode() {}

    void DijkstraNode::setClosed(bool value) { closed = value; }
    void DijkstraNode::setDistance(float value) { distance = value; }
    bool DijkstraNode::isClosed() const { return closed; }
    float DijkstraNode::getDistance() const { return distance; }

    void DijkstraNode::release()
    {
        distance = infinity;
        closed = false;
        m_parent = nullptr;
    }

    bool CompareNodes::operator()(const DijkstraNode* n1, const DijkstraNode* n2)
    {
        return n1->getDistance() < n2->getDistance();
    }

    Dijkstra::Dijkstra() {}
    Dijkstra::~Dijkstra() {}

    Dijkstra& Dijkstra::getInstance()
    {
        static Dijkstra instance;
        return instance;
    }

    bool Dijkstra::getPath(DijkstraNode* start, DijkstraNode* goal, std::vector<DijkstraNode*>& path)
    {
        DijkstraNode *currentNode, *childNode;
        float dist;

        std::make_heap(open.begin(), open.end(), CompareNodes());
        pushOpen(start);
        start->setDistance(0.0f);

        while (!open.empty())
        {
            std::sort(open.begin(), open.end(), CompareNodes());

            currentNode = open.front();
            popOpen(currentNode);

            if (currentNode == goal)
            {
                reconstructPath(currentNode, path);
                return true;
            }

            for (const auto& children : currentNode->getChildren())
            {
                childNode = static_cast<DijkstraNode*>(children.first);

                dist = currentNode->getDistance() + children.second;
                if (!childNode->isClosed() && dist < childNode->getDistance())
                {
                    childNode->setDistance(dist);
                    childNode->setParent(currentNode);
                    pushOpen(childNode);
                }
            }
        }

        return false;
    }

    void Dijkstra::pushOpen(DijkstraNode* node)
    {
        open.push_back(node);
        std::push_heap(open.begin(), open.end(), CompareNodes());
    }

    void Dijkstra::popOpen(DijkstraNode* node)
    {
        std::pop_heap(open.begin(), open.end(), CompareNodes());
        open.pop_back();
        node->setClosed(true);
        closed.push_back(node);
    }


    void Dijkstra::releaseNodes()
    {
        for (const auto& node : open)
            node->release();
        for (const auto& node : closed)
            node->release();
    }

    void Dijkstra::clear()
    {
        releaseNodes();
        open.clear();
        closed.clear();
    }
}