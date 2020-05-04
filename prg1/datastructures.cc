// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <queue>
#include <unordered_set>
#include <set>

#include <iostream>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

Datastructures::Datastructures()
{
    // Replace this comment with your implementation
}

Datastructures::~Datastructures()
{

    for (auto p : _stopnodes)
    {
        delete p.second;
    }
    _stopnodes.clear();

    for (auto p : _regionnodes) {
        delete p.second;
    }
    _regionnodes.clear();

    _stopnames.clear();
    _stopcoords.clear();
}

int Datastructures::stop_count()
{
    return static_cast<int>(_stopnodes.size());
}

void Datastructures::clear_all()
{

    for (auto p : _stopnodes)
    {
        delete p.second;
    }
    _stopnodes.clear();

    for (auto p : _regionnodes) {
        delete p.second;
    }
    _regionnodes.clear();

    _stopnames.clear();
    _stopcoords.clear();

}

std::vector<StopID> Datastructures::all_stops()
{
    if (_stopnodes.empty())
        return {};

    std::vector<StopID> output;
    std::transform(_stopnodes.begin(),
                   _stopnodes.end(),
                   std::back_inserter(output),
                   [](const auto& mapElement){return mapElement.first;});
    return output;
}

bool Datastructures::add_stop(StopID id, const Name& name, Coord xy)
{
    if (_stopnodes.find(id) != _stopnodes.end()) {
        return false;
    }
    _stopnodes.insert({id, new StopNode(id, name, xy)});
    _stopnames.insert({name, id});
    _stopcoords.insert({xy, id});
    return true;
}

Name Datastructures::get_stop_name(StopID id)
{
    try {
        return _stopnodes.at(id)->getName();
    } catch (std::out_of_range /*err*/) {

        return NO_NAME;
    }
}

Coord Datastructures::get_stop_coord(StopID id)
{
    try {
        return _stopnodes.at(id)->getCoord();
    } catch (std::out_of_range /*err*/) {

        return NO_COORD;
    }
}

// perftest stops_alphabetically 60 100 300000
//N ,    add (sec) ,   cmds (sec) ,  total (sec)
//300000 ,     0.609425 ,      48.1516 ,       48.761
std::vector<StopID> Datastructures::stops_alphabetically()
{
    if (_stopnodes.empty())
        return {};

    std::vector<StopID> output;
    for (auto& i : _stopnames) {
        output.push_back(i.second);
    }
    return output;


}

std::vector<StopID> Datastructures::stops_coord_order()
{
    if (_stopnodes.empty())
        return {};

    std::vector<StopID> output;
    for (const auto& i : _stopcoords) {
        output.push_back(i.second);
    }
    return output;
}

StopID Datastructures::min_coord()
{
    if (_stopcoords.empty()) {
        return NO_STOP;
    }
    return _stopcoords.begin()->second;
}

StopID Datastructures::max_coord()
{
    if (_stopcoords.empty()) {
        return NO_STOP;
    }
    CSItr it = _stopcoords.end();
    it--;
    return it->second;

//    StopNode min;
//    double dist = std::numeric_limits<int>::min();
//    StopMap::iterator it;
//    for(it = _stopnodes.begin(); it != _stopnodes.end(); ++it) {
//        double d = it->second->distanceTo({0,0});
//        if (d > dist) {
//            min = *it->second;
//            dist = d;
//        }
//    }

//    return min.getId();
}

std::vector<StopID> Datastructures::find_stops(Name const& name)
{
    if (_stopnodes.empty())
        return {NO_STOP};
    std::vector<StopID> output;
    const auto& its = _stopnames.equal_range(name);
    for (NSMItr it = its.first; it != its.second; it++) {
        output.push_back(it->second);
    }

    return output;
}

bool Datastructures::change_stop_name(StopID id, const Name& newname)
{
    try {
        StopNode* node = _stopnodes.at(id);
        Name old = node->getName();
        const auto& its = _stopnames.equal_range(old);
        for (NSMItr it = its.first; it != its.second; it++) {
            if (it->second == id) {

                node->setName(newname);
                _stopnames.erase(it);
                _stopnames.insert({newname, id});
                return true;
            }
        }
        return true;
    } catch (std::out_of_range& /*err*/) {
        return false;
    }

}

bool Datastructures::change_stop_coord(StopID id, Coord newcoord)
{

    try {
        StopNode* node = _stopnodes.at(id);
        Coord old = node->getCoord();
        const auto& its = _stopcoords.equal_range(old);
        for (CSItr it = its.first; it != its.second; it++) {
            if (it->second == id) {

                node->setCoord(newcoord);
                _stopcoords.erase(it);
                _stopcoords.insert({newcoord, id});
                return true;
            }
        }
        return true;
    } catch (std::out_of_range& /*err*/) {
        return false;
    }

}

bool Datastructures::add_region(RegionID id, const Name &name)
{
    if (_regionnodes.find(id) != _regionnodes.end()) {
        return false;
    }
    _regionnodes.emplace(id, new RegionNode(id, name));
    return true;
}

