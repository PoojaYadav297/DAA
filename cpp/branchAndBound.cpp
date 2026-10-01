// ============================================================
//              SMART TRAVEL PLANNER
//             BRANCH & BOUND MODEL
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
// Traveler provides:
//     - Available time
//     - Places
//     - Exploration time
//     - Value of each place
//     - Travel time between locations
//
// Branch and Bound internally:
//     1. Creates possible routes
//     2. Calculates current route value
//     3. Calculates an optimistic upper bound
//     4. Prunes routes that cannot beat the current best route
//
// Final traveler output:
//     - Recommended places
//     - Visit order
//     - Travel time
//     - Exploration time
//     - Total time
//     - Total value
//
// ============================================================

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <climits>
#include "branchAndBound.h"
using namespace std;


// ============================================================
// 1. PLACE STRUCTURE
// ============================================================

struct Place
{
    string name;

    // Time needed to explore the place
    int exploreTime;

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
    // Input data
    // --------------------------------------------------------

    vector<Place> places;

    // travelTime[i][j] =
    // travel time from place i to place j
    vector<vector<int>> travelTime;

    // Maximum time available to traveler
    int maxTime;

    // Starting location
    int start;


    // --------------------------------------------------------
    // Best solution found so far
    // --------------------------------------------------------

    int bestValue;
    int bestTotalTime;
    int bestTravelTime;
    int bestExploreTime;

    vector<int> bestRoute;


    // --------------------------------------------------------
    // Statistics for DAA demonstration
    // --------------------------------------------------------

    long long nodesExplored;
    long long nodesPruned;


public:

    // ========================================================
    // CONSTRUCTOR
    // ========================================================

    TravelPlanner(
        const vector<Place>& p,
        const vector<vector<int>>& t,
        int maximumTime,
        int startingPlace)
    {
        places = p;
        travelTime = t;
        maxTime = maximumTime;
        start = startingPlace;

        bestValue = 0;
        bestTotalTime = 0;
        bestTravelTime = 0;
        bestExploreTime = 0;

        nodesExplored = 0;
        nodesPruned = 0;
    }


    // ========================================================
    // UPPER BOUND FUNCTION
    // ========================================================
    //
    // The bound estimates the maximum value that could possibly
    // be achieved from the current state.
    //
    // IMPORTANT:
    //
    // We intentionally ignore some future travel time while
    // calculating the bound.
    //
    // Therefore the bound is optimistic.
    //
    // If even this optimistic value cannot beat the current
    // best solution, the branch can safely be pruned.
    //
    // ========================================================

