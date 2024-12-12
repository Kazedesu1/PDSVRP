#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

// tinh khoang cach cac node
double tinhkc(const vector<vector<double>>& mat, int node1, int node2) {
    double c = 0;
    c = sqrt(pow(mat[node1][1] - mat[node2][1], 2) + pow(mat[node1][2] - mat[node2][2], 2));
    return c;
}
double tinhkcmanhattan(const vector<vector<double>>& mat, int node1, int node2) {
    return abs(mat[node1][1] - mat[node2][1]) + abs(mat[node1][2] - mat[node2][2]);
}

struct Trucks {
    double current_capacity = 0;
    double total_time = 0;
    vector<int> route = {0,0};
};

struct Drones {
    int pos = 0;
    double total_time = 0; 
    vector<int> route;

};

// tinh chi phi truck
double tinhCostTruck(const vector<int>& route, const vector<vector<double>>& Ctruck) {
    double totalCost = 0;
    int currentNode = 0;  

    for (int i = 0; i < route.size(); i++) {
        int nextNode = route[i];
        totalCost += Ctruck[currentNode][nextNode];  
        currentNode = nextNode;  
    }

    totalCost += Ctruck[currentNode][0];

    return totalCost;

}

// tinh thoi gian khi them mot node cua truck
double tinhTotaltimetruck(Trucks truck, const vector<vector<double>>& Ttruck, int node, int insertPos) {
    vector<int> newRoute = truck.route;
    newRoute.insert(newRoute.begin() + insertPos, node);

    double total_time = 0;
    int prevNode = 0; 

    for (int i = 0; i < newRoute.size(); ++i) {
        int currentNode = newRoute[i];
        total_time += Ttruck[prevNode][currentNode]; 
        prevNode = currentNode;
    }

    total_time += Ttruck[prevNode][0]; 

    return total_time;
}

// tinh chi phi khi them mot node cua truck
double tinhCostTruckTang(const vector<int>& route, const vector<vector<double>>& Ctruck, int node, int insertPos) {
    double totalCost = 0;

    int prevNode = route[insertPos - 1];
    int nextNode = route[insertPos];

    totalCost -= Ctruck[prevNode][nextNode];
    totalCost += Ctruck[prevNode][node];
    totalCost += Ctruck[node][nextNode];

    return totalCost;
}

// tinh chi phi drone
double tinhCostDrone(const vector<int>& route, const vector<double>& Cdrone) {
    double cost = 0;
    for (int i = 0; i < route.size(); ++i) {
        cost += Cdrone[route[i]];
    }
    return cost;
}


