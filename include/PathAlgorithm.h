//Copyright(c) 2015, Antoine Vugliano
#ifndef PATH_ALGORITHM_H
#define PATH_ALGORITHM_H

#include "gamecore.h"

namespace paths
{
    /**
    Represents a generic path finding algorithm. It can not be used
    or instanciated as-is and is provided for implementing algorithms.
    PathAlgorithm only provide general methods for use with PathFinder.

    @see PathFinder
    */
    template<typename T> class PathAlgorithm
    {
    public:

        typedef T node_t;

        /**
        @brief The core method of the algorithm, where the path will be evaluated.
        @param[in] start A pointer referencing the start node.
        @param[in] goal A pointer referencing the goal node.
        @param[out] path A vector of nodes which will be filled with the nodes from
        the path found, if there is one.
        @return true if a path is found, false if there isn't
        */
        virtual bool getPath(node_t* start, node_t* goal, std::vector<node_t*>& path) = 0;

        /**
        @brief Provides a way for the algorithm to clean-up its data, if needed. Useful
        when storing information on nodes, for example.
        */
        virtual void clear() = 0;

    protected:

        /**
        @brief Computes the distance between two nodes using the specified
        Node::distanceTo() method from T.
        @param[in] n1 A pointer referencing the source node.
        @param[in] n2 A pointer referencing the destination node.
        @see Node::distanceTo()
        */
        inline float distanceBetween(node_t* n1, node_t* n2) const
        {
            return n1->distanceTo(n2);
        }

        /**
        @brief Builds the path from the goal found back up to the start.
        @param[in] node The node from where to get the path.
        @param[out] path The vector to fill with the nodes.
        */
        void reconstructPath(node_t* node, std::vector<node_t*>& path)
        {
            node_t* parent = static_cast<node_t*>(node->getParent());
            path.push_back(node);
            while (parent != nullptr)
            {
                path.push_back(parent);
                parent = static_cast<node_t*>(parent->getParent());
            }
        }
    };
}
#endif // PATH_ALGORITHM_H
