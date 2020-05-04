// The main program (provided by the course), TIE-20100/TIE-20106
//
// DO ****NOT**** EDIT THIS FILE!
// (Preferably do not edit this even temporarily. And if you still decide to do so
//  (for debugging, for example), DO NOT commit any changes to git, but revert all
//  changes later. Otherwise you won't be able to get any updates/fixes to this
//  file from git!)

#include <string>
using std::string;
using std::getline;

#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::flush;
using std::noskipws;

#include <iomanip>
using std::setfill;
using std::setw;

#include <istream>
using std::istream;

#include <ostream>
using std::ostream;

#include <fstream>
using std::ifstream;

#include <sstream>
using std::istringstream;
using std::ostringstream;
using std::stringstream;

#include <iomanip>
using std::setw;

#include <tuple>
using std::tuple;
using std::make_tuple;
using std::get;
using std::tie;

#include <regex>
using std::regex_match;
using std::regex_search;
using std::smatch;
using std::regex;
using std::sregex_token_iterator;

#include <algorithm>
using std::find_if;
using std::find;
using std::binary_search;
using std::max_element;
using std::max;
using std::min;
using std::shuffle;
using std::sort;

#include <random>
using std::minstd_rand;
using std::uniform_int_distribution;

#include <chrono>

#include <functional>
using std::function;
using std::equal_to;

#include <vector>
using std::vector;

#include <set>
using std::set;

#include <array>
using std::array;

#include <bitset>
using std::bitset;

#include <iterator>
using std::next;

#include <ctime>
using std::time;

#include <memory>
using std::move;

#include <utility>
using std::pair;

#include <cmath>
using std::abs;

#include <cstdlib>
using std::div;

#include <algorithm>
using std::transform;

#include <iterator>
using std::back_inserter;

#include <cstddef>
#include <cassert>


#include "mainprogram.hh"

#include "datastructures.hh"

#ifdef GRAPHICAL_GUI
#include "mainwindow.hh"
#endif

string const MainProgram::PROMPT = "> ";

MainProgram::CmdResult MainProgram::cmd_add_stop(std::ostream& /*output*/, MatchIter begin, MatchIter end)
{
    string idstr = *begin++;
    string name = *begin++;
    string xstr = *begin++;
    string ystr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    StopID id = convert_string_to<StopID>(idstr);
    Coord xy = {convert_string_to<int>(xstr), convert_string_to<int>(ystr)};

    bool success = ds_.add_stop(id, name, xy);

    view_dirty = true;
    return {ResultType::STOPIDLIST, CmdResultStopIDs{NO_REGION, {success ? id: NO_STOP}}};
}

MainProgram::CmdResult MainProgram::cmd_stop_name(std::ostream &output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string stopidstr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    StopID stopid = convert_string_to<StopID>(stopidstr);

    auto result = ds_.get_stop_name(stopid);
    if (result == NO_NAME)
    {
        return {ResultType::STOPIDLIST, MainProgram::CmdResultStopIDs{NO_REGION, {NO_STOP}}};
    }
    else
    {
        output << "Stop ID " << stopid << " has name '" << result << "'" << endl;
        return {ResultType::STOPIDLIST, MainProgram::CmdResultStopIDs{NO_REGION, {stopid}}};
    }
}

void MainProgram::test_stop_name()
{
    if (random_stops_added_ > 0) // Don't do anything if there's no beacons
    {
        StopID id = random<decltype(random_stops_added_)>(0, random_stops_added_);
        ds_.get_stop_name(id);
    }
}

MainProgram::CmdResult MainProgram::cmd_stop_coord(std::ostream &output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string stopidstr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    StopID stopid = convert_string_to<StopID>(stopidstr);

    auto result = ds_.get_stop_coord(stopid);
    if (result == NO_COORD)
    {
        return {ResultType::STOPIDLIST, MainProgram::CmdResultStopIDs{NO_REGION, {NO_STOP}}};
    }
    else
    {
        output << "Stop ID " << stopid << " is in position ";
        print_coord(result, output);
        output << endl;
        return {ResultType::STOPIDLIST, MainProgram::CmdResultStopIDs{NO_REGION, {stopid}}};
    }
}

void MainProgram::test_stop_coord()
{
    if (random_stops_added_ > 0) // Don't do anything if there's no beacons
    {
        StopID id = random<decltype(random_stops_added_)>(0, random_stops_added_);
        ds_.get_stop_coord(id);
    }
}

MainProgram::CmdResult MainProgram::cmd_add_region(std::ostream& output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string idstr = *begin++;
    string name = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    RegionID id = idstr;

    bool success = ds_.add_region(id, name);

    if (success)
    {
        view_dirty = true;
        return {ResultType::STOPIDLIST, CmdResultStopIDs{id, {}}};
    }
    else
    {
        output << "Adding region failed." << endl;
        return {};
    }
}

MainProgram::CmdResult MainProgram::cmd_region_name(std::ostream &output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string regionidstr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    RegionID regionid = regionidstr;

    auto result = ds_.get_region_name(regionid);
    if (result == NO_NAME)
    {
        return {ResultType::REGIONIDLIST, MainProgram::CmdResultRegionIDs{NO_REGION}};
    }
    else
    {
        output << "Region ID " << regionid << " has name '" << result << "'" << endl;
        return {ResultType::REGIONIDLIST, MainProgram::CmdResultRegionIDs{regionid}};
    }
}

void MainProgram::test_region_name()
{
    if (random_stops_added_ > 0)
    {
        auto regid = n_to_strid(random<decltype(random_stops_added_)>(0, (random_stops_added_-1)/10+1));
        ds_.get_region_name(regid);
    }
}

