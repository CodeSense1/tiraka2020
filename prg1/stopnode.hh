#ifndef STOPNODE_HH
#define STOPNODE_HH

#include <string>
#include <vector>
#include <cmath>
#include <utility>
#include <limits>

#include <iostream>

using StopID = long int;
using RegionID = std::string;
using Name = std::string;
// Return values for cases where required thing was not found

StopID const NO_STOP = -1;

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "!!NO_NAME!!";

struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
    Coord() {

    }
    Coord(int x, int y) {
        this->x = x;
        this->y = y;
    }
    double dist(const Coord& other) {
        return std::sqrt(std::pow((this->x-other.x), 2) + std::pow(this->y - other.y, 2));
    }

    double dist() {
        // std::sqrt is slow operation (according to stackOverflow)
        return std::pow(x,2) + std::pow(y,2);
    }

};
// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.x < c2.x && c1.y < c2.y) {
        return true;
    }
    double d1 = c1.dist();
    double d2 = c2.dist();
//    if (d1 == d2) {
//        return c1.y < c2.y;
//    }
    return d1 < d2;

//    if (c1.y < c2.y) { return true; }
//    else if (c2.y < c1.y) { return false; }
//    else { return c1.x < c2.x; }
}

class RegionNode;

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

class StopNode
{
private:
    StopID id;
    Name name;
    Coord coord;
    std::vector<StopNode*> nextStops; // Datastructures will perhaps take care of this?
    RegionNode* region; // Which region stop belongs
public:
    StopNode();
    StopNode(StopID id, Name name, Coord coord);
    ~StopNode();
    StopID getId();
    Name getName();
    Coord getCoord();
    RegionNode* getRegion();

    Name setName(Name newName);
    void setCoord(Coord newCoord);

    void addRegion(RegionNode* regId);
    void removeRegion(RegionID regId);

    void addNewStop(StopNode* stop);
    double distanceTo(StopNode* other);
    double distanceTo(Coord other);
    double distanceTo(int i, int j);

    bool operator==(StopID id);
    bool operator==(StopNode* other);

};

#endif // STOPNODE_HH
