// ============================================================
//              SMART TRAVEL PLANNER
//                BRANCH & BOUND
// ============================================================
//
// This program receives trip data from the backend.
//
// Input:
// - Starting location
// - Tourist places
// - Exploration time
// - Value of each place
// - Final location
// - Available time
// - Travel-time matrix from GPS/Routing API
//
// The Branch & Bound algorithm decides:
// - Which places to visit
// - Which places to skip
// - Order of selected places
//
// Output:
// - Selected places
// - Skipped places
// - Recommended route
// - Travel time
// - Exploration time
// - Total time
// - Remaining time
// - Total value
// - Nodes explored
// - Nodes pruned
//
// ============================================================

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace std;


// ============================================================
// 1. PLACE STRUCTURE
// ============================================================

struct Place
{
    string name;

    // Exploration time in hours
    double exploreTime;

    // Importance / attraction value
    int value;
};


// ============================================================
// 2. TRAVEL PLANNER CLASS
// ============================================================

class TravelPlanner
{
private:

    // --------------------------------------------------------
    // Input data
    // --------------------------------------------------------

    vector<Place> places;

    /*
        travelTime[i][j]

        Travel time from location i
        to location j.

        This matrix will come from the
        GPS / routing API through Node.js.
    */

    vector<vector<double>> travelTime;

    // Maximum time available
    double maxTime;

    // Index of starting location
    int start;

    // Index of final location
    int finalLocation;


    // --------------------------------------------------------
    // Best solution
    // --------------------------------------------------------

    int bestValue;

    double bestTotalTime;

    double bestTravelTime;

    double bestExploreTime;

    vector<int> bestRoute;


    // --------------------------------------------------------
    // DAA statistics
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
        // Find unvisited tourist places
        // ----------------------------------------------------

        vector<pair<double, int>> candidates;


        /*
            Location structure:

            0                 = Start
            1 ... n           = Tourist places
            n + 1             = Final
        */

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
        // Sort by value / exploration-time ratio
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
                /*
                    Fractional value is used only for
                    calculating the optimistic bound.

                    A real solution can never take
                    a fraction of a tourist place.
                */

                boundValue +=
                    candidate.first *
                    remaining;

                break;
            }
        }


        return boundValue;
    }


    // ========================================================
    // BRANCH AND BOUND
    // ========================================================

    void branchAndBound(
        int current,
        vector<bool>& visited,
        vector<int>& currentRoute,
        double currentTime,
        double currentTravelTime,
        double currentExploreTime,
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
        // Check whether we can return to final location
        // ----------------------------------------------------

        double returnTime =
            travelTime[current][finalLocation];


        double totalTimeWithReturn =
            currentTime + returnTime;


        if (totalTimeWithReturn <= maxTime)
        {
            /*
                Current route can return to the
                final destination.

                Therefore it is a valid candidate.
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
            if (visited[next])
            {
                continue;
            }


            // ------------------------------------------------
            // Travel from current place to next place
            // ------------------------------------------------

            double moveTime =
                travelTime[current][next];


            // ------------------------------------------------
            // Total time after visiting next place
            // ------------------------------------------------

            double newTime =
                currentTime
                + moveTime
                + places[next].exploreTime;


            // ------------------------------------------------
            // If time limit is exceeded, prune
            // ------------------------------------------------

            if (newTime > maxTime)
            {
                nodesPruned++;
                continue;
            }


            // ------------------------------------------------
            // Choose the next place
            // ------------------------------------------------

            visited[next] =
                true;

            currentRoute.push_back(next);


            // ------------------------------------------------
            // Continue Branch & Bound
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

                currentValue +
                    places[next].value
            );


            // ------------------------------------------------
            // Backtrack
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
        double availableTime,
        int startingLocation,
        int endingLocation)
    {
        places =
            p;

        travelTime =
            t;

        maxTime =
            availableTime;

        start =
            startingLocation;

        finalLocation =
            endingLocation;


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


        /*
            Start and final locations are not
            tourist places.

            They should not be selected for value.
        */

        visited[start] =
            true;

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

            0
        );
    }


    // ========================================================
    // GET RESULTS
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


    double getRemainingTime() const
    {
        return maxTime -
               bestTotalTime;
    }


    vector<int> getBestRoute() const
    {
        return bestRoute;
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
            cout << "\nNo feasible travel plan found.\n";

            return;
        }


        // ----------------------------------------------------
        // Starting location
        // ----------------------------------------------------

        cout << "\nStarting Location:\n";

        cout << "  "
             << places[start].name
             << "\n";


        // ----------------------------------------------------
        // Selected places
        // ----------------------------------------------------

        cout << "\nPlaces to Visit:\n";

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
                     << places[index].exploreTime
                     << " hrs"

                     << " | Value: "
                     << places[index].value

                     << "\n";
            }
        }


        // ----------------------------------------------------
        // Skipped places
        // ----------------------------------------------------

        cout << "\nPlaces Not Selected:\n";

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
        // Recommended route
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
        // Trip summary
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
             << "Total Time"
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
    // DISPLAY DAA INFORMATION
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
        cout << "   value that a partial route could achieve.\n";


        cout << "\n3. PRUNING\n";

        cout << "   A branch is discarded when it cannot improve\n";
        cout << "   the current best solution.\n";


        cout << "\n4. BEST SOLUTION\n";

        cout << "   The feasible route with the highest total value\n";
        cout << "   is selected within the available time.\n";
    }
};


