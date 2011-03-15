#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
using namespace std;


// verbosity \in {0..5}
#define VERBOSITY 0

// maximum command length
#define MAX_COM_LEN 20


#define INFORM( ver, ... ) do{ if( VERBOSITY >= ver ) printf( __VA_ARGS__ ); }while(0)
#define PRINT_ERROR( ex, ... ) do{ INFORM( 0, __VA_ARGS__ ); INFORM( 0, ex ? "\nExiting...\n" : "\n" ); if( ex ) exit(1); }while(0)

#define forall( it, c ) for( typeof((c).end()) it = (c).begin(); it != (c).end(); ++it )

typedef int pipe_t[2];


enum { UP, RIGHT, DOWN, LEFT };
enum { MOVE, SHOOT, LASER, COM_ERROR };


struct game;


struct tank {

	int x,y;
	int player, tn;
	bool alive;

	tank () {}

	tank ( int x, int y, int player, int tank ): x(x), y(y), player(player), tn(tank), alive(true) {}

	pair<int,int> get_command( game* g );

	void kill( game* g );

	pair<tank*, int> find_nearest( game* g, int dir );
};


struct play {

	int player, tn, direction;

	play () {}

	play ( int player, int tank, int direction ): player(player), tn(tank), direction(direction) {}

	tank* get_tank( game* g );
};


struct tank_proc {

	int pid;
	int read_fd, write_fd;

	tank_proc () {}

	tank_proc ( char* child_proc ) {

		pipe_t father_pipe, child_pipe;

		// create pipes

		if( pipe( father_pipe ) < 0 || pipe( child_pipe ) < 0 ) {

			PRINT_ERROR( true, "Error while setting up the pipes" );
		}

		read_fd = father_pipe[0];
		write_fd = child_pipe[1];

		// create subprocess

		if( ( pid = fork() ) < 0 ) {

			PRINT_ERROR( true, "Error while setting up the child processes" );
		}
		if ( pid == 0 ) {

			dup2( child_pipe[0], 0 );
			dup2( father_pipe[1], 1 );

			close( child_pipe[1] );
			close( father_pipe[0] );

			execl( child_proc, NULL, NULL );

			PRINT_ERROR( true, "Error while calling a child process" );
		}

		close( child_pipe[0] );
		close( father_pipe[1] );
	}


	int readln( int max, char* str ) {

		int len = 0;

		while ( len < max) {

			len += read( read_fd, str + len, 1 );

			if( str[ len - 1 ] == '\n' ) {
				str[ len - 1 ] = 0;
				break;
			}
		}

		return len;
	}


	int writeln( char* str ) {

		return write( write_fd, str, strlen( str ) );
	}


	void kill_proc() {

		if( pid == -1 ) return;

		close( read_fd );
		close( write_fd );

		kill( pid, SIGKILL );

		pid = read_fd = write_fd = -1;
	}
};


struct mini_tank {

	int player, tn;

	mini_tank () {}
	mini_tank ( int player, int tank ): player(player), tn(tank) {}

	tank* get_tank( game* g );
};


class game {

	char** executables;
	bool played;

public:

	int players, tanks_per_player, board_size, shooting_range, max_time;
	int remaining_tanks, remaining_players, time;

	vector<vector<tank> > tanks;
	vector<vector<tank_proc> > tank_procs;

	map<int, map<int, mini_tank > > row_tank_list;
	map<int, map<int, mini_tank > > col_tank_list;

	vector<int> left_tanks;


	game ( int players, int tanks_per_player, int board_size, int shooting_range, int max_time, char** executables ) {

		played = false;

		this->players = players;
		this->tanks_per_player = tanks_per_player;
		this->board_size = board_size;
		this->shooting_range = shooting_range;
		this->max_time = max_time;
		this->executables = executables;

		remaining_tanks = players * tanks_per_player;
		remaining_players = players;
		time = 0;

		left_tanks = vector<int>( players, tanks_per_player );

		init_procs();
		init_tanks();

	}


