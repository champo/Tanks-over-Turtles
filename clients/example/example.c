#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char* get_random_direction() {

	static const char* directions[] = { "UP", "RIGHT", "DOWN", "LEFT" };

	return directions[ rand() % 4 ];
}

int main( void ) {

	int team_id, shoot_range, tank_id, tank_dis;
	const char* direction;

	srand( time( NULL ) );

	scanf( "%d %d\n", &team_id, &shoot_range );

	while( 1 ) {

		direction = get_random_direction();

		printf( "LASER %s\n", direction );
		scanf( "%d %d\n", &tank_id, &tank_dis );

		if ( tank_id != team_id ) {
			if ( tank_dis < shoot_range ) {
				printf( "SHOOT %s\n", direction );
			}
		}

		printf( "MOVE %s\n", get_random_direction() );
	}

	return 0;
}