    int calculateBound(
        const vector<bool>& visited,
        int currentTime,
        int currentValue)
    {
        int remainingTime = maxTime - currentTime;

        if (remainingTime <= 0)
            return currentValue;


        // ----------------------------------------------------
        // Collect unvisited places
        // ----------------------------------------------------

        vector<pair<double, int>> candidates;

        for (int i = 0; i < places.size(); i++)
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


        int boundValue = currentValue;


        // ----------------------------------------------------
        // Optimistically fill remaining time
        //
        // We allow a fractional final place for the bound.
        // This makes the bound optimistic.
        // ----------------------------------------------------

        double remaining = remainingTime;


        for (auto candidate : candidates)
        {
            int index = candidate.second;

            if (places[index].exploreTime <= remaining)
            {
                remaining -= places[index].exploreTime;

                boundValue += places[index].value;
            }
            else
            {
                // Fractional value for upper bound
                boundValue +=
                    (int)(
                        candidate.first * remaining
                    );

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
        int currentTime,
        int currentTravelTime,
        int currentExploreTime,
        int currentValue)
    {
        nodesExplored++;


        // ----------------------------------------------------
        // Calculate optimistic upper bound
        // ----------------------------------------------------

        int bound =
            calculateBound(
                visited,
                currentTime,
                currentValue
            );


        // ----------------------------------------------------
        // PRUNING
        // ----------------------------------------------------
        //
        // If even the optimistic upper bound cannot beat the
        // current best value, there is no reason to continue.
        // ----------------------------------------------------

        if (bound <= bestValue)
        {
            nodesPruned++;
            return;
        }


        // ----------------------------------------------------
        // Update best solution
        // ----------------------------------------------------

        if (currentValue > bestValue)
        {
            bestValue = currentValue;

            bestTotalTime = currentTime;

            bestTravelTime = currentTravelTime;

            bestExploreTime = currentExploreTime;

            bestRoute = currentRoute;
        }


        // ----------------------------------------------------
        // Try every unvisited place
        // ----------------------------------------------------

        for (int next = 0; next < places.size(); next++)
        {
            if (visited[next])
                continue;


            // ------------------------------------------------
            // Calculate travel time to next place
            // ------------------------------------------------

            int moveTime =
                travelTime[current][next];


            // ------------------------------------------------
            // Calculate total time after visiting next place
            // ------------------------------------------------

            int newTime =
                currentTime
                + moveTime
                + places[next].exploreTime;


            // ------------------------------------------------
            // If traveler does not have enough time,
            // this branch is impossible.
            // ------------------------------------------------

            if (newTime > maxTime)
            {
                nodesPruned++;
                continue;
            }


            // ------------------------------------------------
            // Choose next place
            // ------------------------------------------------

            visited[next] = true;

            currentRoute.push_back(next);


            branchAndBound(
                next,
                visited,
                currentRoute,

                newTime,

                currentTravelTime + moveTime,

                currentExploreTime
                    + places[next].exploreTime,

                currentValue
                    + places[next].value
            );


            // ------------------------------------------------
            // BACKTRACK
            // ------------------------------------------------

            currentRoute.pop_back();

            visited[next] = false;
        }
    }


    // ========================================================
    // RUN ALGORITHM
    // ========================================================

    void solve()
    {
        vector<bool> visited(
            places.size(),
            false
        );


        vector<int> currentRoute;


        // Starting point is not considered a tourist
        // destination to collect value.
        visited[start] = true;


        // Start with zero travel/exploration time.
        branchAndBound(
            start,
            visited,
            currentRoute,
            0,
            0,
            0,
            0
        );
    }


    // ========================================================
    // DISPLAY FINAL RESULT
    // ========================================================

    void displayResult()
    {
        cout << "\n";
        cout << "============================================================\n";
        cout << "                  YOUR TRAVEL PLAN\n";
        cout << "============================================================\n";


        if (bestRoute.empty())
        {
            cout << "\nNo tourist place can be visited within "
                 << maxTime << " hours.\n";

            return;
        }


        cout << "\nStarting Location:\n";
        cout << "  " << places[start].name << "\n";


        cout << "\nRecommended Places:\n";

        cout << "------------------------------------------------------------\n";


        for (int i = 0; i < bestRoute.size(); i++)
        {
            int index = bestRoute[i];

            cout << "  "
                 << i + 1
                 << ". "
                 << places[index].name
                 << "  | Explore: "
                 << places[index].exploreTime
                 << " hrs"
                 << " | Value: "
                 << places[index].value
                 << "\n";
        }


        cout << "------------------------------------------------------------\n";


        cout << "\nTrip Summary\n";

        cout << "------------------------------------------------------------\n";

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
             << maxTime - bestTotalTime
             << " hours\n";

        cout << left
             << setw(25)
             << "Total Place Value"
             << ": "
             << bestValue
             << "\n";

        cout << left
             << setw(25)
             << "Places to Explore"
             << ": "
             << bestRoute.size()
             << "\n";

        cout << "------------------------------------------------------------\n";
    }


    // ========================================================
    // DISPLAY BOUND / SEARCH INFORMATION
    // ========================================================

    void displayAlgorithmInfo()
    {
        cout << "\n";
        cout << "============================================================\n";
        cout << "             BRANCH & BOUND ANALYSIS\n";
        cout << "============================================================\n";


        cout << "\nNodes Explored : "
             << nodesExplored << "\n";

        cout << "Nodes Pruned   : "
             << nodesPruned << "\n";


        cout << "\nWhat happened internally?\n";

        cout << "\n1. BRANCHING\n";
        cout << "   The algorithm tries different places as the next\n";
        cout << "   destination in the travel route.\n";


        cout << "\n2. BOUNDING\n";
        cout << "   For every partial route, an optimistic maximum\n";
        cout << "   possible value is calculated.\n";


        cout << "\n3. PRUNING\n";
        cout << "   If the optimistic value cannot beat the current\n";
        cout << "   best route, that branch is discarded.\n";


        cout << "\n4. BEST SOLUTION\n";
        cout << "   The route with the highest achievable value within\n";
        cout << "   the available time is returned.\n";
    }


    // ========================================================
    // DISPLAY SIMPLE SEARCH TREE EXPLANATION
    // ========================================================

    void displaySearchConcept()
    {
        cout << "\n";
        cout << "============================================================\n";
        cout << "             BRANCH & BOUND SEARCH MODEL\n";
        cout << "============================================================\n";


        cout << R"(

                       START
                    /    |    \
                   /     |     \
              Museum    Fort    Park
                /        |       \
               /         |        \
             ...        ...       ...
              |          |         |
            BOUND      BOUND     BOUND
              |          |         |
           EXPLORE      PRUNE    EXPLORE

    BRANCH  = Try another possible destination

    BOUND   = Estimate the best value this branch
              could possibly achieve

    PRUNE   = Stop exploring a branch when its bound
              cannot improve the current best solution

)";
    }


    // ========================================================
    // GETTERS
    // ========================================================

    int getBestValue()
    {
        return bestValue;
    }

    int getBestTime()
    {
        return bestTotalTime;
    }

    vector<int> getBestRoute()
    {
        return bestRoute;
    }
};


// ============================================================
// 3. DISPLAY PLACE DATABASE
// ============================================================

void displayPlaces(
    const vector<Place>& places)
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                    PLACE DATABASE\n";
    cout << "============================================================\n";


