#ifndef ROUTENODE_HH
#define ROUTENODE_HH

#include <vector>
#include <list>
#include <tuple>
#include <unordered_map>

#include "stopnode.hh"

// Types for IDs
using StopID = long int;
using RegionID = std::string;
using RouteID = std::string;
using Name = std::string;



// Return values for cases where required thing was not found
RouteID const NO_ROUTE = "!!NO_ROUTE!!";

// Type for time of day in minutes from midnight (i.e., 60*hours + minutes)
using Time = int;

// Return value for cases where color was not found
Time const NO_TIME = std::numeric_limits<Time>::min();

// Type for a duration of time (in minutes)
using Duration = int;

// Return value for cases where Duration is unknown
Duration const NO_DURATION = NO_VALUE;

// Type for a distance (in metres)
using Distance = int;

// Return value for cases where Duration is unknown
Distance const NO_DISTANCE = NO_VALUE;


class RouteNode
{
public:
    RouteNode(RouteID id);
    ~RouteNode();
    RouteID getId();
    // Distance is measured from last stop
    void addStops(std::vector<std::pair<StopNode*, StopNode* >> stopRoutes); // This is tuple so if needed time element can be added
    bool hasStops();
    StopNode *getStopInRoute(StopNode* node);
    std::vector<StopID> getStopsInRoute();
    void clearAllStops();

    StopNode* getLastStopNode();

    bool operator==(RouteNode* other);

private:
    RouteID id;
    StopNode* first;
    StopNode* last = nullptr;
    std::unordered_map<StopNode*, StopNode*> stopMap;
};

#endif // ROUTENODE_HH