int main() {
    ifstream file("30-c-0-c.txt");
    if (!file.is_open()) {
        cerr << "Không thể mở file!" << endl;
        return 1;
    }

    string line;
    int num_drones, num_trucks;
    double truck_cap, drone_cap, truck_speed, drone_speed, drone_endurance;
    double drone_time_limit, truck_time_limit, truck_unit_cost, drone_unit_cost;

    // doc file
    while (getline(file, line)) {
        if (line.find("NUM DRONES") != string::npos) {
            sscanf_s(line.c_str(), "NUM DRONES,%d", &num_drones);
        }
        else if (line.find("NUM TRUCKS") != string::npos) {
            sscanf_s(line.c_str(), "NUM TRUCKS,%d", &num_trucks);
        }
        else if (line.find("TRUCK CAP") != string::npos) {
            sscanf_s(line.c_str(), "TRUCK CAP,%lf", &truck_cap);
        }
        else if (line.find("DRONE CAP") != string::npos) {
            sscanf_s(line.c_str(), "DRONE CAP,%lf", &drone_cap);
        }
        else if (line.find("TRUCK SPEED") != string::npos) {
            sscanf_s(line.c_str(), "TRUCK SPEED,%lf", &truck_speed);
        }
        else if (line.find("DRONE SPEED") != string::npos) {
            sscanf_s(line.c_str(), "DRONE SPEED,%lf", &drone_speed);
        }
        else if (line.find("DRONE ENDURANCE") != string::npos) {
            sscanf_s(line.c_str(), "DRONE ENDURANCE,%lf", &drone_endurance);
        }
        else if (line.find("DRONE TIME LIMIT") != string::npos) {
            sscanf_s(line.c_str(), "DRONE TIME LIMIT,%lf", &drone_time_limit);
        }
        else if (line.find("TRUCK TIME LIMIT") != string::npos) {
            sscanf_s(line.c_str(), "TRUCK TIME LIMIT,%lf", &truck_time_limit);
        }
        else if (line.find("TRUCK UNIT COST") != string::npos) {
            sscanf_s(line.c_str(), "TRUCK UNIT COST,%lf", &truck_unit_cost);
        }
        else if (line.find("DRONE UNIT COST") != string::npos) {
            sscanf_s(line.c_str(), "DRONE UNIT COST,%lf", &drone_unit_cost);
        }
        else {
            break;
        }
    }

    vector<vector<double>> matrix;
    do {
        vector<double> row;
        stringstream ss(line);
        double value;
        while (ss >> value) {
            row.push_back(value);
            if (ss.peek() == ',') ss.ignore();
        }
        if (!row.empty()) {
            matrix.push_back(row);
        }
    } while (getline(file, line));

    file.close();
    vector<int> N;
    vector<int> C;
    vector<int> Ct;
    vector<int> Cf;

    vector<vector<double>> Ttruck(matrix.size(), vector<double>(matrix.size()));
    vector<vector<double>> Ctruck(matrix.size(), vector<double>(matrix.size()));

    //tinh Ttruck va Ctruck
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix.size(); j++) {
            Ttruck[i][j] = tinhkc(matrix, i, j) / truck_speed;
            Ctruck[i][j] = tinhkc(matrix, i, j) * truck_unit_cost;
        }
    }

    vector<double> Tdrone(matrix.size());
    vector<double> Cdrone(matrix.size());

    //tinh Tdrone va Cdrone
    for (int i = 0; i < matrix.size(); i++) {
        Tdrone[i] = tinhkc(matrix, 0, i) * 2 / drone_speed;
        Cdrone[i] = tinhkc(matrix, 0, i) * drone_unit_cost;

    }

    //chia Cf Ct C va N
    for (int i = 0; i < matrix.size(); i++) {
        if (drone_cap >= matrix[i][3] && Tdrone[i] <= drone_endurance)
            Cf.push_back(matrix[i][0]);
        else
            Ct.push_back(matrix[i][0]);
        N.push_back(matrix[i][0]);
        if (i != 0)
            C.push_back(matrix[i][0]);
    }

    vector<Trucks> trucks(num_trucks);
    vector<Drones> drones(num_drones);
    vector<int> remainingNodes = C;

    // greedy
    while (!remainingNodes.empty()) {
        int bestTruck = -1;
        int bestTruckNode = -1;
        double minTruckCostIncrease = numeric_limits<double>::infinity();
        int bestTruckPos = -1;  

        for (int i = 0; i < trucks.size(); ++i) {
            for (int node : remainingNodes) {
                if (trucks[i].current_capacity + matrix[node][3] <= truck_cap) {
                    for (int j = 1; j < trucks[i].route.size(); ++j) {  
                        double costIncrease = tinhCostTruckTang(trucks[i].route, Ctruck, node, j);

                        double newTotalTime = tinhTotaltimetruck(trucks[i], Ttruck, node,j);

                        if (newTotalTime <= truck_time_limit && costIncrease < minTruckCostIncrease) {
                            minTruckCostIncrease = costIncrease;
                            bestTruck = i;
                            bestTruckNode = node;
                            bestTruckPos = j;
                        }
                    }
                }
            }
        }

        int bestDrone = -1;
        int bestDroneNode = -1;
        double minDroneCost = numeric_limits<double>::infinity();

        for (int i = 0; i < drones.size(); ++i) {
            for (int node : remainingNodes) {
                if (find(Cf.begin(), Cf.end(), node) != Cf.end()) {
                    double droneCost = Cdrone[node];
                    if (droneCost < minDroneCost) {
                        if (drones[i].total_time + Tdrone[node] <= drone_time_limit) {
                            minDroneCost = droneCost;
                            bestDrone = i;
                            bestDroneNode = node;
                        }
                    }
                }
            }
        }

        // Nếu drone có chi phí thấp hơn truck thì chọn drone
        if (bestDroneNode != -1 && minDroneCost < minTruckCostIncrease) {
            drones[bestDrone].route.push_back(bestDroneNode);
            drones[bestDrone].total_time += Tdrone[bestDroneNode];
            remainingNodes.erase(remove(remainingNodes.begin(), remainingNodes.end(), bestDroneNode), remainingNodes.end());
        }
        // Nếu truck có chi phí thấp hơn hoặc bằng drone, chọn truck và chèn node vào vị trí tốt nhất
        else if (bestTruckNode != -1) {
            trucks[bestTruck].route.insert(trucks[bestTruck].route.begin() + bestTruckPos, bestTruckNode);
            trucks[bestTruck].current_capacity += matrix[bestTruckNode][3];
            trucks[bestTruck].total_time = tinhTotaltimetruck(trucks[bestTruck], Ttruck, bestTruckNode, bestTruckPos);
            remainingNodes.erase(remove(remainingNodes.begin(), remainingNodes.end(), bestTruckNode), remainingNodes.end());
        }
    }


    // in kq
    double totalCost = 0;

    for (int i = 0; i < trucks.size(); ++i) {
        double truckCost = tinhCostTruck(trucks[i].route, Ctruck);
        totalCost += truckCost;
        cout << "T" << i << "\t" << trucks[i].total_time << "\t";
        for (int node : trucks[i].route) {
            cout << node << " ";
        }
        cout << "\tCost: " << truckCost << endl;
    }

    for (int i = 0; i < drones.size(); ++i) {
        double droneCost = tinhCostDrone(drones[i].route, Cdrone);
        totalCost += droneCost;
        cout << "D" << i << "\t" << drones[i].total_time << "\t";
        for (int node : drones[i].route) {
            cout << node << " ";
        }
        cout << "\tCost: " << droneCost << endl;
    }

    cout << "Total cost: " << totalCost << endl;

    /*cout << "\nTtruck:" << endl;
    for (const auto& row : Ttruck) {
        for (const auto& element : row) {
            cout << element << " ";
        }
        cout << endl;
    }

    cout << "\nCtruck:" << endl;
    for (const auto& row : Ctruck) {
        for (const auto& element : row) {
            cout << element << " ";
        }
        cout << endl;
    }

    cout << "\nTdrone:" << endl;
    for (const auto& element : Tdrone) {
        cout << element << " ";
    }
    cout << endl;

    cout << "\nCdrone:" << endl;
    for (const auto& element : Cdrone) {
        cout << element << " ";
    }
    cout << endl;*/

    return 0;

}