MainProgram::CmdResult MainProgram::cmd_creation_finished(std::ostream& output, MainProgram::MatchIter /*begin*/, MainProgram::MatchIter /*end*/)
{
    ds_.creation_finished();
    output << "Creation finished.";

    return {};
}

MainProgram::CmdResult MainProgram::cmd_change_stop_name(std::ostream& /*output*/, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string idstr = *begin++;
    string newname = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    StopID id = convert_string_to<StopID>(idstr);

    bool success = ds_.change_stop_name(id, newname);

    view_dirty = true;
    return {ResultType::STOPIDLIST, CmdResultStopIDs{NO_REGION, {success ? id : NO_STOP}}};
}

void MainProgram::test_change_stop_name()
{
  if (random_stops_added_ > 0) // Don't do anything if there's no beacons
  {
      StopID id = random<decltype(random_stops_added_)>(0, random_stops_added_);
      auto newname = n_to_name(random<decltype(random_stops_added_)>(0, random_stops_added_));
      ds_.change_stop_name(id, newname);
  }
}

MainProgram::CmdResult MainProgram::cmd_change_stop_coord(std::ostream& /*output*/, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string idstr = *begin++;
    string xstr = *begin++;
    string ystr = *begin++;

    assert( begin == end && "Impossible number of parameters!");

    StopID id = convert_string_to<StopID>(idstr);
    int x = convert_string_to<int>(xstr);
    int y = convert_string_to<int>(ystr);

    bool success = ds_.change_stop_coord(id, {x, y});

    view_dirty = true;
    return {ResultType::STOPIDLIST, CmdResultStopIDs{NO_REGION, {success ? id : NO_STOP}}};
}

void MainProgram::test_change_stop_coord()
{
    if (random_stops_added_ > 0) // Don't do anything if there's no beacons
    {
        StopID id = random<decltype(random_stops_added_)>(0, random_stops_added_);
        auto x = random(0, 1000);
        auto y = random(0, 1000);
        ds_.change_stop_coord(id, {x, y});
    }
}

MainProgram::CmdResult MainProgram::cmd_add_stop_to_region(std::ostream& output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string stopidstr = *begin++;
    string regionidstr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    StopID stopid = convert_string_to<StopID>(stopidstr);
    RegionID regionid = regionidstr;

    view_dirty = true;

    bool ok = ds_.add_stop_to_region(stopid, regionid);
    if (ok)
    {
        auto sourcename = ds_.get_stop_name(stopid);
        auto targetname = ds_.get_region_name(regionid);
        output << "Added stop " << sourcename << " to region " << targetname << endl;
        return {ResultType::STOPIDLIST, MainProgram::CmdResultStopIDs{regionid, {stopid}}};
    }
    else
    {
        output << "Adding stop to region failed!" << endl;
        return {ResultType::STOPIDLIST, MainProgram::CmdResultStopIDs{NO_REGION, {NO_STOP}}};
    }
}

MainProgram::CmdResult MainProgram::cmd_add_subregion_to_region(std::ostream& output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string sourceidstr = *begin++;
    string targetidstr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    RegionID sourceid = sourceidstr;
    RegionID targetid = targetidstr;

    view_dirty = true;

    bool ok = ds_.add_subregion_to_region(sourceid, targetid);
    if (ok)
    {
        auto sourcename = ds_.get_region_name(sourceid);
        auto targetname = ds_.get_region_name(targetid);
        output << "Added subregion " << sourcename << " to region " << targetname << endl;
    }
    else
    {
        output << "Adding subregion to region failed!" << endl;
    }

    return {};
}

MainProgram::CmdResult MainProgram::cmd_stop_regions(std::ostream& output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string stopidstr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    StopID stopid = convert_string_to<StopID>(stopidstr);

    output << "Regions for stop ";
    print_stop(stopid, output);

    auto result = ds_.stop_regions(stopid);
    if (result.empty()) { return {ResultType::REGIONIDLIST, MainProgram::CmdResultRegionIDs{NO_REGION}}; }
    else { return {ResultType::REGIONIDLIST, result}; }
}

void MainProgram::test_stop_regions()
{
    if (random_stops_added_ > 0) // Don't do anything if there's no beacons
    {
        auto id = n_to_id(random<decltype(random_stops_added_)>(0, random_stops_added_));
        ds_.stop_regions(id);
    }
}

MainProgram::CmdResult MainProgram::cmd_stops_closest_to(std::ostream& /*output*/, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
  string stopidstr = *begin++;
  assert( begin == end && "Impossible number of parameters!");

  StopID stopid = convert_string_to<StopID>(stopidstr);

  auto result = ds_.stops_closest_to(stopid);
  return {ResultType::STOPIDLIST, CmdResultStopIDs{NO_REGION, result}};
}

void MainProgram::test_stops_closest_to()
{
    if (random_stops_added_ > 0) // Don't do anything if there's no beacons
    {
        auto id = n_to_id(random<decltype(random_stops_added_)>(0, random_stops_added_));
        ds_.stop_regions(id);
    }
}

MainProgram::CmdResult MainProgram::cmd_region_bounding_box(std::ostream& output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string regionidstr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    RegionID regionid = regionidstr;

    auto result = ds_.region_bounding_box(regionid);
    auto name = ds_.get_region_name(regionid);
    output << "Stops in region " << name << " (and subregions) are within ";
    print_coord(result.first, output);
    output << "-";
    print_coord(result.second, output);
    output << endl;

    return {ResultType::REGIONIDLIST, CmdResultRegionIDs{regionid}};
}

void MainProgram::test_region_bounding_box()
{
    if (random_stops_added_ > 0) // Don't do anything if there's no beacons
    {
        auto regid = n_to_strid(random<decltype(random_stops_added_)>(0, (random_stops_added_-1)/10+1));
        ds_.region_bounding_box(regid);
    }
}