	vector<int> play_game() {

		if ( !played ) {
			played = true;
		} else {
			return vector<int>();
		}

		while ( time++ < max_time && remaining_players > 1 ) {

			if( time % 1000 )
				INFORM( 1, "time: %d\n", time );
			else
				INFORM( 2, "time: %d\n", time );

			vector<vector<play> > commands = get_commands();

			move_phase( commands[ MOVE ] );
			shoot_phase( commands[ SHOOT ] );
			laser_phase( commands[ LASER ] );
		}

		if( time >= max_time ) {

			INFORM( 0, "TIME LIMIT EXCEDEED\n" );
		}

		// calculate winners
		int M = *max_element( left_tanks.begin(), left_tanks.end() );

		vector<int> ret;

		for( int player = 0; player < players; player++ ) {

			if( left_tanks[ player ] == M ) {
				ret.push_back( player );
			}

			for( int tn = 0; tn < tanks_per_player; tn++ ) {
				kill_tank( player, tn );
			}
		}

		return ret;
	}


private:


	vector<vector<play> > get_commands() {

		vector<vector<play> > ret(3, vector<play>(0) );

		for( int player = 0; player < players; player++ ) {
			forall( tn, tanks[ player ] ) {

				if( !tn->alive ) continue;

				INFORM( 4, "pos: %d %d\n", tn->x, tn->y );

				pair<int,int> command = tn->get_command( this );

				if ( command.first == COM_ERROR ) {

					PRINT_ERROR( false, "Player's %d tank number %d produced"
										" a wrong command\nKilling tank...",
										player, tn->tn );
					// kill mother fucker
					kill_tank( player, tn->tn );
				}

				ret[ command.first ].push_back( play( player, tn->tn, command.second ) );
			}
		}

		return ret;
	}


	void move_phase( vector<play> moves ) {

		static const int movY[] = {-1,0,1,0};
		static const int movX[] = {0,1,0,-1};

		map<pair<int,int>,list<tank*> > new_pos;

		forall( move, moves ) {

			tank* tn = move->get_tank( this );

			row_tank_list[ tn->y ].erase( tn->x );
			col_tank_list[ tn->x ].erase( tn->y );

			tn->x += movX[ move->direction ] + board_size;
			tn->x %= board_size;
			tn->y += movY[ move->direction ] + board_size;
			tn->y %= board_size;

			new_pos[ make_pair( tn->x, tn->y ) ].push_back( tn );
		}

		forall( l, new_pos ) {

			map<int, mini_tank>::iterator it = col_tank_list[ l->first.first ].find( l->first.second );

			if ( it != col_tank_list[ l->first.first ].end() ) {

				l->second.push_back( it->second.get_tank( this ) );

				row_tank_list[ l->first.second ].erase( l->first.first );
				col_tank_list[ l->first.first ].erase( it );
			}

			if ( l->second.size() == 1 ) {

				tank* tn = l->second.front();

				col_tank_list[ tn->x ][ tn->y ] = mini_tank( tn->player, tn->tn );
				row_tank_list[ tn->y ][ tn->x ] = mini_tank( tn->player, tn->tn );
			} else {
				forall( tn, l->second ) {
					kill_tank( (*tn)->player, (*tn)->tn );
				}
			}
		}
	}


	void shoot_phase( vector<play> shoots ) {

		vector<tank*> kills;

		forall( shoot, shoots ) {

			pair<tank*, int> nearest = shoot->get_tank( this )->find_nearest( this, shoot->direction );

			if( nearest.second <= shooting_range ) {
				kills.push_back( nearest.first );
			}
		}

		forall( kill, kills )
			kill_tank( (*kill)->player, (*kill)->tn );
	}


	void laser_phase( vector<play> lasers ) {

		char str[ MAX_COM_LEN ];

		forall( laser, lasers ) {

			pair<tank*, int> nearest = laser->get_tank( this )->find_nearest( this, laser->direction );

			sprintf( str, "%d %d\n", nearest.first->player, nearest.second );

			tank_procs[ laser->player ][ laser->tn ].writeln( str );

			INFORM( 3, "ans: %s", str );
		}
	}


	void init_procs() {

		tank_procs = vector<vector<tank_proc> >( players, vector<tank_proc>( tanks_per_player ) );

		char str[ MAX_COM_LEN ];

		for( int player = 0; player < players; player++ )
			forall( proc, tank_procs[ player ] ) {

				*proc = tank_proc( executables[ player ] );

				sprintf( str, "%d %d\n", player, shooting_range );

				proc->writeln( str );
			}
	}


