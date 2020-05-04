#include "regionnode.hh"
#include <algorithm>

#include <iostream>

RegionNode::RegionNode()
{
    this->id = NO_REGION;
    this->name = NO_NAME;
}

RegionNode::RegionNode(RegionID id, Name name)
{
    this->id = id;
    this->name = name;
}

RegionNode::~RegionNode()
{
    // datastrucures.cc poistaa kaikki stopnodet ja regionnodet,
    // joten täällä ei tarvitse tehdä mitään
}

RegionID RegionNode::getId()
{
    return id;
}

Name RegionNode::getName()
{
    return name;
}

std::vector<RegionNode*> RegionNode::getSubRegion(std::vector<RegionNode*> subReg)
{
    for (auto& i : subregions) {
        subReg.push_back(i);
        i->getSubRegion(subReg);
    }
    return subReg;
}



std::vector<StopNode*> RegionNode::getStopsInRegion()
{
    return stops;
}

void RegionNode::getSubregionIds(std::vector<RegionID>& ids)
{
    RegionNode* it = parent;
    while (it != nullptr) {
        ids.push_back(it->getId());
        it = it->getParentRegion();
    }

    for (auto& subregion : subregions) {
        ids.push_back(subregion->getId());
        subregion->getSubregionIds(ids);
    }
}

void RegionNode::getBoundingRect(Coord& leftBot, Coord& rightTop)
{
    for (auto& stop : stops) {
        Coord current = stop->getCoord();
        if (current.x < leftBot.x) {
            leftBot.x = current.x;
        }
        if (current.x > rightTop.x) {
            rightTop.x = current.x;
        }
        if (current.y < leftBot.y) {
            leftBot.y = current.y;
        }
        if (current.y > rightTop.y) {
            rightTop.y = current.y;
        }
    }
    for (auto& subregion : subregions) {
        subregion->getBoundingRect(leftBot, rightTop);
    }

}

void RegionNode::setParentRegion(RegionNode* _parent)
{
    parent = _parent;
}

RegionNode *RegionNode::getParentRegion()
{
    return parent;
}


bool RegionNode::removeStop(StopID id)
{
    // Check if stop is present in current region
    auto it = std::find_if(stops.begin(), stops.end(), [&](StopNode* node){return node->getId() == id;});
    if (it != stops.end()) {
        stops.erase(it);
        return true;
    }

    for (auto& region : subregions) {
        region->removeStop(id);
    }

    return false;
}

bool RegionNode::addSubRegion(RegionNode *subRegion)
{
    if (!subRegion->getSubRegion().empty()) {
        return false;
    }
    subRegion->setParentRegion(this);
    subregions.push_back(subRegion);
    return true;
}

void RegionNode::addStop(StopNode* stop)
{
    stops.push_back(stop);
}
