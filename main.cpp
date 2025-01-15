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

struct Train {
    int id;
    CityNode* origin;
    CityNode* destination;
    string stoppingStations[10];
    int numStations;
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
    newTrain->numStations = numStations;
    for (int i = 0; i < numStations; ++i) {
        newTrain->stoppingStations[i] = stations[i];
    }
    newTrain->wagonsHead = NULL;
    newTrain->nextTrain = NULL;

    Wagon* tail = NULL;
    for (int i = 1; i <= 6; ++i) {  // Initialize 6 wagons per train
        Wagon* newWagon = new Wagon;
        newWagon->number = i;
        newWagon->seatsHead = NULL;
        newWagon->next = NULL;

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
            tail->next = newWagon;
        }
        tail = newWagon;
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

// Function to add a bidirectional route to the graph
void addBidirectionalRoute(CityNode*& graphHead, int trainId, const string& originName, const string& destinationName, const string* stations, int numStations) {
    // Find or add the origin city
    CityNode* originCity = findOrAddCity(graphHead, originName);

    // Find or add the destination city
    CityNode* destinationCity = findOrAddCity(graphHead, destinationName);

    // Create a new train
    Train* train = createTrain(trainId, originCity, destinationCity, stations, numStations);

    // Create route from origin to destination
    RouteNode* route1 = new RouteNode;
    route1->destination = destinationCity;
    route1->train = train;
    route1->nextRoute = originCity->routesHead;
    originCity->routesHead = route1;

    // Create route from destination to origin (reverse connection)
    RouteNode* route2 = new RouteNode;
    route2->destination = originCity;
    route2->train = train;
    route2->nextRoute = destinationCity->routesHead;
    destinationCity->routesHead = route2;
}

// Function to add a passenger to a specific seat
bool addPassenger(Train* train, int wagonNum, const string& seatId, const Passenger& passenger) {
    Wagon* currentWagon = train->wagonsHead;
    for (int i = 1; i < wagonNum && currentWagon; ++i) {
        currentWagon = currentWagon->next;
    }
    if (!currentWagon) return false;

    Seat* currentSeat = currentWagon->seatsHead;
    while (currentSeat) {
        if (currentSeat->seatId == seatId && !currentSeat->passenger) {
            currentSeat->passenger = new Passenger(passenger);
            return true;
        }
        currentSeat = currentSeat->next;
    }
    return false;
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

// Function to display the routes in the graph
void displayRoutes(CityNode* graphHead) {
    cout << "\nTrain Routes (Bidirectional):\n";
    CityNode* currentCity = graphHead;
    while (currentCity) {
        cout << currentCity->cityName << " -> ";
        RouteNode* currentRoute = currentCity->routesHead;
        while (currentRoute) {
            cout << currentRoute->destination->cityName << " (Train " << currentRoute->train->id << ") ";
            currentRoute = currentRoute->nextRoute;
        }
        cout << "\n";
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
        cout << "5. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            int trainId, numStations;
            string origin, destination;
            cout << "Enter Train ID: ";
            cin >> trainId;
            cout << "Enter Origin City: ";
            cin >> origin;
            cout << "Enter Destination City: ";
            cin >> destination;
            cout << "Enter Number of Stopping Stations: ";
            cin >> numStations;

            string stations[10];
            cin.ignore();
            for (int i = 0; i < numStations; ++i) {
                cout << "Enter Station " << i + 1 << ": ";
                getline(cin, stations[i]);
            }

            addBidirectionalRoute(graphHead, trainId, origin, destination, stations, numStations);
            cout << "Train and routes added successfully!\n";
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
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}
