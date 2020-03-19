#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    string calcDir(double angle) const;
    PointToPointRouter m_Router;
    const StreetMap* m_SM;
    DeliveryOptimizer m_Optimizer;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
    : m_Router(sm), m_Optimizer(sm)
{
   m_SM = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    commands.clear();
    totalDistanceTravelled = 0;

    vector<DeliveryRequest> copy = deliveries;

    double oldD = 0;
    double newD = 0;

    m_Optimizer.optimizeDeliveryOrder(depot, copy, oldD, newD);


    vector<StreetSegment> segs; //streetsegs
    m_SM->getSegmentsThatStartWith(depot, segs);
    if (segs.empty() == true)
    {
        return BAD_COORD;
    }
    bool checkedDepot = false;
    list<StreetSegment> streetSegs; //Street Segs Traveled

    list<StreetSegment>::iterator First;
    list<StreetSegment>::iterator Second;

    for (int i = 0; i < copy.size(); i++)
    {
        streetSegs.clear();
        double distTraveledperDest = -1;
        int result;

        if (checkedDepot == false)
        {
            result = m_Router.generatePointToPointRoute(depot, copy[0].location, streetSegs, distTraveledperDest); //no route to be found
            i--;
            checkedDepot = true;
        }
        else if (i == copy.size() - 1)
        {
            result = m_Router.generatePointToPointRoute(copy[i].location, depot, streetSegs, distTraveledperDest);
        }
        else
        {
            result = m_Router.generatePointToPointRoute(copy[i].location, copy[i + 1].location, streetSegs, distTraveledperDest);
        }
        if (result == BAD_COORD)
        {
            return BAD_COORD;
        }
        if (distTraveledperDest == -1)
        {
            return NO_ROUTE;
        }
        totalDistanceTravelled += distTraveledperDest;
        if (distTraveledperDest == 0 && (i < copy.size() - 1))
        {
            DeliveryCommand newD;
            newD.initAsDeliverCommand(copy[i + 1].item);
            commands.push_back(newD);
        }
        else if (distTraveledperDest == 0 && (i == copy.size() - 1))
        {
            continue;
        }
        else
        {
            First = streetSegs.begin();
            Second = streetSegs.begin();
            while (Second != streetSegs.end())
            {
                int streetSegsInARow = 1;
                Second++;
                while (Second != streetSegs.end() && (*Second).name == (*First).name)
                {
                    streetSegsInARow++;
                    First++;
                    Second++;
                }
                if (streetSegsInARow == 1)
                {
                    double angle = angleOfLine((*First));
                    string dir = calcDir(angle);
                    DeliveryCommand newD;
                    double dist = distanceEarthMiles((*First).start, (*First).end);
                    newD.initAsProceedCommand(dir, (*First).name, dist);
                    commands.push_back(newD);
                }
                else
                {
                    int numToSub = streetSegsInARow - 1;
                    while (numToSub > 0)
                    {
                        First--;
                        numToSub--;
                    }
                    Second--;
                    list<StreetSegment>::iterator temp = First;
                    double angle = angleOfLine((*First));
                    string dir = calcDir(angle);
                    DeliveryCommand newD;

                    double dist = 0;
                    while (temp != Second)
                    {
                        dist += distanceEarthMiles((*temp).start, (*temp).end);
                        temp++;
                    }
                    dist += distanceEarthMiles((*temp).start, (*temp).end); //here temp is equal to second which points to the last streetseg in a row
                    newD.initAsProceedCommand(dir, (*First).name, dist);
                    commands.push_back(newD);
                    Second++; //now points to either end or next street like first case
                }
                if (Second == streetSegs.end())
                {
                    if (i == copy.size() - 1)
                    {
                        continue;
                    }
                    DeliveryCommand newD;
                    newD.initAsDeliverCommand(copy[i + 1].item);
                    commands.push_back(newD);
                }
                else
                {
                    First = Second;
                    First--; //points to street before
                    double turnAngle = angleBetween2Lines((*First), (*Second));
                    if (turnAngle < 1 || turnAngle > 359)
                    {
                        First++;
                        continue;
                    }
                    else if (turnAngle >= 1 && turnAngle < 180)
                    {
                        DeliveryCommand newD;
                        newD.initAsTurnCommand("left", (*Second).name);
                        commands.push_back(newD);
                        First++;
                    }
                    else
                    {
                        DeliveryCommand newD;
                        newD.initAsTurnCommand("right", (*Second).name);
                        commands.push_back(newD);
                        First++;
                    }
                }
            }
        }
    }



    return DELIVERY_SUCCESS;
}
string DeliveryPlannerImpl::calcDir(double angle) const
{
    if (angle >= 0 && angle < 22.5)
    {
        return "east";
    }
    if (angle >= 22.5 && angle < 67.5)
    {
        return "northeast";
    }
    if (angle >= 67.5 && angle < 112.5)
    {
        return "north";
    }
    if (angle >= 112.5 && angle < 157.5)
    {
        return "northwest";
    }
    if (angle >= 157.5 && angle < 202.5)
    {
        return "west";
    }
    if (angle >= 202.5 && angle < 247.5)
    {
        return "southwest";
    }
    if (angle >= 247.5 && angle < 292.5)
    {
        return "south";
    }
    if (angle >= 292.5 && angle < 337.5)
    {
        return "southeast";
    }
    if (angle >= 337.5)
    {
        return "east";
    }
    return "east";
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
