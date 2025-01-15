#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

struct Passenger {
    string id;
    string firstName;
    string lastName;
    string origin;
    string destination;
    Passenger* next;
};

struct Seat {
    string seatId;
    Passenger* passenger;
    Seat* next;
};

struct Wagon {
    int number;
    Seat* seatsHead;
    Wagon* next;
};

struct Train {
    int id;
    string stoppingStations[10];
    int numStations;
    Wagon* wagonsHead;
};

// Function to initialize a wagon with seats.
Wagon* createWagon(int num) {
    Wagon* wagon = new Wagon;
    wagon->number = num;
    wagon->seatsHead = NULL;
    wagon->next = NULL;

    Seat* tail = NULL;
    for (char row = 'A'; row <= 'D'; ++row) {
        for (int col = 1; col <= 20; ++col) {
            Seat* newSeat = new Seat;
            newSeat->seatId = to_string(col) + row;
            newSeat->passenger = NULL;
            newSeat->next = NULL;

            if (!wagon->seatsHead) {
                wagon->seatsHead = newSeat;
            } else {
                tail->next = newSeat;
            }
            tail = newSeat;
        }
    }
    return wagon;
}

// Function to initialize a train with wagons.
Train* createTrain(int id, const string* stations, int stationCount, int numWagons) {
    Train* train = new Train;
    train->id = id;
    train->numStations = stationCount;
    for (int i = 0; i < stationCount; ++i) {
        train->stoppingStations[i] = stations[i];
    }
    train->wagonsHead = NULL;

    Wagon* tail = NULL;
    for (int i = 1; i <= numWagons; ++i) {
        Wagon* newWagon = createWagon(i);
        if (!train->wagonsHead) {
            train->wagonsHead = newWagon;
        } else {
            tail->next = newWagon;
        }
        tail = newWagon;
    }
    return train;
}

// Function to add a route to the graph (expressGraph)
void addRouteToGraph(map<string, vector<string>>& expressGraph, const string& origin, const string& destination) {
    expressGraph[origin].push_back(destination);
}

// Function to display all routes in the graph
void displayRoutes(const map<string, vector<string>>& expressGraph) {
    cout << "\nTrain Routes (Origin -> Destination):\n";
    for (const auto& route : expressGraph) {
        cout << route.first << " -> ";
        for (const auto& city : route.second) {
            cout << city << " ";
        }
        cout << "\n";
    }
}

// Function to add a passenger to a specific seat.
bool addPassenger(Train* train, int wagonNum, const string& seatId, const Passenger& passenger, map<string, vector<string>>& expressGraph) {
    Wagon* currentWagon = train->wagonsHead;
    for (int i = 1; i < wagonNum && currentWagon; ++i) {
        currentWagon = currentWagon->next;
    }
    if (!currentWagon) return false;

    Seat* currentSeat = currentWagon->seatsHead;
    while (currentSeat) {
        if (currentSeat->seatId == seatId && !currentSeat->passenger) {
            currentSeat->passenger = new Passenger(passenger);
            addRouteToGraph(expressGraph, passenger.origin, passenger.destination);  // Add route to graph
            return true;
        }
        currentSeat = currentSeat->next;
    }
    return false;
}

// Function to search for passengers by name.
void searchPassengersByName(const map<int, Train*>& trains, const string& firstName, const string& lastName) {
    bool found = false;
    for (const auto& trainPair : trains) {
        Train* train = trainPair.second;
        Wagon* currentWagon = train->wagonsHead;

        while (currentWagon) {
            Seat* currentSeat = currentWagon->seatsHead;
            while (currentSeat) {
                if (currentSeat->passenger) {
                    Passenger* passenger = currentSeat->passenger;
                    bool firstNameMatches = (firstName == "-" || passenger->firstName == firstName);
                    bool lastNameMatches = (lastName == "-" || passenger->lastName == lastName);

                    if (firstNameMatches && lastNameMatches) {
                        cout << "Passenger Found: " << passenger->firstName << " " << passenger->lastName
                             << " (ID: " << passenger->id << "), Train ID: " << train->id
                             << ", Wagon: " << currentWagon->number << ", Seat: " << currentSeat->seatId
                             << ", From: " << passenger->origin << " to " << passenger->destination << "\n";
                        found = true;
                    }
                }
                currentSeat = currentSeat->next;
            }
            currentWagon = currentWagon->next;
        }
    }
    if (!found) {
        cout << "No passengers found with the given name.\n";
    }
}