// ============================================================
// 3. MAIN FUNCTION
// ============================================================

int main()
{
    /*
        IMPORTANT:

        This main function is intentionally kept simple.

        In the final website version, Node.js will provide:

        1. Places
        2. Exploration times
        3. Place values
        4. Starting location
        5. Final location
        6. Available time
        7. GPS-generated travel-time matrix

        Therefore, NO tourist places are hard-coded here.
    */


    // --------------------------------------------------------
    // Temporary example input
    // --------------------------------------------------------
    //
    // This section is only for testing the C++ algorithm
    // until Node.js is connected.
    //
    // It can later be replaced by JSON input from Node.js.
    // --------------------------------------------------------


    int numberOfPlaces;

    cout << "Enter number of tourist places: ";

    cin >> numberOfPlaces;


    if (numberOfPlaces <= 0)
    {
        cout << "Invalid number of places.\n";

        return 0;
    }


    // --------------------------------------------------------
    // Total locations:
    //
    // 0                 = Start
    // 1 ... n           = Tourist places
    // n + 1             = Final
    // --------------------------------------------------------

    int totalLocations =
        numberOfPlaces + 2;


    vector<Place> places(
        totalLocations
    );


    // --------------------------------------------------------
    // Starting location
    // --------------------------------------------------------

    cout << "\nEnter starting location: ";

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


        while (places[i].exploreTime <= 0)
        {
            cout << "Enter a positive value: ";

            cin >> places[i].exploreTime;
        }


        cout << "Place value: ";

        cin >> places[i].value;


        while (places[i].value <= 0)
        {
            cout << "Enter a positive value: ";

            cin >> places[i].value;
        }
    }


    // --------------------------------------------------------
    // Final location
    // --------------------------------------------------------

    int finalLocation =
        numberOfPlaces + 1;


    cout << "\nEnter final location: ";

    cin >> ws;

    getline(
        cin,
        places[finalLocation].name
    );


    places[finalLocation].exploreTime =
        0.0;

    places[finalLocation].value =
        0;


    // --------------------------------------------------------
    // Travel-time matrix
    // --------------------------------------------------------
    //
    // IMPORTANT:
    //
    // In the final system this matrix will NOT be entered
    // manually.
    //
    // Node.js will obtain it from the GPS/routing API.
    //
    // This temporary input is only to test the algorithm.
    // --------------------------------------------------------

    vector<vector<double>> travelTime(
        totalLocations,
        vector<double>(
            totalLocations,
            0.0
        )
    );


    cout << "\n";
    cout << "Enter travel time between locations.\n";
    cout << "Use hours. Example: 0.5 = 30 minutes.\n";


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
                 << " : ";


            cin >> travelTime[i][j];


            while (travelTime[i][j] < 0)
            {
                cout << "Travel time cannot be negative. ";

                cin >> travelTime[i][j];
            }
        }
    }


    // --------------------------------------------------------
    // Available time
    // --------------------------------------------------------

    double availableTime;


    cout << "\nAvailable total time (hours): ";

    cin >> availableTime;


    while (availableTime <= 0)
    {
        cout << "Available time must be greater than 0: ";

        cin >> availableTime;
    }


    // --------------------------------------------------------
    // Create Branch & Bound planner
    // --------------------------------------------------------

    TravelPlanner planner(
        places,

        travelTime,

        availableTime,

        0,

        finalLocation
    );


    // --------------------------------------------------------
    // Run algorithm
    // --------------------------------------------------------

    cout << "\n";
    cout << "============================================================\n";
    cout << "             PLANNING YOUR TRIP...\n";
    cout << "============================================================\n";


    planner.solve();


    // --------------------------------------------------------
    // Display final result
    // --------------------------------------------------------

    planner.displayResult();


    // --------------------------------------------------------
    // Display algorithm information
    // --------------------------------------------------------

    planner.displayAlgorithmInfo();


    cout << "\n";
    cout << "============================================================\n";
    cout << "                  END OF PROGRAM\n";
    cout << "============================================================\n";


    return 0;
}