MainProgram::CmdResult MainProgram::cmd_stops_common_region(std::ostream &output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    string stopid1str = *begin++;
    string stopid2str = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    StopID stopid1 = convert_string_to<StopID>(stopid1str);
    StopID stopid2 = convert_string_to<StopID>(stopid2str);

    auto result = ds_.stops_common_region(stopid1, stopid2);
    if (result == NO_REGION)
    {
        output << "No common region found!" << endl;
    }
    return {ResultType::STOPIDLIST, CmdResultStopIDs{result, {stopid1, stopid2}}};
}

void MainProgram::test_stops_common_region()
{
    if (random_stops_added_ > 0) // Don't do anything if there's no beacons
    {
        auto id1 = n_to_id(random<decltype(random_stops_added_)>(0, random_stops_added_));
        auto id2 = n_to_id(random<decltype(random_stops_added_)>(0, random_stops_added_));
        ds_.stops_common_region(id1, id2);
    }
}

MainProgram::CmdResult MainProgram::cmd_remove_stop(std::ostream& output, MatchIter begin, MatchIter end)
{
    string stopidstr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    StopID id = convert_string_to<StopID>(stopidstr);
    auto name = ds_.get_stop_name(id);
    bool success = ds_.remove_stop(id);
    if (success)
    {
        output << "Stop " << name << " removed." << endl;
        view_dirty = true;
        return {};
    }
    else
    {
        return {ResultType::STOPIDLIST, MainProgram::CmdResultStopIDs{NO_REGION, {NO_STOP}}};
    }
}

void MainProgram::test_remove_stop()
{
    // Choose random number to remove
    if (random_stops_added_ > 0) // Don't remove if there's nothing to remove
    {
        auto id = n_to_id(random<decltype(random_stops_added_)>(0, random_stops_added_));
        ds_.remove_stop(id);
    }
}

void MainProgram::add_random_stops_regions(unsigned int size, Coord min, Coord max)
{
    for (unsigned int i = 0; i < size; ++i)
    {
        auto name = n_to_name(random_stops_added_);
        StopID id = n_to_id(random_stops_added_);

        int x = random<int>(min.x, max.x);
        int y = random<int>(min.y, max.y);

        ds_.add_stop(id, name, {x, y});

        // Add a new region for every 10 stops
        if (random_stops_added_ % 10 == 0)
        {
            auto regidname = n_to_strid(random_stops_added_ / 10);
            ds_.add_region(regidname, regidname);
            // Add region as subregion for some earlier region
            if (random_stops_added_/10 > 0)
            {
                auto parentid = n_to_strid(random<decltype(random_stops_added_)>(0, random_stops_added_/10));
                ds_.add_subregion_to_region(regidname, parentid);
            }
        }

        // Add stop to a region that has already been created
        auto regid = n_to_strid(random<decltype(random_stops_added_)>(0, random_stops_added_/10+1));
        ds_.add_stop_to_region(id, regid);

        ++random_stops_added_;
    }
}

MainProgram::CmdResult MainProgram::cmd_random_add(std::ostream& output, MatchIter begin, MatchIter end)
{
    string sizestr = *begin++;
    string minxstr = *begin++;
    string minystr = *begin++;
    string maxxstr = *begin++;
    string maxystr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    unsigned int size = convert_string_to<unsigned int>(sizestr);

    Coord def_min{1, 1};
    Coord def_max{100, 100};
    Coord min = def_min;
    Coord max = def_max;
    if (!minxstr.empty() && !minystr.empty() && !maxxstr.empty() && !maxystr.empty())
    {
        min.x = convert_string_to<unsigned int>(minxstr);
        min.y = convert_string_to<unsigned int>(minystr);
        max.x = convert_string_to<unsigned int>(maxxstr);
        max.y = convert_string_to<unsigned int>(maxystr);
    }
    else
    {
        auto stops = ds_.all_stops();
        if (!stops.empty())
        {
            // Find out bounding box
            min = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
            max = {std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};
            for (auto  stop : stops)
            {
                auto [x,y] = ds_.get_stop_coord(stop);
                if (x < min.x) { min.x = x; }
                if (y < min.y) { min.y = y; }
                if (x > max.x) { max.x = x; }
                if (y > max.y) { max.y = y; }
            }
        }
    }

    if (min == max)
    {
        min = def_min;
        max = def_max;
    }

    add_random_stops_regions(size, min, max);

    output << "Added: " << size << " stops." << endl;

    view_dirty = true;

    return {};
}

void MainProgram::test_random_add()
{
    add_random_stops_regions(1);
}

MainProgram::CmdResult MainProgram::cmd_randseed(std::ostream& output, MatchIter begin, MatchIter end)
{
    string seedstr = *begin++;
    assert(begin == end && "Invalid number of parameters");

    unsigned long int seed = convert_string_to<unsigned long int>(seedstr);

    rand_engine_.seed(seed);
    init_primes();

    output << "Random seed set to " << seed << endl;

    return {};
}

MainProgram::CmdResult MainProgram::cmd_read(std::ostream& output, MatchIter begin, MatchIter end)
{
    string filename = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    ifstream input(filename);
    if (input)
    {
        output << "** Commands from '" << filename << "'" << endl;
        command_parser(input, output, PromptStyle::NORMAL);
        output << "** End of commands from '" << filename << "'" << endl;
    }
    else
    {
        output << "Cannot open file '" << filename << "'!" << endl;
    }

    return {};
}


