#ifndef REGIONNODE_HH
#define REGIONNODE_HH

#include <string>
#include <vector>
#include "stopnode.hh"

using RegionID = std::string;
// Return values for cases where required thing was not found
RegionID const NO_REGION = "!!NO_REGION!!";


class RegionNode
{
private:
    RegionID id;
    Name name;
    RegionNode* parent = nullptr;
    std::vector<RegionNode*> subregions;
    std::vector<StopNode*> stops;
public:
    RegionNode();
    RegionNode(RegionID id, Name name);
    ~RegionNode();
    RegionID getId();
    Name getName();
    std::vector<RegionNode*> getSubRegion(std::vector<RegionNode*> subReg = {});
    std::vector<StopNode*> getStopsInRegion();

    void getSubregionIds(std::vector<RegionID>& ids);
    void getBoundingRect(Coord& leftBot, Coord& rightTop);

    void setParentRegion(RegionNode* parent);
    RegionNode *getParentRegion();
    bool removeStop(StopID id);
    bool addSubRegion(RegionNode* subRegion);
    void addStop(StopNode* stop);
};

#endif // REGIONNODE_HH



