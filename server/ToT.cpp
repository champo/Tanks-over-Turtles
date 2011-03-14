#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
using namespace std;


// 0 1 2 3 4 5
#define VERBOSITY 0

#define INFORM( n, ... ) do{ if( VERBOSITY >= n ) printf( __VA_ARGS__ ); }while(0)


typedef int pipe_t[2];

struct tank {
	int x,y;
	int player, tn;
	
	tank () {}
	tank ( int x, int y, int player, int tn ): x(x), y(y), player(player), tn(tn) {}
};

struct play {
	int player, tn, direction;
	
	play () {}
	play ( int player, int tn, int direction ): player(player), tn(tn), direction(direction) {}
};

int movY[] = {-1,0,1,0};
int movX[] = {0,1,0,-1};


void readln( int fd, char* str) {
	
	int n = 0;

	do {
		str += n;
		n = read( fd, str, 1 );
	} while( *str != '\n' );

	*str = 0;
}

int main( int argc, char** argv ) {

	int players, tanks_per_player, board_size, shooting_range, max_time;
	
	if ( argc < 7 ) {
		printf( "usage: ToT tanks_per_player board_size shooting_range max_time players [player1program] [player2program] ...\n" );
		exit(1);
	}
	
	sscanf( argv[1], "%d", &tanks_per_player );
	sscanf( argv[2], "%d", &board_size );
	sscanf( argv[3], "%d", &shooting_range );
	sscanf( argv[4], "%d", &max_time );
	sscanf( argv[5], "%d", &players );

	if ( argc != 6 + players ) {
		printf( "usage: ToT tanks_per_player board_size shooting_range max_time players [player1program] [player2program] ...\n" );
		exit(1);
	}
	
	vector<vector<pair<pipe_t,pipe_t> > > pipes( players, vector<pair<pipe_t,pipe_t> >( tanks_per_player ) );
	vector<vector<int> > pids( players, vector<int>( tanks_per_player ) );

	// create pipes
	for( vector<vector<pair<pipe_t,pipe_t> > >::iterator player = pipes.begin(); player != pipes.end(); ++player ) {
		for( vector<pair<pipe_t,pipe_t> >::iterator tn = player->begin(); tn != player->end(); ++tn ) {
			if( pipe( tn->first ) < 0 || pipe( tn->second ) < 0 ) {
				printf( "Error con los pipes putaso\n" );
				exit(1);
			}
		}
	}

	// create subprocesses
	for( int player = 0; player < players; player++ ) {
		for( int tn = 0; tn < tanks_per_player; tn++ ) {
			
			int read_fd = pipes[ player ][ tn ].second[0];
			int write_fd = pipes[ player ][ tn ].first[1];
			
			if( ( pids[ player ][ tn ] = fork() ) < 0 ) {
				
				printf( "Error con los fork putaso\n" );
				exit(1);
			}
			if ( pids[ player ][ tn ] == 0 ) {
				
				dup2( read_fd, 0 );
				dup2( write_fd, 1 );
				
				execl( argv[ player + 6 ], (char*) 0 );
			}
			
			close( read_fd );
			close( write_fd );
		}
	}

	// init data structures
	int total_tanks = players * tanks_per_player;
	int remaining_players = players;
	int time = 0;

	vector<int> left_tanks( players, tanks_per_player );
	vector<map<int, tank> > tanks( players, map<int,tank>() );
	vector<map<int, pair<int, int> > > row_tank_list( board_size, map<int,pair<int, int> >() );
	vector<map<int, pair<int, int> > > col_tank_list( board_size, map<int,pair<int, int> >() );

	// create tanks
	for( int player = 0; player < players; player++ ) {
		for( int tn = 0; tn < tanks_per_player; tn++ ) {

			int x = rand() % board_size;
			int y = rand() % board_size;

			tanks[player].insert( make_pair( tn, tank( x, y, player, tn ) ) );
			row_tank_list[y].insert( make_pair( x, make_pair( player, tn ) ) );
			col_tank_list[x].insert( make_pair( y, make_pair( player, tn ) ) );
		}
	}

	// init tanks
	for( int player = 0; player < players; player++ ) {
		for( int tank = 0; tank < tanks_per_player; tank++ ) {
			
			int write_fd = pipes[ player ][ tank ].second[1];
			char str[20];

			write( write_fd, str, sprintf( str, "%d %d\n", player, shooting_range ) );
		}
	}
	
	// game
	while ( time++ < max_time && remaining_players > 1 ) {

		if( time % 1000 )
			INFORM( 1, "time: %d\n", time );
		INFORM( 2, "time: %d\n", time );
		
		list<play> move;
		list<play> shoot;
		list<play> laser;

		// fetch plays
		for( int player = 0; player < players; player++ ) {
			for( map<int,tank>::iterator tn = tanks[player].begin(); tn != tanks[player].end(); ++tn ) {
				int dir;
				char str[20], command[10], direction[10];
				int read_fd = pipes[ player ][ tn->second.tn ].first[0];
		
				readln( read_fd, str );
				sscanf( str, "%s %s", command, direction );

				INFORM( 4, "pos: %d %d\n", tn->second.x, tn->second.y );
				INFORM( 2, "%s\n", str );

				if( !strcmp( direction, "UP" ) ) dir = 0;
				else if( !strcmp( direction, "RIGHT" ) ) dir = 1;
				else if( !strcmp( direction, "DOWN" ) ) dir = 2;
				else if( !strcmp( direction, "LEFT" ) ) dir = 3;
				
				if( !strcmp( command, "MOVE" ) ) move.push_back( play( player, tn->second.tn, dir ) );
				else if( !strcmp( command, "SHOOT" ) ) shoot.push_back( play( player, tn->second.tn, dir ) );
				else if( !strcmp( command, "LASER" ) ) laser.push_back( play( player, tn->second.tn, dir ) );
			}
		}

		// move
		for( list<play>::iterator play = move.begin(); play != move.end(); ++play ) {

			map<int, tank>::iterator tn = tanks[ play->player ].find( play->tn );
			row_tank_list[ tn->second.y ].erase( tn->second.x );
			col_tank_list[ tn->second.x ].erase( tn->second.y );
			
			tn->second.x += movX[ play->direction ] + board_size;
			tn->second.x %= board_size;
			tn->second.y += movY[ play->direction ] + board_size;
			tn->second.y %= board_size;
			
			row_tank_list[ tn->second.y ].insert( make_pair( tn->second.x, make_pair( tn->second.player, tn->second.tn ) ) );
			col_tank_list[ tn->second.x ].insert( make_pair( tn->second.y, make_pair( tn->second.player, tn->second.tn ) ) );
		}

		// shoot
		map<pair<int,int>, tank> kills;
		for( list<play>::iterator play = shoot.begin(); play != shoot.end(); ++play ) {
			
			map<int, tank>::iterator tn = tanks[ play->player ].find( play->tn );
			map<int, pair<int,int> >::iterator it;
			int distance = 0;

			switch( play->direction ) {
				case 0:
					if( col_tank_list[ tn->second.x ].begin()->first != tn->second.y ) {
						it = col_tank_list[ tn->second.x ].lower_bound( tn->second.y - 1 );
					} else {
						--(it = col_tank_list[ tn->second.x ].end());
						distance = board_size;
					}
					distance += tn->second.y - it->first;
					break;
				case 1:
					if( (--( it = row_tank_list[ tn->second.y ].end() ))->first != tn->second.x ) {
						it = row_tank_list[ tn->second.y ].upper_bound( tn->second.x );
					} else {
						it = row_tank_list[ tn->second.y ].begin();
						distance = board_size;
					}
					distance += it->first - tn->second.x;
					break;
				case 2:
					if( (--( col_tank_list[ tn->second.x ].end() ))->first != tn->second.y ) {
						it = col_tank_list[ tn->second.x ].upper_bound( tn->second.y );
					} else {
						it = col_tank_list[ tn->second.x ].begin();
						distance = board_size;
					}
					break;
					distance += it->first - tn->second.y;
				case 3:
					if( row_tank_list[ tn  ->second.y ].begin()->first != tn->second.x ) {
						it = row_tank_list[ tn->second.y ].lower_bound( tn->second.x - 1 );
					} else {
						--( it = row_tank_list[ tn->second.y ].end() );
						distance = board_size;
					}
					distance += tn->second.x - it->first;
					break;
			}
			
			if( distance <= shooting_range ) {
				
				tn = tanks[ it->second.first ].find( it->second.second );
				kills.insert( make_pair( make_pair( tn->second.x, tn->second.y ), tn->second ) );
			}
		}

		for( map<pair<int,int>, tank>::iterator kl = kills.begin(); kl != kills.end(); ++kl ) {
			
			kill( pids[ kl->second.player ][ kl->second.tn ], SIGKILL );
			total_tanks--;
			if( !--left_tanks[ kl->second.player ] )
				remaining_players--;
			
			col_tank_list[ kl->second.x ].erase( kl->second.y );
			row_tank_list[ kl->second.y ].erase( kl->second.x );
			tanks[ kl->second.player ].erase( kl->second.tn );
		}
		
		// laser
		for( list<play>::iterator play = laser.begin(); play != laser.end(); ++play ) {
			
			map<int, tank>::iterator tn = tanks[ play->player ].find( play->tn );
			map<int, pair<int,int> >::iterator it;
			int distance = 0;

			switch( play->direction ) {
				case 0:
					if( col_tank_list[ tn->second.x ].begin()->first != tn->second.y ) {
						it = col_tank_list[ tn->second.x ].lower_bound( tn->second.y - 1 );
					} else {
						--( it = col_tank_list[ tn->second.x ].end() );
						distance = board_size;
					}
					distance += tn->second.y - it->first;
					break;
				case 1:
					if( (--( row_tank_list[ tn->second.y ].end() ))->first != tn->second.x ) {
						it = row_tank_list[ tn->second.y ].upper_bound( tn->second.x );
					} else {
						it = row_tank_list[ tn->second.y ].begin();
						distance = board_size;
					}
					distance += it->first - tn->second.x;
					break;
				case 2:
					if( (--( col_tank_list[ tn->second.x ].end() ))->first != tn->second.y ) {
						it = col_tank_list[ tn->second.x ].upper_bound( tn->second.y );
					} else {
						it = col_tank_list[ tn->second.x ].begin();
						distance = board_size;
					}
					distance += it->first - tn->second.y;
					break;
				case 3:
					if( row_tank_list[ tn->second.y ].begin()->first != tn->second.x ) {
						it = row_tank_list[ tn->second.y ].lower_bound( tn->second.x - 1 );
					} else {
						--( it = row_tank_list[ tn->second.y ].end() );
						distance = board_size;
					}
					distance += tn->second.x - it->first;
					break;
			}

			int write_fd = pipes[ play->player ][ play->tn ].second[1];
			char str[25];
			
			write( write_fd, str, sprintf( str, "%d %d\n", it->second.first, distance ) );
			
			INFORM( 3, "ans: %s", str );
		}
	}
	
	// kill all subprocesses
	for( int player = 0; player < players; player++ ) {
		for( map<int, tank>::iterator tn = tanks[ player ].begin(); tn != tanks[ player ].end(); ++tn ) {
			kill( pids[ player ][ tn->second.tn ], SIGKILL );
		}
	}

	// calculate winners
	int M = *max_element( left_tanks.begin(), left_tanks.end() );

	for( int player = 0; player < players; player++ ) {
		if( left_tanks[ player ] == M ) {
			INFORM( 0, "WINNER %d\n", player );
		}
	}

	INFORM( 0, "END\n" );
}