    cout << left
         << setw(5) << "No."
         << setw(25) << "Place"
         << setw(15) << "Explore Time"
         << setw(10) << "Value"
         << "\n";


    cout << "------------------------------------------------------------\n";


    for (int i = 0; i < places.size(); i++)
    {
        cout << left
             << setw(5) << i
             << setw(25) << places[i].name
             << setw(15) << places[i].exploreTime
             << setw(10) << places[i].value
             << "\n";
    }


    cout << "------------------------------------------------------------\n";
}


// ============================================================
// 4. DISPLAY TRAVEL TIME MATRIX
// ============================================================

void displayTravelMatrix(
    const vector<Place>& places,
    const vector<vector<int>>& travelTime)
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                  TRAVEL TIME MATRIX\n";
    cout << "============================================================\n";


    cout << "\nTravel time between locations (hours):\n\n";


    cout << left << setw(18) << "Location";


    for (int i = 0; i < places.size(); i++)
    {
        cout << setw(10) << i;
    }

    cout << "\n";


    cout << "------------------------------------------------------------\n";


    for (int i = 0; i < places.size(); i++)
    {
        cout << left
             << setw(18)
             << places[i].name.substr(
                    0,
                    min(17,
                        (int)places[i].name.length())
                );


        for (int j = 0; j < places.size(); j++)
        {
            cout << setw(10)
                 << travelTime[i][j];
        }

        cout << "\n";
    }


    cout << "------------------------------------------------------------\n";
}


// ============================================================
// 5. DEMO DATA
// ============================================================

void createDemoData(
    vector<Place>& places,
    vector<vector<int>>& travelTime)
{
    // --------------------------------------------------------
    // Location 0 is the starting location.
    // It is not a tourist place to collect value.
    // --------------------------------------------------------

    places =
    {
        {"City Center", 0, 0},

        {"Museum", 2, 6},

        {"Fort", 2, 8},

        {"Park", 1, 4},

        {"Temple", 2, 7},

        {"Lake", 2, 5}
    };


    // --------------------------------------------------------
    // Travel time matrix
    //
    // travelTime[i][j] = time from i to j
    //
    // 0 means same location.
    // --------------------------------------------------------

    travelTime =
    {
        //  C   M   F   P   T   L
        {  0,  1,  2,  1,  3,  2 }, // City Center

        {  1,  0,  2,  1,  2,  2 }, // Museum

        {  2,  2,  0,  2,  1,  3 }, // Fort

        {  1,  1,  2,  0,  2,  1 }, // Park

        {  3,  2,  1,  2,  0,  2 }, // Temple

        {  2,  2,  3,  1,  2,  0 }  // Lake
    };
}


// ============================================================
// 6. CUSTOM DATA INPUT
// ============================================================