Name Datastructures::get_region_name(RegionID id)
{
    try {
        return _regionnodes.at(id)->getName();

    } catch (std::out_of_range& /*err*/) {
        return NO_NAME;
    }
}

std::vector<RegionID> Datastructures::all_regions()
{
    if (_regionnodes.empty()) {
        return {};
    }
    std::vector<RegionID> output;
    for (const auto& region : _regionnodes) {
        output.push_back(region.first);
    }
    return output;
}

bool Datastructures::add_stop_to_region(StopID id, RegionID parentid)
{
    try {
        _regionnodes.at(parentid)->addStop(_stopnodes.at(id));
        _stopnodes.at(id)->addRegion(_regionnodes.at(parentid));
        return true;
    } catch (std::out_of_range& /*err*/) {
       return false;
    }
}

bool Datastructures::add_subregion_to_region(RegionID id, RegionID parentid)
{
    try {

        RegionNode* node = _regionnodes.at(parentid);
        return node->addSubRegion(_regionnodes.at(id));

    } catch (std::out_of_range& /*err*/) {
        return false;
    }
}

std::vector<RegionID> Datastructures::stop_regions(StopID id)
{
    StopNode* stop;
    try {

        stop = _stopnodes.at(id);
    } catch (std::out_of_range& /*err*/) {

        return {NO_REGION};
    }
    RegionNode* region = stop->getRegion();
    std::vector<RegionID> output = {};
    while(region != nullptr) {
        output.push_back(region->getId());
        region = region->getParentRegion();
    }
    return output;
}

void Datastructures::creation_finished()
{
    // Build some sort of graph from the stops
}

std::pair<Coord,Coord> Datastructures::region_bounding_box(RegionID id)
{
    RegionNode* current;
    try {

        current = _regionnodes.at(id);
    } catch (std::out_of_range& /*err*/) {
        return {NO_COORD, NO_COORD};
    }

    if (current->getStopsInRegion().empty()) {

        return {NO_COORD, NO_COORD};
    }

    Coord leftbot = current->getStopsInRegion()[0]->getCoord();
    Coord righttop = leftbot;

    current->getBoundingRect(leftbot, righttop);
    return {leftbot, righttop};
}
// testread "example-all-in.txt" "example-all-out.txt"
std::vector<StopID> Datastructures::stops_closest_to(StopID id)
{
    // Build max heap!

    std::vector<StopID> output;
    StopNode* ref;
    try {

        ref = _stopnodes.at(id);
    } catch (std::out_of_range& /*err*/) {

        return {NO_STOP};
    }
    std::priority_queue<std::pair<double, StopID>> closest;

        for (auto& node : _stopnodes) {
            double dist = node.second->distanceTo(ref);
            if (dist < 0.01 && dist > -0.01)
                continue;

            if (closest.size() < 5) {
                closest.push({dist, node.second->getId()});

            } else {
                double ref = closest.top().first;
                if (ref > dist) {
                    closest.push({dist, node.second->getId()});
                    closest.pop();
                }
            }
        }


    std::vector<std::pair<double, StopID>> temp;
    while(!closest.empty()) {
        temp.push_back(closest.top());
        closest.pop();
    }

    // Since there will always be only 5 elements in temp
    // sorting is constant complexity
    std::sort(temp.begin(),
              temp.end()
              , [](const auto& f, const auto& s)
                    {return f.first < s.first;});

    for (auto& el : temp) {

        output.push_back(el.second);
    }

    return output;

}

bool Datastructures::remove_stop(StopID id)
{
    StopNode* stopToRemove;
    try {

        stopToRemove = _stopnodes.at(id);
    } catch (std::out_of_range& /*err*/) {
        return false;
    }

    RegionNode* region = stopToRemove->getRegion();

    if (region != nullptr) {
        region->removeStop(id);

    }

    _stopnodes.erase(id);
    // Remove stop also from stopnames, stopcoord
    Name name = stopToRemove->getName();
    Coord coord = stopToRemove->getCoord();


    const auto& n_itrs = _stopnames.equal_range(name);
    auto name_it = n_itrs.first;
    while (name_it != n_itrs.second) {
        if (name_it->second == id) {
            // poista tää
            _stopnames.erase(name_it);
            break;
        }
        name_it++;
    }

    const auto& c_itrs = _stopcoords.equal_range(coord);
    auto coord_it = c_itrs.first;
    while (coord_it != c_itrs.second) {
        if (coord_it->second == id) {
            // poista tää
            _stopcoords.erase(coord_it);
            break;
        }
        coord_it++;
    }

    delete stopToRemove;
    return true;


}

// testread "Tretest-all-in.txt" "Tretest-all-out.txt"
RegionID Datastructures::stops_common_region(StopID id1, StopID id2)
{
    std::vector<RegionID> ids1 = stop_regions(id1);
    std::vector<RegionID> ids2 = stop_regions(id2);


    std::unordered_set<RegionID> ref;
    for (auto& i : ids1) {
        ref.insert(i);
    }

    for (auto& reg : ids2) {
        if (ref.find(reg) != ref.end()) {
            RegionID r = reg;
            return reg;
        }
    }

    return NO_REGION;
}
