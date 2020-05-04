// Datastructures.cc

#include "datastructures.hh"
#include "routenode.hh"
#include <algorithm>
#include <random>
#include <cmath>
#include <list>
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
    // Handle memory
    for (auto p : _stopnodes)
    {
        delete p.second;
    }

    for (auto p : _regionnodes) {
        delete p.second;
    }

    for( auto p : _routes) {
        delete p.second;
    }

    // Clear datastructures
    _stopnodes.clear();
    _regionnodes.clear();
    _stopnames.clear();
    _stopcoords.clear();
    _routes.clear();
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

std::vector<RouteID> Datastructures::all_routes()
{
    // returns all possible routes
    if (_routes.empty())
        return {};

    std::vector<RouteID> output;
    std::transform(_routes.begin(),
                   _routes.end(),
                   std::back_inserter(output),
                   [](const auto& mapElement){return mapElement.first;});
    return output;
}

bool Datastructures::add_route(RouteID id, std::vector<StopID> stops)
{
    RMItr routeit = _routes.find(id);
    if ( routeit == _routes.end()) {
        // Route was not found
            std::vector<std::pair<StopNode*, StopNode*>> stopDistances;
            RouteNode* node = new RouteNode(id);
            _routes.insert({id, node});
            for (size_t i = 0; i < stops.size(); i++) {
                // Check if stopid exist
                try {
                    StopNode* current = _stopnodes.at(stops.at(i));
                    current->addRoute(node);
                    StopNode* next = nullptr;
                    if (i+1 < stops.size())
                         next = _stopnodes.at(stops.at(i+1));

                    stopDistances.push_back({current, next});

                }
                catch (std::out_of_range& /*err*/) {
                    node->clearAllStops();
                    return false;
                }
            }

            node->addStops(stopDistances);
    } else {
        // Handle route was already declared
        return false;
    }

    return true;
}
// testread "example-compulsory-in.txt" "example-compulsory-out.txt"
std::vector<std::pair<RouteID, StopID>> Datastructures::routes_from(StopID stopid)
{
    StopNode* stopnode;
    std::vector<std::pair<RouteID, StopID>> output;
    try {
        stopnode = _stopnodes.at(stopid); // Find stop
        // Find all routes that go through this stop
        for (const auto& route : stopnode->getRoutes()) {
            // for all routes, check what is the next stop
            StopNode* nextStop = route->getStopInRoute(stopnode);
            // route was found
            // Check if the stop is the last stop, and push back correct stop id
            if (nextStop == nullptr) {
                // No stop was found, this is mostlikely case when
                // We are checking the last stop, since after that there are no stops
                // In that case we don't want to add anything to output
                continue;
            }
            // Add route and the next stop to output
            output.push_back({route->getId(), nextStop->getId()});

        }
        return output;

    } catch (std::out_of_range& /*err*/) {
        return {{NO_ROUTE, NO_STOP}};
    }
}

std::vector<StopID> Datastructures::route_stops(RouteID id)
{
    // Check if id exists
    std::vector<StopID> output;
    auto node = _routes.find(id);

    // Find all stops in order
    if (node == _routes.end()) {
        return {NO_STOP};
    } else {
        return node->second->getStopsInRoute();
    }

}

