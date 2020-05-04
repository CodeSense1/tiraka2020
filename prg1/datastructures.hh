// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <cmath>
#include <limits>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


#include "regionnode.hh"

// Tämä käyttää paljon muistia, mutta on kohtuullisen tehokas
// Nykyaikaisilla koneilla en usko näin suuren muistinkäytön
// olevan ongelma.

using StopMap = std::unordered_map<StopID, StopNode*>;
using RegionMap = std::unordered_map<RegionID, RegionNode*>;

using NameSearchMap = std::multimap<Name, StopID>;
using CoordSearch = std::multimap<Coord, StopID>;

using NSMItr = NameSearchMap::iterator;
using CSItr = CoordSearch::iterator;



class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: O(1)
    // Short rationale for estimate:
    // Säiliön koon hakeminen on vakioaikaista
    int stop_count();

    // Estimate of performance: O(n)
    // Short rationale for estimate:
    // Säiliöiden tyhjentäminen on lineaarista
    void clear_all();

    // Estimate of performance: O(n)
    // Short rationale for estimate:
    // _stopnodes käydään kerran läpi ja sen avaimet
    // kopioidaan toiseen vectoriin.
    std::vector<StopID> all_stops();

    // Estimate of performance: O(n), theta(logn)
    // Short rationale for estimate:
    // Mikäli _stopnodes tietorakenteeseen ei jouduta tekemään
    // rehash operaatiota, hitain operaatio on multimap:iin lisääminen
    bool add_stop(StopID id, Name const& name, Coord xy);

    // Estimate of performance: theta( log(n) ), O(n)
    // Short rationale for estimate:
    // cppreferencen mukaan unordered_map:stä haku on keskimäärin vakioaikaista
    Name get_stop_name(StopID id);

    // Estimate of performance:
    // Short rationale for estimate:
    Coord get_stop_coord(StopID id);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n)
    // Short rationale for estimate:
    // Kaikki arvot on tallennettu aakkosjärjestyksessä _stopnames multimappiin
    // Tämän mapin arvot (StopID) täytyy siis vain kopioida vectoriin
    std::vector<StopID> stops_alphabetically();

    // Estimate of performance: O(logn)
    // Short rationale for estimate:
    // Kaikki arvot on tallennettu aakkosjärjestyksessä _stopcoords multimappiin
    // Tämän mapin arvot (StopID) täytyy siis vain kopioida vectoriin
    std::vector<StopID> stops_coord_order();

    // Estimate of performance: O(n)
    // Short rationale for estimate:
    // Each element is checked once and it's distance to (0,0)
    // is measured, shortest Coordinates Stopid is returned
    StopID min_coord();

    // Estimate of performance: O(1)
    // Short rationale for estimate:
    // Multimapin ensimmäisen elementin arvon hakeminen on vakioaikaista
    StopID max_coord();


    // Estimate of performance: O(1)
    // Short rationale for estimate:
    // Multimapin viimeisen elementin arvon hakeminen on vakioaikaista
    std::vector<StopID> find_stops(Name const& name);

    // Estimate of performance: O(k) theta (logn)
    // Short rationale for estimate:
    // Ensin etsitään oikea nimi, pahimmassa tapauksessa kaikilla
    // pysäkeillä on sama nimi, jolloin operaatiosta tulee lineaarinen
    // k siis tarkoittaa tässä <name> nimisten pysäkkien määrää.

    bool change_stop_name(StopID id, Name const& newname);

    // Estimate of performance: O(k) theta (logn)
    // Short rationale for estimate:
    // Ensin etsitään oikea koodinaatti, pahimmassa tapauksessa kaikilla
    // pysäkeillä on sama coordinaatti, jolloin operaatiosta tulee lineaarinen
    // k siis tarkoittaa tässä <name> nimisten pysäkkien määrää.
    bool change_stop_coord(StopID id, Coord newcoord);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n), theta(1)
    // Short rationale for estimate:
    // Unordered_mappiin lisättäessä keskimäärin vakioaikainen tehokkuus.
    bool add_region(RegionID id, Name const& name);

    // Estimate of performance: O(n), theta(1)
    // Short rationale for estimate:
    // Unordered_mapista haku keskimäärin vakioaikaista, pahimmassa tapauksessa lineaarinen
    Name get_region_name(RegionID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate:
    // Koko säiliö täytyy käydä läpi, ja kopioda arvot vectoriin
    std::vector<RegionID> all_regions();

    // Estimate of performance: O(1)
    // Short rationale for estimate:
    // StopNode::addRegion sekä RegionNode::addStop ovat molemmat vakioaikaisia
    // käytännössä vektoriin lisätään uusi arvo push_back komennolla.
    bool add_stop_to_region(StopID id, RegionID parentid);

    // Estimate of performance: O(1)
    // Short rationale for estimate:
    // Tämä tarkoittaa käytännössä vain vektoriin lisäämistä
    bool add_subregion_to_region(RegionID id, RegionID parentid);

    // Estimate of performance: O(n), n = "ylempien" alueiden lkm
    // Short rationale for estimate:
    // Käydään läpi kaikki alueet, johon annettu pysäkki kuuluu
    std::vector<RegionID> stop_regions(StopID id);

    // Non-compulsory operations

    // Estimate of performance: -
    // Short rationale for estimate:
    // Verkko luodaan aina kun lisätään pysäkkejä alueisiin ja alueita toisiinsa
    void creation_finished();

    // Estimate of performance: O(n + k),
    // missä n on alueen pysäkkien määrä ja k alueen alialueiden määrä
    // Short rationale for estimate:
    // Funktio käy läpi kaikki pysäkkien koordinaatit annuttua id:tä vastaavalla alueella
    // tämän jälkeen sama operaatio toistetaan kaikille alialueille
    std::pair<Coord, Coord> region_bounding_box(RegionID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate:
    // Kaikki pysäkit käydään kerran löpi
    std::vector<StopID> stops_closest_to(StopID id);

    // Estimate of performance: O(n), theta(1)
    // Short rationale for estimate:
    // Etsitään iteraattorin sijainti ja poistetaan alkio _stopnodes tietorakenteesta
    // tämän jälkeen täytyy poisto tehdä myös _stopnames ja _stopcoords mapeista
    bool remove_stop(StopID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate:
    // Kutsuu molemmille pysäkeille stop_regions -> O(2n)
    // Muuntaa toisen näistä set:ksi ja etsii toisen vectorin avulla
    // ensimmäisen (=alimman) yhteisen alueen
    // Kokonaisuudessaan siis O(4n)
    RegionID stops_common_region(StopID id1, StopID id2);

private:
    // Add stuff needed for your class implementation here
    NameSearchMap _stopnames;
    CoordSearch _stopcoords;

    StopMap _stopnodes;
    RegionMap _regionnodes;



};

#endif // DATASTRUCTURES_HH
