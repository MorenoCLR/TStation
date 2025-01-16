#include <iostream>
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

struct CityNode;  // Forward declaration

struct StationNode {
    string stationName;
    StationNode* nextStation;
};

struct Train {
    int id;
    CityNode* origin;
    CityNode* destination;
    StationNode* stationsHead;
    Wagon* wagonsHead;
    Train* nextTrain;
};

struct RouteNode {
    CityNode* destination;  // Pointer to the destination city
    Train* train;           // Pointer to the train connecting the cities
    RouteNode* nextRoute;
};

struct CityNode {
    string cityName;
    RouteNode* routesHead;  // Linked list of routes (connections)
    CityNode* nextCity;
};

// Function to create a new city node
CityNode* createCityNode(const string& cityName) {
    CityNode* newCity = new CityNode;
    newCity->cityName = cityName;
    newCity->routesHead = NULL;
    newCity->nextCity = NULL;
    return newCity;
}

// Function to create a new train
Train* createTrain(int id, CityNode* origin, CityNode* destination, const string* stations, int numStations) {
    Train* newTrain = new Train;
    newTrain->id = id;
    newTrain->origin = origin;
    newTrain->destination = destination;
    newTrain->stationsHead = NULL;  // Initialize head of linked list for stations
    newTrain->wagonsHead = NULL;
    newTrain->nextTrain = NULL;

    // Create linked list of stopping stations
    StationNode* tailStation = NULL;
    for (int i = 0; i < numStations; ++i) {
        StationNode* newStation = new StationNode;
        newStation->stationName = stations[i];
        newStation->nextStation = NULL;

        if (!newTrain->stationsHead) {
            newTrain->stationsHead = newStation;  // Set head if it's the first station
        } else {
            tailStation->nextStation = newStation;  // Link the previous station to the new one
        }
        tailStation = newStation;  // Move tail pointer to the new station
    }

    // Initialize 6 wagons per train
    Wagon* tailWagon = NULL;
    for (int i = 1; i <= 6; ++i) {
        Wagon* newWagon = new Wagon;
        newWagon->number = i;
        newWagon->seatsHead = NULL;
        newWagon->next = NULL;

        // Create seats for the wagon
        Seat* seatTail = NULL;
        for (char row = 'A'; row <= 'D'; ++row) {
            for (int col = 1; col <= 20; ++col) {
                Seat* newSeat = new Seat;
                newSeat->seatId = to_string(col) + row;
                newSeat->passenger = NULL;
                newSeat->next = NULL;

                if (!newWagon->seatsHead) {
                    newWagon->seatsHead = newSeat;
                } else {
                    seatTail->next = newSeat;
                }
                seatTail = newSeat;
            }
        }

        if (!newTrain->wagonsHead) {
            newTrain->wagonsHead = newWagon;
        } else {
            tailWagon->next = newWagon;
        }
        tailWagon = newWagon;
    }

    return newTrain;
}


// Function to find or add a city to the graph
CityNode* findOrAddCity(CityNode*& head, const string& cityName) {
    CityNode* current = head;
    CityNode* prev = NULL;
    while (current) {
        if (current->cityName == cityName) {
            return current;  // City found
        }
        prev = current;
        current = current->nextCity;
    }
    // If city not found, create a new city node
    CityNode* newCity = createCityNode(cityName);
    if (!prev) {
        head = newCity;
    } else {
        prev->nextCity = newCity;
    }
    return newCity;
}

bool trainExists(CityNode* graphHead, int trainId) {
    CityNode* currentCity = graphHead;
    while (currentCity) {
        RouteNode* currentRoute = currentCity->routesHead;
        while (currentRoute) {
            if (currentRoute->train->id == trainId) {
                return true; // Train with this ID exists
            }
            currentRoute = currentRoute->nextRoute;
        }
        currentCity = currentCity->nextCity;
    }
    return false; // Train with this ID does not exist
}

// Function to add a bidirectional route to the graph
void addRoute(CityNode*& graphHead, int trainId, const string& originName, const string& destinationName, const string* stations, int numStations) {
    // Find or add the origin city
    CityNode* originCity = findOrAddCity(graphHead, originName);
    CityNode* destinationCity = findOrAddCity(graphHead, destinationName);

    // Create a new train
    Train* train = createTrain(trainId, originCity, destinationCity, stations, numStations);

    // Create a route from origin to destination
    RouteNode* newRoute = new RouteNode;
    newRoute->destination = destinationCity;
    newRoute->train = train;
    newRoute->nextRoute = originCity->routesHead;
    originCity->routesHead = newRoute;

    cout << "Train and routes added successfully!\n";
}

// Function to add a passenger to a specific seat
bool addPassenger(Train* train, int wagonNum, const string& seatId, const Passenger& passenger) {
    // Check if the Passenger ID already exists in the same train
    Wagon* currentWagon = train->wagonsHead;
    while (currentWagon) {
        Seat* currentSeat = currentWagon->seatsHead;
        while (currentSeat) {
            if (currentSeat->passenger && currentSeat->passenger->id == passenger.id) {
                cout << "Error: Passenger ID " << passenger.id << " already exists in this train.\n";
                return false;  // Duplicate Passenger ID found in the same train
            }
            currentSeat = currentSeat->next;
        }
        currentWagon = currentWagon->next;
    }

    // Validate if the train serves the specified origin and destination
    bool originFound = false, destinationFound = false;
    StationNode* currentStation = train->stationsHead;

    // Check start and end cities first
    if (train->origin->cityName == passenger.origin) originFound = true;
    if (train->destination->cityName == passenger.destination) destinationFound = true;

    // Traverse the stations list to find origin and destination
    while (currentStation) {
        if (currentStation->stationName == passenger.origin) {
            originFound = true;
        }
        if (originFound && currentStation->stationName == passenger.destination) {
            destinationFound = true;
            break;  // Destination found after origin, valid route
        }
        currentStation = currentStation->nextStation;
    }

    if (!originFound || !destinationFound) {
        cout << "Error: Train does not serve the specified origin or destination in order.\n";
        return false;
    }

    // Find the specified wagon
    currentWagon = train->wagonsHead;
    for (int i = 1; i < wagonNum && currentWagon; ++i) {
        currentWagon = currentWagon->next;
    }
    if (!currentWagon) {
        cout << "Error: Wagon number not found.\n";
        return false;  // Wagon not found
    }

    // Find the specified seat in the wagon
    Seat* currentSeat = currentWagon->seatsHead;
    while (currentSeat) {
        if (currentSeat->seatId == seatId && !currentSeat->passenger) {
            currentSeat->passenger = new Passenger(passenger);
            return true;  // Passenger added successfully
        }
        currentSeat = currentSeat->next;
    }

    cout << "Error: Seat not found or already occupied.\n";
    return false;  // Seat not found or already occupied
}

