// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <map>
#include <unordered_map>

#include "regionnode.hh"
#include "routenode.hh"





// -------- From prg1 ----------------------
using StopMap = std::unordered_map<StopID, StopNode*>;
using RegionMap = std::unordered_map<RegionID, RegionNode*>;

using NameSearchMap = std::multimap<Name, StopID>;
using CoordSearch = std::multimap<Coord, StopID>;


using NSMItr = NameSearchMap::iterator;
using CSItr = CoordSearch::iterator;
//---------------------------

// --------- Prg2 --------------------------
using RouteMap = std::unordered_map<RouteID, RouteNode*>;
using RMItr = RouteMap::iterator;



// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance:
    // Short rationale for estimate:
    int stop_count();

    // Estimate of performance:
    // Short rationale for estimate:
    void clear_all();

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<StopID> all_stops();

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_stop(StopID id, Name const& name, Coord xy);

    // Estimate of performance:
    // Short rationale for estimate:
    Name get_stop_name(StopID id);

    // Estimate of performance:
    // Short rationale for estimate:
    Coord get_stop_coord(StopID id);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<StopID> stops_alphabetically();

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<StopID> stops_coord_order();

    // Estimate of performance:
    // Short rationale for estimate:
    StopID min_coord();

    // Estimate of performance:
    // Short rationale for estimate:
    StopID max_coord();

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<StopID> find_stops(Name const& name);

    // Estimate of performance:
    // Short rationale for estimate:
    bool change_stop_name(StopID id, Name const& newname);

    // Estimate of performance:
    // Short rationale for estimate:
    bool change_stop_coord(StopID id, Coord newcoord);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_region(RegionID id, Name const& name);

    // Estimate of performance:
    // Short rationale for estimate:
    Name get_region_name(RegionID id);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<RegionID> all_regions();

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_stop_to_region(StopID id, RegionID parentid);

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_subregion_to_region(RegionID id, RegionID parentid);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<RegionID> stop_regions(StopID id);

    // Non-compulsory operations

    // Estimate of performance:
    // Short rationale for estimate:
    void creation_finished();

    // Estimate of performance:
    // Short rationale for estimate:
    std::pair<Coord, Coord> region_bounding_box(RegionID id);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<StopID> stops_closest_to(StopID id);

    // Estimate of performance:
    // Short rationale for estimate:
    bool remove_stop(StopID id);

    // Estimate of performance:
    // Short rationale for estimate:
    RegionID stops_common_region(StopID id1, StopID id2);

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    // prg 2 operations
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    // Estimate of performance: O(n)
    // Short rationale for estimate: Every element must be individually be copied from the data structure
    std::vector<RouteID> all_routes();

    // Estimate of performance: O(n), n: size of stops
    // Short rationale for estimate: Every stops id checked that it is valid
    bool add_route(RouteID id, std::vector<StopID> stops);

    // Estimate of performance:O(n*m), n: all routes that go through stopid
    //                                 m: Amount of stops in every route
    // Short rationale for estimate:
    // First every route that go through the stop is processed
    // For every route, we need to find
    std::vector<std::pair<RouteID, StopID>> routes_from(StopID stopid);

    // Estimate of performance: O(n), n: amount of stops in route
    // Short rationale for estimate:
    // RouteNode holds information about its stops
    // to get all stops, they must be added individually to returned vector
    std::vector<StopID> route_stops(RouteID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate:
    // Clearing container is linear operation
    void clear_routes();

    // Estimate of performance: O(n*m), Theta (nlogm), n: amount of stops, m: amount of neighbour stops
    // Short rationale for estimate:
    // This is identical for journey_shortest_distance

    // First find all stops that are next to first stop
    // Then calculate distances to those and add then to queue and same process will be repeated
    // If destination stop is found, stops that are furher away are rejected, since they
    // cannot be closest route
    // Finally calculate the total distance and add stops to output vector
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_any(StopID fromstop, StopID tostop);

//    // Non-compulsory operations

    // Estimate of performance: O(n*m), Theta (nlogm), n: amount of stops, m: amount of neighbour stops
    // Short rationale for estimate:
    // This is very similar to journey_any

    // First find all stops that are next to first stop
    // Then calculate how many stops since start neighbour stops to queue
    // If destination stop is found, we know the maximum stopcount, therefore stops that are over stopcount
    // can be rejected
    // When queue is empty, calculate the least stop path
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_least_stops(StopID fromstop, StopID tostop);

    // Estimate of performance: O(n*m), Theta (nlogm), n: amount of stops, m: amount of neighbour stops
    // Short rationale for estimate:
    // Similar algorithm, but when neighbour stop is already been visited, we can immediately
    // continue to path construction

    std::vector<std::tuple<StopID, RouteID, Distance>> journey_with_cycle(StopID fromstop);

    // Estimate of performance: O(n*m), Theta (nlogm), n: amount of stops, m: amount of neighbour stops
    // Short rationale for estimate:
    // Identical to journey_any
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_shortest_distance(StopID fromstop, StopID tostop);

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_trip(RouteID routeid, const std::vector<Time> &stop_times);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<std::pair<Time, Duration> > route_times_from(RouteID routeid, StopID stopid);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<std::tuple<StopID, RouteID, Time>> journey_earliest_arrival(StopID fromstop, StopID tostop, Time starttime);

    // Estimate of performance:
    // Short rationale for estimate:
    void add_walking_connections(); // Note! This method is completely optional, and not part of any testing

private:
    // Add stuff needed for your class implementation here
    NameSearchMap _stopnames;
    CoordSearch _stopcoords;

    StopMap _stopnodes;
    RegionMap _regionnodes;

    RouteMap _routes;



};

#endif // DATASTRUCTURES_HH