MainProgram::CmdResult MainProgram::cmd_testread(std::ostream& output, MatchIter begin, MatchIter end)
{
    string infilename = *begin++;
    string outfilename = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    ifstream input(infilename);
    if (input)
    {
        ifstream expected_output(outfilename);
        if (output)
        {
            stringstream actual_output;
            command_parser(input, actual_output, PromptStyle::NO_NESTING);

            vector<string> actual_lines;
            while (actual_output)
            {
                string line;
                getline(actual_output, line);
                if (!actual_output) { break; }
                actual_lines.push_back(line);
            }

            vector<string> expected_lines;
            while (expected_output)
            {
                string line;
                getline(expected_output, line);
                if (!expected_output) { break; }
                expected_lines.push_back(line);
            }

            string heading_actual = "Actual output";
            unsigned int actual_max_length = heading_actual.length();
            auto actual_max_iter = max_element(actual_lines.cbegin(), actual_lines.cend(),
                                               [](string s1, string s2){ return s1.length() < s2.length(); });
            if (actual_max_iter != actual_lines.cend())
            {
                actual_max_length = actual_max_iter->length();
            }

            string heading_expected = "Expected output";
            unsigned int expected_max_length = heading_expected.length();
            auto expected_max_iter = std::max_element(expected_lines.cbegin(), expected_lines.cend(),
                                                    [](string s1, string s2){ return s1.length() < s2.length(); });
            if (expected_max_iter != expected_lines.cend())
            {
                expected_max_length = expected_max_iter->length();
            }

            auto pos_actual = actual_lines.cbegin();
            auto pos_expected = expected_lines.cbegin();
            output << "  " << heading_actual << string(actual_max_length - heading_actual.length(), ' ') << " | " << heading_expected << endl;
            output << "--" << string(actual_max_length, '-') << "-|-" << string(expected_max_length, '-') << endl;

            bool lines_ok = true;
            while (pos_expected != expected_lines.cend() || pos_actual != actual_lines.cend())
            {
                if (pos_expected != expected_lines.cend())
                {
                    if (pos_actual != actual_lines.cend())
                    {
                        bool ok = (*pos_expected == *pos_actual);
                        output << (ok ? ' ' : '?') << ' ' << *pos_actual << string(actual_max_length - pos_actual->length(), ' ')
                               << " | " << *pos_expected << endl;
                        lines_ok = lines_ok && ok;
                        ++pos_actual;
                    }
                    else
                    { // Actual output was too short
                        output << "? " << string(actual_max_length, ' ')
                               << " | " << *pos_expected << endl;
                        lines_ok = false;
                    }
                    ++pos_expected;
                }
                else
                { // Actual output was too long
                    output << "? " << *pos_actual << string(actual_max_length - pos_actual->length(), ' ')
                           << " | " << endl;
                    lines_ok = false;
                    ++pos_actual;
                }
            }
            if (lines_ok)
            {
                output << "**No differences in output.**" << endl;
                if (test_status_ == TestStatus::NOT_RUN)
                {
                    test_status_ = TestStatus::NO_DIFFS;
                }
            }
            else
            {
                output << "**Differences found! (Lines beginning with '?')**" << endl;
                test_status_ = TestStatus::DIFFS_FOUND;
            }

        }
        else
        {
            output << "Cannot open file '" << outfilename << "'!" << endl;
        }
    }
    else
    {
        output << "Cannot open file '" << infilename << "'!" << endl;
    }

    return {};
}

MainProgram::CmdResult MainProgram::cmd_stop_count(std::ostream& output, MatchIter begin, MatchIter end)
{
    assert( begin == end && "Impossible number of parameters!");

    output << "Number of stops: " << ds_.stop_count() << endl;

    return {};
}

MainProgram::CmdResult MainProgram::cmd_all_stops(std::ostream& output, MatchIter begin, MatchIter end)
{
    assert( begin == end && "Impossible number of parameters!");

    auto stops = ds_.all_stops();
    if (stops.empty())
    {
        output << "No stops!" << endl;
    }

    sort(stops.begin(), stops.end());
    return {ResultType::STOPIDLIST, CmdResultStopIDs{NO_REGION, stops}};
}

MainProgram::CmdResult MainProgram::cmd_all_regions(std::ostream& output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    assert( begin == end && "Impossible number of parameters!");

    auto regions = ds_.all_regions();
    if (regions.empty())
    {
        output << "No regions!" << endl;
    }

    sort(regions.begin(), regions.end());
    return {ResultType::REGIONIDLIST, regions};
}

MainProgram::CmdResult MainProgram::cmd_stopwatch(std::ostream& output, MatchIter begin, MatchIter end)
{
    string on = *begin++;
    string off = *begin++;
    string next = *begin++;
    assert(begin == end && "Invalid number of parameters");

    if (!on.empty())
    {
        stopwatch_mode = StopwatchMode::ON;
        output << "Stopwatch: on" << endl;
    }
    else if (!off.empty())
    {
        stopwatch_mode = StopwatchMode::OFF;
        output << "Stopwatch: off" << endl;
    }
    else if (!next.empty())
    {
        stopwatch_mode = StopwatchMode::NEXT;
        output << "Stopwatch: on for the next command" << endl;
    }
    else
    {
        assert(!"Impossible stopwatch mode!");
    }

    return {};
}

MainProgram::CmdResult MainProgram::cmd_clear_all(std::ostream& output, MatchIter begin, MatchIter end)
{
    assert(begin == end && "Invalid number of parameters");

    ds_.clear_all();
    init_primes();

    output << "Cleared everything." << endl;

    view_dirty = true;

    return {};
}

