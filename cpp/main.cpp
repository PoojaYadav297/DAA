#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <stdexcept>

#include "branchAndBound.h"

using namespace std;


// ------------------------------------------------------------
// Load places from places.txt
// ------------------------------------------------------------
bool loadPlaces(const string& filename, vector<Place>& places)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Error: Could not open " << filename << endl;
        return false;
    }

    int n;

    if (!(file >> n))
    {
        cerr << "Error: Invalid number of places in " << filename << endl;
        return false;
    }

    if (n < 2)
    {
        cerr << "Error: At least 2 places are required." << endl;
        return false;
    }

    places.clear();
    places.reserve(n);

    for (int i = 0; i < n; i++)
    {
        Place place;

        if (!(file >> place.name
                  >> place.explorationTime
                  >> place.value))
        {
            cerr << "Error: Invalid place data at line "
                 << i + 2 << " in " << filename << endl;
            return false;
        }

        if (place.explorationTime < 0)
        {
            cerr << "Error: Exploration time cannot be negative." << endl;
            return false;
        }

        if (place.value < 0)
        {
            cerr << "Error: Place value cannot be negative." << endl;
            return false;
        }

        places.push_back(place);
    }

    file.close();

    return true;
}


// ------------------------------------------------------------
// Load distance matrix from distances.txt
// ------------------------------------------------------------
bool loadDistances(
    const string& filename,
    vector<vector<double>>& distances,
    int expectedSize)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Error: Could not open " << filename << endl;
        return false;
    }

    int n;

    if (!(file >> n))
    {
        cerr << "Error: Invalid matrix size in " << filename << endl;
        return false;
    }

    if (n != expectedSize)
    {
        cerr << "Error: Number of places (" << expectedSize
             << ") does not match distance matrix size (" << n << ")."
             << endl;
        return false;
    }

    distances.assign(n, vector<double>(n));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (!(file >> distances[i][j]))
            {
                cerr << "Error: Invalid distance data at row "
                     << i + 1 << ", column " << j + 1 << endl;
                return false;
            }

            if (distances[i][j] < 0)
            {
                cerr << "Error: Distance cannot be negative." << endl;
                return false;
            }
        }
    }

    file.close();

    // Check diagonal
    for (int i = 0; i < n; i++)
    {
        if (distances[i][i] != 0)
        {
            cerr << "Warning: Distance from a place to itself should be 0."
                 << endl;
        }
    }

    return true;
}


// ------------------------------------------------------------
// Display all available places
// ------------------------------------------------------------
void displayPlaces(const vector<Place>& places)
{
    cout << "\nAvailable Places\n";
    cout << "--------------------------------------------------\n";

    cout << left
         << setw(5)  << "ID"
         << setw(20) << "Place"
         << setw(20) << "Explore Time"
         << setw(10) << "Value"
         << endl;

    cout << "--------------------------------------------------\n";

    for (int i = 0; i < static_cast<int>(places.size()); i++)
    {
        cout << left
             << setw(5)  << i
             << setw(20) << places[i].name
             << setw(20) << places[i].explorationTime
             << setw(10) << places[i].value
             << endl;
    }

    cout << "--------------------------------------------------\n";
}


// ------------------------------------------------------------
// Display distance matrix
// ------------------------------------------------------------
void displayDistances(
    const vector<vector<double>>& distances,
    const vector<Place>& places)
{
    cout << "\nDistance Matrix (km)\n";
    cout << "--------------------------------------------------\n";

    cout << setw(18) << "";

    for (const auto& place : places)
    {
        cout << setw(12) << place.name;
    }

    cout << endl;

    for (size_t i = 0; i < distances.size(); i++)
    {
        cout << setw(18) << places[i].name;

        for (size_t j = 0; j < distances[i].size(); j++)
        {
            cout << setw(12) << distances[i][j];
        }

        cout << endl;
    }

    cout << "--------------------------------------------------\n";
}


// ------------------------------------------------------------
// Main program
// ------------------------------------------------------------
int main()
{
    cout << "=============================================\n";
    cout << "        SMART TRAVEL PLANNER\n";
    cout << "        Branch and Bound Engine\n";
    cout << "=============================================\n";

    vector<Place> places;
    vector<vector<double>> distances;

    // --------------------------------------------------------
    // Step 1: Load travel data
    // --------------------------------------------------------

    if (!loadPlaces("data/places.txt", places))
    {
        return 1;
    }

    if (!loadDistances(
            "data/distances.txt",
            distances,
            static_cast<int>(places.size())))
    {
        return 1;
    }

    // --------------------------------------------------------
    // Step 2: Start and destination
    // --------------------------------------------------------

    // First place = starting location
    int start = 0;

    // Last place = final destination
    int destination = static_cast<int>(places.size()) - 1;

    cout << "\nStarting Location : "
         << places[start].name << endl;

    cout << "Final Destination : "
         << places[destination].name << endl;

    // --------------------------------------------------------
    // Step 3: Display input data
    // --------------------------------------------------------

    displayPlaces(places);

    // Uncomment if you want to display the complete matrix.
    // displayDistances(distances, places);

    // --------------------------------------------------------
    // Step 4: Get user constraints
    // --------------------------------------------------------

    double maxTime;
    double speed;

    cout << "\nEnter available travel time (hours): ";
    cin >> maxTime;

    if (cin.fail() || maxTime <= 0)
    {
        cerr << "Error: Available time must be greater than 0."
             << endl;
        return 1;
    }

    cout << "Enter average travel speed (km/h): ";
    cin >> speed;

    if (cin.fail() || speed <= 0)
    {
        cerr << "Error: Speed must be greater than 0."
             << endl;
        return 1;
    }

    // --------------------------------------------------------
    // Step 5: Call Branch and Bound
    // --------------------------------------------------------

    cout << "\n---------------------------------------------\n";
    cout << "Running Branch and Bound...\n";
    cout << "---------------------------------------------\n";

    Result result = branchAndBound(
        places,
        distances,
        start,
        destination,
        maxTime,
        speed
    );

    // --------------------------------------------------------
    // Step 6: Display final result
    // --------------------------------------------------------

    cout << "\n=============================================\n";
    cout << "           OPTIMAL TRAVEL PLAN\n";
    cout << "=============================================\n";

    cout << fixed << setprecision(2);

    cout << "\nRoute:\n";

    for (size_t i = 0; i < result.route.size(); i++)
    {
        int placeIndex = result.route[i];

        if (placeIndex >= 0 &&
            placeIndex < static_cast<int>(places.size()))
        {
            cout << places[placeIndex].name;

            if (i + 1 < result.route.size())
            {
                cout << " -> ";
            }
        }
    }

    cout << "\n\nTotal Distance   : "
         << result.totalDistance << " km";

    cout << "\nTravel Time      : "
         << result.travelTime << " hours";

    cout << "\nExploration Time : "
         << result.explorationTime << " hours";

    cout << "\nTotal Time       : "
         << result.totalTime << " hours";

    cout << "\nTotal Value      : "
         << result.totalValue;

    cout << "\n\nAvailable Time   : "
         << maxTime << " hours";

    cout << "\nAverage Speed    : "
         << speed << " km/h";

    cout << "\n=============================================\n";

    return 0;
}
