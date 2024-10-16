#include <emscripten.h>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

class Request {
public:
    int floor;
    bool direction;
    int priority;

    Request(int f, bool d, int p) : floor(f), direction(d), priority(p) {}
};

class Elevator {
private:
    int currentFloor;
    bool direction;
    const int MAX_FLOOR = 10;
    const int MIN_FLOOR = 1;

public:
    Elevator() : currentFloor(1), direction(true) {}

    void move() {
        if (direction && currentFloor < MAX_FLOOR)
            currentFloor++;
        else if (!direction && currentFloor > MIN_FLOOR)
            currentFloor--;
    }

    void setDirection(bool dir) { direction = dir; }
    int getCurrentFloor() const { return currentFloor; }
    bool getDirection() const { return direction; }
};

class ElevatorSystem {
private:
    Elevator elevator;
    std::vector<Request> requests;

    void sortRequests() {
        std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {
            return a.priority > b.priority;
        });
    }

public:
    void addRequest(int floor, bool direction, int priority) {
        if (floor < 1 || floor > 10 || priority < 1 || priority > 5) {
            return;
        }
        requests.emplace_back(floor, direction, priority);
    }

    std::string processRequests() {
        std::ostringstream output;
        while (!requests.empty()) {
            sortRequests();
            Request currentRequest = requests.front();
            requests.erase(requests.begin());

            output << "Processing request: Floor " << currentRequest.floor 
                   << ", Direction: " << (currentRequest.direction ? "Up" : "Down")
                   << ", Priority: " << currentRequest.priority << "\n";

            while (elevator.getCurrentFloor() != currentRequest.floor) {
                elevator.setDirection(elevator.getCurrentFloor() < currentRequest.floor);
                elevator.move();
                output << "Elevator at floor " << elevator.getCurrentFloor() 
                       << ", moving " << (elevator.getDirection() ? "up" : "down") << "\n";
            }

            output << "Request completed.\n";
        }
        return output.str();
    }

    std::string getStatus() {
        std::ostringstream status;
        status << "Current Floor: " << elevator.getCurrentFloor() << "\n";
        status << "Direction: " << (elevator.getDirection() ? "Up" : "Down") << "\n";
        status << "Pending Requests: " << requests.size() << "\n";

        if (!requests.empty()) {
            status << "Requests in queue:\n";
            for (const auto& req : requests) {
                status << "  - Floor: " << req.floor 
                       << ", Direction: " << (req.direction ? "Up" : "Down")
                       << ", Priority: " << req.priority << "\n";
            }
        } else {
            status << "No pending requests.\n";
        }

        return status.str();
    }
};

// Global ElevatorSystem instance
ElevatorSystem elevatorSystem;

// Exported C functions
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void addRequest(int floor, bool direction, int priority) {
        elevatorSystem.addRequest(floor, direction, priority);
    }

    EMSCRIPTEN_KEEPALIVE
    char* processRequests() {
        std::string result = elevatorSystem.processRequests();
        char* cstr = new char[result.length() + 1];
        std::strcpy(cstr, result.c_str());
        return cstr;
    }

    EMSCRIPTEN_KEEPALIVE
    char* getStatus() {
        std::string status = elevatorSystem.getStatus();
        char* cstr = new char[status.length() + 1];
        std::strcpy(cstr, status.c_str());
        return cstr;
    }
}