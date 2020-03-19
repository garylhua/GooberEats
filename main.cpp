#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v);
bool parseDelivery(string line, string& lat, string& lon, string& item);

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " mapdata.txt deliveries.txt" << endl;
        return 1;
    }

    StreetMap sm;

    if (!sm.load(argv[1]))
    {
        cout << "Unable to load map data file " << argv[1] << endl;
        return 1;
    }

    GeoCoord depot;
    vector<DeliveryRequest> deliveries;
    if (!loadDeliveryRequests(argv[2], depot, deliveries))
    {
        cout << "Unable to load delivery request file " << argv[2] << endl;
        return 1;
    }

    cout << "Generating route...\n\n";

    DeliveryPlanner dp(&sm);
    vector<DeliveryCommand> dcs;
    double totalMiles;
    DeliveryResult result = dp.generateDeliveryPlan(depot, deliveries, dcs, totalMiles);
    if (dcs.empty() == true)
    {
        cout << "Commands are empty" << endl;
    }
    if (result == BAD_COORD)
    {
        cout << "One or more depot or delivery coordinates are invalid." << endl;
        return 1;
    }
    if (result == NO_ROUTE)
    {
        cout << "No route can be found to deliver all items." << endl;
        return 1;
    }
    cout << "Starting at the depot...\n";
    for (const auto& dc : dcs)
        cout << dc.description() << endl;
    cout << "You are back at the depot and your deliveries are done!\n";
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << totalMiles << " miles travelled for all deliveries." << endl;
}

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v)
{
    ifstream inf(deliveriesFile);
    if (!inf)
        return false;
    string lat;
    string lon;
    inf >> lat >> lon;
    inf.ignore(10000, '\n');
    depot = GeoCoord(lat, lon);
    string line;
    while (getline(inf, line))
    {
        string item;
        if (parseDelivery(line, lat, lon, item))
            v.push_back(DeliveryRequest(item, GeoCoord(lat, lon)));
    }
    return true;
}

bool parseDelivery(string line, string& lat, string& lon, string& item)
{
    const size_t colon = line.find(':');
    if (colon == string::npos)
    {
        cout << "Missing colon in deliveries file line: " << line << endl;
        return false;
    }
    istringstream iss(line.substr(0, colon));
    if (!(iss >> lat >> lon))
    {
        cout << "Bad format in deliveries file line: " << line << endl;
        return false;
    }
    item = line.substr(colon + 1);
    if (item.empty())
    {
        cout << "Missing item in deliveries file line: " << line << endl;
        return false;
    }
    return true;
}

//#include "ExpandableHashMap.h"
//#include "provided.h"
//#include <iostream>
//#include <string>
//using namespace std;
//
//unsigned int hasher(const unsigned int& k)
//{
//	return k;
//}
//
//int main() {
//	StreetMap test;
//	test.load("mapdata.txt");
//	vector<StreetSegment> segsss;
//	cout << "Fill Success" << endl;
//	GeoCoord g("34.0853742", "-118.4955413");
//	test.getSegmentsThatStartWith(g, segsss);
//	for (unsigned int i = 0; i < segsss.size(); i++)
//	{
//		cout << segsss[i].start.latitude << " ";
//		cout << segsss[i].start.longitude << " ";
//		cout << segsss[i].end.latitude << " ";
//		cout << segsss[i].end.longitude << " ";
//		cout << segsss[i].name << endl;
//	}
//	
//	PointToPointRouter letsgo(&test);
//	list<StreetSegment> route;
//	GeoCoord begin("34.0463922", "-118.4953255");
//	GeoCoord end("34.0858873", "-118.4955762");
//	double dist = 0;
//	letsgo.generatePointToPointRoute(begin, end, route, dist);
//	list<StreetSegment>::iterator myIT;
//	for (myIT = route.begin(); myIT != route.end(); myIT++)
//	{
//		cout << (*myIT).start.latitude << " ";
//		cout << (*myIT).start.longitude << " ";
//		cout << (*myIT).end.latitude << " ";
//		cout << (*myIT).end.longitude << " ";
//		cout << (*myIT).name << endl;
//	}
//	/*cout << dist;*/
//
//	DeliveryPlanner newPlan(&test);
//	GeoCoord depot("34.0625329", "-118.4470263");
//	GeoCoord Sproul("34.0712323", "-118.4505969");
//	GeoCoord Eng("34.0687443", "-118.4449195");
//	GeoCoord Beta("34.0685657", "-118.4489289");
//	DeliveryRequest one("Chicken Tenders", Sproul);
//	DeliveryRequest two("B-Plate Salmon", Eng);
//	DeliveryRequest three("Beer", Beta);
//	DeliveryRequest four("Toast", GeoCoord("34.0666168", "-118.4395786"));
//	DeliveryRequest five("Sandwich", GeoCoord("34.0706349", "-118.4492679"));
//	DeliveryRequest six("Steak", GeoCoord("34.0683189", "-118.4536522"));
//	DeliveryRequest seven("Chinese Food", GeoCoord("34.0636860", "-118.4453568"));
//	DeliveryRequest eight("Ice Cream", GeoCoord("34.0906704", "-118.4159599"));
//	DeliveryRequest nine("yogurt", GeoCoord("34.0540805", "-118.4564459"));
//	DeliveryRequest ten("thai", GeoCoord("34.0904052", "-118.4113822"));
//	DeliveryRequest eleven("thaisecond", GeoCoord("34.0904052", "-118.4113822"));
//
//	vector<DeliveryRequest> myDel;
//	myDel.push_back(one);
//	myDel.push_back(two);
//	myDel.push_back(three);
//	myDel.push_back(four);
//	myDel.push_back(five);
//	myDel.push_back(six);
//	myDel.push_back(seven);
//	myDel.push_back(eight);
//	myDel.push_back(nine);
//	myDel.push_back(ten);
//	myDel.push_back(eleven);
//	vector<DeliveryCommand> instruct;
//	double totalD = 0;
//	DeliveryResult result = newPlan.generateDeliveryPlan(depot, myDel, instruct, totalD);
//	for (int i = 0; i < instruct.size(); i++)
//	{
//		cout << instruct[i].description();
//		cout << endl;
//	}
//	cout << totalD;
//	DeliveryOptimizer myOpt(&test);
//	double oldD = 0;
//	double  newD = 0;
//	myOpt.optimizeDeliveryOrder(depot, myDel, oldD, newD);
//	cout << oldD << endl;
//	cout << newD;
//
//
//
//
//
//
//	//hi.associate(8, "Hi");
//	//hi.associate(7, "Bye");
//	//hi.associate(6, "Nigger");
//	//hi.associate(5, "Teddy");
//	//hi.associate(4, "HNG");
//	//hi.associate(5, "Gary");
//	//hi.associate(24, "Duh");
//	//hi.associate(40, "FUCK YES");
//	//hi.associate(50, "Dung");
//	//hi.associate(51, "BUG");
//	//hi.reset();
//	//cout << hi.size() << endl;
//	//const string* test = hi.find(40);
//	//if (test != nullptr)
//	//{
//	//	cout << *test;
//	//}
//}