#include "provided.h"
#include <vector>
#include <math.h>
#include <random>
#include <utility> 
using namespace std;

inline
int randInt(int min, int max)
{
    if (max < min)
        std::swap(max, min);
    static std::random_device rd;
    static std::default_random_engine generator(rd());
    std::uniform_int_distribution<> distro(min, max);
    return distro(generator);
}

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    const StreetMap* m_SM;
    double acceptanceProb(double oLength, double nLength, double temp) const;
    double calcTotalDist(const GeoCoord& depot, vector<DeliveryRequest>& deliveries) const;
    void swap(DeliveryRequest& a, DeliveryRequest& b) const;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_SM = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    //calc the old crow distance
    oldCrowDistance = 0;
    oldCrowDistance = calcTotalDist(depot, deliveries);



   /* newCrowDistance = oldCrowDistance;*/
    //finish if time
    //SIMULATED ANNEALING BABY
    double temp = 50;
    double coolingrate = 0.997;
    double currDist = oldCrowDistance;
    while (temp > 0.01)
    {


        int pos1 = randInt(0, deliveries.size() - 1);
        int pos2 = randInt(0, deliveries.size() - 1);

        
        swap(deliveries[pos1], deliveries[pos2]);
        double newLength = calcTotalDist(depot, deliveries);
        double probability = acceptanceProb(currDist, newLength, temp);
        probability *= 100;
        if (probability >= (randInt(1, 100) * 1.0))
        {
            currDist = newLength;
        }
        else
        {
            swap(deliveries[pos1], deliveries[pos2]);
        }

        temp *= coolingrate;



    }
    newCrowDistance = currDist;

}

double DeliveryOptimizerImpl::acceptanceProb(double oLength, double nLength, double temp) const
{
    if (nLength < oLength)
    {
        return 1.0;
    }
    else
    {
        return exp((oLength - nLength) / temp);
    }
}
double DeliveryOptimizerImpl::calcTotalDist(const GeoCoord& depot, vector<DeliveryRequest>& deliveries) const
{
    double dist = 0;
    dist += distanceEarthMiles(depot, deliveries[0].location);
    for (int i = 0; i < deliveries.size() - 1; i++)
    {
        dist += distanceEarthMiles(deliveries[i].location, deliveries[i + 1].location);
    }
    dist += distanceEarthMiles(deliveries[deliveries.size() - 1].location, depot);
    return dist;

}
void DeliveryOptimizerImpl::swap(DeliveryRequest& a, DeliveryRequest& b) const
{
    DeliveryRequest temp = a;
    a = b;
    b = temp;
}


//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
