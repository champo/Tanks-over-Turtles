#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

string getRandomDirection(){

	string directions[] = {"UP", "RIGHT", "DOWN", "LEFT"};
	
	return directions[rand() % 4];
}

int main(){

    int TEAM_ID, SHOOT_RANGE;

    cin >> TEAM_ID >> SHOOT_RANGE;

    while (true) {
        string direction = getRandomDirection();

        cout << "LASER " << direction << endl;

        int TANK_ID, TANK_DIS;
        cin >> TANK_ID >> TANK_DIS;

        if ( TANK_ID && TANK_ID != TEAM_ID ) {
            if ( TANK_DIS < SHOOT_RANGE ) {
                cout << "SHOOT " << direction << endl;
            }
        }

        cout << "MOVE " << getRandomDirection() << endl;
    }

    return 0;
}