std::string MainProgram::print_stop(StopID id, ostream& output)
{
    if (id != NO_STOP)
    {
        auto name = ds_.get_stop_name(id);
        auto xy = ds_.get_stop_coord(id);
        if (!name.empty())
        {
            output << name;
        }
        else
        {
            output << "*";
        }

         output << ": pos=";
         print_coord(xy, output);
         output << ", id=" << id << endl;

        ostringstream retstream;
        retstream << id;
        return retstream.str();
    }
    else
    {
        output << "--NO_STOP--" << endl;
        return "";
    }
}

std::string MainProgram::print_stop_name(StopID id, std::ostream &output)
{
    if (id != NO_STOP)
    {
        auto name = ds_.get_stop_name(id);
        if (!name.empty())
        {
            output << name;
        }
        else
        {
            output << "*";
        }

        ostringstream retstream;
        retstream << name;
        return retstream.str();
    }
    else
    {
        output << "--NO_STOP--";
        return "";
    }
}

std::string MainProgram::print_region(RegionID id, std::ostream& output)
{
    if (id != NO_REGION)
    {
        auto name = ds_.get_region_name(id);
        if (!name.empty())
        {
            output << name << ": id=" << id << endl;
        }
        else
        {
            output << "*" << ": id=" << id << endl;
        }
        ostringstream retstream;
        retstream << id;
        return retstream.str();
    }
    else
    {
        output << "--NO_REGION--" << endl;
        return "";
    }
}

std::string MainProgram::print_coord(Coord coord, std::ostream& output)
{
    if (coord != NO_COORD)
    {
        output << "(" << coord.x << "," << coord.y << ")";
        ostringstream retstream;
        retstream << "(" << coord.x << "," << coord.y << ")";
        return retstream.str();
    }
    else
    {
        output << "(--NO_COORD--)";
        return "";
    }
}

MainProgram::CmdResult MainProgram::cmd_find_stops(std::ostream& /*output*/, MatchIter begin, MatchIter end)
{
    string name = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    auto result = ds_.find_stops(name);
    if (result.empty()) { result = {NO_STOP}; }

    sort(result.begin(), result.end());
    return {ResultType::STOPIDLIST, CmdResultStopIDs{NO_REGION, result}};
}

void MainProgram::test_find_stops()
{
    // Choose random number to remove
    if (random_stops_added_ > 0) // Don't find if there's nothing to find
    {
        auto name = n_to_name(random<decltype(random_stops_added_)>(0, random_stops_added_));
        ds_.find_stops(name);
    }
}

string const stpidx = "([0-9]+)";
string const regidx = "([a-zA-Z0-9]+)";
string const routeidx = "([a-zA-Z0-9]+)";
string const namex = "([a-zA-Z0-9 -]+)";
string const numx = "([0-9]+)";
string const coordx = "\\([[:space:]]*([0-9]+)[[:space:]]*,[[:space:]]*([0-9]+)[[:space:]]*\\)";
string const wsx = "[[:space:]]+";

vector<MainProgram::CmdInfo> MainProgram::cmds_ =
{
    {"add_stop", "ID Name (x,y)", stpidx+wsx+namex+wsx+coordx, &MainProgram::cmd_add_stop, nullptr },
    {"random_add", "number_of_stops_to_add  (minx,miny) (maxx,maxy) (coordinates optional)", numx+"(?:"+wsx+coordx+wsx+coordx+")?",
     &MainProgram::cmd_random_add, &MainProgram::test_random_add },
    {"all_stops", "", "", &MainProgram::cmd_all_stops, nullptr },
    {"stop_name", "ID", stpidx, &MainProgram::cmd_stop_name, &MainProgram::test_stop_name },
    {"stop_coord", "ID", stpidx, &MainProgram::cmd_stop_coord, &MainProgram::test_stop_coord },
    {"add_region", "ID Name", regidx+wsx+namex, &MainProgram::cmd_add_region, nullptr },
    {"all_regions", "", "", &MainProgram::cmd_all_regions, nullptr },
    {"region_name", "RegionID", regidx, &MainProgram::cmd_region_name, &MainProgram::test_region_name },
    {"creation_finished", "", "", &MainProgram::cmd_creation_finished, nullptr },
    {"stop_count", "", "", &MainProgram::cmd_stop_count, nullptr },
    {"clear_all", "", "", &MainProgram::cmd_clear_all, nullptr },
    {"stops_alphabetically", "", "", &MainProgram::NoParStopListCmd<&Datastructures::stops_alphabetically>, &MainProgram::NoParStopListTestCmd<&Datastructures::stops_alphabetically> },
    {"stops_coord_order", "", "", &MainProgram::NoParStopListCmd<&Datastructures::stops_coord_order>, &MainProgram::NoParStopListTestCmd<&Datastructures::stops_coord_order> },
    {"min_coord", "", "", &MainProgram::NoParStopCmd<&Datastructures::min_coord>, &MainProgram::NoParStopTestCmd<&Datastructures::min_coord> },
    {"max_coord", "", "", &MainProgram::NoParStopCmd<&Datastructures::max_coord>, &MainProgram::NoParStopTestCmd<&Datastructures::max_coord> },
    {"stops_closest_to", "ID", stpidx, &MainProgram::cmd_stops_closest_to, &MainProgram::test_stops_closest_to },
    {"stops_common_region", "ID1 ID2", stpidx+wsx+stpidx, &MainProgram::cmd_stops_common_region, &MainProgram::test_stops_common_region },
    {"remove_stop", "ID", stpidx, &MainProgram::cmd_remove_stop, &MainProgram::test_remove_stop },
    {"find_stops", "name", namex, &MainProgram::cmd_find_stops, &MainProgram::test_find_stops },
    {"change_stop_name", "ID newname", stpidx+wsx+namex, &MainProgram::cmd_change_stop_name, &MainProgram::test_change_stop_name },
    {"change_stop_coord", "ID (x,y)", stpidx+wsx+coordx, &MainProgram::cmd_change_stop_coord, &MainProgram::test_change_stop_coord },
    {"add_stop_to_region", "StopID RegionID", stpidx+wsx+regidx, &MainProgram::cmd_add_stop_to_region, nullptr },
    {"add_subregion_to_region", "SubregionID RegionID", regidx+wsx+regidx, &MainProgram::cmd_add_subregion_to_region, nullptr },
    {"stop_regions", "StopID", stpidx, &MainProgram::cmd_stop_regions, &MainProgram::test_stop_regions },
    {"region_bounding_box", "RegionID", regidx, &MainProgram::cmd_region_bounding_box, &MainProgram::test_region_bounding_box },
    {"quit", "", "", nullptr, nullptr },
    {"help", "", "", &MainProgram::help_command, nullptr },
    {"read", "\"in-filename\"", "\"([-a-zA-Z0-9 ./:_]+)\"", &MainProgram::cmd_read, nullptr },
    {"testread", "\"in-filename\" \"out-filename\"", "\"([-a-zA-Z0-9 ./:_]+)\""+wsx+"\"([-a-zA-Z0-9 ./:_]+)\"", &MainProgram::cmd_testread, nullptr },
    {"perftest", "cmd1/all/compulsory[;cmd2;cmd3...] timeout repeat_count n1[;n2;n3...] (parts in [] are optional)",
     "([0-9a-zA-Z_]+(?:;[0-9a-zA-Z_]+)*)"+wsx+numx+wsx+numx+wsx+"([0-9]+(?:;[0-9]+)*)", &MainProgram::cmd_perftest, nullptr },
    {"stopwatch", "on/off/next (one of these)", "(?:(on)|(off)|(next))", &MainProgram::cmd_stopwatch, nullptr },
    {"random_seed", "new-random-seed-integer", numx, &MainProgram::cmd_randseed, nullptr },
    {"#", "comment text", ".*", &MainProgram::cmd_comment, nullptr },
};

