#include <iostream>
#include <vector>
#include <string>

#include "branchAndBound.h"

using namespace std;

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
    // Location indexing
    //
    // 0       = Starting location
    // 1..n    = Tourist places
    // n+1     = Final location
    // --------------------------------------------------------

    int totalLocations = numberOfPlaces + 2;
    int finalLocation = numberOfPlaces + 1;

    vector<Place> places(totalLocations);

    // --------------------------------------------------------
    // Starting location
    // --------------------------------------------------------

    cout << "\nStarting location: ";

    cin >> ws;
    getline(cin, places[0].name);

    places[0].exploreTime = 0.0;
    places[0].value = 0;

    // --------------------------------------------------------
    // Tourist places
    // --------------------------------------------------------

    for (int i = 1; i <= numberOfPlaces; i++)
    {
        cout << "\nTourist Place " << i << "\n";

        cout << "Name: ";

        cin >> ws;
        getline(cin, places[i].name);

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
    getline(cin, places[finalLocation].name);

    places[finalLocation].exploreTime = 0.0;
    places[finalLocation].value = 0;

    // ========================================================
    // TEMPORARY TRAVEL TIME MATRIX
    // ========================================================
    //
    // For now, we enter this manually.
    //
    // Later:
    //
    // GPS / Routing API
    //        ↓
    //     Node.js
    //        ↓
    //      C++
    //
    // ========================================================

    vector<vector<double>> travelTime(
        totalLocations,
        vector<double>(totalLocations, 0.0)
    );

    cout << "\n";
    cout << "============================================================\n";
    cout << "                 TRAVEL TIME DATA\n";
    cout << "============================================================\n";

    cout << "Enter travel time in hours.\n";
    cout << "Example: 30 minutes = 0.50 hours.\n";

    for (int i = 0; i < totalLocations; i++)
    {
        for (int j = 0; j < totalLocations; j++)
        {
            if (i == j)
            {
                travelTime[i][j] = 0.0;
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
    // TEMPORARY DISTANCE MATRIX
    // ========================================================

    vector<vector<double>> distanceMatrix(
        totalLocations,
        vector<double>(totalLocations, 0.0)
    );

    cout << "\n";
    cout << "============================================================\n";
    cout << "                 DISTANCE DATA\n";
    cout << "============================================================\n";

    cout << "Enter road distance in kilometres.\n";

    for (int i = 0; i < totalLocations; i++)
    {
        for (int j = 0; j < totalLocations; j++)
        {
            if (i == j)
            {
                distanceMatrix[i][j] = 0.0;
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
    // Available total time
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
    // CREATE TRAVEL PLANNER
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
