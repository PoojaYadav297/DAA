// ============================================================
//              SMART TRAVEL PLANNER
//                 BRANCH & BOUND
// ============================================================
//
// DAA PROJECT
//
// Main Goal:
// Find the best set and order of tourist places that a traveler
// can visit within a limited amount of time.
//
// The algorithm uses Branch and Bound.
//
// Data received from backend:
//     - Starting location
//     - Final location
//     - Available time
//     - Tourist places
//     - Exploration time
//     - Value of each place
//     - Travel time matrix from GPS/Routing API
//     - Distance matrix from GPS/Routing API
//
// Branch and Bound internally:
//     1. Creates possible routes
//     2. Calculates current route value
//     3. Calculates optimistic upper bound
//     4. Prunes routes that cannot beat the current best route
//
// Final output:
//     - Recommended places
//     - Visit order
//     - Skipped places
//     - Total distance
//     - Travel time
//     - Exploration time
//     - Total time
//     - Remaining time
//     - Total value
//
// ============================================================

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;


// ============================================================
// 1. PLACE STRUCTURE
// ============================================================

struct Place
{
    string name;

    // Time needed to explore the place
    double exploreTime;

    // Importance / attraction value
    int value;
};


// ============================================================
// 2. BRANCH AND BOUND CLASS
// ============================================================

class TravelPlanner
{
private:

    // --------------------------------------------------------
    // All locations
    //
    // Index 0:
    // Starting location
    //
    // Index 1 to n:
    // Tourist places
    //
    // Last index:
    // Final location
    // --------------------------------------------------------

    vector<Place> places;


    // --------------------------------------------------------
    // Travel time matrix
    //
    // Provided by GPS / Routing API
    //
    // travelTime[i][j] =
    // travel duration from i to j in hours
    // --------------------------------------------------------

    vector<vector<double>> travelTime;


    // --------------------------------------------------------
    // Distance matrix
    //
    // Provided by GPS / Routing API
    //
    // distance[i][j] =
    // road distance from i to j in km
    // --------------------------------------------------------

    vector<vector<double>> distanceMatrix;


    // Maximum time available
    double maxTime;


    // Starting location index
    int start;


    // Final location index
    int finalLocation;


    // --------------------------------------------------------
    // Best solution found
    // --------------------------------------------------------

    int bestValue;

    double bestTotalTime;

    double bestTravelTime;

    double bestExploreTime;

    double bestDistance;

    vector<int> bestRoute;


    // --------------------------------------------------------
    // Branch & Bound statistics
    // --------------------------------------------------------

    long long nodesExplored;

    long long nodesPruned;


    // ========================================================
    // UPPER BOUND
    // ========================================================