	void init_tanks() {

		tanks = vector<vector<tank> >( players, vector<tank>( tanks_per_player ) );


		for( int player = 0; player < players; player++ ) {
			for( int tn = 0; tn < tanks_per_player; tn++ ) {

				int x = rand() % board_size;
				int y = rand() % board_size;

				tanks[ player ][ tn ] = tank( x, y, player, tn );

				row_tank_list[y][x] = mini_tank( player, tn );
				col_tank_list[x][y] = mini_tank( player, tn );
			}
		}
	}


	void kill_tank( int player, int tn ) {

		tanks[ player ][ tn ].kill( this );
		tank_procs[ player ][ tn ].kill_proc();
	}
};


tank* mini_tank::get_tank( game* g) {

	return &g->tanks[ player ][ tn ];
}


tank* play::get_tank( game* g ) {

	return &g->tanks[ player ][ tn ];
}


pair<int,int> tank::get_command( game* g ) {

	int com, dir;
	char str[ MAX_COM_LEN ], command[ MAX_COM_LEN  ], direction[ MAX_COM_LEN ];

	if( g->tank_procs[ player ][ tn ].readln( MAX_COM_LEN - 1, str ) == MAX_COM_LEN ) {
		return make_pair( (int)COM_ERROR, 0 );
	}

	if( sscanf( str, "%s %s %s", command, direction, direction ) != 2 ) {
		return make_pair( (int)COM_ERROR, 0 );
	}

	INFORM( 2, "%s\n", str );

	if( !strcmp( direction, "UP" ) ) dir = UP;
	else if( !strcmp( direction, "RIGHT" ) ) dir = RIGHT;
	else if( !strcmp( direction, "DOWN" ) ) dir = DOWN;
	else if( !strcmp( direction, "LEFT" ) ) dir = LEFT;
	else {
		return make_pair( (int)COM_ERROR, 0 );
	}

	if( !strcmp( command, "MOVE" ) ) com = MOVE;
	else if( !strcmp( command, "SHOOT" ) ) com = SHOOT;
	else if( !strcmp( command, "LASER" ) ) com = LASER;
	else {
		return make_pair( (int)COM_ERROR, 0 );
	}

	return make_pair( com, dir );
}


void tank::kill( game* g ) {

	if ( !alive ) return;

	g->remaining_tanks--;
	if( !--g->left_tanks[ player ] )
		g->remaining_players--;

	g->col_tank_list[ x ].erase( y );
	g->row_tank_list[ y ].erase( x );

	alive = false;
}


pair<tank*, int> tank::find_nearest( game* g, int dir ) {

	map<int, mini_tank> *tank_strip;
	pair<int,int> coords;

	if( dir == UP || dir == DOWN ) {
		coords = make_pair( x, y );
		tank_strip = &g->col_tank_list[ x ];
	} else {
		coords = make_pair( y, x );
		tank_strip = &g->row_tank_list[ y ];
	}

	mini_tank tn;
	int dis = 0;

	if( dir == UP || dir == LEFT ) {

		if( tank_strip->begin()->first != coords.second ) {
			tn = tank_strip->lower_bound( coords.second - 1 )->second;
		} else {
			tn = tank_strip->rbegin()->second;
			dis = g->board_size;
		}
		dis += coords.second - ( dir == LEFT ) ? tn.get_tank(g)->x : tn.get_tank(g)->y;

	} else {

		if( tank_strip->rbegin()->first != coords.second ) {
			tn = tank_strip->upper_bound( coords.second )->second;
		} else {
			tn = tank_strip->begin()->second;
			dis = g->board_size;
		}
		dis += - coords.second + ( dir == RIGHT ) ? tn.get_tank(g)->x : tn.get_tank(g)->y;
	}

	return make_pair( tn.get_tank(g), dis );
}


int main( int argc, char** argv ) {

	int tanks_per_player, board_size, shooting_range, max_time;

	if ( argc < 6 ) {
		PRINT_ERROR( true, "usage: ToT tanks_per_player board_size shooting_range "
						   "max_time [player1program] [player2program] ..." );
	}

	sscanf( argv[1], "%d", &tanks_per_player );
	sscanf( argv[2], "%d", &board_size );
	sscanf( argv[3], "%d", &shooting_range );
	sscanf( argv[4], "%d", &max_time );

	game g( argc - 5, tanks_per_player, board_size, shooting_range, max_time, argv + 5 );

	vector<int> winners = g.play_game();

	forall( winner, winners ) {
		INFORM( 0, "WINNER %d\n", *winner );
	}

	INFORM( 0, "END\n" );
}