// Function to display all passengers on a train.
void displayPassengersOnTrain(const map<int, Train*>& trains, int trainId) {
    if (trains.find(trainId) == trains.end()) {
        cout << "Train with ID " << trainId << " not found.\n";
        return;
    }

    Train* train = trains.at(trainId);
    Wagon* currentWagon = train->wagonsHead;

    cout << "Passengers on Train ID: " << trainId << "\n";
    while (currentWagon) {
        Seat* currentSeat = currentWagon->seatsHead;
        while (currentSeat) {
            if (currentSeat->passenger) {
                Passenger* passenger = currentSeat->passenger;
                cout << "Name: " << passenger->firstName << " " << passenger->lastName
                     << ", Seat: " << currentSeat->seatId
                     << ", Wagon: " << currentWagon->number << "\n";
            }
            currentSeat = currentSeat->next;
        }
        currentWagon = currentWagon->next;
    }
}

int main() {
    map<int, Train*> trains;
    map<string, vector<string>> expressGraph;  // Initialize the expressGraph

    // Predefined trains.
    string stations1[] = {"CityA", "CityB", "CityC"};
    trains[1] = createTrain(1, stations1, 3, 6);

    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Add Passenger\n";
        cout << "2. Search Passenger by Name\n";
        cout << "3. Display All Passengers on Train\n";
        cout << "4. Display Routes\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            int trainId, wagonNum;
            string seatId, pid, firstName, lastName, origin, destination;

            cout << "Enter Train ID: ";
            cin >> trainId;
            if (trains.find(trainId) == trains.end()) {
                cout << "Train not found.\n";
                continue;
            }

            cout << "Enter Wagon Number: ";
            cin >> wagonNum;

            cout << "Enter Seat ID (e.g., 1A): ";
            cin >> seatId;

            cout << "Enter Passenger ID: ";
            cin >> pid;

            cout << "Enter First Name: ";
            cin.ignore();
            getline(cin, firstName);

            cout << "Enter Last Name: ";
            getline(cin, lastName);

            cout << "Enter Origin: ";
            getline(cin, origin);

            cout << "Enter Destination: ";
            getline(cin, destination);

            if (firstName.empty() || lastName.empty()) {
                cout << "Both first and last names are required.\n";
                continue;
            }

            Passenger passenger = {pid, firstName, lastName, origin, destination, NULL};
            if (addPassenger(trains[trainId], wagonNum, seatId, passenger, expressGraph)) {
                cout << "Passenger added successfully!\n";
            } else {
                cout << "Failed to add passenger. Seat may already be occupied.\n";
            }
        } else if (choice == 2) {
            string firstName, lastName;
            cout << "Enter First Name (or '-' if empty): ";
            cin.ignore();
            getline(cin, firstName);

            cout << "Enter Last Name (or '-' if empty): ";
            getline(cin, lastName);

            searchPassengersByName(trains, firstName, lastName);
        } else if (choice == 3) {
            int trainId;
            cout << "Enter Train ID: ";
            cin >> trainId;
            displayPassengersOnTrain(trains, trainId);
        } else if (choice == 4) {
            displayRoutes(expressGraph);
        } else if (choice == 5) {
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    // Clean up allocated memory.
    for (auto& trainPair : trains) {
        Train* train = trainPair.second;
        Wagon* currentWagon = train->wagonsHead;

        while (currentWagon) {
            Seat* currentSeat = currentWagon->seatsHead;
            while (currentSeat) {
                delete currentSeat->passenger;
                Seat* tempSeat = currentSeat;
                currentSeat = currentSeat->next;
                delete tempSeat;
            }
            Wagon* tempWagon = currentWagon;
            currentWagon = currentWagon->next;
            delete tempWagon;
        }
        delete train;
    }

    return 0;
}
