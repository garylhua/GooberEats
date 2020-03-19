#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> m_Map;
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream infile(mapFile); //change this
    if (!infile)
    {
        return false;
    }
    string line = "";
    while (getline(infile, line))
    {
        //get street name
        string streetName = line;
        line.clear();

        //get number of street segments for the streets
        getline(infile, line);
        stringstream toInt(line);
        int nStreetSegmentsLines;
        toInt >> nStreetSegmentsLines;
        line.clear();
        
        while (nStreetSegmentsLines > 0)
        {
            //start getting GeoCords for street
            getline(infile, line, ' ');
            string slatitude = line;
            line.clear();

            getline(infile, line, ' ');
            string slongitude = line;
            line.clear();

            getline(infile, line, ' ');
            string elatitude = line;
            line.clear();

            getline(infile, line);
            string elongitude = line;
            
            GeoCoord beginC(slatitude, slongitude);
            if (m_Map.find(beginC) == nullptr)
            {
                m_Map.associate(beginC, vector<StreetSegment>());
            }

            GeoCoord endC(elatitude, elongitude);
            if (m_Map.find(endC) == nullptr)
            {
                m_Map.associate(endC, vector<StreetSegment>());
            }

            StreetSegment s(beginC, endC, streetName);
            vector<StreetSegment>* AssocVector = m_Map.find(beginC);
            AssocVector->push_back(s);

            StreetSegment sReverse(endC, beginC, streetName);
            AssocVector = m_Map.find(endC);
            AssocVector->push_back(sReverse);

            nStreetSegmentsLines--;
            line.clear();
        }

    }
    
    return true;

  
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment>* AssocVector = m_Map.find(gc);
    if (AssocVector == nullptr)
    {
        return false;
    }
    segs.clear();
    vector<StreetSegment>::const_iterator myIT = AssocVector->begin();
    for (; myIT != AssocVector->end(); myIT++)
    {
        segs.push_back((*myIT));
    }
    
    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
