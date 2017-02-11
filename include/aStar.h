//Copyright(c) 2015, Antoine Vugliano
#ifndef A_STAR_H
#define A_STAR_H

#include "gamecore.h"
#include "PathAlgorithm.h"
#include "Node.h"

namespace paths
{
    struct AStarNode : public Node
    {
        AStarNode();
        virtual ~AStarNode();

        void setPosition(int x, int y);
        void setF(float f);
        void setG(float g);
        void setH(float h);
        void setOpen(bool value);
        void setClosed(bool value);
        unsigned int getX() const;
        unsigned int getY() const;
        float getF() const;
        float getG() const;
        float getH() const;
        bool isOpen() const;
        bool isClosed() const;
        virtual float distanceTo(AStarNode* node) const = 0;
        void release();

    protected:
        float m_f, m_g, m_h;
        unsigned int m_x, m_y;
        bool open, closed;
    };

    struct CompareNodes
    {
        bool operator() (const AStarNode* s1, const AStarNode *s2) const;
    };

    class AStar : public PathAlgorithm<AStarNode>
    {
        AStar();
        ~AStar();

        void releaseNodes();
        void pushOpen(AStarNode* node);
        void popOpen(AStarNode* node);

        std::vector<AStarNode*> open, closed;

    public:

        static AStar& getInstance();

        bool getPath(AStarNode* start, AStarNode* goal, std::vector<AStarNode*>& path);
        void clear();        
    };
}
#endif // A_STAR_H