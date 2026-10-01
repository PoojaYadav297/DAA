#include "branchAndBound.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

using namespace std;


// ============================================================
// CONSTRUCTOR
// ============================================================

TravelPlanner::TravelPlanner(
    const vector<Place>& p,
    const vector<vector<double>>& t,
    const vector<vector<double>>& d,
    double availableTime,
    int startingPlace,
    int endingPlace)
{
    places = p;

    travelTime = t;

    distanceMatrix = d;

    maxTime = availableTime;

    start = startingPlace;

    finalLocation = endingPlace;

    bestValue = -1;

    bestTotalTime = 0.0;

    bestTravelTime = 0.0;

    bestExploreTime = 0.0;

    bestDistance = 0.0;

    nodesExplored = 0;

    nodesPruned = 0;
}


// ============================================================
// UPPER BOUND
// ============================================================

double TravelPlanner::calculateBound(
    const vector<bool>& visited,
    double currentTime,
    int currentValue)
{
    double remainingTime =
        maxTime - currentTime;


    if (remainingTime <= 0)
    {
        return currentValue;
    }


    // --------------------------------------------------------
    // Store unvisited places
    // --------------------------------------------------------

    vector<pair<double, int>> candidates;


    for (int i = 1;
         i < finalLocation;
         i++)
    {
        if (!visited[i])
        {
            if (places[i].exploreTime > 0)
            {
                double ratio =
                    (double)places[i].value /
                    places[i].exploreTime;


                candidates.push_back(
                    {ratio, i}
                );
            }
        }
    }


    // --------------------------------------------------------
    // Sort by value / exploration time
    // --------------------------------------------------------

    sort(
        candidates.begin(),
        candidates.end(),

        [](const pair<double, int>& a,
           const pair<double, int>& b)
        {
            return a.first > b.first;
        }
    );


    // --------------------------------------------------------
    // Calculate optimistic bound
    // --------------------------------------------------------

    double boundValue =
        currentValue;


    double remaining =
        remainingTime;


    for (auto candidate : candidates)
    {
        int index =
            candidate.second;


        if (places[index].exploreTime <= remaining)
        {
            remaining -=
                places[index].exploreTime;


            boundValue +=
                places[index].value;
        }
        else
        {
            // Fractional value is used only
            // for calculating the optimistic bound.

            boundValue +=
                candidate.first *
                remaining;


            break;
        }
    }


    return boundValue;
}


// ============================================================
// BRANCH AND BOUND
// ============================================================

