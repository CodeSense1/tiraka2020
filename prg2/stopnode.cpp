#include "stopnode.hh"
#include "regionnode.hh"
#include "routenode.hh"

#include <cmath>
#include <algorithm>

StopNode::StopNode()
{
    id = NO_STOP;
    name = NO_NAME;
    coord = NO_COORD;
}

StopNode::StopNode(StopID id, Name name, Coord coord)
{
    this->id = id;
    this->name = name;
    this->coord = coord;
}

StopNode::~StopNode()
{
    // datastructure.cc poistaa kaikki stopnodet, sekä
    // regionit, joten tänne ei tarvitse tehdä mitään
}

StopID StopNode::getId()
{
    return id;
}

Name StopNode::getName() {
    return name;
}

Coord StopNode::getCoord() {
    return coord;
}

RegionNode *StopNode::getRegion()
{
    return region;
}

Name StopNode::setName(Name newName)
{
    Name temp = this->name;
    this->name = newName;
    return temp;

}

void StopNode::setCoord(Coord newCoord)
{
    this->coord = newCoord;
}

void StopNode::addRegion(RegionNode* reqNode)
{
    region = reqNode;
}


void StopNode::addNewStop(StopNode *stop)
{
    nextStops.push_back(stop);
}

double StopNode::distanceTo(StopNode *other)
{
    return coord.dist(other->getCoord());
}

double StopNode::distanceTo(Coord other)
{
    return coord.dist(other);
}

double StopNode::distanceTo(int i, int j)
{
    return distanceTo({i,j});
}

bool StopNode::addRoute(RouteNode *route)
{
    routes.push_back(route);
    return true;
}

std::vector<RouteNode *> StopNode::getRoutes()
{
    return routes;
}

bool StopNode::operator==(StopID id)
{
    return this->id == id;
}

bool StopNode::operator==(StopNode *other)
{
    return this->id = other->getId();
}







