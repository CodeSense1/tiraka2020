#include "routenode.hh"

RouteNode::RouteNode(RouteID id)
{
    this->id = id;
}

RouteNode::~RouteNode()
{

}

RouteID RouteNode::getId()
{
    return id;
}



void RouteNode::addStops(std::vector<std::pair<StopNode*, StopNode* >> stopRoutes)
{
    first = stopRoutes.front().first;
    for (auto i : stopRoutes) {
        stopMap.insert({i.first, i.second});
        last = i.first;
    }
}


bool RouteNode::hasStops()
{
    return !stopMap.empty();
}

StopNode* RouteNode::getStopInRoute(StopNode* node)
{
    // loop over stops to find the node
    // Then return the next stop relative to that node
    auto it = stopMap.find(node);
    if (it != stopMap.end()) {
        return it->second;
    } else
        // No stop was found
        return nullptr;
}

std::vector<StopID> RouteNode::getStopsInRoute()
{
    std::vector<StopID> output;
    for (auto& i : stopMap) {
        output.push_back(i.first->getId());
    }
    return output;
}

void RouteNode::clearAllStops()
{
    stopMap.clear();
}

StopNode* RouteNode::getLastStopNode()
{
    return last;
}

bool RouteNode::operator==(RouteNode *other)
{
    return this->id == other->getId();
}


