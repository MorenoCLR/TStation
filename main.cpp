#include <iostream>
#include <string>
#include <map>

using namespace std;

struct Passenger {
    string id;
    string firstName;
    string lastName;
    string origin;
    string destination;
    Passenger* next;

    Passenger(string pid, string fname, string lname, string porigin, string pdestination) {
        id = pid;
        firstName = fname;
        lastName = lname;
        origin = porigin;
        destination = pdestination;
        next = nullptr;
    }
};

struct Seat {
    string seatId;
    Passenger* passenger;
    Seat* next;

    Seat(string sid) {
        seatId = sid;
        passenger = nullptr;
        next = nullptr;
    }
};

struct Wagon {
    int number;
    Seat* seatsHead;
    Wagon* next;

    Wagon(int num) {
        number = num;
        seatsHead = nullptr;
        next = nullptr;

        // Initialize seats in the wagon (manual linked list).
        Seat* tail = nullptr;
        for (char row = 'A'; row <= 'D'; ++row) {
            for (int col = 1; col <= 20; ++col) {
                Seat* newSeat = new Seat(to_string(col) + row);
                if (!seatsHead) {
                    seatsHead = newSeat;
                } else {
                    tail->next = newSeat;
                }
                tail = newSeat;
            }
        }
    }
};

struct Train {
    int id;
    string stoppingStations[10];
    int numStations;
    Wagon* wagonsHead;

    Train(int tid, const string* stations, int stationCount, int numWagons) {
        id = tid;
        numStations = stationCount;
        for (int i = 0; i < stationCount; ++i) {
            stoppingStations[i] = stations[i];
        }
        wagonsHead = nullptr;

        // Initialize wagons (manual linked list).
        Wagon* tail = nullptr;
        for (int i = 1; i <= numWagons; ++i) {
            Wagon* newWagon = new Wagon(i);
            if (!wagonsHead) {
                wagonsHead = newWagon;
            } else {
                tail->next = newWagon;
            }
            tail = newWagon;
        }
    }
};

class TrainSystem {
private:
    map<int, Train*> trains;

public:
    void addTrain(int id, const string* stations, int stationCount, int numWagons) {
        if (trains.find(id) != trains.end()) {
            cout << "Train with ID " << id << " already exists.\n";
            return;
        }
        trains[id] = new Train(id, stations, stationCount, numWagons);
    }

    void addPassengerInteractive() {
        int trainId, wagonNum;
        string seatId, pid, firstName, lastName, origin, destination;

        cout << "Enter Train ID: ";
        cin >> trainId;
        if (trains.find(trainId) == trains.end()) {
            cout << "Train not found.\n";
            return;
        }

        cout << "Enter Wagon Number: ";
        cin >> wagonNum;

        Train* train = trains[trainId];
        Wagon* currentWagon = train->wagonsHead;
        for (int i = 1; i < wagonNum && currentWagon; ++i) {
            currentWagon = currentWagon->next;
        }
        if (!currentWagon) {
            cout << "Invalid Wagon Number.\n";
            return;
        }

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
            return;
        }

        Passenger passenger(pid, firstName, lastName, origin, destination);
        if (addPassenger(train, currentWagon, seatId, passenger)) {
            cout << "Passenger added successfully!\n";
        } else {
            cout << "Failed to add passenger. Seat may already be occupied.\n";
        }
    }

    bool addPassenger(Train* train, Wagon* wagon, const string& seatId, const Passenger& passenger) {
        Seat* currentSeat = wagon->seatsHead;
        while (currentSeat) {
            if (currentSeat->seatId == seatId && !currentSeat->passenger) {
                currentSeat->passenger = new Passenger(passenger);
                return true;
            }
            currentSeat = currentSeat->next;
        }
        return false;
    }

    void searchPassengersByName(const string& firstName, const string& lastName) {
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

    void displayPassengersOnTrain(int trainId) {
        if (trains.find(trainId) == trains.end()) {
            cout << "Train with ID " << trainId << " not found.\n";
            return;
        }

        Train* train = trains[trainId];
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

    ~TrainSystem() {
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
    }
};

int main() {
    TrainSystem system;

    // Predefined trains.
    string stations1[] = {"CityA", "CityB", "CityC"};
    system.addTrain(1, stations1, 3, 6);

    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Add Passenger\n";
        cout << "2. Search Passenger by Name\n";
        cout << "3. Display All Passengers on Train\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            system.addPassengerInteractive();
        } else if (choice == 2) {
            string firstName, lastName;
            cout << "Enter First Name (or '-' if empty): ";
            cin.ignore();
            getline(cin, firstName);

            cout << "Enter Last Name (or '-' if empty): ";
            getline(cin, lastName);

            system.searchPassengersByName(firstName, lastName);
        } else if (choice == 3) {
            int trainId;
            cout << "Enter Train ID: ";
            cin >> trainId;
            system.displayPassengersOnTrain(trainId);
        } else if (choice == 4) {
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
