#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

string get_random_direction(){

	static const string directions[] = {"UP", "RIGHT", "DOWN", "LEFT"};
	
	return directions[ rand() % 4 ];
}

int main(){

	srand( time( NULL ) );

    int team_id, shoot_range;

    cin >> team_id >> shoot_range;

    while (true) {

        string direction = get_random_direction();

        cout << "LASER " << direction << endl;

        int tank_id, tank_dis;

        cin >> tank_id >> tank_dis;

        if ( tank_id != team_id ) {
            if ( tank_dis < shoot_range ) {
                cout << "SHOOT " << direction << endl;
            }
        }

        cout << "MOVE " << get_random_direction() << endl;
    }

    return 0;
}