MainProgram::CmdResult MainProgram::help_command(std::ostream& output, MatchIter /*begin*/, MatchIter /*end*/)
{
    output << "Commands:" << endl;
    for (auto& i : cmds_)
    {
        output << "  " << i.cmd << " " << i.info << endl;
    }

    return {};
}

MainProgram::CmdResult MainProgram::cmd_perftest(std::ostream& output, MatchIter begin, MatchIter end)
{
#ifdef _GLIBCXX_DEBUG
    output << "WARNING: Debug STL enabled, performance will be worse than expected (maybe also asymptotically)!" << endl;
#endif // _GLIBCXX_DEBUG

    vector<string> optional_cmds({"region_bounding_box", "stops_closest_to", "stops_common_region"});
    vector<string> nondefault_cmds({"remove_stop", "find_stops"});

    string commandstr = *begin++;
    unsigned int timeout = convert_string_to<unsigned int>(*begin++);
    unsigned int repeat_count = convert_string_to<unsigned int>(*begin++);
//    unsigned int friend_count = convert_string_to<unsigned int>(*begin++);
    string sizes = *begin++;
    assert(begin == end && "Invalid number of parameters");

    vector<string> testcmds;
    if (commandstr != "all" && commandstr != "compulsory")
    {
        smatch scmd;
        auto cbeg = commandstr.cbegin();
        auto cend = commandstr.cend();
        for ( ; regex_search(cbeg, cend, scmd, commands_regex_); cbeg = scmd.suffix().first)
        {
            testcmds.push_back(scmd[1]);
        }
    }

    vector<unsigned int> init_ns;
    smatch size;
    auto sbeg = sizes.cbegin();
    auto send = sizes.cend();
    for ( ; regex_search(sbeg, send, size, sizes_regex_); sbeg = size.suffix().first)
    {
        init_ns.push_back(convert_string_to<unsigned int>(size[1]));
    }

    output << "Timeout for each N is " << timeout << " sec. " << endl;
//    output << "Add 0.." << friend_count << " friends for every employee." << endl;
    output << "For each N perform " << repeat_count << " random command(s) from:" << endl;

    // Initialize test functions
    vector<void(MainProgram::*)()> testfuncs;
    if (testcmds.empty())
    { // Add all commands
        for (auto& i : cmds_)
        {
            if (i.testfunc)
            {
                if (find(nondefault_cmds.begin(), nondefault_cmds.end(), i.cmd) == nondefault_cmds.end() &&
                    (commandstr == "all" || find(optional_cmds.begin(), optional_cmds.end(), i.cmd) == optional_cmds.end()))
                {
                    output << i.cmd << " ";
                    testfuncs.push_back(i.testfunc);
                }
            }
        }
    }
    else
    {
        for (auto& i : testcmds)
        {
            auto pos = find_if(cmds_.begin(), cmds_.end(), [&i](auto& cmd){ return cmd.cmd == i; });
            if (pos != cmds_.end() && pos->testfunc)
            {
                output << i << " ";
                testfuncs.push_back(pos->testfunc);
            }
            else
            {
                output << "(cannot test " << i << ") ";
            }
        }
    }
    output << endl << endl;

    if (testfuncs.empty())
    {
        output << "No commands to test!" << endl;
        return {};
    }

    output << setw(7) << "N" << " , " << setw(12) << "add (sec)" << " , " << setw(12) << "cmds (sec)"  << " , " << setw(12) << "total (sec)" << endl;
    flush_output(output);

    auto stop = false;
    for (unsigned int n : init_ns)
    {
        if (stop) { break; }

        output << setw(7) << n << " , " << flush;

        ds_.clear_all();
        init_primes();

        Stopwatch stopwatch;
        stopwatch.start();

        // Add random beacons (+ light sources)
        for (unsigned int i = 0; i < n / 1000; ++i)
        {
            add_random_stops_regions(1000);

            stopwatch.stop();
            if (stopwatch.elapsed() >= timeout)
            {
                output << "Timeout!" << endl;
                stop = true;
                break;
            }
            if (check_stop())
            {
                output << "Stopped!" << endl;
                stop = true;
                break;
            }
            stopwatch.start();
        }
        if (stop) { break; }

        add_random_stops_regions(n % 1000);

        auto addsec = stopwatch.elapsed();
        output << setw(12) << addsec << " , " << flush;
        if (addsec >= timeout)
        {
            output << "Timeout!" << endl;
            stop = true;
            break;
        }

        ds_.creation_finished();
        for (unsigned int repeat = 0; repeat < repeat_count; ++repeat)
        {
            auto cmdpos = random(testfuncs.begin(), testfuncs.end());

            (this->**cmdpos)();

            if (repeat % 10 == 0)
            {
                stopwatch.stop();
                if (stopwatch.elapsed() >= timeout)
                {
                    output << "Timeout!" << endl;
                    stop = true;
                    break;
                }
                if (check_stop())
                {
                    output << "Stopped!" << endl;
                    stop = true;
                    break;
                }
                stopwatch.start();
            }
        }
        if (stop) { break; }

        stopwatch.stop();
        auto totalsec = stopwatch.elapsed();
        output << setw(12) << totalsec-addsec << " , " << setw(12) << totalsec;

//        unsigned long int maxmem;
//        string unit;
//        tie(maxmem, unit) = mempeak();
//        maxmem -=  startmem;
//        if (maxmem != 0)
//        {
//            output << ", memory " << maxmem << " " << unit;
//        }
        output << endl;
        flush_output(output);
    }

    ds_.clear_all();
    init_primes();

#ifdef _GLIBCXX_DEBUG
    output << "WARNING: Debug STL enabled, performance will be worse than expected (maybe also asymptotically)!" << endl;
#endif // _GLIBCXX_DEBUG

    return {};
}

