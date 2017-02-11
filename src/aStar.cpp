//Copyright (c) 2015, Antoine Vugliano
#include "../include/AStar.h"

namespace paths
{
    AStarNode::AStarNode() : m_f(0.0f), m_g(0.0f), m_h(0.0f), closed(false), open(false) {}
    AStarNode::~AStarNode() {}

    void AStarNode::setPosition(int x, int y)
    {
        m_x = x;
        m_y = y;
    }

    void AStarNode::setF(float f) { m_f = f; }
    void AStarNode::setG(float g) { m_g = g; }
    void AStarNode::setH(float h) { m_h = h; }
    void AStarNode::setOpen(bool value) { open = value; }
    void AStarNode::setClosed(bool value) { closed = value; }
    unsigned int AStarNode::getX() const { return m_x; }
    unsigned int AStarNode::getY() const { return m_y; }
    float AStarNode::getF() const { return m_f; }
    float AStarNode::getG() const { return m_g; }
    float AStarNode::getH() const { return m_h; }
    bool AStarNode::isOpen() const { return open; }
    bool AStarNode::isClosed() const { return closed; }

    void AStarNode::release()
    {
        open = closed = false;
        m_f = m_g = m_h = 0.0f;
        m_parent = nullptr;
    }

    bool CompareNodes::operator() (const AStarNode* s1, const AStarNode *s2) const
    {
        return s1->getF() < s2->getF();
    }

    AStar::AStar() {}
    AStar::~AStar() {}

    AStar& AStar::getInstance()
    {
        static AStar instance;
        return instance;
    }

    bool AStar::getPath(AStarNode* start, AStarNode* goal, std::vector<AStarNode*>& path)
    {
        AStarNode *currentNode, *childNode;
        float f, g, h;

        std::make_heap(open.begin(), open.end(), CompareNodes());
        pushOpen(start);

        while (!open.empty())
        {
            std::sort(open.begin(), open.end(), CompareNodes());

            currentNode = open.front(); // pop n node from open for which f is minimal
            popOpen(currentNode);

            currentNode->setClosed(true);
            closed.push_back(currentNode);

            if (currentNode == goal)
            {
                reconstructPath(currentNode, path);
                return true;
            }

            for (const auto& children : currentNode->getChildren())// for each successor n' of n
            {
                childNode = static_cast<AStarNode*>(children.first);
                g = currentNode->getG() + children.second; // stance from start + distance between the two nodes
                if ((childNode->isOpen() || childNode->isClosed()) && childNode->getG() < g) // n' is already in opend or closed with a lower cost g(n')
                    continue; // consider next successor

                h = distanceBetween(childNode, goal);
                f = g + h; // compute f(n')
                childNode->setF(f);
                childNode->setG(g);
                childNode->setH(h);
                childNode->setParent(currentNode);

                if (childNode->isClosed())
                    childNode->setClosed(false);
                if (!childNode->isOpen())
                    pushOpen(childNode);
            }
        }
        return false;
    }

    void AStar::pushOpen(AStarNode* node)
    {
        open.push_back(node);
        std::push_heap(open.begin(), open.end(), CompareNodes());
        node->setOpen(true);
    }

    void AStar::popOpen(AStarNode* node)
    {
        std::pop_heap(open.begin(), open.end(), CompareNodes());
        open.pop_back();
        node->setOpen(false);
    }

    void AStar::releaseNodes()
    {
        for (const auto& node : open)
            node->release();
        for (const auto& node : closed)
            node->release();
    }

    void AStar::clear()
    {
        releaseNodes();
        open.clear();
        closed.clear();
    }
}