void Datastructures::clear_routes()
{
    // Replace this comment and the line below with your implementation
    for (auto& p : _routes) {
        delete p.second;
    }
    _routes.clear();
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_any(StopID startStop, StopID destStop)
{
    return journey_shortest_distance(startStop, destStop);
}


std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_least_stops(StopID startStop, StopID destStop)
{

    std::unordered_map<StopID, std::tuple<StopID, RouteID, Distance>> visited;
    /*
     * visited- data structure is used to determine distances and relations between stops
     * key StopID, is stop that is currently beign processed
     * tuple:
     *  StopID: Stop that was previous to this stop
     *  RouteID: What route we used to get to the current stop
     *  Distance: How long it took to get to the current stop
     */
    StopID fromstop = startStop; // Reference to first stop
    std::vector<std::tuple<StopID, RouteID, Distance>> path; // This contains only stops that lead to destination
    Distance currentDistance = std::numeric_limits<int>::max();
    int currentStopCount = std::numeric_limits<int>::max(); // This is used to track which path has least stops
    Distance cumulative = 0;
    int stopcount = 0;
    // Mark fromstop as visited
    visited.insert({fromstop, {NO_STOP, NO_ROUTE, 0}});
    std::list<std::tuple<StopID, Distance, int>> queue;
    queue.push_back({startStop, 0, 0});
    while(!queue.empty()) {

        // Deque a stop from queue
        auto [_fromstop, _cumulative, _stopcount] = queue.front();
        fromstop = _fromstop;
        cumulative = _cumulative;
        stopcount = _stopcount;
        queue.pop_front();


        StopNode* currentStop = _stopnodes.at(fromstop);

        auto [prevStop, route, dist] = visited.at(fromstop); // This is route that currently processed stop belongs
        // Get all adjacent stops of the dequed stop
        // If adjacent has not been visited, then mark it as visited
        // and enque it
        for(auto i : routes_from(fromstop)) {
            // If this adjacent stops is the destination
            // journey found
            StopID stop = i.second;
            RouteID routeToNext = i.first;


            // figure out distance between fromstop and stop
            Distance stopDistance = static_cast<Distance>(currentStop->distanceTo(_stopnodes.at(stop)));
            if (i.second == destStop) {
                // Calculate current distance fromstop to tostop
                visited.insert({stop, {fromstop, routeToNext, stopDistance}});
                if (currentStopCount > stopcount +1) {
                    currentStopCount = stopcount+1;
                    currentDistance = cumulative + stopDistance;
                }
            }
            // No need to process if distance is bigger than currently found route
            if (currentStopCount < stopcount + 1) {
                continue;
            }
            // Add next stop to be checked

            auto vit = visited.find(stop); // vit = visited::iterator
            if ( vit == visited.end()) {
                queue.push_back({stop, cumulative + stopDistance, stopcount+1});
                // this stop has not been visited yet
                // Add stop, previous stop, route and distance
                visited.insert({stop, {fromstop, routeToNext, stopDistance}});
            } else {
                // This stop has been visited
                // Check if this which route was the fastest way to this stop
                auto& [s, r, d] = vit->second; // This is information about the previous stop!
                // Check if this was the shortest path to this stop, or maybe current stop was faster way
                Distance ref = 0; // Current total distance
                int refcount = 0;
                StopID temp = stop;
                        // Need to check here that things won't end up in a loop
                while(temp != startStop) {
                    auto [s, r, d] = visited.at(temp);
                    ref += d;
                    temp = s;
                    refcount++;

                }
                if (stopcount+1 < refcount) {
                    // Current stop was faster way to get to current stop
                    s = fromstop;
                    r = routeToNext;
                    d = stopDistance;

                    // Update current max distance and stopcount
                    currentStopCount += (stopcount+1) - refcount;
                    currentDistance = cumulative + stopDistance; // This might be the problem
                }
            }
        }
    }

    if (visited.find(destStop) == visited.end()) {
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    }
    StopID stopiter = destStop;
    Distance elapsed = 0;
    while (stopiter != startStop)  {
        auto [s, r, d] = visited.at(stopiter);
        path.push_back({s, r, currentDistance-d-elapsed});
        elapsed += d;
        stopiter = s;


    }
    std::reverse(path.begin(), path.end());

    path.push_back({destStop, NO_ROUTE, elapsed});
    // Finally add the destination stop
    /*
     * That is not included, since out datastructure stores only distance and route of the previous
     * Since destination is not previous to any stop, it must be added manually
     */

    return path;

}


std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_with_cycle(StopID startStop)
{
    if (_stopnodes.find(startStop) == _stopnodes.end()) {
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    }
    std::unordered_map<StopID, std::tuple<StopID, RouteID, Distance>> visited;
    /*
     * visited- data structure is used to determine distances and relations between stops
     * key StopID, is stop that is currently beign processed
     * tuple:
     *  StopID: Stop that was previous to this stop
     *  RouteID: What route we used to get to the current stop
     *  Distance: How long it took to get to the current stop
     */
    StopID fromstop = startStop; // Reference to first stop
    std::vector<std::tuple<StopID, RouteID, Distance>> path; // This contains only stops that lead to destination

    // Mark fromstop as visited
    visited.insert({fromstop, {NO_STOP, NO_ROUTE, 0}});
    std::list<StopID> queue;
    std::list<StopID>::iterator it ;
    queue.push_back(startStop);

    while(!queue.empty()) {

        // Deque a stop from queue
        fromstop = queue.front();
        queue.pop_front();

        // Calculate distance from start to this stop

        StopNode* currentStop = _stopnodes.at(fromstop);

        auto [prevStop, route, dist] = visited.at(fromstop); // This is route that currently processed stop belongs
        // Get all adjacent stops of the dequed stop
        // If adjacent has not been visited, then mark it as visited
        // and enque it
        for(auto i : routes_from(fromstop)) {
            // If this adjacent stops is the destination
            // journey found
            StopID stop = i.second;
            RouteID routeToNext = i.first;

            // figure out distance between fromstop and stop
            Distance stopDistance = static_cast<Distance>(currentStop->distanceTo(_stopnodes.at(stop)));

            // Add next stop to be checked

            auto vit = visited.find(stop); // vit = visited::iterator
            if ( vit == visited.end()) {
                queue.push_back(stop);
                // this stop has not been visited yet
                // Add stop, previous stop, route and distance
                visited.insert({stop, {fromstop, routeToNext, stopDistance}});
            } else {
                // Loop found

                // Since visited already contains startStop, we cannot add this one
                std::tuple<StopID, RouteID, Distance> loopStop = {fromstop, routeToNext, stopDistance};
                StopID stopiter = fromstop;

                // Calculate the loop distance
                Distance elapsed = stopDistance;
                while (stopiter != startStop)  {

                    auto [s, r, d] = visited.at(stopiter);
                    elapsed += d;
                    stopiter = s;
                }
                // At this point elapsed is the max distance
                stopiter = fromstop;

                Distance totalDistance = elapsed;
                elapsed = 0;
                while (stopiter != startStop)  {

                    auto [s, r, d] = visited.at(stopiter);
                    path.push_back({s, r, totalDistance-d-elapsed});
                    elapsed += d;
                    stopiter = s;
                }
                std::reverse(path.begin(), path.end());

                path.push_back({startStop, NO_ROUTE, elapsed});
                return path;
            }
        }
    }
    // This means that no loop was found
    return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_shortest_distance(StopID startStop, StopID destStop)
{
    if (_stopnodes.find(startStop) == _stopnodes.end() || _stopnodes.find(destStop) == _stopnodes.end()) {
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    }
    std::unordered_map<StopID, std::tuple<StopID, RouteID, Distance>> visited;
    /*
     * visited- data structure is used to determine distances and relations between stops
     * key StopID, is stop that is currently beign processed
     * tuple:
     *  StopID: Stop that was previous to this stop
     *  RouteID: What route we used to get to the current stop
     *  Distance: How long it took to get to the current stop
     */
    StopID fromstop = startStop; // Reference to first stop
    std::vector<std::tuple<StopID, RouteID, Distance>> path; // This contains only stops that lead to destination
    Distance currentDistance = std::numeric_limits<int>::max();
    Distance cumulative = 0;
    // Mark first stop as visited
    visited.insert({fromstop, {NO_STOP, NO_ROUTE, 0}}); // No previous stop nor route
    std::list<std::pair<StopID, Distance>> queue;
    queue.push_back({startStop, 0});

    while(!queue.empty()) {

        // Deque a stop from queue
        fromstop = queue.front().first;
        cumulative = queue.front().second;
        queue.pop_front();

        // Calculate distance from start to this stop
        //cumulative = 0;



        StopNode* currentStop = _stopnodes.at(fromstop);

        auto [prevStop, route, dist] = visited.at(fromstop); // This is route that currently processed stop belongs
        // Get all adjacent stops of the dequed stop
        // If adjacent has not been visited, then mark it as visited
        // and enque it
        for(auto i : routes_from(fromstop)) {
            // If this adjacent stops is the destination
            // journey found
            StopID stop = i.second;
            RouteID routeToNext = i.first;


            // figure out distance between fromstop and stop
            Distance stopDistance = static_cast<Distance>(currentStop->distanceTo(_stopnodes.at(stop)));
            if (i.second == destStop) {
                // Calculate current distance fromstop to tostop


                // Is there possibility that this doesn't update correctly?
                visited.insert({stop, {fromstop, routeToNext, stopDistance}});
                if ((cumulative + stopDistance) < currentDistance) {
                    // This route was faster than previous
                    currentDistance = cumulative + stopDistance;
                }
            }
            // No need to process if distance is bigger than currently found route
            if (currentDistance < (cumulative + stopDistance)) {
                continue;
            }
            // Add next stop to be checked

            auto vit = visited.find(stop); // vit = visited::iterator
            if ( vit == visited.end()) {
                queue.push_back({stop, cumulative + stopDistance});
                // this stop has not been visited yet
                // Add stop, previous stop, route and distance
                visited.insert({stop, {fromstop, routeToNext, stopDistance}});
            } else {
                // This stop has been visited
                // Check if this which route was the fastest way to this stop
                auto& [s, r, d] = vit->second; // This is information about the previous stop!
                // Check if this was the shortest path to this stop, or maybe current stop was faster way
                Distance ref = 0; // Current total distance
                StopID temp = stop;
                while(temp != startStop) {

                    auto [s, r, d] = visited.at(temp);
                    ref += d;
                    temp = s;
                }

                if ((cumulative + stopDistance) < ref) {
                    // Current stop was faster way to get to current stop

                    s = fromstop;
                    r = routeToNext;
                    d = stopDistance;

                    // Update current max distance
                    currentDistance += (cumulative + stopDistance) - ref;
                }
            }
        }
    }

    if (visited.find(destStop) == visited.end()) {
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    }
    StopID stopiter = destStop;

    Distance elapsed = 0;
    while (stopiter != startStop)  {

        auto [s, r, d] = visited.at(stopiter);
        path.push_back({s, r, currentDistance-d-elapsed});
        elapsed += d;
        stopiter = s;
    }
    std::reverse(path.begin(), path.end());

    // Finally add the destination stop
    path.push_back({destStop, NO_ROUTE, elapsed});

    return path;

}

bool Datastructures::add_trip(RouteID routeid, std::vector<Time> const& stop_times)
{
    // Replace this comment and the line below with your implementation
    return false;
}

std::vector<std::pair<Time, Duration>> Datastructures::route_times_from(RouteID routeid, StopID stopid)
{
    // Replace this comment and the line below with your implementation
    return {{NO_TIME, NO_DURATION}};
}

std::vector<std::tuple<StopID, RouteID, Time> > Datastructures::journey_earliest_arrival(StopID fromstop, StopID tostop, Time starttime)
{
    // Replace this comment and the line below with your implementation
    return {{NO_STOP, NO_ROUTE, NO_TIME}};
}

void Datastructures::add_walking_connections()
{
    // Replace this comment and the line below with your implementation
}
