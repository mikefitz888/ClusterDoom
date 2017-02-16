//Copyright (c) 2015, Antoine Vugliano
#ifndef PATH_FINDER_H
#define PATH_FINDER_H

#include "gamecore.h"

namespace paths
{
    /**
    The PathFinder is the main class to use for (obviously) path finding.
    @tparam T The type of nodes you are working on. It <b>must</b> derive
    from Node and implement the virtual methods.
    @see Node
    */
    template <typename T> class PathFinder
    {
        T* m_start;
        T* m_goal;
    public:

        /**
        @brief Default constructor.
        */
        explicit PathFinder() : m_start(nullptr), m_goal(nullptr) {}

        /**
        @brief Sets the start node for the next path findings.
        @param[in] start A reference to the start node.
        */
        void setStart(T& start)
        {
            m_start = &start;
        }

        /**
        @brief Sets the goal node for the next path findings.
        @param[in] start A reference to the goal node.
        */
        void setGoal(T& goal)
        {
            m_goal = &goal;
        }

        /**
        @brief Returns the address of the start node for.
        @return The address of the start node.
        */
        T* getStart() const
        {
            return m_start;
        }

        /**
        @brief Returns the address of the goal node for.
        @return The address of the goal node.
        */
        T* getGoal() const
        {
            return m_goal;
        }

        /**
        @brief Use the specified algorithm to find a path
        between the previously set start and goal.
        @tparam U The algorithm to use (for example : AStar).
        @param[out] solution The vector receiving the path found.
        @param[in] hint Optional : gives the algorithm a hint about
        the number of nodes it is subject to have in the path. Do
        not provide if you don't know what to give.
        */
        template <typename A> bool findPath(std::vector<T*>& solution, int hint = 0)
        {
            std::vector<A::node_t*> path;
            A &algorithm = A::getInstance();

            if (hint > 0) path.reserve(hint);
            
            bool pathFound = algorithm.getPath(m_start, m_goal, path);
            algorithm.clear();
            
            if (!pathFound)  return false;
            if (hint > 0)  solution.reserve(hint);
            for (auto rit = std::rbegin(path); rit != std::rend(path); ++rit) solution.push_back(static_cast<T*>(*rit));
            return true;
        }
    };
}

#endif // PATH_FINDER_H