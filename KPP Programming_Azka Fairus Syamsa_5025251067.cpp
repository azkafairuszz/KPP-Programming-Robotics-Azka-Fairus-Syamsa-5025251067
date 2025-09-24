#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <algorithm>

using namespace std;

const int MAX_ENERGY = 1000;
const int INF = 1e9+7;
string restPointStr, chargingStationStr, mechanical, electrical;
char start, target;
int n,m, jamAwal;

struct Edge{
    char to_node;
    int weight, obstacle;
};

struct State{
    char node;
    int energy, time, consumedEnergy;
    bool operator>(const State& other) const {
        return consumedEnergy > other.consumedEnergy;
    }
};


//OOP
class Graph{
    private:
        map<char, vector<Edge>> adjList;
        set<char> restPoint, chargingStation;

        int calculateEnergy(const Edge& edge, int currentTime){
            int baseEnergy = edge.weight + edge.obstacle;
            int currentHour = currentTime/60;
            // cout << currentTime << ' ';
            return (currentHour%2 != 0) ? (baseEnergy*1.3) : (baseEnergy*0.8);
        }
    
    public:
        void addEdge(char u, char v, int w, int o){
            adjList[u].push_back({v,w,o});
            adjList[v].push_back({u,w,o});
        }

        void setSpecialNodes(const set<char>& rests, const set<char>& charges){
            restPoint = rests;
            chargingStation = charges;
        }

        void dijkstra(char start, char target, int& minEnergy, map<pair<char,int>, pair<char,int>>& parentNode){
            priority_queue<State, vector<State>, greater<State>> pq;
            map<pair<char,int>, int> minEnergyMap;
            int initialTime = jamAwal*60;

            pq.push({start, MAX_ENERGY, initialTime, 0});
            minEnergyMap[{start, initialTime}] = 0;
            parentNode[{start, initialTime}] = {'\0', -1};

            minEnergy = -1;

            while(!pq.empty()){
                State currentState = pq.top();
                pq.pop();
                char u = currentState.node;
                int currentEnergy = currentState.energy;
                int currentTime = currentState.time;
                int consumedEnergy = currentState.consumedEnergy;
                // cout << currentTime << ' ';
                if(u == target){
                    minEnergy = consumedEnergy;
                    return;
                }
                if(consumedEnergy > minEnergyMap[{u, currentTime}]) continue;
                // cout << "cek ";
                if(restPoint.count(u)){
                    int currentHour = (currentTime/60);
                    if(currentHour%2 != 0){
                        int nextStartTime = (currentHour+1)*60;
                        int newTime = nextStartTime;
                        // cout << nextStartTime << ' ';
                        // cout << ((minEnergyMap.find({u, newTime}) == minEnergyMap.end()) ? "1 " : "0 ");
                        if(minEnergyMap.find({u, newTime}) == minEnergyMap.end() || consumedEnergy < minEnergyMap[{u, newTime}]){
                            minEnergyMap[{u, newTime}] = consumedEnergy;
                            pq.push({u, currentEnergy, newTime, consumedEnergy});
                            parentNode[{u, newTime}] = {u, currentTime};
                        }
                    }
                }

                if(chargingStation.count(u)) currentEnergy = MAX_ENERGY;

                if(adjList.count(u)){
                    for(const auto& edge : adjList.at(u)){
                        char v = edge.to_node;
                        // cout << currentTime << ' ';
                        int energyCost = calculateEnergy(edge, currentTime);

                        if(currentEnergy < energyCost) continue;

                        int newEnergy = currentEnergy - energyCost;
                        int newTime = currentTime + 2;
                        int newConsumedEnergy = consumedEnergy + energyCost;

                        if(minEnergyMap.find({v, newTime}) == minEnergyMap.end() || newConsumedEnergy < minEnergyMap[{v, newTime}]){
                            minEnergyMap[{v, newTime}] = newConsumedEnergy;
                            pq.push({v, newEnergy, newTime, newConsumedEnergy});
                            parentNode[{v, newTime}] = {u, currentTime};
                        }
                    }
                }
            }
        }
};


int main(){
    //Input
    Graph graph;
    cin >> n >> m;
    for(int i = 0; i < m; i++){
        char u,v;
        int w, o;
        cin >> u >> v >> w >> o;
        graph.addEdge(u,v,w,o);
    }
    cin >> start >> target;
    cin.ignore();
    getline(cin, restPointStr);
    getline(cin, chargingStationStr);
    getline(cin, mechanical);
    getline(cin, electrical);
    cin >> jamAwal;

    //Algorithm
    set<char> restPoint, chargingStation;
    if(restPointStr != "-"){
        stringstream ss(restPointStr);
        char node;
        while(ss >> node) restPoint.insert(node);
    }
    if(chargingStationStr != "-"){
        stringstream ss(chargingStationStr);
        char node;
        while(ss >> node) chargingStation.insert(node);
    }
    graph.setSpecialNodes(restPoint, chargingStation);
    int minEnergy;
    map<pair<char,int>, pair<char,int>> parentNode;

    graph.dijkstra(start, target, minEnergy, parentNode);
    //Output
    if(minEnergy == -1){
        cout << "Robot gagal dalam mencapai tujuan :(\n";
    }else{
        cout << "Total energi minimum: " << minEnergy << '\n';
        
        char finalNode = target;
        int finalTime = -1;
        for(const auto& entry : parentNode){
            if(entry.first.first == target){
                finalTime = entry.first.second;
                break;
            }
        }

        vector<pair<char,int>> pathStates;
        pair<char,int> currentState = {finalNode, finalTime};

        while(currentState.first != '\0' && currentState.second != -1){
            pathStates.insert(pathStates.begin(), currentState);
            if(parentNode.count(currentState)){
                currentState = parentNode.at(currentState);
            }else{
                break;
            }
        }

        cout << "Jalur: ";
        for(int i = 0; i < pathStates.size(); i++){
            if(i > 0 && pathStates[i].first == pathStates[i-1].first) continue;
            cout << pathStates[i].first;
            if(i < pathStates.size()-1) cout << " -> ";
        }

        vector<pair<char,int>> departPath;
        if(!pathStates.empty()){
            int n = pathStates.size();
            departPath.push_back(pathStates[n-1]);
            for(int i = n-2; i >= 0; i--){
                if(pathStates[i].first != pathStates[i+1].first){
                    departPath.push_back(pathStates[i]);
                }
            }
        }
        reverse(departPath.begin(), departPath.end());
        cout << "\nWaktu tiba:\n";
        for(const auto& state : departPath){
            cout <<  state.first << " (menit " << state.second - jamAwal*60 << ")\n";
        }
    }
    return 0;
}