void createCustomData(
    vector<Place>& places,
    vector<vector<int>>& travelTime)
{
    int n;


    cout << "\nEnter number of tourist places: ";
    cin >> n;


    while (n <= 0)
    {
        cout << "Number must be greater than 0.\n";
        cout << "Enter again: ";
        cin >> n;
    }


    // --------------------------------------------------------
    // +1 because location 0 is starting location
    // --------------------------------------------------------

    places.resize(n + 1);


    cout << "\nStarting location\n";

    cout << "Name: ";
    cin >> ws;
    getline(cin, places[0].name);

    places[0].exploreTime = 0;
    places[0].value = 0;


    // --------------------------------------------------------
    // Tourist places
    // --------------------------------------------------------

    for (int i = 1; i <= n; i++)
    {
        cout << "\nTourist Place " << i << "\n";


        cout << "Name: ";
        cin >> ws;
        getline(cin, places[i].name);


        cout << "Exploration time (hours): ";
        cin >> places[i].exploreTime;


        while (places[i].exploreTime <= 0)
        {
            cout << "Enter a positive value: ";
            cin >> places[i].exploreTime;
        }


        cout << "Place value/importance: ";
        cin >> places[i].value;


        while (places[i].value <= 0)
        {
            cout << "Enter a positive value: ";
            cin >> places[i].value;
        }
    }


    // --------------------------------------------------------
    // Travel time matrix
    // --------------------------------------------------------

    travelTime.assign(
        n + 1,
        vector<int>(n + 1, 0)
    );


    cout << "\n";
    cout << "Enter travel times between locations.\n";
    cout << "Enter 0 for the same location.\n";


    for (int i = 0; i <= n; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            if (i == j)
            {
                travelTime[i][j] = 0;
            }
            else
            {
                cout << "\n"
                     << places[i].name
                     << " -> "
                     << places[j].name
                     << " : ";

                cin >> travelTime[i][j];


                while (travelTime[i][j] < 0)
                {
                    cout << "Travel time cannot be negative. "
                         << "Enter again: ";

                    cin >> travelTime[i][j];
                }
            }
        }
    }
}


// ============================================================
// 7. MAIN FUNCTION
// ============================================================

int main()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                 SMART TRAVEL PLANNER\n";
    cout << "                  BRANCH & BOUND\n";
    cout << "============================================================\n";


    cout << "\nWelcome Traveler!\n";

    cout << "\nThis planner finds a useful set and order of places\n";
    cout << "that can be explored within your available time.\n";


    // --------------------------------------------------------
    // Data containers
    // --------------------------------------------------------

    vector<Place> places;

    vector<vector<int>> travelTime;


    // --------------------------------------------------------
    // Select data mode
    // --------------------------------------------------------

    int choice;


    cout << "\n";
    cout << "------------------------------------------------------------\n";
    cout << "                    DATA OPTIONS\n";
    cout << "------------------------------------------------------------\n";

    cout << "\n1. Use Demo Travel Data";
    cout << "\n2. Enter Custom Travel Data";


    cout << "\n\nEnter choice: ";
    cin >> choice;


    if (choice == 1)
    {
        createDemoData(
            places,
            travelTime
        );

        cout << "\nDemo travel data loaded.\n";
    }

    else if (choice == 2)
    {
        createCustomData(
            places,
            travelTime
        );
    }

    else
    {
        cout << "\nInvalid choice.\n";
        return 0;
    }


    // --------------------------------------------------------
    // Display data
    // --------------------------------------------------------

    displayPlaces(places);

    displayTravelMatrix(
        places,
        travelTime
    );


    // --------------------------------------------------------
    // Get available time
    // --------------------------------------------------------

    int maxTime;


    cout << "\nEnter your available time (hours): ";
    cin >> maxTime;


    while (maxTime <= 0)
    {
        cout << "Available time must be greater than 0.\n";
        cout << "Enter again: ";
        cin >> maxTime;
    }


    // --------------------------------------------------------
    // Create Branch & Bound planner
    // --------------------------------------------------------

    TravelPlanner planner(
        places,
        travelTime,
        maxTime,
        0
    );


    // --------------------------------------------------------
    // Run Branch & Bound
    // --------------------------------------------------------

    cout << "\n";
    cout << "============================================================\n";
    cout << "             PLANNING YOUR TRIP...\n";
    cout << "============================================================\n";


    cout << "\nBranch & Bound is evaluating possible routes...\n";


    planner.solve();


    // --------------------------------------------------------
    // Display traveler-friendly result
    // --------------------------------------------------------

    planner.displayResult();


    // --------------------------------------------------------
    // Display DAA information
    // --------------------------------------------------------

    planner.displayAlgorithmInfo();


    // --------------------------------------------------------
    // Display search concept
    // --------------------------------------------------------

    planner.displaySearchConcept();


    // --------------------------------------------------------
    // Final message
    // --------------------------------------------------------

    cout << "\n";
    cout << "============================================================\n";
    cout << "              HAPPY TRAVELING!\n";
    cout << "============================================================\n";


    return 0;
}