// Function to display passengers on a train
void displayPassengersOnTrain(Train* train) {
    Wagon* currentWagon = train->wagonsHead;
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

void searchPassengersByName(CityNode* graphHead, const string& firstName, const string& lastName) {
    bool found = false;

    CityNode* currentCity = graphHead;
    while (currentCity) {
        RouteNode* currentRoute = currentCity->routesHead;
        while (currentRoute) {
            Train* train = currentRoute->train;

            if (currentCity == train->origin) {
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
            currentRoute = currentRoute->nextRoute;
        }
        currentCity = currentCity->nextCity;
    }

    if (!found) {
        cout << "No passengers found with the given name.\n";
    }
}


// Function to display the routes in the graph
void displayRoutes(CityNode* graphHead) {
    cout << "\nTrain Routes with Stopping Stations:\n";
    CityNode* currentCity = graphHead;
    while (currentCity) {
        RouteNode* currentRoute = currentCity->routesHead;
        while (currentRoute) {
            Train* train = currentRoute->train;
            cout << currentCity->cityName;

            // Display linked list of stopping stations
            StationNode* currentStation = train->stationsHead;
            while (currentStation) {
                cout << " -> " << currentStation->stationName;
                currentStation = currentStation->nextStation;
            }

            cout << " -> " << currentRoute->destination->cityName;
            cout << " (Train " << train->id << ")\n";
            currentRoute = currentRoute->nextRoute;
        }
        currentCity = currentCity->nextCity;
    }
}

int main() {
    CityNode* graphHead = NULL;

    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Add Train\n";
        cout << "2. Add Passenger\n";
        cout << "3. Display Routes\n";
        cout << "4. Display Passengers on Train\n";
        cout << "5. Search Passanger By Name on Train\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            int trainId, numStations;
            string origin, destination;
            cout << "Enter Train ID: ";
            cin >> trainId;

            // Check if the train ID already exists
            if (trainExists(graphHead, trainId)) {
                cout << "Error: A train with ID " << trainId << " already exists. Cannot add duplicate trains.\n";
                continue; // Go back to the menu
            }

            cout << "Enter Origin City: ";
            cin >> origin;
            cout << "Enter Destination City: ";
            cin >> destination;
            cout << "Enter Number of Stopping Stations: ";
            cin >> numStations;

            string stations[10];
            cin.ignore(); // Clear the newline character from the input buffer
            for (int i = 0; i < numStations; ++i) {
                cout << "Enter Station " << i + 1 << ": ";
                getline(cin, stations[i]);
            }

            // Add the train and route
            addRoute(graphHead, trainId, origin, destination, stations, numStations);
        } else if (choice == 2) {
            int trainId, wagonNum;
            string seatId, pid, firstName, lastName, origin, destination;

            cout << "Enter Train ID: ";
            cin >> trainId;

            Train* train = NULL;
            CityNode* currentCity = graphHead;
            while (currentCity) {
                RouteNode* currentRoute = currentCity->routesHead;
                while (currentRoute) {
                    if (currentRoute->train->id == trainId) {
                        train = currentRoute->train;
                        break;
                    }
                    currentRoute = currentRoute->nextRoute;
                }
                if (train) break;
                currentCity = currentCity->nextCity;
            }

            if (!train) {
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

            Passenger passenger = {pid, firstName, lastName, origin, destination, NULL};
            if (addPassenger(train, wagonNum, seatId, passenger)) {
                cout << "Passenger added successfully!\n";
            } else {
                cout << "Failed to add passenger. Seat may already be occupied.\n";
            }
        } else if (choice == 3) {
            displayRoutes(graphHead);
        } else if (choice == 4) {
            int trainId;
            cout << "Enter Train ID: ";
            cin >> trainId;

            Train* train = NULL;
            CityNode* currentCity = graphHead;
            while (currentCity) {
                RouteNode* currentRoute = currentCity->routesHead;
                while (currentRoute) {
                    if (currentRoute->train->id == trainId) {
                        train = currentRoute->train;
                        break;
                    }
                    currentRoute = currentRoute->nextRoute;
                }
                if (train) break;
                currentCity = currentCity->nextCity;
            }

            if (!train) {
                cout << "Train not found.\n";
            } else {
                displayPassengersOnTrain(train);
            }
        } else if (choice == 5) {
            string firstName, lastName;
            cout << "Enter First Name (use '-' for wildcard): ";
            cin.ignore();
            getline(cin, firstName);
            cout << "Enter Last Name (use '-' for wildcard): ";
            getline(cin, lastName);
            searchPassengersByName(graphHead, firstName, lastName);
        } else if (choice == 6) {
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}
