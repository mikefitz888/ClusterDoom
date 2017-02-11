//Copyright (c) 2015, Antoine Vugliano
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "gamecore.h"
#include "PathAlgorithm.h"
#include "Node.h"

namespace paths
{
    struct DijkstraNode : public Node
    {
        DijkstraNode();
        ~DijkstraNode();

        void setClosed(bool value);
        void setDistance(float value);
        bool isClosed() const;
        float getDistance() const;

        void release();
    private: static const float infinity;
    protected:
        bool closed;
        float distance;
    };


    struct CompareNodes
    {
        bool operator()(const DijkstraNode* n1, const DijkstraNode* n2);
    };


    class Dijkstra : public PathAlgorithm<DijkstraNode>
    {
    public:

        static Dijkstra& getInstance();
        bool getPath(DijkstraNode* start, DijkstraNode* goal, std::vector<DijkstraNode*>& path);
        void clear();

    private:

        Dijkstra();
        ~Dijkstra();

        void pushOpen(DijkstraNode* node);
        void popOpen(DijkstraNode* node);
        void releaseNodes();

        std::vector<DijkstraNode*> open, closed;
    };
}
#endif // DIJKSTRA_H