#ifndef BRANCH_AND_BOUND_H
#define BRANCH_AND_BOUND_H

#include <vector>
#include <string>

using namespace std;

struct Place
{
    string name;
    int exploreTime;
    int value;
};

class TravelPlanner
{
private:
    vector<Place> places;
    vector<vector<int>> travelTime;
    int maxTime;
    int start;

    int bestValue;
    int bestTotalTime;
    int bestTravelTime;
    int bestExploreTime;

    vector<int> bestRoute;

    long long nodesExplored;
    long long nodesPruned;

    int calculateBound(
        const vector<bool>& visited,
        int currentTime,
        int currentValue
    );

    void branchAndBound(
        int current,
        vector<bool>& visited,
        vector<int>& currentRoute,
        int currentTime,
        int currentTravelTime,
        int currentExploreTime,
        int currentValue
    );

public:
    TravelPlanner(
        const vector<Place>& p,
        const vector<vector<int>>& t,
        int maximumTime,
        int startingPlace
    );

    void solve();

    void displayResult();

    void displayAlgorithmInfo();

    void displaySearchConcept();

    int getBestValue();

    int getBestTime();

    vector<int> getBestRoute();
};

void displayPlaces(
    const vector<Place>& places
);

void displayTravelMatrix(
    const vector<Place>& places,
    const vector<vector<int>>& travelTime
);

void createDemoData(
    vector<Place>& places,
    vector<vector<int>>& travelTime
);

void createCustomData(
    vector<Place>& places,
    vector<vector<int>>& travelTime
);

#endif
