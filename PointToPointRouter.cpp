#include "provided.h"
#include <algorithm>
#include <list>
using namespace std;



class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    struct GeoCoordPair
    {
        GeoCoordPair(GeoCoord i, GeoCoordPair* p, double fi) { identity = i; parent = p; f = fi; }
        bool operator==(const GeoCoordPair& other) { return this->identity == other.identity; }
        GeoCoord identity;
        GeoCoordPair* parent;
        double f, g, h;
        StreetSegment connect;
    };

    const StreetMap* m_SM;

    GeoCoordPair findSmallestF(list<GeoCoordPair>& open, list<GeoCoordPair>::iterator& myIT) const;


};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_SM = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    list<GeoCoordPair> open;
    list<GeoCoordPair> closed;
    bool foundRoute = false;

    //A* BAABY
    if (start == end)
    {
        route.clear();
        totalDistanceTravelled = 0;
        return DELIVERY_SUCCESS;
    }

    //check start has some associated streetsegments
    vector<StreetSegment> currSS;
    m_SM->getSegmentsThatStartWith(end, currSS);
    if (currSS.empty() == true)
    {
        return BAD_COORD;
    }

   m_SM->getSegmentsThatStartWith(start, currSS);
   if (currSS.empty() == true)
   {
       return BAD_COORD;
   }

   GeoCoordPair s(start, nullptr, 0);
   s.g = 0;
   open.push_back(s);


   while (open.empty() == false) //start A* analysis
   {
       list<GeoCoordPair>::iterator myIT = open.begin();
       GeoCoordPair curr = findSmallestF(open , myIT); //pops and sets the geocoordpair with smallest f to curr
       if (curr.identity == end) //we found it
       {
           foundRoute = true;
           closed.push_back(curr);
           break;
       }
       m_SM->getSegmentsThatStartWith(curr.identity, currSS);
       for (int i = 0; i < currSS.size(); i++)
       {
           if (i == 0)
           {
               closed.push_back(curr);
           }

           list<GeoCoordPair>::iterator findIT = find(closed.begin(), closed.end(), GeoCoordPair(currSS[i].end, &s, 0)); //curr can be replacable here
           if (findIT != closed.end())
           {
               continue;
           }
           if (curr.identity == start)
           {
               findIT = find(closed.begin(), closed.end(), curr);
               double gn = distanceEarthMiles(curr.identity, currSS[i].end);
               double hn = distanceEarthMiles(end, currSS[i].end);
               double f = gn + hn;
               GeoCoordPair newGeo(currSS[i].end, &(*findIT), f);
               newGeo.g = gn;
               newGeo.h = hn;
               newGeo.connect = currSS[i];
               open.push_back(newGeo);
               continue;
           }


           findIT = find(closed.begin(), closed.end(), curr);
           double gn = distanceEarthMiles(curr.identity, currSS[i].end) + curr.g;
           double hn = distanceEarthMiles(end, currSS[i].end);
           double f = gn + hn;
           GeoCoordPair newGeo(currSS[i].end, &(*findIT), f); //new, parent, new f
           newGeo.g = gn;
           newGeo.h = hn;
           newGeo.connect = currSS[i];



           findIT = find(open.begin(), open.end(), newGeo);
           if (findIT != open.end())
           {
               if (newGeo.f < (*findIT).f)
               {
                   (*findIT) = newGeo;
                   continue;
               }
               else
               {
                   continue;
               }
           }

           open.push_back(newGeo);



           //calc f
           //check if in open and compare f's
           //potentially change the f's in open
           //set parent to current
           //if not in open, add to open
       }


   }
   if (foundRoute == true)
   {
       GeoCoordPair* currGeo = &closed.back(); //starts at the end point pair
       route.clear();
       totalDistanceTravelled = currGeo->g;
       for (; currGeo->parent != nullptr; currGeo = currGeo->parent)
       {
           route.push_back(currGeo->connect);
       }

       route.reverse();
       return DELIVERY_SUCCESS;

   }
   return NO_ROUTE;
    

}


PointToPointRouterImpl::GeoCoordPair PointToPointRouterImpl::findSmallestF(list<GeoCoordPair>& open, list<GeoCoordPair>::iterator& myIT) const
{
    if (open.size() == 1)
    {
        GeoCoordPair r = (*myIT);
        myIT = open.erase(myIT);
        return r;
    }

    int indexSmallest = 0;
    double smallestf = (*myIT).f;
    myIT++;
    int count = 1;
    for (; myIT != open.end(); myIT++)
    {
        if ((*myIT).f < smallestf)
        {
            indexSmallest = count;
            smallestf = (*myIT).f;
            count++;
            continue;
        }
        count++;
    }

    myIT = open.begin();
    
    for (int i = 0; i <= indexSmallest; i++)
    {
        if (i == indexSmallest)
        {
            GeoCoordPair r = (*myIT);
            myIT = open.erase(myIT);
            return r;
        }
        myIT++;
    }

    GeoCoordPair r = (*myIT);
    return r;

}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
