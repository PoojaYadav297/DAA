#ifndef BRANCH_AND_BOUND_H
#define BRANCH_AND_BOUND_H

#include "common.h"
#include <vector>
#include <string>

using namespace std;

class TravelPlanner
{
private:
    // Input data
    vector<Place> places;

    // Travel time between locations
    vector<vector<double>> travelTime;

    // Distance between locations
    vector<vector<double>> distanceMatrix;

    // Trip constraints
    double maxTime;

    // Starting and final locations
    int start;
    int finalLocation;

    // Best solution found
    int bestValue;
    double bestTotalTime;
    double bestTravelTime;
    double bestExploreTime;
    double bestDistance;

    // Best route
    vector<int> bestRoute;

    // Branch and Bound statistics
    long long nodesExplored;
    long long nodesPruned;

    // Calculate upper bound
    double calculateBound(
        int current,
        double currentTime,
        int currentValue,
        const vector<bool>& visited
    );

    // Main Branch and Bound function
    void branchAndBound(
        int current,
        double currentTime,
        double currentTravelTime,
        double currentExploreTime,
        double currentDistance,
        int currentValue,
        vector<bool>& visited,
        vector<int>& currentRoute
    );

public:

    // Constructor
    TravelPlanner(
        const vector<Place>& p,
        const vector<vector<double>>& t,
        const vector<vector<double>>& d,
        double availableTime,
        int startingPlace,
        int endingPlace
    );

    // Start Branch and Bound
    void solve();

    // Display final result
    void displayResult() const;

    // Display algorithm information
    void displayAlgorithmInfo() const;

    // Get best route
    vector<int> getBestRoute() const;

    // Get skipped places
    vector<int> getSkippedPlaces() const;

    // Get best value
    int getBestValue() const;

    // Get total time
    double getBestTotalTime() const;

    // Get travel time
    double getBestTravelTime() const;

    // Get exploration time
    double getBestExploreTime() const;

    // Get total distance
    double getBestDistance() const;

    // Get number of explored nodes
    long long getNodesExplored() const;

    // Get number of pruned nodes
    long long getNodesPruned() const;
};

#endif