MainProgram::CmdResult MainProgram::cmd_comment(std::ostream& /*output*/, MatchIter /*begin*/, MatchIter /*end*/)
{
    return {};
}

bool MainProgram::command_parse_line(string inputline, ostream& output)
{
//    static unsigned int nesting_level = 0; // UGLY! Remember nesting level to print correct amount of >:s.
//    if (promptstyle != PromptStyle::NO_NESTING) { ++nesting_level; }

    if (inputline.empty()) { return true; }

    smatch match;
    bool matched = regex_match(inputline, match, cmds_regex_);
    if (matched)
    {
        assert(match.size() == 3);
        string cmd = match[1];
        string params = match[2];

        auto pos = find_if(cmds_.begin(), cmds_.end(), [cmd](CmdInfo const& ci) { return ci.cmd == cmd; });
        assert(pos != cmds_.end());

        smatch match;
        bool matched = regex_match(params, match, pos->param_regex);
        if (matched)
        {
            if (pos->func)
            {
                assert(!match.empty());

                Stopwatch stopwatch;
                bool use_stopwatch = (stopwatch_mode != StopwatchMode::OFF);
                // Reset stopwatch mode if only for the next command
                if (stopwatch_mode == StopwatchMode::NEXT) { stopwatch_mode = StopwatchMode::OFF; }

               TestStatus initial_status = test_status_;
               test_status_ = TestStatus::NOT_RUN;

                if (use_stopwatch)
                {
                    stopwatch.start();
                }

                auto result = (this->*(pos->func))(output, ++(match.begin()), match.end());

                if (use_stopwatch)
                {
                    stopwatch.stop();
                }

                switch (result.first)
                {
                    case ResultType::NOTHING:
                    {
                        break;
                    }
                    case ResultType::STOPIDLIST:
                    {
                        auto& [region, stops] = std::get<CmdResultStopIDs>(result.second);
                        if (region != NO_REGION)
                        {
                            output << "Region: ";
                            print_region(region, output);
                        }
                        if (!stops.empty())
                        {
                            if (stops.size() == 1 && stops.front() == NO_STOP)
                            {
                                output << "Failed (NO_... returned)!!" << std::endl;
                            }
                            else
                            {
                                unsigned int num = 0;
                                for (StopID id : stops)
                                {
                                    ++num;
                                    if (stops.size() > 1) { output << num << ". "; }
                                    print_stop(id, output);
                                }
                            }
                        }
                        break;
                    }
                    case ResultType::REGIONIDLIST:
                    {
                        auto& regions = std::get<CmdResultRegionIDs>(result.second);
                        if (!regions.empty())
                        {
                            if (regions.size() == 1 && regions.front() == NO_REGION)
                            {
                                output << "Failed (NO_... returned)!!" << std::endl;
                            }
                            else
                            {
                                unsigned int num = 0;
                                for (auto region : regions)
                                {
                                    ++num;
                                    if (regions.size() > 1) { output << num << ". "; }
                                    print_region(region, output);
                                }
                            }
                        }
                        break;
                    }
                    default:
                    {
                        assert(false && "Unsupported result type!");
                    }
                }

                if (result != prev_result)
                {
                    prev_result = move(result);
                    view_dirty = true;
                }

                if (use_stopwatch)
                {
                    output << "Command '" << cmd << "': " << stopwatch.elapsed() << " sec" << endl;
                }

                if (test_status_ != TestStatus::NOT_RUN)
                {
                    output << "Testread-tests have been run, " << ((test_status_ == TestStatus::DIFFS_FOUND) ? "differences found!" : "no differences found.") << endl;
                }
                if (test_status_ == TestStatus::NOT_RUN || (test_status_ == TestStatus::NO_DIFFS && initial_status == TestStatus::DIFFS_FOUND))
                {
                    test_status_ = initial_status;
                }
            }
            else
            { // No function to run = quit command
                return false;
            }
        }
        else
        {
            output << "Invalid parameters for command '" << cmd << "'!" << endl;
        }
    }
    else
    {
        output << "Unknown command!" << endl;
    }

    return true; // Signal continuing
}