void TravelPlanner::branchAndBound(
    int current,
    vector<bool>& visited,
    vector<int>& currentRoute,
    double currentTime,
    double currentTravelTime,
    double currentExploreTime,
    double currentDistance,
    int currentValue)
{
    nodesExplored++;


    // --------------------------------------------------------
    // Calculate optimistic upper bound
    // --------------------------------------------------------

    double bound =
        calculateBound(
            visited,
            currentTime,
            currentValue
        );


    // --------------------------------------------------------
    // PRUNING
    // --------------------------------------------------------

    if (bound <= bestValue)
    {
        nodesPruned++;

        return;
    }


    // --------------------------------------------------------
    // Check whether current route can reach final location
    // --------------------------------------------------------

    double returnTime =
        travelTime[current][finalLocation];


    double returnDistance =
        distanceMatrix[current][finalLocation];


    double totalTimeWithReturn =
        currentTime + returnTime;


    // --------------------------------------------------------
    // Valid complete route
    // --------------------------------------------------------

    if (totalTimeWithReturn <= maxTime)
    {
        if (currentValue > bestValue)
        {
            bestValue =
                currentValue;


            bestTotalTime =
                totalTimeWithReturn;


            bestTravelTime =
                currentTravelTime +
                returnTime;


            bestExploreTime =
                currentExploreTime;


            bestDistance =
                currentDistance +
                returnDistance;


            bestRoute =
                currentRoute;
        }
    }


    // --------------------------------------------------------
    // Try every unvisited tourist place
    // --------------------------------------------------------

    for (int next = 1;
         next < finalLocation;
         next++)
    {
        if (visited[next])
        {
            continue;
        }


        // ----------------------------------------------------
        // Travel time to next place
        // ----------------------------------------------------

        double moveTime =
            travelTime[current][next];


        // ----------------------------------------------------
        // Distance to next place
        // ----------------------------------------------------

        double moveDistance =
            distanceMatrix[current][next];


        // ----------------------------------------------------
        // New time after visiting next place
        // ----------------------------------------------------

        double newTime =
            currentTime
            + moveTime
            + places[next].exploreTime;


        // ----------------------------------------------------
        // Time limit exceeded
        // ----------------------------------------------------

        if (newTime > maxTime)
        {
            nodesPruned++;

            continue;
        }


        // ----------------------------------------------------
        // Choose next place
        // ----------------------------------------------------

        visited[next] = true;

        currentRoute.push_back(next);


        // ----------------------------------------------------
        // Recursive call
        // ----------------------------------------------------

        branchAndBound(
            next,

            visited,

            currentRoute,

            newTime,

            currentTravelTime +
                moveTime,

            currentExploreTime +
                places[next].exploreTime,

            currentDistance +
                moveDistance,

            currentValue +
                places[next].value
        );


        // ----------------------------------------------------
        // BACKTRACK
        // ----------------------------------------------------

        currentRoute.pop_back();

        visited[next] = false;
    }
}


// ============================================================
// SOLVE
// ============================================================

void TravelPlanner::solve()
{
    vector<bool> visited(
        places.size(),
        false
    );


    vector<int> currentRoute;


    // Starting location is already visited
    visited[start] = true;


    // Final location is not a tourist destination
    visited[finalLocation] = true;


    // Start Branch & Bound
    branchAndBound(
        start,

        visited,

        currentRoute,

        0.0,

        0.0,

        0.0,

        0.0,

        0
    );
}


// ============================================================
// GET BEST ROUTE
// ============================================================

vector<int> TravelPlanner::getBestRoute() const
{
    return bestRoute;
}


// ============================================================
// GET SKIPPED PLACES
// ============================================================

vector<int> TravelPlanner::getSkippedPlaces() const
{
    vector<bool> selected(
        places.size(),
        false
    );


    for (int index : bestRoute)
    {
        selected[index] = true;
    }


    vector<int> skipped;


    for (int i = 1;
         i < finalLocation;
         i++)
    {
        if (!selected[i])
        {
            skipped.push_back(i);
        }
    }


    return skipped;
}


// ============================================================
// GETTERS
// ============================================================

int TravelPlanner::getBestValue() const
{
    return bestValue;
}


double TravelPlanner::getBestTotalTime() const
{
    return bestTotalTime;
}


double TravelPlanner::getBestTravelTime() const
{
    return bestTravelTime;
}


double TravelPlanner::getBestExploreTime() const
{
    return bestExploreTime;
}


double TravelPlanner::getBestDistance() const
{
    return bestDistance;
}


double TravelPlanner::getRemainingTime() const
{
    return maxTime - bestTotalTime;
}


long long TravelPlanner::getNodesExplored() const
{
    return nodesExplored;
}


long long TravelPlanner::getNodesPruned() const
{
    return nodesPruned;
}


// ============================================================
// DISPLAY RESULT
// ============================================================