    double calculateBound(
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


        // ----------------------------------------------------
        // Store unvisited places with their
        // value / exploration-time ratio
        // ----------------------------------------------------

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


        // ----------------------------------------------------
        // Highest value/time ratio first
        // ----------------------------------------------------

        sort(
            candidates.begin(),
            candidates.end(),

            [](const pair<double, int>& a,
               const pair<double, int>& b)
            {
                return a.first > b.first;
            }
        );


        // ----------------------------------------------------
        // Calculate optimistic bound
        // ----------------------------------------------------

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
                // Fractional value is allowed only
                // for calculating the optimistic bound.

                boundValue +=
                    candidate.first *
                    remaining;


                break;
            }
        }


        return boundValue;
    }


    // ========================================================
    // BRANCH AND BOUND SEARCH
    // ========================================================

    void branchAndBound(
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


        // ----------------------------------------------------
        // Calculate optimistic upper bound
        // ----------------------------------------------------

        double bound =
            calculateBound(
                visited,
                currentTime,
                currentValue
            );


        // ----------------------------------------------------
        // PRUNING
        // ----------------------------------------------------

        if (bound <= bestValue)
        {
            nodesPruned++;

            return;
        }


        // ----------------------------------------------------
        // Travel from current location
        // to final location
        // ----------------------------------------------------

        double returnTime =
            travelTime[current][finalLocation];


        double returnDistance =
            distanceMatrix[current][finalLocation];


        // ----------------------------------------------------
        // Check whether current route can return
        // to final location within available time
        // ----------------------------------------------------

        double totalTimeWithReturn =
            currentTime + returnTime;


        if (totalTimeWithReturn <= maxTime)
        {
            /*
                This is a valid complete route.

                We include the travel from the current
                location to the final location.
            */

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


        // ----------------------------------------------------
        // Try every unvisited tourist place
        // ----------------------------------------------------

        for (int next = 1;
             next < finalLocation;
             next++)
        {
            // Already visited
            if (visited[next])
            {
                continue;
            }


            // ------------------------------------------------
            // Travel time to next place
            // ------------------------------------------------

            double moveTime =
                travelTime[current][next];


            // ------------------------------------------------
            // Distance to next place
            // ------------------------------------------------

            double moveDistance =
                distanceMatrix[current][next];


            // ------------------------------------------------
            // New total time
            //
            // Current time
            // + travel time
            // + exploration time
            // ------------------------------------------------

            double newTime =
                currentTime
                + moveTime
                + places[next].exploreTime;


            // ------------------------------------------------
            // If time limit is exceeded
            // prune this branch
            // ------------------------------------------------

            if (newTime > maxTime)
            {
                nodesPruned++;

                continue;
            }


            // ------------------------------------------------
            // Choose next place
            // ------------------------------------------------

            visited[next] =
                true;


            currentRoute.push_back(next);


            // ------------------------------------------------
            // Recursive Branch & Bound call
            // ------------------------------------------------

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


            // ------------------------------------------------
            // BACKTRACK
            // ------------------------------------------------

            currentRoute.pop_back();

            visited[next] =
                false;
        }
    }


public:

    // ========================================================
    // CONSTRUCTOR
    // ========================================================

    TravelPlanner(
        const vector<Place>& p,
        const vector<vector<double>>& t,
        const vector<vector<double>>& d,
        double availableTime,
        int startingPlace,
        int endingPlace)
    {
        places =
            p;

        travelTime =
            t;

        distanceMatrix =
            d;

        maxTime =
            availableTime;

        start =
            startingPlace;

        finalLocation =
            endingPlace;


        // ----------------------------------------------------
        // Initial best solution
        // ----------------------------------------------------

        bestValue =
            -1;

        bestTotalTime =
            0.0;

        bestTravelTime =
            0.0;

        bestExploreTime =
            0.0;

        bestDistance =
            0.0;


        bestRoute.clear();


        // ----------------------------------------------------
        // Statistics
        // ----------------------------------------------------

        nodesExplored =
            0;

        nodesPruned =
            0;
    }


    // ========================================================
    // SOLVE
    // ========================================================

    void solve()
    {
        vector<bool> visited(
            places.size(),
            false
        );


        vector<int> currentRoute;


        // ----------------------------------------------------
        // Start is already visited
        // ----------------------------------------------------

        visited[start] =
            true;


        // Final location is not a tourist place
        visited[finalLocation] =
            true;


        // ----------------------------------------------------
        // Start Branch & Bound
        // ----------------------------------------------------

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


    // ========================================================
    // GET SELECTED PLACES
    // ========================================================

    vector<int> getBestRoute() const
    {
        return bestRoute;
    }


    // ========================================================
    // GET SKIPPED PLACES
    // ========================================================

    vector<int> getSkippedPlaces() const
    {
        vector<bool> selected(
            places.size(),
            false
        );


        for (int index : bestRoute)
        {
            selected[index] =
                true;
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


    // ========================================================
    // GETTERS
    // ========================================================

    int getBestValue() const
    {
        return bestValue;
    }


    double getBestTotalTime() const
    {
        return bestTotalTime;
    }


    double getBestTravelTime() const
    {
        return bestTravelTime;
    }


    double getBestExploreTime() const
    {
        return bestExploreTime;
    }


    double getBestDistance() const
    {
        return bestDistance;
    }


    double getRemainingTime() const
    {
        return maxTime -
               bestTotalTime;
    }


    long long getNodesExplored() const
    {
        return nodesExplored;
    }


    long long getNodesPruned() const
    {
        return nodesPruned;
    }


    // ========================================================
    // DISPLAY RESULT
    // ========================================================

    void displayResult() const
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


        // ----------------------------------------------------
        // Start
        // ----------------------------------------------------

        cout << "\nStarting Location:\n";

        cout << "  "
             << places[start].name
             << "\n";


        // ----------------------------------------------------
        // Selected places
        // ----------------------------------------------------

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


        // ----------------------------------------------------
        // Skipped places
        // ----------------------------------------------------

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


        // ----------------------------------------------------
        // Route
        // ----------------------------------------------------

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


        // ----------------------------------------------------
        // Summary
        // ----------------------------------------------------

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


    // ========================================================
    // DISPLAY ALGORITHM INFORMATION
    // ========================================================

    void displayAlgorithmInfo() const
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

        cout << "   Every route must also have enough time to reach\n";

        cout << "   the final destination.\n";


        cout << "\n5. BEST SOLUTION\n";

        cout << "   The feasible route with the highest total value\n";

        cout << "   is selected.\n";
    }
};


// ============================================================
// 3. MAIN FUNCTION
// ============================================================
//
// IMPORTANT:
//
// In the final application, Node.js will send this information
// to C++:
//
//     Start location
//     Tourist places
//     Exploration times
//     Place values
//     Final location
//     Available time
//     GPS travel-time matrix
//     GPS distance matrix
//
// Therefore there are NO fixed tourist places here.
//
// ============================================================

int main()
{
    cout << "\n";

    cout << "============================================================\n";

    cout << "                 SMART TRAVEL PLANNER\n";

    cout << "                  BRANCH & BOUND\n";

    cout << "============================================================\n";


    // --------------------------------------------------------
    // Number of tourist places
    // --------------------------------------------------------

    int numberOfPlaces;


    cout << "\nEnter number of tourist places: ";

    cin >> numberOfPlaces;


    if (numberOfPlaces <= 0)
    {
        cout << "Invalid number of places.\n";

        return 0;
    }


    // --------------------------------------------------------
    // Total locations
    //
    // 0       = Start
    // 1..n    = Tourist places
    // n+1     = Final
    // --------------------------------------------------------

    int totalLocations =
        numberOfPlaces + 2;


    int finalLocation =
        numberOfPlaces + 1;


    vector<Place> places(
        totalLocations
    );


    // --------------------------------------------------------
    // Starting location
    // --------------------------------------------------------

    cout << "\nStarting location: ";

    cin >> ws;

    getline(
        cin,
        places[0].name
    );


    places[0].exploreTime =
        0.0;

    places[0].value =
        0;


    // --------------------------------------------------------
    // Tourist places
    // --------------------------------------------------------

    for (int i = 1;
         i <= numberOfPlaces;
         i++)
    {
        cout << "\nTourist Place "
             << i
             << "\n";


        cout << "Name: ";

        cin >> ws;

        getline(
            cin,
            places[i].name
        );


        cout << "Exploration time (hours): ";

        cin >> places[i].exploreTime;


        if (places[i].exploreTime <= 0)
        {
            cout << "Exploration time must be positive.\n";

            return 0;
        }


        cout << "Place value: ";

        cin >> places[i].value;


        if (places[i].value <= 0)
        {
            cout << "Place value must be positive.\n";

            return 0;
        }
    }


    // --------------------------------------------------------
    // Final location
    // --------------------------------------------------------

    cout << "\nFinal location: ";

    cin >> ws;

    getline(
        cin,
        places[finalLocation].name
    );


    places[finalLocation].exploreTime =
        0.0;

    places[finalLocation].value =
        0;


    // ========================================================
    // TRAVEL TIME MATRIX
    // ========================================================
    //
    // IMPORTANT:
    //
    // In the final website:
    //
    // GPS / ROUTING API
    //          ↓
    //     Node.js
    //          ↓
    //    C++ receives matrix
    //
    // This interactive input is only a temporary way to
    // test the C++ algorithm before Node.js is connected.
    //
    // ========================================================

    vector<vector<double>> travelTime(
        totalLocations,
        vector<double>(
            totalLocations,
            0.0
        )
    );


    cout << "\n";

    cout << "Travel-time data\n";

    cout << "Enter travel time in hours.\n";

    cout << "Example: 30 minutes = 0.50 hours.\n";


    for (int i = 0;
         i < totalLocations;
         i++)
    {
        for (int j = 0;
             j < totalLocations;
             j++)
        {
            if (i == j)
            {
                travelTime[i][j] =
                    0.0;

                continue;
            }


            cout << "\n"
                 << places[i].name
                 << " -> "
                 << places[j].name
                 << ": ";


            cin >> travelTime[i][j];


            if (travelTime[i][j] < 0)
            {
                cout << "Travel time cannot be negative.\n";

                return 0;
            }
        }
    }


    // ========================================================
    // DISTANCE MATRIX
    // ========================================================
    //
    // In the final application this will also come from the
    // GPS / routing API.
    //
    // Distance is measured in kilometres.
    //
    // ========================================================

    vector<vector<double>> distanceMatrix(
        totalLocations,
        vector<double>(
            totalLocations,
            0.0
        )
    );


    cout << "\n";

    cout << "Distance data\n";

    cout << "Enter road distance in kilometres.\n";


    for (int i = 0;
         i < totalLocations;
         i++)
    {
        for (int j = 0;
             j < totalLocations;
             j++)
        {
            if (i == j)
            {
                distanceMatrix[i][j] =
                    0.0;

                continue;
            }


            cout << "\n"
                 << places[i].name
                 << " -> "
                 << places[j].name
                 << ": ";


            cin >> distanceMatrix[i][j];


            if (distanceMatrix[i][j] < 0)
            {
                cout << "Distance cannot be negative.\n";

                return 0;
            }
        }
    }


    // --------------------------------------------------------
    // Available time
    // --------------------------------------------------------

    double availableTime;


    cout << "\nAvailable total time (hours): ";

    cin >> availableTime;


    if (availableTime <= 0)
    {
        cout << "Available time must be positive.\n";

        return 0;
    }


    // ========================================================
    // CREATE PLANNER
    // ========================================================

    TravelPlanner planner(
        places,

        travelTime,

        distanceMatrix,

        availableTime,

        0,

        finalLocation
    );


    // ========================================================
    // RUN BRANCH & BOUND
    // ========================================================

    cout << "\n";

    cout << "============================================================\n";

    cout << "             PLANNING YOUR TRIP...\n";

    cout << "============================================================\n";


    planner.solve();


    // ========================================================
    // DISPLAY RESULT
    // ========================================================

    planner.displayResult();


    // ========================================================
    // DISPLAY ALGORITHM INFORMATION
    // ========================================================

    planner.displayAlgorithmInfo();


    cout << "\n";

    cout << "============================================================\n";

    cout << "                    END OF PROGRAM\n";

    cout << "============================================================\n";


    return 0;
}
