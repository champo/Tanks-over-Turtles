#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

string getRandomDirection(){
    string direction;
    switch (rand()%4){
        case 0: direction = "UP"; break;
        case 1: direction = "DOWN"; break;
        case 2: direction = "LEFT"; break;
        case 3: direction = "RIGHT"; break;
    }
    return direction;
}

int main(){
    int TEAM, RANGE;

    cin >> TEAM >> RANGE;
    srand(0);

    while (true){
        string direction = getRandomDirection();

        cout << "LASER " << direction << endl;
        int what, distance;
        cin >> what >> distance;

        if (what && what != TEAM){
            if (distance < RANGE){
                cout << "SHOOT " << direction << endl;
            }
        }

        cout << "MOVE " << getRandomDirection() << endl;
    }

    return 0;
}