void TravelPlanner::displayResult() const
{
    cout << "\n";

    cout << "============================================================\n";
    cout << "                  YOUR TRAVEL PLAN\n";
    cout << "============================================================\n";


    if (bestValue < 0)
    {
        cout << "\nNo feasible travel plan was found.\n";

        return;
    }


    cout << "\nStarting Location:\n";

    cout << "  "
         << places[start].name
         << "\n";


    // --------------------------------------------------------
    // Recommended places
    // --------------------------------------------------------

    cout << "\nRecommended Places:\n";

    cout << "------------------------------------------------------------\n";


    if (bestRoute.empty())
    {
        cout << "  No tourist place selected.\n";
    }
    else
    {
        for (int i = 0;
             i < bestRoute.size();
             i++)
        {
            int index =
                bestRoute[i];


            cout << "  "
                 << i + 1
                 << ". "
                 << places[index].name

                 << " | Explore: "
                 << fixed
                 << setprecision(2)
                 << places[index].exploreTime
                 << " hrs"

                 << " | Value: "
                 << places[index].value

                 << "\n";
        }
    }


    cout << "------------------------------------------------------------\n";


    // --------------------------------------------------------
    // Skipped places
    // --------------------------------------------------------

    cout << "\nSkipped Places:\n";


    vector<int> skipped =
        getSkippedPlaces();


    if (skipped.empty())
    {
        cout << "  None\n";
    }
    else
    {
        for (int index : skipped)
        {
            cout << "  "
                 << places[index].name
                 << "\n";
        }
    }


    // --------------------------------------------------------
    // Route
    // --------------------------------------------------------

    cout << "\nRecommended Route:\n";

    cout << "------------------------------------------------------------\n";


    cout << places[start].name;


    for (int index : bestRoute)
    {
        cout << " -> "
             << places[index].name;
    }


    cout << " -> "
         << places[finalLocation].name;


    cout << "\n";


    // --------------------------------------------------------
    // Trip summary
    // --------------------------------------------------------

    cout << "\nTrip Summary\n";

    cout << "------------------------------------------------------------\n";


    cout << fixed
         << setprecision(2);


    cout << left
         << setw(25)
         << "Available Time"
         << ": "
         << maxTime
         << " hours\n";


    cout << left
         << setw(25)
         << "Travel Time"
         << ": "
         << bestTravelTime
         << " hours\n";


    cout << left
         << setw(25)
         << "Exploration Time"
         << ": "
         << bestExploreTime
         << " hours\n";


    cout << left
         << setw(25)
         << "Total Time Used"
         << ": "
         << bestTotalTime
         << " hours\n";


    cout << left
         << setw(25)
         << "Remaining Time"
         << ": "
         << getRemainingTime()
         << " hours\n";


    cout << left
         << setw(25)
         << "Total Distance"
         << ": "
         << bestDistance
         << " km\n";


    cout << left
         << setw(25)
         << "Total Place Value"
         << ": "
         << bestValue
         << "\n";


    cout << left
         << setw(25)
         << "Places Selected"
         << ": "
         << bestRoute.size()
         << "\n";


    cout << "------------------------------------------------------------\n";
}


// ============================================================
// DISPLAY ALGORITHM INFORMATION
// ============================================================

void TravelPlanner::displayAlgorithmInfo() const
{
    cout << "\n";

    cout << "============================================================\n";
    cout << "             BRANCH & BOUND ANALYSIS\n";
    cout << "============================================================\n";


    cout << "\nNodes Explored : "
         << nodesExplored
         << "\n";


    cout << "Nodes Pruned   : "
         << nodesPruned
         << "\n";


    cout << "\n1. BRANCHING\n";

    cout << "   The algorithm tries different tourist places\n";
    cout << "   as the next destination.\n";


    cout << "\n2. BOUNDING\n";

    cout << "   An optimistic upper bound estimates the maximum\n";
    cout << "   possible value of a partial route.\n";


    cout << "\n3. PRUNING\n";

    cout << "   A branch is discarded when it cannot improve\n";
    cout << "   the current best solution.\n";


    cout << "\n4. TIME CONSTRAINT\n";

    cout << "   Every selected route must have enough time to\n";
    cout << "   reach the final destination.\n";


    cout << "\n5. BEST SOLUTION\n";

    cout << "   The feasible route with the highest total value\n";
    cout << "   is selected.\n";
}