void MainProgram::command_parser(istream& input, ostream& output, PromptStyle promptstyle)
{
    string line;
    do
    {
//        output << string(nesting_level, '>') << " ";
        output << PROMPT;
        getline(input, line, '\n');

        if (promptstyle != PromptStyle::NO_ECHO)
        {
            output << line << endl;
        }

        if (!input) { break; }

        bool cont = command_parse_line(line, output);
        view_dirty = false; // No need to keep track of individual result changes
        if (!cont) { break; }
    }
    while (input);
    //    if (promptstyle != PromptStyle::NO_NESTING) { --nesting_level; }

    view_dirty = true; // To be safe, assume that results have been changed
}

void MainProgram::setui(MainWindow* ui)
{
    ui_ = ui;
}

#ifdef GRAPHICAL_GUI
void MainProgram::flush_output(std::ostream& output)
{
    if (ui_)
    {
        if (auto soutput = dynamic_cast<ostringstream*>(&output))
        {
            ui_->output_text(*soutput);
        }
    }
}
#else
void MainProgram::flush_output(std::ostream& /*output*/)
{
}
#endif

bool MainProgram::check_stop() const
{
#ifdef GRAPHICAL_GUI
    if (ui_)
    {
        return ui_->check_stop_pressed();
    }
#endif
    return false;
}

std::array<unsigned long int, 20> const MainProgram::primes1{4943,   4951,   4957,   4967,   4969,   4973,   4987,   4993,   4999,   5003,
                                                             5009,   5011,   5021,   5023,   5039,   5051,   5059,   5077,   5081,   5087};
std::array<unsigned long int, 20> const MainProgram::primes2{81031,  81041,  81043,  81047,  81049,  81071,  81077,  81083,  81097,  81101,
                                                             81119,  81131,  81157,  81163,  81173,  81181,  81197,  81199,  81203,  81223};

MainProgram::MainProgram()
{
    rand_engine_.seed(time(nullptr));

    //    startmem = get<0>(mempeak());

    init_primes();
    init_regexs();
}

int MainProgram::mainprogram(int argc, char* argv[])
{
    vector<string> args(argv, argv+argc);

    if (args.size() > 2)
    {
        cerr << "Usage: " + ((args.size() > 0) ? args[0] : "<program name>") + " [<command file>]" << endl;
        return EXIT_FAILURE;
    }

    MainProgram mainprg;

    if (args.size() == 2 && args[1] != "--console")
    {
        string filename = args[1];
        ifstream input(filename);
        if (input)
        {
            mainprg.command_parser(input, cout, MainProgram::PromptStyle::NORMAL);
        }
        else
        {
            cout << "Cannot open file '" << filename << "'!" << endl;
        }
    }
    else
    {
        mainprg.command_parser(cin, cout, MainProgram::PromptStyle::NO_ECHO);
    }

    cerr << "Program ended normally." << endl;
    return EXIT_SUCCESS;
}

void MainProgram::init_primes()
{
    // Initialize id generator
    prime1_ = primes1[random<int>(0, primes1.size())];
    prime2_ = primes2[random<int>(0, primes2.size())];
    random_stops_added_ = 0;
}

string MainProgram::n_to_name(unsigned long n)
{
    unsigned long int hash = prime1_*n + prime2_;
    string name;

    while (hash > 0)
    {
        auto hexnum = hash % 26;
        hash /= 26;
        name.push_back('a'+hexnum);
    }

    return name;
}

string MainProgram::n_to_strid(unsigned long n)
{
 unsigned long int hash = prime1_*n + prime2_;
 string name = "Reg";

 while (hash > 0)
 {
     auto hexnum = hash % 26;
     hash /= 26;
     name.push_back('a'+hexnum);
 }

 return name;
}

StopID MainProgram::n_to_id(unsigned long int n)
{
    unsigned long int hash = prime2_*n + prime1_;

    return hash % static_cast<unsigned long int>(std::numeric_limits<StopID>::max());
}

void MainProgram::init_regexs()
{
    // Create regex <whitespace>(cmd1|cmd2|...)<whitespace>(.*)
    string cmds_regex_str = "[[:space:]]*(";
    bool first = true;
    for (auto& cmd : cmds_)
    {
        cmds_regex_str += (first ? "" : "|") + cmd.cmd;
        first = false;

        cmd.param_regex = regex(cmd.param_regex_str+"[[:space:]]*", std::regex_constants::ECMAScript | std::regex_constants::optimize);
    }
    cmds_regex_str += ")(?:[[:space:]]*$|"+wsx+"(.*))";
    cmds_regex_ = regex(cmds_regex_str, std::regex_constants::ECMAScript | std::regex_constants::optimize);
    stops_regex_ = regex(stpidx+"[[:space:]]?", std::regex_constants::ECMAScript | std::regex_constants::optimize);
    times_regex_ = regex(wsx+"([0-9][0-9]):([0-9][0-9]):([0-9][0-9])", std::regex_constants::ECMAScript | std::regex_constants::optimize);
    commands_regex_ = regex("([0-9a-zA-Z_]+);?", std::regex_constants::ECMAScript | std::regex_constants::optimize);
    sizes_regex_ = regex(numx+";?", std::regex_constants::ECMAScript | std::regex_constants::optimize);
}
